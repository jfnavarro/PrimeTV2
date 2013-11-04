/* 
  The NHX extensions are from Zmasek-Eddy: http://www.genetics.wustl.edu/eddy/forester/NHX.html.
*/

/*
* IMPORTANT NOTE!
*
* To accomodate for multiple lexers/parsers in the program, yy-identifiers have been 
* renamed to 'yytree_'-something instead. 
*/ 

%{
#include "NHXparse.h"


#include <string.h>

unsigned int lineno = 0;
unsigned int n_left_parens = 0;
unsigned int n_right_parens = 0;
unsigned int n_leaves = 0;

extern char * padded_anti_chain_markup_tag;

/* Support for string-reading (see user code section below). */
YY_BUFFER_STATE str_buf;
YY_BUFFER_STATE prev_buf;

%}

%option noyywrap

/* Handle tree comments sensible */
%x comment
%x annotation
/* 
   Some shortcuts
*/

TAG     [a-zA-Z]+
STR     [a-zA-Z0-9\|\-\_/+*#!%&?\.]+
FLOAT   [+-]?[0-9]+\.?[0-9]*([eE][-+]?[0-9]+)?
INTEGER [+-]?[0-9]+

%%
\(       { n_left_parens++; return LEFT_PAREN; }
\)       { n_right_parens++; return RIGHT_PAREN; }
\:       { return COLON; }
,        { return COMMA; }
\;       { /*return SEMICOLON; Do not bother! */ }
\'       { return APOSTROPHE; }
{INTEGER} { yytree_lval.integer = atoi(yytext); return INTEGER; }
{FLOAT}  { yytree_lval.branch_time = atof(yytext); return FLOAT; }
{STR}    { yytree_lval.str = strdup(yytext); return STRING; }
\[&&NHX  { BEGIN(annotation); return NHX_ANNOTATION_START; }
\[&&BEEP  { BEGIN(annotation); return BEEP_ANNOTATION_START; }
\[&&PRIME  { BEGIN(annotation); return BEEP_ANNOTATION_START; }
\[       { BEGIN(comment); }
\n       { lineno++;}
[ \t] /* Do nothing */


<annotation>\] { BEGIN(INITIAL); return ANNOTATION_END; }
<annotation>=         { return EQUAL; }
<annotation>\(        { return LEFT_PAREN; }
<annotation>\)        { return RIGHT_PAREN; }
<annotation>[,:]      { return SEPARATOR; }
<annotation>{INTEGER} { yytree_lval.integer = atoi(yytext); return INTEGER;}
<annotation>{FLOAT}   { yytree_lval.branch_time = atof(yytext); return FLOAT; }
<annotation>'[^']*'   { yytree_lval.str = strdup(yytext+1); *(strstr(yytree_lval.str, "'")) = '\0'; return STRING; }
<annotation>{STR}     { yytree_lval.str = strdup(yytext);  ; return STRING; }
<annotation>.	/* Ignore spaces and such */

<comment>\] { BEGIN(INITIAL); }
<comment>.  /* eat comment */
<comment>\n { lineno++;}

%%
/*
  Support for reading free strings rather than files.

  Important: These calls do not nest!
*/
void
read_from_string(char *str) {
  prev_buf = YY_CURRENT_BUFFER;
  str_buf = yy_scan_string(str);
  yy_switch_to_buffer(str_buf);
}

void
close_string_buffer() {
  yy_delete_buffer(str_buf);
  yy_switch_to_buffer(prev_buf);
}
