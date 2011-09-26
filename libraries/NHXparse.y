%{
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "NHXnode.h"
#include "NHXtree.h"
#include "NHXannotation.h"


/* 
   Defined this to get more detailed error messages from parser. Only
   for debugging really. To messy for a user. 
*/
/* #define YYERROR_VERBOSE */ 

/* Here comes some C declarations */

extern FILE *yytree_in;
extern char *yytree_text;
extern unsigned int lineno; /* Current line number in input file */
extern unsigned int n_left_parens; /* Number of matched left parens '(' */
extern unsigned int n_right_parens; /* Number of matched right parens ')' */
extern unsigned int n_leaves;	/* Number of found tree leaves*/

int err_flag = 0; /* arve's workaround for the non-working YYRECOVERING() macro */

/* The list of found trees */
struct NHXtree *input_trees;

/* For better error messages, we store the name of the current file */
char *current_filename;

/* When we see an annotation in the form "TAG = 4711", an 
   annotation structure is created already after the equal sign.
   That way we have some space to put the value in. OK, bad 
   explanation, but look at the code... (rules for 'annotation'
   and 'value'.)
*/
struct NHXannotation *current_annotation;

/* Some prototypes */
void set_str_annotation(char *str);
void set_int_annotation(int i);
void set_float_annotation(float f);
void set_int_list_annotation(struct int_list *il);
typedef enum {
  string_type=1, 
  int_type=2, 
  float_type=4, 
  number_type=6,	/* == int_type | float_type */
  int_list_type=8
} type;
type get_annotation_type();
void check_annotation_type(type actual_type);

void err_msg(char *s);
void yyerror(char *s);
%}


%union {
  struct NHXtree *t;		/* For returning full trees */
  struct NHXnode *v;		/* For returning tree nodes */
  struct NHXannotation *a;	/* For handling node annotations */
  float branch_time;
  char *str;         /* Dealing with leaf names */
  unsigned integer;
  struct int_list *il;
}

%token LEFT_PAREN 
%token RIGHT_PAREN
%token EQUAL
%token COLON
%token SEMICOLON
%token COMMA
%token SEPARATOR
%token APOSTROPHE
%token <str> TAG
%token <str> STRING
%token <branch_time> FLOAT
%token <integer> INTEGER
%token NHX_ANNOTATION_START
%token BEEP_ANNOTATION_START
%token ANNOTATION_END
%token SPECIES_NAME
%token IS_DUPLICATION
%token NODE_ID
%token UNKNOWN_ANNOTATION

%type <t> tree_file tree_list
%type <v> subtree subtree_list tree
%type <v> leaf
%type <str> label
%type <a> value
%type <a> newick_weight
%type <branch_time> number
%type <a> possible_x_annotation ext_annotations ext_annotation
%type <a> annotation_list annotation
%type <il> int_list


/* I cannot remember where the shift/reduce conflicts are, but two 
   of them should be in the multi-tree or multi-child parsing. */
%expect 4 

%%

tree_file : /* empty */ { err_msg("No input tree!"); }
	  | tree_list   { input_trees = $1; }
	  ;

tree_list : tree           { $$ = new_tree($1, NULL); }
          | tree_list tree { $$ = new_tree($2, $1);   }
          ;

tree : subtree
     ;


subtree_list : subtree
             | subtree_list COMMA subtree { $$ = new_node(NULL); 
					    $1->parent = $$;
					    $3->parent = $$;
					    $$->left = $1;
					    $$->right = $3;
					  }
	     ;

subtree : leaf 
	  newick_weight 
	  possible_x_annotation { $$ = $1;
				  annotate_node($$, append_annotations($2, $3));
				}

        | LEFT_PAREN subtree_list RIGHT_PAREN
	  label newick_weight
	  possible_x_annotation { $2->name = $4; /* label */
				  annotate_node($2, append_annotations($5, $6));
				  $$ = $2;
		                }
        | LEFT_PAREN error { $$ = NULL; err_msg("Could not parse subtree"); }
        ; 

leaf    : STRING { $$ = new_node($1); n_leaves++;}
	| APOSTROPHE STRING APOSTROPHE  { $$ = new_node($2); n_leaves++; }
        ;



label	: /* empty */                  { $$ = NULL; }
	| INTEGER                      { char str[10]; sprintf(str, "%d", $1); $$ = strdup(str);}
        | FLOAT                        { char str[10]; sprintf(str, "%f", $1); $$ = strdup(str);}
	| STRING                       { $$ = $1;}
	| APOSTROPHE STRING APOSTROPHE { $$ = $2;}
	;


newick_weight : /* empty */ {$$ = NULL; }/*{ $$ = new_newick_weight(0.0, NULL); } /* Signal lack of time annotation! */
            | COLON number { $$ = new_newick_weight($2, NULL); }
	    | error { $$ = new_newick_weight(0.0, NULL); err_msg("Expected a branchlength");}
	    ;

number : FLOAT   { $$ = $1;}
       | INTEGER { $$ = (float) $1; }
       ;

possible_x_annotation : /* empty */ { $$ = NULL;}
		      | ext_annotations
		      ;

ext_annotations : ext_annotation
		| ext_annotations ext_annotation { $$ = append_annotations($1, $2); }
		;

ext_annotation : NHX_ANNOTATION_START possible_separator annotation_list ANNOTATION_END { $$ = $3; }
	       | BEEP_ANNOTATION_START possible_separator annotation_list ANNOTATION_END { $$ = $3; }
               | NHX_ANNOTATION_START error { err_msg("Syntax error in extended annotations"); $$ = NULL; }
               | BEEP_ANNOTATION_START error { err_msg("Syntax error in extended annotations"); $$ = NULL; }
	       ;

