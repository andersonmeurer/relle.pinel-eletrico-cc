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
 *  @file ciniparser.c
 *  @author N. Devillard
 *  @date Sep 2007
 *  @version 3.0
 *  @brief Parser for ini files.
 */

#include "inifile.h"
#include <ctype.h>

#define ASCIILINESZ      (1024)
#define INI_INVALID_KEY  ((char*) NULL)

/** Maximum value size for integers and doubles. */
#define MAXVALSZ	1024

/** Minimal allocated number of entries in a dictionary */
#define DICTMINSZ	128

/** Invalid key token */
#define DICT_INVALID_KEY	((char*)-1)

unsigned dictionary_hash(char *key);
tDictionary *dictionary_new(int size);
void dictionary_del(tDictionary *vd);
char *dictionary_get(tDictionary *d, char *key, char *def);
int dictionary_set(tDictionary *vd, char *key, char *val);
void dictionary_unset(tDictionary *d, char *key);
void dictionary_dump(tDictionary *d, FILE *out);
int ciniparser_getnsec(tDictionary *d);
char *ciniparser_getsecname(tDictionary *d, int n);
void ciniparser_dump_ini(tDictionary *d, FILE *f);
void ciniparser_dump(tDictionary *d, FILE *f);
void ciniparser_unset(tDictionary *ini, char *entry);
int ciniparser_find_entry(tDictionary *ini, char *entry) ;
int ciniparser_set(tDictionary *d, char *entry, char *val);




/**
 * This enum stores the status for each parsed line (internal use only).
 */
typedef enum _line_status_ {
	LINE_UNPROCESSED,
	LINE_ERROR,
	LINE_EMPTY,
	LINE_COMMENT,
	LINE_SECTION,
	LINE_VALUE
} line_status;



/**
 * @brief Convert a string to lowercase.
 * @param s String to convert.
 * @return ptr to statically allocated string.
 *
 * This function returns a pointer to a statically allocated string
 * containing a lowercased version of the input string. Do not free
 * or modify the returned string! Since the returned string is statically
 * allocated, it will be modified at each function call (not re-entrant).
 */
static char *strlwc(const char *s)
{
	static char l[ASCIILINESZ+1];
	int i;

	if (s == NULL)
		return NULL;

	for (i = 0; s[i] && i < ASCIILINESZ; i++)
		l[i] = tolower(s[i]);
	l[i] = '\0';
	return l;
}

/**
 * @brief Remove blanks at the beginning and the end of a string.
 * @param s String to parse.
 * @return ptr to statically allocated string.
 *
 * This function returns a pointer to a statically allocated string,
 * which is identical to the input string, except that all blank
 * characters at the end and the beg. of the string have been removed.
 * Do not free or modify the returned string! Since the returned string
 * is statically allocated, it will be modified at each function call
 * (not re-entrant).
 */
static char *strstrip(const char *s)
{
	static char l[ASCIILINESZ+1];
	unsigned int i, numspc;

	if (s == NULL)
		return NULL;

	while (isspace(*s))
		s++;

	for (i = numspc = 0; s[i] && i < ASCIILINESZ; i++) {
		l[i] = s[i];
		if (isspace(l[i]))
			numspc++;
		else
			numspc = 0;
	}
	l[i - numspc] = '\0';
	return l;
}

/**
 * @brief Load a single line from an INI file
 * @param input_line Input line, may be concatenated multi-line input
 * @param section Output space to store section
 * @param key Output space to store key
 * @param value Output space to store value
 * @return line_status value
 */
static
line_status ciniparser_line(char *input_line, char *section,
	char *key, char *value)
{
	line_status sta;
	char line[ASCIILINESZ+1];
	int	 len;

	strcpy(line, strstrip(input_line));
	len = (int) strlen(line);

	if (len < 1) {
		/* Empty line */
		sta = LINE_EMPTY;
	} else if (line[0] == '#') {
		/* Comment line */
		sta = LINE_COMMENT;
	} else if (line[0] == '[' && line[len-1] == ']') {
		/* Section name */
		sscanf(line, "[%[^]]", section);
		strcpy(section, strstrip(section));
		strcpy(section, strlwc(section));
		sta = LINE_SECTION;
	} else if (sscanf (line, "%[^=] = \"%[^\"]\"", key, value) == 2
		   ||  sscanf (line, "%[^=] = '%[^\']'", key, value) == 2
		   ||  sscanf (line, "%[^=] = %[^;#]", key, value) == 2) {
		/* Usual key=value, with or without comments */
		strcpy(key, strstrip(key));
		strcpy(key, strlwc(key));
		strcpy(value, strstrip(value));
		/*
		 * sscanf cannot handle '' or "" as empty values
		 * this is done here
		 */
		if (!strcmp(value, "\"\"") || (!strcmp(value, "''"))) {
			value[0] = 0;
		}
		sta = LINE_VALUE;
	} else if (sscanf(line, "%[^=] = %[;#]", key, value) == 2
		||  sscanf(line, "%[^=] %[=]", key, value) == 2) {
		/*
		 * Special cases:
		 * key=
		 * key=;
		 * key=#
		 */
		strcpy(key, strstrip(key));
		strcpy(key, strlwc(key));
		value[0] = 0;
		sta = LINE_VALUE;
	} else {
		/* Generate syntax error */
		sta = LINE_ERROR;
	}
	return sta;
}



