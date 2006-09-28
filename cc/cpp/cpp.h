/*	$Id: cpp.h,v 1.19 2006/09/28 11:10:07 ragge Exp $	*/

/*
 * Copyright (c) 2004 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h> /* for obuf */

#include "../../config.h"

typedef unsigned char usch;
extern FILE *obuf;
extern char *yytext; /* XXX - only flex */
extern usch *stringbuf;

extern	int	trulvl;
extern	int	flslvl;
extern	int	elflvl;
extern	int	elslvl;
extern	int	tflag, Cflag;

/* args for lookup() */
#define FIND    0
#define ENTER   1

/* Symbol table entry  */
struct symtab {
	usch *namep;    
	usch *value;    
};

#define	ROUND(x) (((x)+sizeof(ALIGNMENT)-1)& ~(sizeof(ALIGNMENT)-1))

/* buffer used internally */
#ifndef CPPBUF
#define CPPBUF  BUFSIZ
#endif

#define	NAMEMAX	64 /* max len of identifier */
struct recur;	/* not used outside cpp.c */
int subst(struct symtab *, struct recur *);
struct symtab *lookup(char *namep, int enterf);
void gotident(struct symtab *nl);
int slow;	/* scan slowly for new tokens */

int pushfile(char *fname);
void popfile(void);
void error(char *s, ...);
void prtline(void);
int yylex(void);
void cunput(int);
int curline(void);
char *curfile(void);
void setline(int);
void setfile(char *);
int yyparse(void);
void yyerror(char *);
void unpstr(usch *);
usch *savstr(usch *str);
void savch(int c);
void mainscan(void);
void putch(int);
void putstr(usch *s);

