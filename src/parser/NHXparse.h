/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LEFT_PAREN = 258,
     RIGHT_PAREN = 259,
     EQUAL = 260,
     COLON = 261,
     SEMICOLON = 262,
     COMMA = 263,
     SEPARATOR = 264,
     APOSTROPHE = 265,
     TAG = 266,
     STRING = 267,
     FLOAT = 268,
     INTEGER = 269,
     NHX_ANNOTATION_START = 270,
     BEEP_ANNOTATION_START = 271,
     ANNOTATION_END = 272,
     SPECIES_NAME = 273,
     IS_DUPLICATION = 274,
     NODE_ID = 275,
     UNKNOWN_ANNOTATION = 276
   };
#endif
/* Tokens.  */
#define LEFT_PAREN 258
#define RIGHT_PAREN 259
#define EQUAL 260
#define COLON 261
#define SEMICOLON 262
#define COMMA 263
#define SEPARATOR 264
#define APOSTROPHE 265
#define TAG 266
#define STRING 267
#define FLOAT 268
#define INTEGER 269
#define NHX_ANNOTATION_START 270
#define BEEP_ANNOTATION_START 271
#define ANNOTATION_END 272
#define SPECIES_NAME 273
#define IS_DUPLICATION 274
#define NODE_ID 275
#define UNKNOWN_ANNOTATION 276




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 63 "/Users/josefernandeznavarro/Projects/primetv2/PrimeTV2/src/parser/NHXparse.y"
{
  struct NHXtree *t;		/* For returning full trees */
  struct NHXnode *v;		/* For returning tree nodes */
  struct NHXannotation *a;	/* For handling node annotations */
  float branch_time;
  char *str;         /* Dealing with leaf names */
  unsigned integer;
  struct int_list *il;
}
/* Line 1529 of yacc.c.  */
#line 101 "/Users/josefernandeznavarro/Projects/primetv2/PrimeTV2/src/parser/NHXparse.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yytree_lval;