// #######################################################################################################################################
// INIFILE
// #######################################################################################################################################
/**
 * @brief    Parse an ini file and return an allocated dictionary object
 * @param    ininame Name of the ini file to read.
 * @return   Pointer to newly allocated dictionary
 *
 * This is the parser for ini files. This function is called, providing
 * the name of the file to be read. It returns a dictionary object that
 * should not be accessed directly, but through accessor functions
 * instead.
 *
 * The returned dictionary must be freed using inifile_Close().
 */
tDictionary *inifile_Open(const char *ininame)
{
	FILE *in;
	char line[ASCIILINESZ+1];
	char section[ASCIILINESZ+1];
	char key[ASCIILINESZ+1];
	char tmp[ASCIILINESZ+1];
	char val[ASCIILINESZ+1];
	int  last = 0, len, lineno = 0, errs = 0;
	tDictionary *dict;

	if ((in = fopen(ininame, "r")) == NULL) {
		fprintf(stderr, "ciniparser: cannot open %s\n", ininame);
		return NULL;
	}

	dict = dictionary_new(0);
	if (!dict) {
		fclose(in);
		return NULL;
	}

	memset(line, 0, ASCIILINESZ + 1);
	memset(section, 0, ASCIILINESZ + 1);
	memset(key, 0, ASCIILINESZ + 1);
	memset(val, 0, ASCIILINESZ + 1);
	last = 0;

	while (fgets(line+last, ASCIILINESZ-last, in)!=NULL) {
		lineno++;
		len = (int) strlen(line)-1;
		/* Safety check against buffer overflows */
		if (line[len] != '\n') {
			fprintf(stderr,
					"ciniparser: input line too long in %s (%d)\n",
					ininame,
					lineno);
			dictionary_del(dict);
			fclose(in);
			return NULL;
		}

		/* Get rid of \n and spaces at end of line */
		while ((len >= 0) &&
				((line[len] == '\n') || (isspace(line[len])))) {
			line[len] = 0;
			len--;
		}

		/* Detect multi-line */
		if (len >= 0 && line[len] == '\\') {
			/* Multi-line value */
			last = len;
			continue;
		}

		switch (ciniparser_line(line, section, key, val)) {
		case LINE_EMPTY:
		case LINE_COMMENT:
			break;

		case LINE_SECTION:
			errs = dictionary_set(dict, section, NULL);
			break;

		case LINE_VALUE:
			snprintf(tmp, ASCIILINESZ + 1, "%s:%s", section, key);
			errs = dictionary_set(dict, tmp, val);
			break;

		case LINE_ERROR:
			fprintf(stderr, "ciniparser: syntax error in %s (%d):\n",
					ininame, lineno);
			fprintf(stderr, "-> %s\n", line);
			errs++;
			break;

		default:
			break;
		}
		memset(line, 0, ASCIILINESZ);
		last = 0;
		if (errs < 0) {
			fprintf(stderr, "ciniparser: memory allocation failure\n");
			break;
		}
	}

	if (errs) {
		dictionary_del(dict);
		dict = NULL;
	}

	fclose(in);

	return dict;
}

/**
 * @brief    Get the string associated to a key
 * @param    d       Dictionary to search
 * @param    key     Key string to look for
 * @param    def     Default value to return if key not found.
 * @return   pointer to statically allocated character string
 *
 * This function queries a dictionary for a key. A key as read from an
 * ini file is given as "section:key". If the key cannot be found,
 * the pointer passed as 'def' is returned.
 * The returned char pointer is pointing to a string allocated in
 * the dictionary, do not free or modify it.
 */
