﻿/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

/* AIUI_cJSON */
/* JSON parser in C. */

#pragma GCC visibility push(default)
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
//#include "app_global_defines.h"
#pragma GCC visibility pop

#include "aiui/aiui_cJSON.h"

/* define our own boolean type */
#define true ((AIUI_cJSON_bool)1)
#define false ((AIUI_cJSON_bool)0)

static const unsigned char *global_ep = NULL;
static void* k_malloc(size_t size);
static void *k_realloc(void *pointer, size_t size);
static void k_free(void *pointer);
AIUI_CJSON_PUBLIC(const char *) AIUI_cJSON_GetErrorPtr(void)
{
    return (const char*) global_ep;
}

/* This is a safeguard to prevent copy-pasters from using incompatible C and header files */
#if (AIUI_CJSON_VERSION_MAJOR != 1) || (AIUI_CJSON_VERSION_MINOR != 4) || (AIUI_CJSON_VERSION_PATCH != 5)
    #error AIUI_cJSON.h and AIUI_cJSON.c have different versions. Make sure that both have the same.
#endif

AIUI_CJSON_PUBLIC(const char*) AIUI_cJSON_Version(void)
{
    static char version[15];
    sprintf(version, "%i.%i.%i", AIUI_CJSON_VERSION_MAJOR, AIUI_CJSON_VERSION_MINOR, AIUI_CJSON_VERSION_PATCH);

    return version;
}

/* case insensitive strcmp */
static int AIUI_cJSON_strcasecmp(const unsigned char *s1, const unsigned char *s2)
{
    if (!s1)
    {
        return (s1 == s2) ? 0 : 1; /* both NULL? */
    }
    if (!s2)
    {
        return 1;
    }
    for(; tolower(*s1) == tolower(*s2); (void)++s1, ++s2)
    {
        if (*s1 == '\0')
        {
            return 0;
        }
    }

    return tolower(*s1) - tolower(*s2);
}

typedef struct internal_hooks
{
    void *(*allocate)(size_t size);
    void (*deallocate)(void *pointer);
    void *(*reallocate)(void *pointer, size_t size);
} internal_hooks;

static internal_hooks global_hooks = { k_malloc, k_free, k_realloc };

static unsigned char* AIUI_cJSON_strdup(const unsigned char* str, const internal_hooks * const hooks)
{
    size_t len = 0;
    unsigned char *copy = NULL;

    if (str == NULL)
    {
        return NULL;
    }

    len = strlen((const char*)str) + 1;
    if (!(copy = (unsigned char*)hooks->allocate(len)))
    {
        return NULL;
    }
    memcpy(copy, str, len);

    return copy;
}

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_InitHooks(AIUI_cJSON_Hooks* hooks)
{
    if (hooks == NULL)
    {
        /* Reset hooks */
        global_hooks.allocate = malloc;
        global_hooks.deallocate = free;
        global_hooks.reallocate = realloc;
        return;
    }

    global_hooks.allocate = malloc;
    if (hooks->malloc_fn != NULL)
    {
        global_hooks.allocate = hooks->malloc_fn;
    }

    global_hooks.deallocate = free;
    if (hooks->free_fn != NULL)
    {
        global_hooks.deallocate = hooks->free_fn;
    }

    /* use realloc only if both free and malloc are used */
    global_hooks.reallocate = NULL;
    if ((global_hooks.allocate == malloc) && (global_hooks.deallocate == free))
    {
        global_hooks.reallocate = realloc;
    }
}

/* Internal constructor. */
static AIUI_cJSON *AIUI_cJSON_New_Item(const internal_hooks * const hooks)
{
    AIUI_cJSON* node = (AIUI_cJSON*)hooks->allocate(sizeof(AIUI_cJSON));
    if (node)
    {
        memset(node, '\0', sizeof(AIUI_cJSON));
    }

    return node;
}

/*�Զ���ֲ�����*/
static void* k_malloc(size_t size)
{
	return malloc(size);
}


 static void *k_realloc(void *pointer, size_t size)
 {
	return realloc(pointer, size);
 }

 static void k_free(void *pointer)
{
	free(pointer);
}
 

/* Delete a AIUI_cJSON structure. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_Delete(AIUI_cJSON *c)
{
    AIUI_cJSON *next = NULL;
    while (c)
    {
        next = c->next;
        if (!(c->type & AIUI_cJSON_IsReference) && c->child)
        {
            AIUI_cJSON_Delete(c->child);
        }
        if (!(c->type & AIUI_cJSON_IsReference) && c->valuestring)
        {
            global_hooks.deallocate(c->valuestring);
        }
        if (!(c->type & AIUI_cJSON_StringIsConst) && c->string)
        {
            global_hooks.deallocate(c->string);
        }
        global_hooks.deallocate(c);
        c = next;
    }
}

/* Parse the input text to generate a number, and populate the result into item. */
static const unsigned char *parse_number(AIUI_cJSON * const item, const unsigned char * const input)
{
    double number = 0;
    unsigned char *after_end = NULL;

    if (input == NULL)
    {
        return NULL;
    }

    number = strtod((const char*)input, (char**)&after_end);
    if (input == after_end)
    {
        return NULL; /* parse_error */
    }

    item->valuedouble = number;

    /* use saturation in case of overflow */
    if (number >= INT_MAX)
    {
        item->valueint = INT_MAX;
    }
    else if (number <= INT_MIN)
    {
        item->valueint = INT_MIN;
    }
    else
    {
        item->valueint = (int)number;
    }

    item->type = AIUI_cJSON_Number;

    return after_end;
}

/* don't ask me, but the original AIUI_cJSON_SetNumberValue returns an integer or double */
AIUI_CJSON_PUBLIC(double) AIUI_cJSON_SetNumberHelper(AIUI_cJSON *object, double number)
{
    if (number >= INT_MAX)
    {
        object->valueint = INT_MAX;
    }
    else if (number <= INT_MIN)
    {
        object->valueint = INT_MIN;
    }
    else
    {
        object->valueint = (int)number;
    }

    return object->valuedouble = number;
}

typedef struct
{
    unsigned char *buffer;
    size_t length;
    size_t offset;
    AIUI_cJSON_bool noalloc;
} printbuffer;

