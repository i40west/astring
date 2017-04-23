
#ifndef __astring__
#define __astring__

/*! @file astring.h
    Implements a string buffer that can be appended endlessly and will
    reallocate its memory as needed to fit the newly appended strings.
 
    @author Jeremy Nixon

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

/*! Struct representing an astring. */
typedef struct astring {
    char *str;              //!< The string
    unsigned long msize;    //!< The current memory alocation size
    size_t add_size;        //!< Increments by which to add to the memory alloction (default 1024)
    void (*debug_logger)(void *arg, const char *fmt, va_list args); //!< Logging function for debug messages
    void (*error_logger)(void *arg, const char *fmt, va_list args); //!< Logging function for error messages
    void *log_arg;          //!< Arbitrary pointer that will be passed to logging callbacks
} astring;

/*! Create an astring.
    @param p Pointer to an astring pointer
    @param sz Initial size of memory allocation

    @warning You must call astring_destroy() on the created object. */
void astring_create(astring **p, unsigned long sz);

/*! Destroy an astring, deallocating its memory.
    @param p Pointer to an allocated astring */
void astring_destroy(astring *p);

/*! Return the length of the string.
    @param p Pointer to an astring
    @return Length of the string */
unsigned long astring_strlen(astring *p);

/*! Append a printf-style format to the string.
    @param p Pointer to an astring
    @param len Expected length of the formatted string
    @param fmt Printf-style format
    @return New length of the string
 
    @warning len must not be less than the length of the expanded fmt string */
unsigned long astring_appendf(astring *p, unsigned long len, const char *fmt, ...);

/*! Append a printf-style format to the string safely.
    This requires an extra allocation and copy to determine the
    length of the formatted string.
    @param p Pointer to an astring
    @param fmt Printf-style format
    @return New length of the string
 */
unsigned long astring_appendf_safe(astring *p, const char *fmt, ...);

/*! Append a string to the string.
    @param p Pointer to an astring
    @param str String to append
    @return New length of the string */
unsigned long astring_append(astring *p, const char *str);

/*! Return the string itself.
    @param p Pointer to an astring
    @return The string from the astring (char *) */
char *astring_string(astring *p);

/*! Send a printf-style string to the debug log function.
    @param p Pointer to an astring
    @param fmt Printf-style string */
void astring_debuglog(astring *p, const char *fmt, ...);

/*! Send a printf-style string to the error log function.
    @param p Pointer to an astring
    @param fmt Printf-style string */
void astring_errlog(astring *p, const char *fmt, ...);

#endif /* defined(__astring__) */