char* inifile_GetString(tDictionary *d, const char *key, char *def)
{
	char *lc_key;
	char *sval;

	if (d == NULL || key == NULL)
		return def;

	lc_key = strlwc(key);
	sval = dictionary_get(d, lc_key, def);

	return sval;
}


/**
 * @brief    Get the string associated to a key, convert to a boolean
 * @param    d Dictionary to search
 * @param    key Key string to look for
 * @param    notfound Value to return in case of error
 * @return   integer
 *
 * This function queries a dictionary for a key. A key as read from an
 * ini file is given as "section:key". If the key cannot be found,
 * the notfound value is returned.
 *
 * A true boolean is found if one of the following is matched:
 *
 * - A string starting with 'y'
 * - A string starting with 'Y'
 * - A string starting with 't'
 * - A string starting with 'T'
 * - A string starting with '1'
 *
 * A false boolean is found if one of the following is matched:
 *
 * - A string starting with 'n'
 * - A string starting with 'N'
 * - A string starting with 'f'
 * - A string starting with 'F'
 * - A string starting with '0'
 *
 * The notfound value returned if no boolean is identified, does not
 * necessarily have to be 0 or 1.
 */
int inifile_GetBoolean (tDictionary *d, const char *key, int notfound)
{
	char *c;
	int ret;

	c = inifile_GetString(d, key, INI_INVALID_KEY);
	if (c == INI_INVALID_KEY)
		return notfound;

	switch(c[0]) {
	case 'y': case 'Y': case '1': case 't': case 'T':
		ret = 1;
		break;
	case 'n': case 'N': case '0': case 'f': case 'F':
		ret = 0;
		break;
	default:
		ret = notfound;
		break;
	}

	return ret;
}

/**
 * @brief    Get the string associated to a key, convert to a double
 * @param    d Dictionary to search
 * @param    key Key string to look for
 * @param    notfound Value to return in case of error
 * @return   double
 *
 * This function queries a dictionary for a key. A key as read from an
 * ini file is given as "section:key". If the key cannot be found,
 * the notfound value is returned.
 */
double inifile_GetDouble(tDictionary *d, char *key, double notfound)
{
	char *str;

	str = inifile_GetString(d, key, INI_INVALID_KEY);

	if (str == INI_INVALID_KEY)
		return notfound;

	return atof(str);
}


/**
 * @brief    Get the string associated to a key, convert to an int
 * @param    d Dictionary to search
 * @param    key Key string to look for
 * @param    notfound Value to return in case of error
 * @return   integer
 *
 * This function queries a dictionary for a key. A key as read from an
 * ini file is given as "section:key". If the key cannot be found,
 * the notfound value is returned.
 *
 * Supported values for integers include the usual C notation
 * so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
 * are supported. Examples:
 *
 * - "42"      ->  42
 * - "042"     ->  34 (octal -> decimal)
 * - "0x42"    ->  66 (hexa  -> decimal)
 *
 * Warning: the conversion may overflow in various ways. Conversion is
 * totally outsourced to strtol(), see the associated man page for overflow
 * handling.
 *
 * Credits: Thanks to A. Becker for suggesting strtol()
 */
int inifile_GetInteger(tDictionary *d, const char *key, int notfound)
{
	char *str;

	str = inifile_GetString(d, key, INI_INVALID_KEY);

	if (str == INI_INVALID_KEY)
		return notfound;

	return (int) strtol(str, NULL, 10);
}



/* The remaining public functions are documented in ciniparser.h */
/**
 * @brief    Get number of sections in a dictionary
 * @param    d   Dictionary to examine
 * @return   int Number of sections found in dictionary, -1 on error
 *
 * This function returns the number of sections found in a dictionary.
 * The test to recognize sections is done on the string stored in the
 * dictionary: a section name is given as "section" whereas a key is
 * stored as "section:key", thus the test looks for entries that do not
 * contain a colon.
 *
 * This clearly fails in the case a section name contains a colon, but
 * this should simply be avoided.
 */
int ciniparser_getnsec(tDictionary *d)
{
	int i;
	int nsec;

	if (d == NULL)
		return -1;

	nsec = 0;
	for (i = 0; i < d->size; i++) {
		if (d->key[i] == NULL)
			continue;
		if (strchr(d->key[i], ':') == NULL) {
			nsec ++;
		}
	}

	return nsec;
}

