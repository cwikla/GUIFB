/* $Id: xreg.c,v 1.2 1996/02/13 22:00:42 cwikla Exp $ */
/*
 * Copyright 1996 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.    John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wri.com
*/


/* Parts of this file from the FWF file FWF/src/Dir/RegExp.c by: */
/*
 * Author:
 *  Brian Totty
 *  Department of Computer Science
 *  University Of Illinois at Urbana-Champaign
 *  1304 West Springfield Avenue
 *  Urbana, IL 61801
 *
 *  totty@cs.uiuc.edu
 *
 */

#include <stdio.h>
#include <string.h>
#include "xreg.h"

#if !REG_EXEC && !REGEXP && !REGCOMP

#if USE_RE_COMP
#define REG_EXEC

#else

#if NO_REGCOMP

#if NO_REGEXP
#define REG_EXEC 1
#else
#define REGEXP 1 
#endif

#if NO_REGEXEC
#define REGEXP 1
#else
#define REG_EXEC 1 
#endif

#else

#define REGCOMP 1
#endif

#endif

#endif



#define BUFFER_SIZE 1024

static void _patternToRegExp(const char *_pattern, char *_regExp)
{
	int in_bracket;

	if ( (_pattern[0] == '\\') && (_pattern[1] == 'R'))
	{
		_pattern += 2;
		strcpy(_regExp, _pattern);
		return;
	}

	in_bracket = 0;
	while (*_pattern != '\0')
	{
		if (in_bracket)
		{
			if (*_pattern == ']') 
				in_bracket = 0;
			*_regExp++ = *_pattern++;
		}
		else
		{
			switch (*_pattern)
			{
				case '[':
					in_bracket = 1;
					*_regExp++ = '[';
				break;
				case '?':
					*_regExp++ = '.';
				break;
				case '*':
					*_regExp++ = '.';
					*_regExp++ = '*';
				break;
				case '.':
					*_regExp++ = '\\';
					*_regExp++ = '.';
				break;
				default:
					*_regExp++ = *_pattern;
				break;
			}
			++_pattern;
		}
	}
	*_regExp++ = '$';
	*_regExp++ = '\0';
}

#if (!REGCOMP && !REGEXP && !REG_EXEC)

void *REGcompile(const char *_pattern)
{
	return (void *)1;
}

int REGmatch(void *_context, const char *_string)
{
	return 1;
}

void REGfree(void *_context)
{
	/* nothing */
}

int REGneedsRecompile(void)
{
	return 0;
}


#endif /* REGCOMP && REGEXP */

#if REG_EXEC

extern char *re_comp(char *s);
extern int re_exec(char *s);

void *REGcompile(const char *_pattern)
{
	char regexp[BUFFER_SIZE];
	_patternToRegExp(_pattern, regexp);
	return (void *)(re_comp(regexp) == NULL);
}

int REGmatch(void *_context, const char *_string)
{
	return re_exec((char *)_string);
}

void REGfree(void *_context)
{
}

int REGneedsRecompile(void)
{
	return 1;
}
#endif /* REG_EXEC */

#if REGCOMP
#include <regex.h>

void *REGcompile(const char *_pattern)
{
	regex_t *re;
	int status;
	char regexp[BUFFER_SIZE];

	re = (regex_t *)malloc(sizeof(regex_t));
	if (re == (void *)NULL)
		return (void *)NULL;

	_patternToRegExp(_pattern, regexp);
	status = regcomp(re, regexp, REG_EXTENDED|REG_NOSUB);
	if (status != 0)
	{
		free((void *)re);
		return (void *)NULL;
	}

	return re;
}

int REGmatch(void *_context, const char *_string)
{
	int status;

	regex_t *re;
	re = (regex_t *)_context;

	status = regexec(re, _string, (size_t)0, NULL, 0);
	if (status != 0)
		return 0;
	else
		return 1;
}

void REGfree(void *_context)
{
	regfree((regex_t *)_context);
}

int REGneedsRecompile(void)
{
	return 0;
}

#endif /* REGCOMP */



#if REGEXP

#define INIT		register char *sp = instring;
#define GETC()	  (*sp++)
#define PEEKC()	 (*sp)
#define UNGETC(c)   --sp
#define RETURN(ptr) return (ptr)
#define ERROR(val) printf("%d\n", val)

#include <regexp.h>

int REGmatch(void *_context, const char *_string)
{
	if (advance(_string, (const char *)_context) != 0)
		return 1;
	else
		return 0;
}

void *REGcompile(const char *_pattern)
{
	char regexp[BUFFER_SIZE];
	char *fsm;

	fsm = (char *)malloc(sizeof(char) * BUFFER_SIZE);
	if (fsm == NULL)
		return (void *)NULL;

	_patternToRegExp(_pattern, regexp);
	compile(regexp, fsm, fsm+1024, 0);

	return (void *)fsm;
}

void REGfree(void *_context)
{
	free((void *)_context);
}

int REGneedsRecompile(void)
{
	return 0;
}


#endif /* REGEXP */

#if TESTING

int main(int _argc, char **_argv)
{
	void *context;
	context = REGcompile(_argv[2]);
	if (context)
	{
		printf("%d\n", REGmatch(context, _argv[1]));
		REGfree(context);
	}
	else
		printf("error\n");
}

#endif /* TESTRING */