/* realloc printbuffer if necessary to have at least "needed" bytes more */
static unsigned char* ensure(printbuffer * const p, size_t needed, const internal_hooks * const hooks)
{
    unsigned char *newbuffer = NULL;
    size_t newsize = 0;

    if ((p == NULL) || (p->buffer == NULL))
    {
        return NULL;
    }

    if ((p->length > 0) && (p->offset >= p->length))
    {
        /* make sure that offset is valid */
        return NULL;
    }

    if (needed > INT_MAX)
    {
        /* sizes bigger than INT_MAX are currently not supported */
        return NULL;
    }

    needed += p->offset + 1;
    if (needed <= p->length)
    {
        return p->buffer + p->offset;
    }

    if (p->noalloc) {
        return NULL;
    }

    /* calculate new buffer size */
    if (newsize > (INT_MAX / 2))
    {
        /* overflow of int, use INT_MAX if possible */
        if (needed <= INT_MAX)
        {
            newsize = INT_MAX;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        newsize = needed * 2;
    }

    if (hooks->reallocate != NULL)
    {
        /* reallocate with realloc if available */
        newbuffer = (unsigned char*)hooks->reallocate(p->buffer, newsize);
    }
    else
    {
        /* otherwise reallocate manually */
        newbuffer = (unsigned char*)hooks->allocate(newsize);
        if (!newbuffer)
        {
            hooks->deallocate(p->buffer);
            p->length = 0;
            p->buffer = NULL;

            return NULL;
        }
        if (newbuffer)
        {
            memcpy(newbuffer, p->buffer, p->offset + 1);
        }
        hooks->deallocate(p->buffer);
    }
    p->length = newsize;
    p->buffer = newbuffer;

    return newbuffer + p->offset;
}

/* calculate the new length of the string in a printbuffer and update the offset */
static void update_offset(printbuffer * const buffer)
{
    const unsigned char *buffer_pointer = NULL;
    if ((buffer == NULL) || (buffer->buffer == NULL))
    {
        return;
    }
    buffer_pointer = buffer->buffer + buffer->offset;

    buffer->offset += strlen((const char*)buffer_pointer);
}

/* Removes trailing zeroes from the end of a printed number */
static AIUI_cJSON_bool trim_trailing_zeroes(printbuffer * const buffer)
{
    size_t offset = 0;
    unsigned char *content = NULL;

    if ((buffer == NULL) || (buffer->buffer == NULL) || (buffer->offset < 1))
    {
        return false;
    }

    offset = buffer->offset - 1;
    content = buffer->buffer;

    while ((offset > 0) && (content[offset] == '0'))
    {
        offset--;
    }
    if ((offset > 0) && (content[offset] == '.'))
    {
        offset--;
    }

    offset++;
    content[offset] = '\0';

    buffer->offset = offset;

    return true;
}

/* Render the number nicely from the given item into a string. */
static AIUI_cJSON_bool print_number(const AIUI_cJSON * const item, printbuffer * const output_buffer, const internal_hooks * const hooks)
{
    unsigned char *output_pointer = NULL;
    double d = item->valuedouble;
    int length = 0;
    AIUI_cJSON_bool trim_zeroes = true; /* should at the end be removed? */

    if (output_buffer == NULL)
    {
        return false;
    }

    /* This is a nice tradeoff. */
    output_pointer = ensure(output_buffer, 64, hooks);
    if (output_pointer == NULL)
    {
        return false;
    }

    /* This checks for NaN and Infinity */
    if ((d * 0) != 0)
    {
        length = sprintf((char*)output_pointer, "null");
    }
    else if ((fabs(floor(d) - d) <= DBL_EPSILON) && (fabs(d) < 1.0e60))
    {
        /* integer */
        length = sprintf((char*)output_pointer, "%.0f", d);
        trim_zeroes = false; /* don't remove zeroes for "big integers" */
    }
    else if ((fabs(d) < 1.0e-6) || (fabs(d) > 1.0e9))
    {
        length = sprintf((char*)output_pointer, "%e", d);
        trim_zeroes = false; /* don't remove zeroes in engineering notation */
    }
    else
    {
        length = sprintf((char*)output_pointer, "%f", d);
    }

    /* sprintf failed */
    if (length < 0)
    {
        return false;
    }

    output_buffer->offset += (size_t)length;

    if (trim_zeroes)
    {
        return trim_trailing_zeroes(output_buffer);
    }

    return true;
}

/* parse 4 digit hexadecimal number */
static unsigned parse_hex4(const unsigned char * const input)
{
    unsigned int h = 0;
    size_t i = 0;

    for (i = 0; i < 4; i++)
    {
        /* parse digit */
        if ((input[i] >= '0') && (input[i] <= '9'))
        {
            h += (unsigned int) input[i] - '0';
        }
        else if ((input[i] >= 'A') && (input[i] <= 'F'))
        {
            h += (unsigned int) 10 + input[i] - 'A';
        }
        else if ((input[i] >= 'a') && (input[i] <= 'f'))
        {
            h += (unsigned int) 10 + input[i] - 'a';
        }
        else /* invalid */
        {
            return 0;
        }

        if (i < 3)
        {
            /* shift left to make place for the next nibble */
            h = h << 4;
        }
    }

    return h;
}

/* converts a UTF-16 literal to UTF-8
 * A literal can be one or two sequences of the form \uXXXX */
static unsigned char utf16_literal_to_utf8(const unsigned char * const input_pointer, const unsigned char * const input_end, unsigned char **output_pointer, const unsigned char **error_pointer)
{
    long unsigned int codepoint = 0;
    unsigned int first_code = 0;
    const unsigned char *first_sequence = input_pointer;
    unsigned char utf8_length = 0;
    unsigned char utf8_position = 0;
    unsigned char sequence_length = 0;
    unsigned char first_byte_mark = 0;

    if ((input_end - first_sequence) < 6)
    {
        /* input ends unexpectedly */
        *error_pointer = first_sequence;
        goto fail;
    }

    /* get the first utf16 sequence */
    first_code = parse_hex4(first_sequence + 2);

    /* check that the code is valid */
    if (((first_code >= 0xDC00) && (first_code <= 0xDFFF)) || (first_code == 0))
    {
        *error_pointer = first_sequence;
        goto fail;
    }

    /* UTF16 surrogate pair */
    if ((first_code >= 0xD800) && (first_code <= 0xDBFF))
    {
        const unsigned char *second_sequence = first_sequence + 6;
        unsigned int second_code = 0;
        sequence_length = 12; /* \uXXXX\uXXXX */

        if ((input_end - second_sequence) < 6)
        {
            /* input ends unexpectedly */
            *error_pointer = first_sequence;
            goto fail;
        }

        if ((second_sequence[0] != '\\') || (second_sequence[1] != 'u'))
        {
            /* missing second half of the surrogate pair */
            *error_pointer = first_sequence;
            goto fail;
        }

        /* get the second utf16 sequence */
        second_code = parse_hex4(second_sequence + 2);
        /* check that the code is valid */
        if ((second_code < 0xDC00) || (second_code > 0xDFFF))
        {
            /* invalid second half of the surrogate pair */
            *error_pointer = first_sequence;
            goto fail;
        }


        /* calculate the unicode codepoint from the surrogate pair */
        codepoint = 0x10000 + (((first_code & 0x3FF) << 10) | (second_code & 0x3FF));
    }
    else
    {
        sequence_length = 6; /* \uXXXX */
        codepoint = first_code;
    }

    /* encode as UTF-8
     * takes at maximum 4 bytes to encode:
     * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (codepoint < 0x80)
    {
        /* normal ascii, encoding 0xxxxxxx */
        utf8_length = 1;
    }
    else if (codepoint < 0x800)
    {
        /* two bytes, encoding 110xxxxx 10xxxxxx */
        utf8_length = 2;
        first_byte_mark = 0xC0; /* 11000000 */
    }
    else if (codepoint < 0x10000)
    {
        /* three bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx */
        utf8_length = 3;
        first_byte_mark = 0xE0; /* 11100000 */
    }
    else if (codepoint <= 0x10FFFF)
    {
        /* four bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        utf8_length = 4;
        first_byte_mark = 0xF0; /* 11110000 */
    }
    else
    {
        /* invalid unicode codepoint */
        *error_pointer = first_sequence;
        goto fail;
    }

    /* encode as utf8 */
    for (utf8_position = (unsigned char)(utf8_length - 1); utf8_position > 0; utf8_position--)
    {
        /* 10xxxxxx */
        (*output_pointer)[utf8_position] = (unsigned char)((codepoint | 0x80) & 0xBF);
        codepoint >>= 6;
    }
    /* encode first byte */
    if (utf8_length > 1)
    {
        (*output_pointer)[0] = (unsigned char)((codepoint | first_byte_mark) & 0xFF);
    }
    else
    {
        (*output_pointer)[0] = (unsigned char)(codepoint & 0x7F);
    }

    *output_pointer += utf8_length;

    return sequence_length;

fail:
    return 0;
}