/**
 * @brief    Get name for section n in a dictionary.
 * @param    d   Dictionary to examine
 * @param    n   Section number (from 0 to nsec-1).
 * @return   Pointer to char string, NULL on error
 *
 * This function locates the n-th section in a dictionary and returns
 * its name as a pointer to a string statically allocated inside the
 * dictionary. Do not free or modify the returned string!
 */
char *ciniparser_getsecname(tDictionary *d, int n)
{
	int i;
	int foundsec;

	if (d == NULL || n < 0)
		return NULL;

	if (n == 0)
		n ++;

	foundsec = 0;

	for (i = 0; i < d->size; i++) {
		if (d->key[i] == NULL)
			continue;
		if (! strchr(d->key[i], ':')) {
			foundsec++;
			if (foundsec >= n)
				break;
		}
	}

	if (foundsec == n) {
		return d->key[i];
	}

	return (char *) NULL;
}

/**
 * @brief    Dump a dictionary to an opened file pointer.
 * @param    d   Dictionary to dump.
 * @param    f   Opened file pointer to dump to.
 * @return   void
 *
 * This function prints out the contents of a dictionary, one element by
 * line, onto the provided file pointer. It is OK to specify @c stderr
 * or @c stdout as output files. This function is meant for debugging
 * purposes mostly.
 */
void ciniparser_dump(tDictionary *d, FILE *f)
{
	int i;

	if (d == NULL || f == NULL)
		return;

	for (i = 0; i < d->size; i++) {
		if (d->key[i] == NULL)
			continue;
		if (d->val[i] != NULL) {
			fprintf(f, "[%s]=[%s]\n", d->key[i], d->val[i]);
		} else {
			fprintf(f, "[%s]=UNDEF\n", d->key[i]);
		}
	}

	return;
}

/**
 * @brief    Save a dictionary to a loadable ini file
 * @param    d   Dictionary to dump
 * @param    f   Opened file pointer to dump to
 * @return   void
 *
 * This function dumps a given dictionary into a loadable ini file.
 * It is Ok to specify @c stderr or @c stdout as output files.
 */
void ciniparser_dump_ini(tDictionary *d, FILE *f)
{
	int i, j;
	char keym[ASCIILINESZ+1];
	int nsec;
	char *secname;
	int seclen;

	if (d == NULL || f == NULL)
		return;

	memset(keym, 0, ASCIILINESZ + 1);

	nsec = ciniparser_getnsec(d);
	if (nsec < 1) {
		/* No section in file: dump all keys as they are */
		for (i = 0; i < d->size; i++) {
			if (d->key[i] == NULL)
				continue;
			fprintf(f, "%s = %s\n", d->key[i], d->val[i]);
		}
		return;
	}

	for (i = 0; i < nsec; i++) {
		secname = ciniparser_getsecname(d, i);
		seclen  = (int)strlen(secname);
		fprintf(f, "\n[%s]\n", secname);
		snprintf(keym, ASCIILINESZ + 1, "%s:", secname);
		for (j = 0; j < d->size; j++) {
			if (d->key[j] == NULL)
				continue;
			if (!strncmp(d->key[j], keym, seclen+1)) {
				fprintf(f, "%-30s = %s\n",
					d->key[j]+seclen+1,
					d->val[j] ? d->val[j] : "");
			}
		}
	}
	fprintf(f, "\n");

	return;
}


/**
 * @brief    Finds out if a given entry exists in a dictionary
 * @param    ini     Dictionary to search
 * @param    entry   Name of the entry to look for
 * @return   integer 1 if entry exists, 0 otherwise
 *
 * Finds out if a given entry exists in the dictionary. Since sections
 * are stored as keys with NULL associated values, this is the only way
 * of querying for the presence of sections in a dictionary.
 */
int ciniparser_find_entry(tDictionary *ini, char *entry)
{
	int found = 0;

	if (inifile_GetString(ini, entry, INI_INVALID_KEY) != INI_INVALID_KEY) {
		found = 1;
	}

	return found;
}

/**
 * @brief Set an item in the dictionary
 * @param d      Dictionary object created by inifile_Open()
 * @param entry  Entry in the dictionary to manipulate
 * @param val    Value to assign to the entry
 * @return       0 on success, -1 on error
 *
 * Remember that string values are converted by inifile_GetBoolean(),
 * inifile_GetDouble(), etc. It is also OK to set an entry to NULL.
 */
int ciniparser_set(tDictionary *d, char *entry, char *val)
{
	return dictionary_set(d, strlwc(entry), val);
}

