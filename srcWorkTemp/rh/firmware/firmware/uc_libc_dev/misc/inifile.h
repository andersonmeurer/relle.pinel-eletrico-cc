#ifndef __INIFILE_H
#define __INIFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//http://ccodearchive.net/info/ciniparser.html

#if !defined(LINUX) && !defined(CYGWIN)
#error ERRO: SOMENTE PARA USO DE SISTEMA OPERACCIONAL POR CAUSA DO SUPORTE A ARQUIVOS
#endif

/* Copyright (c) 2000-2007 by Nicolas Devillard.
 * Copyright (x) 2009 by Tim Post <tinkertim@gmail.com>
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/** @addtogroup ciniparser
 * @{
 */

/**
 * @file    ciniparser.h
 * @author  N. Devillard
 * @date    Sep 2007
 * @version 3.0
 * @brief   Parser for ini files.
 */

#define ciniparser_getstr(d, k)  ciniparser_getstring(d, k, NULL)

/**
 * @brief Dictionary object
 * @param n Number of entries in the dictionary
 * @param size Storage size
 * @param val List of string values
 * @param key List of string keys
 * @param hash List of hash values for keys
 *
 * This object contains a list of string/string associations. Each
 * association is identified by a unique string key. Looking up values
 * in the dictionary is speeded up by the use of a (hopefully collision-free)
 * hash function.
 */
typedef struct {
	int n;
	int size;
	char **val;
	char **key;
	unsigned *hash;
} tDictionary;

tDictionary *inifile_Open(const char *ininame);
char* inifile_GetString(tDictionary *d, const char *key, char *def);
int inifile_GetBoolean(tDictionary *d, const char *key, int notfound);
double inifile_GetDouble(tDictionary *d, char *key, double notfound);
int inifile_GetInteger(tDictionary *d, const char *key, int notfound);
void inifile_Close(tDictionary *d);

#endif
