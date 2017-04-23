/*
 Copyright (c) Jeremy Nixon 2015
 All rights reserved.

 Developed by: Jeremy Nixon
 https://www.gasfoodnolodging.com
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 with the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is furnished
 to do so, subject to the following conditions:

 Redistributions of source code must retain the above copyright notice, this list
 of conditions and the following disclaimers.

 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimers in the documentation and/or
 other materials provided with the distribution.

 Neither the name of the developer nor the names of its contributors may be used
 to endorse or promote products derived from this Software without specific
 prior written permission.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH
 THE SOFTWARE.
*/

/*
 Implements a string buffer that can be appended endlessly and will
 reallocate its memory as needed to fit the newly appended strings.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "astring.h"

void astring_create(astring **p, unsigned long sz)
{
    *p = calloc(1,sizeof(astring));

    char *s = NULL;
    if (sz > 0) {
        s = calloc(sz,sizeof(char));
    }
    (*p)->str = s;
    (*p)->msize = sz;
    (*p)->add_size = 1024;
    (*p)->debug_logger = NULL;
    (*p)->error_logger = NULL;
    (*p)->log_arg = NULL;
}

void astring_destroy(astring *p)
{
    char *b = p->str;
    if (b != NULL) {
        free(b);
        b = NULL;
    }
    p->msize = 0;
    free(p);
}

void astring_debuglog(astring *p, const char *fmt, ...)
{
    if (p->debug_logger == NULL) return;

    va_list args;
    va_start(args, fmt);

    p->debug_logger(p->log_arg, fmt, args);

    va_end(args);
}

void astring_errlog(astring *p, const char *fmt, ...)
{
    if (p->error_logger == NULL) return;

    va_list args;
    va_start(args, fmt);

    p->error_logger(p->log_arg, fmt, args);

    va_end(args);
}

unsigned long astring_strlen(astring *p)
{
    char *b = p->str;
    if (b == NULL) return 0;
    return strlen(b);
}

/** Reallocate the string buffer if it's too short for what we're adding.
    @param p An astring
    @param curlen Current length of the string
    @param len Length of what we're adding to the string
    @return Number of bytes (chars) added to buffer, -1 if memory allocation failed
 */
static long astring_realloc(astring *p, unsigned long curlen, unsigned long len)
{
    char *b = p->str;
    if (curlen + len + 1 > p->msize) {
        size_t addsz;
        if (p->add_size > 0) addsz = p->add_size;
        else addsz = 1024;
        if (addsz < len) addsz = len; // we are overwriting the old null so we don't need to add 1
        if (curlen == 0) addsz++;     // ...unless there's nothing in the string yet.
        astring_debuglog(p, "reallocating %d bytes for astring\n", (int)addsz);

        char *new = realloc(b, p->msize + addsz);
        if (new == NULL) {
            astring_errlog(p, "failed allocating memory\n");
            return -1;
        }
        p->msize = p->msize + addsz;
        p->str = new;
        b = p->str;
        astring_debuglog(p, "new msize is: %ld\n", p->msize);
        return addsz;
    }
    return 0;
}

unsigned long astring_appendf(astring *p, unsigned long len, const char *fmt, ...)
{
    if (strlen(fmt) > len) len = strlen(fmt);

    char *b = p->str;
    unsigned long curlen = 0;
    if (b == NULL) {
        unsigned long sz = 128;
        if (sz < len + 1) sz = len + 1;
        b = calloc(sz,sizeof(char));
        if (b == NULL) {
            astring_errlog(p, "Failed allocating memory.\n");
            return 0;
        }
        p->msize = sz;
    } else {
        curlen = strlen(b);
    }

    if (curlen + len + 1 > p->msize) {
        if (astring_realloc(p, curlen, len) < 0) return 0;
        b = p->str;
    }

    va_list args;
    va_start(args, fmt);

    int c = vsnprintf(b+curlen, len+1, fmt, args);
    curlen += c;
    if (c > len) {
        astring_errlog(p, "string longer than anticipated (%d > %d): %s\n", c, (int)len, fmt);
    }

    va_end(args);

    return curlen;
}

unsigned long astring_appendf_safe(astring *p, const char *fmt, ...)
{
    char *newstr = NULL;
    unsigned long len = 0;

    va_list args;
    va_start(args, fmt);

    len = vasprintf(&newstr, fmt, args);
    va_end(args);

    if (newstr == NULL) {
        astring_errlog(p, "Failed allocating memory\n");
        return 0;
    }

    char *b = p->str;
    unsigned long curlen = 0;
    if (b == NULL) {
        unsigned long sz = len + 1;
        if (sz < 128) sz = 128;
        b = calloc(sz,sizeof(char));
        if (b == NULL) {
            astring_errlog(p, "Failed allocating memory.\n");
            return 0;
        }
        p->msize = sz;
    } else {
        curlen = strlen(b);
    }

    if (curlen + len + 1 > p->msize) {
        if (astring_realloc(p, curlen, len) < 0) {
            free(newstr);
            return 0;
        }
        b = p->str;
    }

    strncat(b, newstr, len);
    free(newstr);

    return curlen + len;
}

unsigned long astring_append(astring *p, const char *str)
{
    unsigned long len = strlen(str);

    char *b = p->str;
    unsigned long curlen = 0;
    if (b == NULL) {
        unsigned long sz = len + 1;
        if (sz < 128) sz = 128;
        b = calloc(sz,sizeof(char));
        if (b == NULL) {
            astring_errlog(p, "Failed allocating memory.\n");
            return 0;
        }
        p->msize = sz;
    } else {
        curlen = strlen(b);
    }

    if (curlen + len + 1 > p->msize) {
        if (astring_realloc(p, curlen, len) < 0) return 0;
        b = p->str;
    }

    strncat(b, str, len);
    return curlen + len;
}

char *astring_string(astring *p)
{
    return p->str;
}