/**
 * @brief    Delete an entry in a dictionary
 * @param    ini     Dictionary to modify
 * @param    entry   Entry to delete (entry name)
 * @return   void
 *
 * If the given entry can be found, it is deleted from the dictionary.
 */
void ciniparser_unset(tDictionary *ini, char *entry)
{
	dictionary_unset(ini, strlwc(entry));
}

/**
 * @brief    Free all memory associated to an ini dictionary
 * @param    d Dictionary to free
 * @return   void
 *
 * Free all memory associated to an ini dictionary.
 * It is mandatory to call this function before the dictionary object
 * gets out of the current context.
 */
void inifile_Close(tDictionary *d)
{
	dictionary_del(d);
}

/**
 * @brief Double the allocated size associated to a pointer
 * @param size the current allocated size
 * @return re-allocated pointer on success, NULL on failure
 */
static void *mem_double(void *ptr, int size)
{
	void *newptr;

	newptr = calloc(2 * size, 1);
	if (newptr == NULL) {
		return NULL;
	}
	memcpy(newptr, ptr, size);
	free(ptr);
	return newptr;
}

/* The remaining exposed functions are documented in dictionary.h */
/** @addtogroup ciniparser
 * @{
 */
/**
 * @file    dictionary.h
 * @author  N. Devillard
 * @date    Sep 2007
 * @version $Revision: 1.12 $
 * @brief   Implements a dictionary for string variables.
 *
 * This module implements a simple dictionary object, i.e. a list
 * of string/string associations. This object is useful to store e.g.
 * information retrieved from a configuration file (ini files).
 */

/**
 * @brief Compute the hash key for a string.
 * @param key Character string to use for key.
 * @return 1 unsigned int on at least 32 bits.
 *
 * This hash function has been taken from an Article in Dr Dobbs Journal.
 * This is normally a collision-free function, distributing keys evenly.
 * The key is stored anyway in the struct so that collision can be avoided
 * by comparing the key itself in last resort.
 */