annotation_list	: annotation
		| annotation possible_separator annotation_list { $$ = append_annotations($1, $3); }
		;

possible_separator : 
		   | SEPARATOR
		   ;

annotation	: STRING 
		  EQUAL { current_annotation = new_annotation($1, NULL);}
		  value { $$ = current_annotation; }
		| error { err_msg("Syntax error in extended annotations");}
		;

value		: STRING	  { set_str_annotation($1);}
		| INTEGER         { set_int_annotation($1); }
		| FLOAT           { set_float_annotation($1); }
		| LEFT_PAREN int_list RIGHT_PAREN { set_int_list_annotation($2); }
		| error { err_msg("Wrong value type"); }
		;

int_list : INTEGER { $$ = new_int_list($1, NULL); }
	 | int_list INTEGER { $$ = new_int_list($2, $1); }
	 ;

%%

void
yyerror (char *s) {
  fprintf (stderr, "%s:line %d: %s\n", current_filename, lineno, s);
  if (n_right_parens > n_left_parens) {
     fprintf(stderr, "\tUnbalanced parenthesis!\n");
  }
  if (n_left_parens == 1) {	/* Style is everything! */
    fprintf (stderr, "\tAfter 1 leaf, %d '(' and %d ')' parens: '%s'\n", n_left_parens, n_right_parens, yytree_text);
  } else {
    fprintf (stderr, "\tAfter %d leaves, %d '(' and %d ')' parens: '%s'\n", n_leaves, n_left_parens, n_right_parens, yytree_text);
  }
  err_flag = 1;
}

void
err_msg(char *s) {
  if (err_flag) {	     
     fprintf(stderr, "%s\n", s);
     err_flag = 0;
  }
}	     

/*
  The following functions update the annotation with the actual values, 
  but also performs some type checking with the known tags.
*/

void
set_str_annotation(char *str) {
  check_annotation_type(string_type);
  current_annotation->arg.str = str;
}
void
set_int_annotation(int i) {
  type t = get_annotation_type();
  if (t == int_type) {
    current_annotation->arg.i = i;
  } else if (t == float_type) {
    current_annotation->arg.t = (float) i;
  }
}
void
set_int_list_annotation(struct int_list *il) {
  check_annotation_type(int_list_type);
  current_annotation->arg.il = il;
}
void
set_float_annotation(float f) {
  check_annotation_type(float_type);
  current_annotation->arg.t = f;
}



char *arb_tags[] = {"S",         "AC",          "ID",     "NT",       "BL",       "ET",       "NW",      "EX",      "D",     "TT",   NULL};
type arb_types[] = {string_type, int_list_type, int_type, float_type, float_type, float_type, float_type,int_type,int_type,float_type};


type
get_annotation_type() {
  int i;
  
  for (i=0; arb_tags[i] != NULL; i++) {
    if (strcmp(current_annotation->anno_type, arb_tags[i]) == 0) {
      	return arb_types[i];
    }
  } 
  
  fprintf(stderr, "%s:%d: Error, tag without known type: %s\n", 
	  current_filename, lineno, current_annotation->anno_type);
  exit(18);
}

/*
  A function to verify the type of an annotation given the tag
*/
void 
check_annotation_type(type actual_type) {
  int i;
/*   char errbuf[1024];  */
  
  for (i=0; arb_tags[i] != NULL; i++) {
    if (strcmp(current_annotation->anno_type, arb_tags[i]) == 0) {
      if (arb_types[i] & actual_type) {
	return;
      } else {
	fprintf(stderr, "%s:%d:  Error, wrong type for tag %s!\n", 
		current_filename,
		lineno,
		current_annotation->anno_type);
	exit(17);
      }
    }
  }
}


void
set_globals(const char *filename) {
   /* For error messages */
  current_filename = strdup(filename);
  n_left_parens = 0;
  n_right_parens = 0;
  n_leaves = 0;
  lineno = 1;
  	
#if YYDEBUG == 1
   yydebug = 1;
#endif

}	


/*
  read_tree
  
  Instruct the parser to read a tree from 'filename' or, if filename
  is NULL, STDIN. Return the read tree on success, or NULL.
*/
struct NHXtree *
read_tree(const char *filename) {
  FILE *f = NULL;
  int ret_val;

  if (filename == NULL) {
    yytree_in = stdin;
    set_globals("STDIN");	/* For better error messages */
  } else {
    f = fopen(filename, "r");
    set_globals(filename);	/* For better error messages */
    if (!f) {
      fprintf(stderr, "Could not open tree file '%s' for reading.\n", filename);
      return NULL;
    } else {
      yytree_in = f;
    }
  } 
  
  ret_val = yyparse();

  /* Cleanup */
  if (f != NULL) {
    close(f);
    yytree_in = stdin;
  }

  if (ret_val == 1)
    return NULL;
  else
    return input_trees;
}



/*
  read_tree_from_file_stream
  
  Instruct the parser to read a tree from a file stream.
*/

struct NHXtree *
read_tree_from_file_stream( FILE * f ) {
  int ret_val;
  set_globals("");
  yytree_in = f;
  ret_val = yyparse();
  if (ret_val == 1)
    return NULL;
  else
    return input_trees;
}



/*
  read_tree_string
  
  Instruct the parser to read from a string rather than a
  file. Otherwise behaves like read_tree.
*/
struct NHXtree *
read_tree_string(const char *str) {
  int ret_val;

  if (str == NULL) {
    fprintf(stderr, "Warning: Tried to read a tree from a NULL string.\n");
    return NULL;
  } 
    
  set_globals("<input string>");	/* For better error messages */
  read_from_string(str);

  ret_val = yyparse();

  /* Cleanup */
  close_string_buffer();

  if (ret_val == 1)
    return NULL;
  else
    return input_trees;
}