/* Parse the input text into an unescaped cinput, and populate item. */
static const unsigned char *parse_string(AIUI_cJSON * const item, const unsigned char * const input, const unsigned char ** const error_pointer, const internal_hooks * const hooks)
{
    const unsigned char *input_pointer = input + 1;
    const unsigned char *input_end = input + 1;
    unsigned char *output_pointer = NULL;
    unsigned char *output = NULL;

    /* not a string */
    if (*input != '\"')
    {
        *error_pointer = input;
        goto fail;
    }

    {
        /* calculate approximate size of the output (overestimate) */
        size_t allocation_length = 0;
        size_t skipped_bytes = 0;
        while ((*input_end != '\"') && (*input_end != '\0'))
        {
            /* is escape sequence */
            if (input_end[0] == '\\')
            {
                if (input_end[1] == '\0')
                {
                    /* prevent buffer overflow when last input character is a backslash */
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (*input_end == '\0')
        {
            goto fail; /* string ended unexpectedly */
        }

        /* This is at most how much we need for the output */
        allocation_length = (size_t) (input_end - input) - skipped_bytes;
        output = (unsigned char*)hooks->allocate(allocation_length + sizeof('\0'));
        if (output == NULL)
        {
            goto fail; /* allocation failure */
        }
    }

    output_pointer = output;
    /* loop through the string literal */
    while (input_pointer < input_end)
    {
        if (*input_pointer != '\\')
        {
            *output_pointer++ = *input_pointer++;
        }
        /* escape sequence */
        else
        {
            unsigned char sequence_length = 2;
            switch (input_pointer[1])
            {
                case 'b':
                    *output_pointer++ = '\b';
                    break;
                case 'f':
                    *output_pointer++ = '\f';
                    break;
                case 'n':
                    *output_pointer++ = '\n';
                    break;
                case 'r':
                    *output_pointer++ = '\r';
                    break;
                case 't':
                    *output_pointer++ = '\t';
                    break;
                case '\"':
                case '\\':
                case '/':
                    *output_pointer++ = input_pointer[1];
                    break;

                /* UTF-16 literal */
                case 'u':
                    sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &output_pointer, error_pointer);
                    if (sequence_length == 0)
                    {
                        /* failed to convert UTF16-literal to UTF-8 */
                        goto fail;
                    }
                    break;

                default:
                    *error_pointer = input_pointer;
                    goto fail;
            }
            input_pointer += sequence_length;
        }
    }

    /* zero terminate the output */
    *output_pointer = '\0';

    item->type = AIUI_cJSON_String;
    item->valuestring = (char*)output;

    return input_end + 1;

fail:
    if (output != NULL)
    {
        hooks->deallocate(output);
    }

    return NULL;
}

/* Render the cstring provided to an escaped version that can be printed. */
static AIUI_cJSON_bool print_string_ptr(const unsigned char * const input, printbuffer * const output_buffer, const internal_hooks * const hooks)
{
    const unsigned char *input_pointer = NULL;
    unsigned char *output = NULL;
    unsigned char *output_pointer = NULL;
    size_t output_length = 0;
    /* numbers of additional characters needed for escaping */
    size_t escape_characters = 0;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* empty string */
    if (input == NULL)
    {
        output = ensure(output_buffer, sizeof("\"\""), hooks);
        if (output == NULL)
        {
            return false;
        }
        strcpy((char*)output, "\"\"");

        return true;
    }

    /* set "flag" to 1 if something needs to be escaped */
    for (input_pointer = input; *input_pointer; input_pointer++)
    {
        if (strchr("\"\\\b\f\n\r\t", *input_pointer))
        {
            /* one character escape sequence */
            escape_characters++;
        }
        else if (*input_pointer < 32)
        {
            /* UTF-16 escape sequence uXXXX */
            escape_characters += 5;
        }
    }
    output_length = (size_t)(input_pointer - input) + escape_characters;

    output = ensure(output_buffer, output_length + sizeof("\"\""), hooks);
    if (output == NULL)
    {
        return false;
    }

    /* no characters have to be escaped */
    if (escape_characters == 0)
    {
        output[0] = '\"';
        memcpy(output + 1, input, output_length);
        output[output_length + 1] = '\"';
        output[output_length + 2] = '\0';

        return true;
    }

    output[0] = '\"';
    output_pointer = output + 1;
    /* copy the string */
    for (input_pointer = input; *input_pointer != '\0'; (void)input_pointer++, output_pointer++)
    {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\'))
        {
            /* normal character, copy */
            *output_pointer = *input_pointer;
        }
        else
        {
            /* character needs to be escaped */
            *output_pointer++ = '\\';
            switch (*input_pointer)
            {
                case '\\':
                    *output_pointer = '\\';
                    break;
                case '\"':
                    *output_pointer = '\"';
                    break;
                case '\b':
                    *output_pointer = 'b';
                    break;
                case '\f':
                    *output_pointer = 'f';
                    break;
                case '\n':
                    *output_pointer = 'n';
                    break;
                case '\r':
                    *output_pointer = 'r';
                    break;
                case '\t':
                    *output_pointer = 't';
                    break;
                default:
                    /* escape and print as unicode codepoint */
                    sprintf((char*)output_pointer, "u%04x", *input_pointer);
                    output_pointer += 4;
                    break;
            }
        }
    }
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';

    return true;
}

/* Invoke print_string_ptr (which is useful) on an item. */
static AIUI_cJSON_bool print_string(const AIUI_cJSON * const item, printbuffer * const p, const internal_hooks * const hooks)
{
    return print_string_ptr((unsigned char*)item->valuestring, p, hooks);
}

/* Predeclare these prototypes. */
static const unsigned char *parse_value(AIUI_cJSON * const item, const unsigned char * const input, const unsigned char ** const ep, const internal_hooks * const hooks);
static AIUI_cJSON_bool print_value(const AIUI_cJSON * const item, const size_t depth, const AIUI_cJSON_bool format, printbuffer * const output_buffer, const internal_hooks * const hooks);
static const unsigned char *parse_array(AIUI_cJSON * const item, const unsigned char *input, const unsigned char ** const ep, const internal_hooks * const hooks);
static AIUI_cJSON_bool print_array(const AIUI_cJSON * const item, const size_t depth, const AIUI_cJSON_bool format, printbuffer * const output_buffer, const internal_hooks * const hooks);
static const unsigned char *parse_object(AIUI_cJSON * const item, const unsigned char *input, const unsigned char ** const ep, const internal_hooks * const hooks);
static AIUI_cJSON_bool print_object(const AIUI_cJSON * const item, const size_t depth, const AIUI_cJSON_bool format, printbuffer * const output_buffer, const internal_hooks * const hooks);

/* Utility to jump whitespace and cr/lf */
static const unsigned char *skip_whitespace(const unsigned char *in)
{
    while (in && *in && (*in <= 32))
    {
        in++;
    }

    return in;
}

/* Parse an object - create a new root, and populate. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_ParseWithOpts(const char *value, const char **return_parse_end, AIUI_cJSON_bool require_null_terminated)
{
    const unsigned char *end = NULL;
    /* use global error pointer if no specific one was given */
    const unsigned char **ep = return_parse_end ? (const unsigned char**)return_parse_end : &global_ep;
    AIUI_cJSON *c = AIUI_cJSON_New_Item(&global_hooks);
    *ep = NULL;
    if (!c) /* memory fail */
    {
        return NULL;
    }

    end = parse_value(c, skip_whitespace((const unsigned char*)value), ep, &global_hooks);
    if (!end)
    {
        /* parse failure. ep is set. */
        AIUI_cJSON_Delete(c);
        return NULL;
    }

    /* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
    if (require_null_terminated)
    {
        end = skip_whitespace(end);
        if (*end)
        {
            AIUI_cJSON_Delete(c);
            *ep = end;
            return NULL;
        }
    }
    if (return_parse_end)
    {
        *return_parse_end = (const char*)end;
    }

    return c;
}

/* Default options for AIUI_cJSON_Parse */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_Parse(const char *value)
{
    return AIUI_cJSON_ParseWithOpts(value, 0, 0);
}

#define min(a, b) ((a < b) ? a : b)

static unsigned char *print(const AIUI_cJSON * const item, AIUI_cJSON_bool format, const internal_hooks * const hooks)
{
    printbuffer buffer[1];
    unsigned char *printed = NULL;

    memset(buffer, 0, sizeof(buffer));

    /* create buffer */
    buffer->buffer = (unsigned char*) hooks->allocate(256);
    if (buffer->buffer == NULL)
    {
        goto fail;
    }

    /* print the value */
    if (!print_value(item, 0, format, buffer, hooks))
    {
        goto fail;
    }
    update_offset(buffer);

    /* copy the buffer over to a new one */
    printed = (unsigned char*) hooks->allocate(buffer->offset + 1);
    if (printed == NULL)
    {
        goto fail;
    }
    strncpy((char*)printed, (char*)buffer->buffer, min(buffer->length, buffer->offset + 1));
    printed[buffer->offset] = '\0'; /* just to be sure */

    /* free the buffer */
    hooks->deallocate(buffer->buffer);

    return printed;

fail:
    if (buffer->buffer != NULL)
    {
        hooks->deallocate(buffer->buffer);
    }

    if (printed != NULL)
    {
        hooks->deallocate(printed);
    }

    return NULL;
}

/* Render a AIUI_cJSON item/entity/structure to text. */
AIUI_CJSON_PUBLIC(char *) AIUI_cJSON_Print(const AIUI_cJSON *item)
{
    return (char*)print(item, true, &global_hooks);
}

AIUI_CJSON_PUBLIC(char *) AIUI_cJSON_PrintUnformatted(const AIUI_cJSON *item)
{
    return (char*)print(item, false, &global_hooks);
}

AIUI_CJSON_PUBLIC(char *) AIUI_cJSON_PrintBuffered(const AIUI_cJSON *item, int prebuffer, AIUI_cJSON_bool fmt)
{
    printbuffer p;

    if (prebuffer < 0)
    {
        return NULL;
    }

    p.buffer = (unsigned char*)global_hooks.allocate((size_t)prebuffer);
    if (!p.buffer)
    {
        return NULL;
    }

    p.length = (size_t)prebuffer;
    p.offset = 0;
    p.noalloc = false;

    if (!print_value(item, 0, fmt, &p, &global_hooks))
    {
        return NULL;
    }

    return (char*)p.buffer;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_PrintPreallocated(AIUI_cJSON *item, char *buf, const int len, const AIUI_cJSON_bool fmt)
{
    printbuffer p;

    if (len < 0)
    {
        return false;
    }

    p.buffer = (unsigned char*)buf;
    p.length = (size_t)len;
    p.offset = 0;
    p.noalloc = true;
    return print_value(item, 0, fmt, &p, &global_hooks);
}

/* Parser core - when encountering text, process appropriately. */
static const unsigned  char *parse_value(AIUI_cJSON * const item, const unsigned char * const input, const unsigned char ** const error_pointer, const internal_hooks * const hooks)
{
    if (input == NULL)
    {
        return NULL; /* no input */
    }

    /* parse the different types of values */
    /* null */
    if (!strncmp((const char*)input, "null", 4))
    {
        item->type = AIUI_cJSON_NULL;
        return input + 4;
    }
    /* false */
    if (!strncmp((const char*)input, "false", 5))
    {
        item->type = AIUI_cJSON_False;
        return input + 5;
    }
    /* true */
    if (!strncmp((const char*)input, "true", 4))
    {
        item->type = AIUI_cJSON_True;
        item->valueint = 1;
        return input + 4;
    }
    /* string */
    if (*input == '\"')
    {
        return parse_string(item, input, error_pointer, hooks);
    }
    /* number */
    if ((*input == '-') || ((*input >= '0') && (*input <= '9')))
    {
        return parse_number(item, input);
    }
    /* array */
    if (*input == '[')
    {
        return parse_array(item, input, error_pointer, hooks);
    }
    /* object */
    if (*input == '{')
    {
        return parse_object(item, input, error_pointer, hooks);
    }

    /* failure. */
    *error_pointer = input;
    return NULL;
}

/* Render a value to text. */
static AIUI_cJSON_bool print_value(const AIUI_cJSON * const item, const size_t depth, const AIUI_cJSON_bool format,  printbuffer * const output_buffer, const internal_hooks * const hooks)
{
    unsigned char *output = NULL;

    if ((item == NULL) || (output_buffer == NULL))
    {
        return false;
    }

    switch ((item->type) & 0xFF)
    {
        case AIUI_cJSON_NULL:
            output = ensure(output_buffer, 5, hooks);
            if (output == NULL)
            {
                return false;
            }
            strcpy((char*)output, "null");
            return true;

        case AIUI_cJSON_False:
            output = ensure(output_buffer, 6, hooks);
            if (output == NULL)
            {
                return false;
            }
            strcpy((char*)output, "false");
            return true;

        case AIUI_cJSON_True:
            output = ensure(output_buffer, 5, hooks);
            if (output == NULL)
            {
                return false;
            }
            strcpy((char*)output, "true");
            return true;

        case AIUI_cJSON_Number:
            return print_number(item, output_buffer, hooks);

        case AIUI_cJSON_Raw:
        {
            size_t raw_length = 0;
            if (item->valuestring == NULL)
            {
                if (!output_buffer->noalloc)
                {
                    hooks->deallocate(output_buffer->buffer);
                }
                return false;
            }

            raw_length = strlen(item->valuestring) + sizeof('\0');
            output = ensure(output_buffer, raw_length, hooks);
            if (output == NULL)
            {
                return false;
            }
            memcpy(output, item->valuestring, raw_length);
            return true;
        }

        case AIUI_cJSON_String:
            return print_string(item, output_buffer, hooks);

        case AIUI_cJSON_Array:
            return print_array(item, depth, format, output_buffer, hooks);

        case AIUI_cJSON_Object:
            return print_object(item, depth, format, output_buffer, hooks);

        default:
            return false;
    }
}

/* Build an array from input text. */
static const unsigned char *parse_array(AIUI_cJSON * const item, const unsigned char *input, const unsigned char ** const error_pointer, const internal_hooks * const hooks)
{
    AIUI_cJSON *head = NULL; /* head of the linked list */
    AIUI_cJSON *current_item = NULL;

    if (*input != '[')
    {
        /* not an array */
        *error_pointer = input;
        goto fail;
    }

    input = skip_whitespace(input + 1);
    if (*input == ']')
    {
        /* empty array */
        goto success;
    }

    /* step back to character in front of the first element */
    input--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        AIUI_cJSON *new_item = AIUI_cJSON_New_Item(hooks);
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse next value */
        input = skip_whitespace(input + 1);
        input = parse_value(current_item, input, error_pointer, hooks);
        input = skip_whitespace(input);
        if (input == NULL)
        {
            goto fail; /* failed to parse value */
        }
    }
    while (*input == ',');

    if (*input != ']')
    {
        *error_pointer = input;
        goto fail; /* expected end of array */
    }

success:
    item->type = AIUI_cJSON_Array;
    item->child = head;

    return input + 1;

fail:
    if (head != NULL)
    {
        AIUI_cJSON_Delete(head);
    }

    return NULL;
}

/* Render an array to text */
static AIUI_cJSON_bool print_array(const AIUI_cJSON * const item, const size_t depth, const AIUI_cJSON_bool format, printbuffer * const output_buffer, const internal_hooks * const hooks)
{
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    AIUI_cJSON *current_element = item->child;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* Compose the output array. */
    /* opening square bracket */
    output_pointer = ensure(output_buffer, 1, hooks);
    if (output_pointer == NULL)
    {
        return false;
    }

    *output_pointer = '[';
    output_buffer->offset++;

    while (current_element != NULL)
    {
        if (!print_value(current_element, depth + 1, format, output_buffer, hooks))
        {
            return false;
        }
        update_offset(output_buffer);
        if (current_element->next)
        {
            length = (size_t) (format ? 2 : 1);
            output_pointer = ensure(output_buffer, length + 1, hooks);
            if (output_pointer == NULL)
            {
                return false;
            }
            *output_pointer++ = ',';
            if(format)
            {
                *output_pointer++ = ' ';
            }
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
        current_element = current_element->next;
    }

    output_pointer = ensure(output_buffer, 2, hooks);
    if (output_pointer == NULL)
    {
        return false;
    }
    *output_pointer++ = ']';
    *output_pointer = '\0';

    return true;
}

/* Build an object from the text. */
static const unsigned char *parse_object(AIUI_cJSON * const item, const unsigned char *input, const unsigned char ** const error_pointer, const internal_hooks * const hooks)
{
    AIUI_cJSON *head = NULL; /* linked list head */
    AIUI_cJSON *current_item = NULL;

    if (*input != '{')
    {
        *error_pointer = input;
        goto fail; /* not an object */
    }

    input = skip_whitespace(input + 1);
    if (*input == '}')
    {
        goto success; /* empty object */
    }

    /* step back to character in front of the first element */
    input--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        AIUI_cJSON *new_item = AIUI_cJSON_New_Item(hooks);
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse the name of the child */
        input = skip_whitespace(input + 1);
        input = parse_string(current_item, input, error_pointer, hooks);
        input = skip_whitespace(input);
        if (input == NULL)
        {
            goto fail; /* faile to parse name */
        }

        /* swap valuestring and string, because we parsed the name */
        current_item->string = current_item->valuestring;
        current_item->valuestring = NULL;

        if (*input != ':')
        {
            *error_pointer = input;
            goto fail; /* invalid object */
        }

        /* parse the value */
        input = skip_whitespace(input + 1);
        input = parse_value(current_item, input, error_pointer, hooks);
        input = skip_whitespace(input);
        if (input == NULL)
        {
            goto fail; /* failed to parse value */
        }
    }
    while (*input == ',');

    if (*input != '}')
    {
        *error_pointer = input;
        goto fail; /* expected end of object */
    }

success:
    item->type = AIUI_cJSON_Object;
    item->child = head;

    return input + 1;

fail:
    if (head != NULL)
    {
        AIUI_cJSON_Delete(head);
    }

    return NULL;
}

/* Render an object to text. */
static AIUI_cJSON_bool print_object(const AIUI_cJSON * const item, const size_t depth, const AIUI_cJSON_bool format, printbuffer * const output_buffer, const internal_hooks * const hooks)
{
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    AIUI_cJSON *current_item = item->child;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* Compose the output: */
    length = (size_t) (format ? 2 : 1); /* fmt: {\n */
    output_pointer = ensure(output_buffer, length + 1, hooks);
    if (output_pointer == NULL)
    {
        return false;
    }

    *output_pointer++ = '{';
    if (format)
    {
        *output_pointer++ = '\n';
    }
    output_buffer->offset += length;

    while (current_item)
    {
        if (format)
        {
            size_t i;
            output_pointer = ensure(output_buffer, depth + 1, hooks);
            if (output_pointer == NULL)
            {
                return false;
            }
            for (i = 0; i < depth + 1; i++)
            {
                *output_pointer++ = '\t';
            }
            output_buffer->offset += depth + 1;
        }

        /* print key */
        if (!print_string_ptr((unsigned char*)current_item->string, output_buffer, hooks))
        {
            return false;
        }
        update_offset(output_buffer);

        length = (size_t) (format ? 2 : 1);
        output_pointer = ensure(output_buffer, length, hooks);
        if (output_pointer == NULL)
        {
            return false;
        }
        *output_pointer++ = ':';
        if (format)
        {
            *output_pointer++ = '\t';
        }
        output_buffer->offset += length;

        /* print value */
        if (!print_value(current_item, depth + 1, format, output_buffer, hooks))
        {
            return false;
        }
        update_offset(output_buffer);

        /* print comma if not last */
        length = (size_t) ((format ? 1 : 0) + (current_item->next ? 1 : 0));
        output_pointer = ensure(output_buffer, length + 1, hooks);
        if (output_pointer == NULL)
        {
            return false;
        }
        if (current_item->next)
        {
            *output_pointer++ = ',';
        }

        if (format)
        {
            *output_pointer++ = '\n';
        }
        *output_pointer = '\0';
        output_buffer->offset += length;

        current_item = current_item->next;
    }

    output_pointer = ensure(output_buffer, format ? (depth + 2) : 2, hooks);
    if (output_pointer == NULL)
    {
        return false;
    }
    if (format)
    {
        size_t i;
        for (i = 0; i < (depth); i++)
        {
            *output_pointer++ = '\t';
        }
    }
    *output_pointer++ = '}';
    *output_pointer = '\0';

    return true;
}

/* Get Array size/item / object item. */
AIUI_CJSON_PUBLIC(int) AIUI_cJSON_GetArraySize(const AIUI_cJSON *array)
{
    AIUI_cJSON *c = array->child;
    size_t i = 0;
    while(c)
    {
        i++;
        c = c->next;
    }

    /* FIXME: Can overflow here. Cannot be fixed without breaking the API */

    return (int)i;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_GetArrayItem(const AIUI_cJSON *array, int item)
{
    AIUI_cJSON *c = array ? array->child : NULL;
    while (c && item > 0)
    {
        item--;
        c = c->next;
    }

    return c;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_GetObjectItem(const AIUI_cJSON *object, const char *string)
{
    AIUI_cJSON *c = object ? object->child : NULL;
    while (c && AIUI_cJSON_strcasecmp((unsigned char*)c->string, (const unsigned char*)string))
    {
        c = c->next;
    }
    return c;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_GetObjectItemCaseSensitive(const AIUI_cJSON * const object, const char * const string)
{
    AIUI_cJSON *current_element = NULL;

    if ((object == NULL) || (string == NULL))
    {
        return NULL;
    }

    current_element = object->child;
    while ((current_element != NULL) && (strcmp(string, current_element->string) != 0))
    {
        current_element = current_element->next;
    }

    return current_element;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_HasObjectItem(const AIUI_cJSON *object, const char *string)
{
    return AIUI_cJSON_GetObjectItem(object, string) ? 1 : 0;
}

/* Utility for array list handling. */
static void suffix_object(AIUI_cJSON *prev, AIUI_cJSON *item)
{
    prev->next = item;
    item->prev = prev;
}

/* Utility for handling references. */
static AIUI_cJSON *create_reference(const AIUI_cJSON *item, const internal_hooks * const hooks)
{
    AIUI_cJSON *ref = AIUI_cJSON_New_Item(hooks);
    if (!ref)
    {
        return NULL;
    }
    memcpy(ref, item, sizeof(AIUI_cJSON));
    ref->string = NULL;
    ref->type |= AIUI_cJSON_IsReference;
    ref->next = ref->prev = NULL;
    return ref;
}

/* Add item to array/object. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemToArray(AIUI_cJSON *array, AIUI_cJSON *item)
{
    AIUI_cJSON *child = NULL;

    if ((item == NULL) || (array == NULL))
    {
        return;
    }

    child = array->child;

    if (child == NULL)
    {
        /* list is empty, start new one */
        array->child = item;
    }
    else
    {
        /* append to the end */
        while (child->next)
        {
            child = child->next;
        }
        suffix_object(child, item);
    }
}

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemToObject(AIUI_cJSON *object, const char *string, AIUI_cJSON *item)
{
    /* call AIUI_cJSON_AddItemToObjectCS for code reuse */
    AIUI_cJSON_AddItemToObjectCS(object, (char*)AIUI_cJSON_strdup((const unsigned char*)string, &global_hooks), item);
    /* remove AIUI_cJSON_StringIsConst flag */
    item->type &= ~AIUI_cJSON_StringIsConst;
}

#if defined (__clang__) || ((__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
//    #pragma GCC diagnostic push
#endif
//#pragma GCC diagnostic ignored "-Wcast-qual"
/* Add an item to an object with constant string as key */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemToObjectCS(AIUI_cJSON *object, const char *string, AIUI_cJSON *item)
{
    if (!item)
    {
        return;
    }
    if (!(item->type & AIUI_cJSON_StringIsConst) && item->string)
    {
        global_hooks.deallocate(item->string);
    }
    item->string = (char*)string;
    item->type |= AIUI_cJSON_StringIsConst;
    AIUI_cJSON_AddItemToArray(object, item);
}
#if defined (__clang__) || ((__GNUC__)  && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
//    #pragma GCC diagnostic pop
#endif

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemReferenceToArray(AIUI_cJSON *array, AIUI_cJSON *item)
{
    AIUI_cJSON_AddItemToArray(array, create_reference(item, &global_hooks));
}

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemReferenceToObject(AIUI_cJSON *object, const char *string, AIUI_cJSON *item)
{
    AIUI_cJSON_AddItemToObject(object, string, create_reference(item, &global_hooks));
}

static AIUI_cJSON *DetachItemFromArray(AIUI_cJSON *array, size_t which)
{
    AIUI_cJSON *c = array->child;
    while (c && (which > 0))
    {
        c = c->next;
        which--;
    }
    if (!c)
    {
        /* item doesn't exist */
        return NULL;
    }
    if (c->prev)
    {
        /* not the first element */
        c->prev->next = c->next;
    }
    if (c->next)
    {
        c->next->prev = c->prev;
    }
    if (c==array->child)
    {
        array->child = c->next;
    }
    /* make sure the detached item doesn't point anywhere anymore */
    c->prev = c->next = NULL;

    return c;
}
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_DetachItemFromArray(AIUI_cJSON *array, int which)
{
    if (which < 0)
    {
        return NULL;
    }

    return DetachItemFromArray(array, (size_t)which);
}

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_DeleteItemFromArray(AIUI_cJSON *array, int which)
{
    AIUI_cJSON_Delete(AIUI_cJSON_DetachItemFromArray(array, which));
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_DetachItemFromObject(AIUI_cJSON *object, const char *string)
{
    size_t i = 0;
    AIUI_cJSON *c = object->child;
    while (c && AIUI_cJSON_strcasecmp((unsigned char*)c->string, (const unsigned char*)string))
    {
        i++;
        c = c->next;
    }
    if (c)
    {
        return DetachItemFromArray(object, i);
    }

    return NULL;
}

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_DeleteItemFromObject(AIUI_cJSON *object, const char *string)
{
    AIUI_cJSON_Delete(AIUI_cJSON_DetachItemFromObject(object, string));
}

/* Replace array/object items with new ones. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_InsertItemInArray(AIUI_cJSON *array, int which, AIUI_cJSON *newitem)
{
    AIUI_cJSON *c = array->child;
    while (c && (which > 0))
    {
        c = c->next;
        which--;
    }
    if (!c)
    {
        AIUI_cJSON_AddItemToArray(array, newitem);
        return;
    }
    newitem->next = c;
    newitem->prev = c->prev;
    c->prev = newitem;
    if (c == array->child)
    {
        array->child = newitem;
    }
    else
    {
        newitem->prev->next = newitem;
    }
}

static void ReplaceItemInArray(AIUI_cJSON *array, size_t which, AIUI_cJSON *newitem)
{
    AIUI_cJSON *c = array->child;
    while (c && (which > 0))
    {
        c = c->next;
        which--;
    }
    if (!c)
    {
        return;
    }
    newitem->next = c->next;
    newitem->prev = c->prev;
    if (newitem->next)
    {
        newitem->next->prev = newitem;
    }
    if (c == array->child)
    {
        array->child = newitem;
    }
    else
    {
        newitem->prev->next = newitem;
    }
    c->next = c->prev = NULL;
    AIUI_cJSON_Delete(c);
}
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_ReplaceItemInArray(AIUI_cJSON *array, int which, AIUI_cJSON *newitem)
{
    if (which < 0)
    {
        return;
    }

    ReplaceItemInArray(array, (size_t)which, newitem);
}

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_ReplaceItemInObject(AIUI_cJSON *object, const char *string, AIUI_cJSON *newitem)
{
    size_t i = 0;
    AIUI_cJSON *c = object->child;
    while(c && AIUI_cJSON_strcasecmp((unsigned char*)c->string, (const unsigned char*)string))
    {
        i++;
        c = c->next;
    }
    if(c)
    {
        /* free the old string if not const */
        if (!(newitem->type & AIUI_cJSON_StringIsConst) && newitem->string)
        {
             global_hooks.deallocate(newitem->string);
        }

        newitem->string = (char*)AIUI_cJSON_strdup((const unsigned char*)string, &global_hooks);
        ReplaceItemInArray(object, i, newitem);
    }
}

/* Create basic types: */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateNull(void)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = AIUI_cJSON_NULL;
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateTrue(void)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = AIUI_cJSON_True;
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateFalse(void)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = AIUI_cJSON_False;
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateBool(AIUI_cJSON_bool b)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = b ? AIUI_cJSON_True : AIUI_cJSON_False;
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateNumber(double num)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = AIUI_cJSON_Number;
        item->valuedouble = num;

        /* use saturation in case of overflow */
        if (num >= INT_MAX)
        {
            item->valueint = INT_MAX;
        }
        else if (num <= INT_MIN)
        {
            item->valueint = INT_MIN;
        }
        else
        {
            item->valueint = (int)num;
        }
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateString(const char *string)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = AIUI_cJSON_String;
        item->valuestring = (char*)AIUI_cJSON_strdup((const unsigned char*)string, &global_hooks);
        if(!item->valuestring)
        {
            AIUI_cJSON_Delete(item);
            return NULL;
        }
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateRaw(const char *raw)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type = AIUI_cJSON_Raw;
        item->valuestring = (char*)AIUI_cJSON_strdup((const unsigned char*)raw, &global_hooks);
        if(!item->valuestring)
        {
            AIUI_cJSON_Delete(item);
            return NULL;
        }
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateArray(void)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if(item)
    {
        item->type=AIUI_cJSON_Array;
    }

    return item;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateObject(void)
{
    AIUI_cJSON *item = AIUI_cJSON_New_Item(&global_hooks);
    if (item)
    {
        item->type = AIUI_cJSON_Object;
    }

    return item;
}

/* Create Arrays: */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateIntArray(const int *numbers, int count)
{
    size_t i = 0;
    AIUI_cJSON *n = NULL;
    AIUI_cJSON *p = NULL;
    AIUI_cJSON *a = NULL;

    if (count < 0)
    {
        return NULL;
    }

    a = AIUI_cJSON_CreateArray();
    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = AIUI_cJSON_CreateNumber(numbers[i]);
        if (!n)
        {
            AIUI_cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    return a;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateFloatArray(const float *numbers, int count)
{
    size_t i = 0;
    AIUI_cJSON *n = NULL;
    AIUI_cJSON *p = NULL;
    AIUI_cJSON *a = NULL;

    if (count < 0)
    {
        return NULL;
    }

    a = AIUI_cJSON_CreateArray();

    for(i = 0; a && (i < (size_t)count); i++)
    {
        n = AIUI_cJSON_CreateNumber((double)numbers[i]);
        if(!n)
        {
            AIUI_cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    return a;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateDoubleArray(const double *numbers, int count)
{
    size_t i = 0;
    AIUI_cJSON *n = NULL;
    AIUI_cJSON *p = NULL;
    AIUI_cJSON *a = NULL;

    if (count < 0)
    {
        return NULL;
    }

    a = AIUI_cJSON_CreateArray();

    for(i = 0;a && (i < (size_t)count); i++)
    {
        n = AIUI_cJSON_CreateNumber(numbers[i]);
        if(!n)
        {
            AIUI_cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    return a;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateStringArray(const char **strings, int count)
{
    size_t i = 0;
    AIUI_cJSON *n = NULL;
    AIUI_cJSON *p = NULL;
    AIUI_cJSON *a = NULL;

    if (count < 0)
    {
        return NULL;
    }

    a = AIUI_cJSON_CreateArray();

    for (i = 0; a && (i < (size_t)count); i++)
    {
        n = AIUI_cJSON_CreateString(strings[i]);
        if(!n)
        {
            AIUI_cJSON_Delete(a);
            return NULL;
        }
        if(!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p,n);
        }
        p = n;
    }

    return a;
}

/* Duplication */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_Duplicate(const AIUI_cJSON *item, AIUI_cJSON_bool recurse)
{
    AIUI_cJSON *newitem = NULL;
    AIUI_cJSON *child = NULL;
    AIUI_cJSON *next = NULL;
    AIUI_cJSON *newchild = NULL;

    /* Bail on bad ptr */
    if (!item)
    {
        goto fail;
    }
    /* Create new item */
    newitem = AIUI_cJSON_New_Item(&global_hooks);
    if (!newitem)
    {
        goto fail;
    }
    /* Copy over all vars */
    newitem->type = item->type & (~AIUI_cJSON_IsReference);
    newitem->valueint = item->valueint;
    newitem->valuedouble = item->valuedouble;
    if (item->valuestring)
    {
        newitem->valuestring = (char*)AIUI_cJSON_strdup((unsigned char*)item->valuestring, &global_hooks);
        if (!newitem->valuestring)
        {
            goto fail;
        }
    }
    if (item->string)
    {
        newitem->string = (item->type&AIUI_cJSON_StringIsConst) ? item->string : (char*)AIUI_cJSON_strdup((unsigned char*)item->string, &global_hooks);
        if (!newitem->string)
        {
            goto fail;
        }
    }
    /* If non-recursive, then we're done! */
    if (!recurse)
    {
        return newitem;
    }
    /* Walk the ->next chain for the child. */
    child = item->child;
    while (child != NULL)
    {
        newchild = AIUI_cJSON_Duplicate(child, true); /* Duplicate (with recurse) each item in the ->next chain */
        if (!newchild)
        {
            goto fail;
        }
        if (next != NULL)
        {
            /* If newitem->child already set, then crosswire ->prev and ->next and move on */
            next->next = newchild;
            newchild->prev = next;
            next = newchild;
        }
        else
        {
            /* Set newitem->child and move to it */
            newitem->child = newchild;
            next = newchild;
        }
        child = child->next;
    }

    return newitem;

fail:
    if (newitem != NULL)
    {
        AIUI_cJSON_Delete(newitem);
    }

    return NULL;
}

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_Minify(char *json)
{
    unsigned char *into = (unsigned char*)json;
    while (*json)
    {
        if (*json == ' ')
        {
            json++;
        }
        else if (*json == '\t')
        {
            /* Whitespace characters. */
            json++;
        }
        else if (*json == '\r')
        {
            json++;
        }
        else if (*json=='\n')
        {
            json++;
        }
        else if ((*json == '/') && (json[1] == '/'))
        {
            /* double-slash comments, to end of line. */
            while (*json && (*json != '\n'))
            {
                json++;
            }
        }
        else if ((*json == '/') && (json[1] == '*'))
        {
            /* multiline comments. */
            while (*json && !((*json == '*') && (json[1] == '/')))
            {
                json++;
            }
            json += 2;
        }
        else if (*json == '\"')
        {
            /* string literals, which are \" sensitive. */
            *into++ = (unsigned char)*json++;
            while (*json && (*json != '\"'))
            {
                if (*json == '\\')
                {
                    *into++ = (unsigned char)*json++;
                }
                *into++ = (unsigned char)*json++;
            }
            *into++ = (unsigned char)*json++;
        }
        else
        {
            /* All other characters. */
            *into++ = (unsigned char)*json++;
        }
    }

    /* and null-terminate. */
    *into = '\0';
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsInvalid(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_Invalid;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsFalse(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_False;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsTrue(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xff) == AIUI_cJSON_True;
}


AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsBool(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & (AIUI_cJSON_True | AIUI_cJSON_False)) != 0;
}
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsNull(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_NULL;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsNumber(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_Number;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsString(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_String;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsArray(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_Array;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsObject(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_Object;
}

AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsRaw(const AIUI_cJSON * const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == AIUI_cJSON_Raw;
}