unsigned dictionary_hash(char *key)
{
	int len;
	unsigned hash;
	int i;

	len = strlen(key);
	for (hash = 0, i = 0; i < len; i++) {
		hash += (unsigned) key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

/**
 * @brief Create a new dictionary object.
 * @param size Optional initial size of the dictionary.
 * @return allocated dictionary object on success, NULL on failure
 *
 * This function allocates a new dictionary object of given size and returns
 * it. If you do not know in advance (roughly) the number of entries in the
 * dictionary, give size=0.
 */
tDictionary *dictionary_new(int size)
{
	tDictionary *d;

	/* If no size was specified, allocate space for DICTMINSZ */
	if (size<DICTMINSZ) size=DICTMINSZ;

	if (!(d = (tDictionary *) calloc(1, sizeof(tDictionary)))) {
		return NULL;
	}
	d->size = size;
	d->val  = (char **) calloc(size, sizeof(char *));
	d->key  = (char **) calloc(size, sizeof(char *));
	d->hash = (unsigned int *) calloc(size, sizeof(unsigned));
	return d;
}

/**
 * @brief Delete a dictionary object
 * @param d dictionary object to deallocate.
 * @return void
 *
 * Deallocate a dictionary object and all memory associated to it.
 */
void dictionary_del(tDictionary *d)
{
	int i;

	if (d == NULL)
		return;
	for (i = 0; i < d->size; i++) {
		if (d->key[i] != NULL)
			free(d->key[i]);
		if (d->val[i] != NULL)
			free(d->val[i]);
	}
	free(d->val);
	free(d->key);
	free(d->hash);
	free(d);
	return;
}

/**
 * @brief Get a value from a dictionary.
 * @param d dictionary object to search.
 * @param key Key to look for in the dictionary.
 * @param def Default value to return if key not found.
 * @return 1 pointer to internally allocated character string.
 *
 * This function locates a key in a dictionary and returns a pointer to its
 * value, or the passed 'def' pointer if no such key can be found in
 * dictionary. The returned character pointer points to data internal to the
 * dictionary object, you should not try to free it or modify it.
 */
char *dictionary_get(tDictionary *d, char *key, char *def)
{
	unsigned hash;
	int i;

	hash = dictionary_hash(key);
	for (i=0; i < d->size; i++) {
		if (d->key[i] == NULL)
			continue;
		/* Compare hash */
		if (hash == d->hash[i]) {
			/* Compare string, to avoid hash collisions */
			if (!strcmp(key, d->key[i])) {
				return d->val[i];
			}
		}
	}
	return def;
}

/**
 * @brief Set a value in a dictionary.
 * @param d dictionary object to modify.
 * @param key Key to modify or add.
 * @param val Value to add.
 * @return int 0 if Ok, anything else otherwise
 *
 * If the given key is found in the dictionary, the associated value is
 * replaced by the provided one. If the key cannot be found in the
 * dictionary, it is added to it.
 *
 * It is Ok to provide a NULL value for val, but NULL values for the dictionary
 * or the key are considered as errors: the function will return immediately
 * in such a case.
 *
 * Notice that if you dictionary_set a variable to NULL, a call to
 * dictionary_get will return a NULL value: the variable will be found, and
 * its value (NULL) is returned. In other words, setting the variable
 * content to NULL is equivalent to deleting the variable from the
 * dictionary. It is not possible (in this implementation) to have a key in
 * the dictionary without value.
 *
 * This function returns non-zero in case of failure.
 */
int dictionary_set(tDictionary *d, char *key, char *val)
{
	int i;
	unsigned hash;

	if (d==NULL || key==NULL)
		return -1;

	/* Compute hash for this key */
	hash = dictionary_hash(key);
	/* Find if value is already in dictionary */
	if (d->n > 0) {
		for (i = 0; i < d->size; i++) {
			if (d->key[i] == NULL)
				continue;
			/* Same hash value */
			if (hash == d->hash[i]) {
				/* Same key */
				if (!strcmp(key, d->key[i])) {
					/* Found a value: modify and return */
					if (d->val[i] != NULL)
						free(d->val[i]);
					d->val[i] = val ? strdup(val) : NULL;
					/* Value has been modified: return */
					return 0;
				}
			}
		}
	}

	/* Add a new value
	 * See if dictionary needs to grow */
	if (d->n == d->size) {
		/* Reached maximum size: reallocate dictionary */
		d->val  = (char **) mem_double(d->val, d->size * sizeof(char *));
		d->key  = (char **) mem_double(d->key, d->size * sizeof(char *));
		d->hash = (unsigned int *)
			mem_double(d->hash, d->size * sizeof(unsigned));
		if ((d->val == NULL) || (d->key == NULL) || (d->hash == NULL))
			/* Cannot grow dictionary */
			return -1;
		/* Double size */
		d->size *= 2;
	}

	/* Insert key in the first empty slot */
	for (i = 0; i < d->size; i++) {
		if (d->key[i] == NULL) {
			/* Add key here */
			break;
		}
	}
	/* Copy key */
	d->key[i] = strdup(key);
	d->val[i] = val ? strdup(val) : NULL;
	d->hash[i] = hash;
	d->n ++;
	return 0;
}


/**
 * @brief Delete a key in a dictionary
 * @param d dictionary object to modify.
 * @param key Key to remove.
 * @return void
 *
 * This function deletes a key in a dictionary. Nothing is done if the
 * key cannot be found.
 */
void dictionary_unset(tDictionary *d, char *key)
{
	unsigned hash;
	int i;

	if (key == NULL)
		return;

	hash = dictionary_hash(key);
	for (i = 0; i < d->size; i++) {
		if (d->key[i] == NULL)
			continue;
		/* Compare hash */
		if (hash == d->hash[i]) {
			/* Compare string, to avoid hash collisions */
			if (!strcmp(key, d->key[i])) {
				/* Found key */
				break;
			}
		}
	}
	if (i >= d->size)
		/* Key not found */
		return;

	free(d->key[i]);
	d->key[i] = NULL;
	if (d->val[i]!=NULL) {
		free(d->val[i]);
		d->val[i] = NULL;
	}
	d->hash[i] = 0;
	d->n --;
	return;
}

/**
 * @brief Dump a dictionary to an opened file pointer.
 * @param d Dictionary to dump
 * @param out Opened file pointer
 * @return void
 *
 * Dumps a dictionary onto an opened file pointer. Key pairs are printed out
 * as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
 * output file pointers.
 */
void dictionary_dump(tDictionary *d, FILE *out)
{
	int i;

	if (d == NULL || out == NULL)
		return;
	if (d->n < 1) {
		fprintf(out, "empty dictionary\n");
		return;
	}
	for (i = 0; i < d->size; i++) {
		if (d->key[i]) {
			fprintf(out, "%20s\t[%s]\n",
				d->key[i],
				d->val[i] ? d->val[i] : "UNDEF");
		}
	}
	return;
}
