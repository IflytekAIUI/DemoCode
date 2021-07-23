/*
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

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

/* project version */
#define AIUI_CJSON_VERSION_MAJOR 1
#define AIUI_CJSON_VERSION_MINOR 4
#define AIUI_CJSON_VERSION_PATCH 5

#include <stddef.h>

/* cJSON Types: */
#define AIUI_cJSON_Invalid (0)
#define AIUI_cJSON_False  (1 << 0)
#define AIUI_cJSON_True   (1 << 1)
#define AIUI_cJSON_NULL   (1 << 2)
#define AIUI_cJSON_Number (1 << 3)
#define AIUI_cJSON_String (1 << 4)
#define AIUI_cJSON_Array  (1 << 5)
#define AIUI_cJSON_Object (1 << 6)
#define AIUI_cJSON_Raw    (1 << 7) /* raw json */

#define AIUI_cJSON_IsReference 256
#define AIUI_cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct AIUI_cJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct AIUI_cJSON *next;
    struct AIUI_cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct AIUI_cJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *valuestring;
    /* The item's number, if type==cJSON_Number */
    int valueint;
    /* The item's number, if type==cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} AIUI_cJSON;

typedef struct AIUI_cJSON_Hooks
{
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} AIUI_cJSON_Hooks;

typedef int AIUI_cJSON_bool;

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif
#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 2 define options:

AIUI_CJSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
AIUI_CJSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols

For *nix builds that support visibility attribute, you can define similar behavior by 

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the AIUI_CJSON_API_VISIBILITY flag to "export" the same symbols the way AIUI_CJSON_EXPORT_SYMBOLS does

*/

#if defined(AIUI_CJSON_HIDE_SYMBOLS)
#define AIUI_CJSON_PUBLIC(type)   type __stdcall
#elif defined(AIUI_CJSON_EXPORT_SYMBOLS)
#define AIUI_CJSON_PUBLIC(type)   __declspec(dllexport) type __stdcall
#else
#define AIUI_CJSON_PUBLIC(type)   __declspec(dllimport) type __stdcall
#endif
#else /* !WIN32 */
#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(AIUI_CJSON_API_VISIBILITY)
#define AIUI_CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define AIUI_CJSON_PUBLIC(type) type
#endif
#endif

/* returns the version of cJSON as a string */
AIUI_CJSON_PUBLIC(const char*) AIUI_cJSON_Version(void);

/* Supply malloc, realloc and free functions to AIUI_cJSON */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_InitHooks(AIUI_cJSON_Hooks* hooks);


/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call AIUI_cJSON_Delete when finished. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_Parse(const char *value);
/* Render a cJSON entity to text for transfer/storage. Free the char* when finished. */
AIUI_CJSON_PUBLIC(char *) AIUI_cJSON_Print(const AIUI_cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
AIUI_CJSON_PUBLIC(char *) AIUI_cJSON_PrintUnformatted(const AIUI_cJSON *item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
AIUI_CJSON_PUBLIC(char *) AIUI_cJSON_PrintBuffered(const AIUI_cJSON *item, int prebuffer, AIUI_cJSON_bool fmt);
/* Render a cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: If you are printing numbers, the buffer hat to be 63 bytes bigger then the printed JSON (worst case) */
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_PrintPreallocated(AIUI_cJSON *item, char *buffer, const int length, const AIUI_cJSON_bool format);
/* Delete a cJSON entity and all subentities. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_Delete(AIUI_cJSON *c);

/* Returns the number of items in an array (or object). */
AIUI_CJSON_PUBLIC(int) AIUI_cJSON_GetArraySize(const AIUI_cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_GetArrayItem(const AIUI_cJSON *array, int item);
/* Get item "string" from object. Case insensitive. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_GetObjectItem(const AIUI_cJSON *object, const char *string);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_GetObjectItemCaseSensitive(const AIUI_cJSON *object, const char *string);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_HasObjectItem(const AIUI_cJSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when AIUI_cJSON_Parse() returns 0. 0 when AIUI_cJSON_Parse() succeeds. */
AIUI_CJSON_PUBLIC(const char *) AIUI_cJSON_GetErrorPtr(void);

/* These functions check the type of an item */
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsInvalid(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsFalse(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsTrue(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsBool(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsNull(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsNumber(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsString(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsArray(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsObject(const AIUI_cJSON * const item);
AIUI_CJSON_PUBLIC(AIUI_cJSON_bool) AIUI_cJSON_IsRaw(const AIUI_cJSON * const item);

/* These calls create a cJSON item of the appropriate type. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateNull(void);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateTrue(void);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateFalse(void);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateBool(AIUI_cJSON_bool boolean);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateNumber(double num);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateString(const char *string);
/* raw json */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateRaw(const char *raw);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateArray(void);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateObject(void);

/* These utilities create an Array of count items. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateIntArray(const int *numbers, int count);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateFloatArray(const float *numbers, int count);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateDoubleArray(const double *numbers, int count);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_CreateStringArray(const char **strings, int count);

/* Append item to the specified array/object. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemToArray(AIUI_cJSON *array, AIUI_cJSON *item);
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemToObject(AIUI_cJSON *object, const char *string, AIUI_cJSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & AIUI_cJSON_StringIsConst) is zero before
 * writing to `item->string` */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemToObjectCS(AIUI_cJSON *object, const char *string, AIUI_cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemReferenceToArray(AIUI_cJSON *array, AIUI_cJSON *item);
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_AddItemReferenceToObject(AIUI_cJSON *object, const char *string, AIUI_cJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_DetachItemFromArray(AIUI_cJSON *array, int which);
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_DeleteItemFromArray(AIUI_cJSON *array, int which);
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_DetachItemFromObject(AIUI_cJSON *object, const char *string);
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_DeleteItemFromObject(AIUI_cJSON *object, const char *string);

/* Update array items. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_InsertItemInArray(AIUI_cJSON *array, int which, AIUI_cJSON *newitem); /* Shifts pre-existing items to the right. */
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_ReplaceItemInArray(AIUI_cJSON *array, int which, AIUI_cJSON *newitem);
AIUI_CJSON_PUBLIC(void) AIUI_cJSON_ReplaceItemInObject(AIUI_cJSON *object,const char *string,AIUI_cJSON *newitem);

/* Duplicate a cJSON item */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_Duplicate(const AIUI_cJSON *item, AIUI_cJSON_bool recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error. If not, then AIUI_cJSON_GetErrorPtr() does the job. */
AIUI_CJSON_PUBLIC(AIUI_cJSON *) AIUI_cJSON_ParseWithOpts(const char *value, const char **return_parse_end, AIUI_cJSON_bool require_null_terminated);

AIUI_CJSON_PUBLIC(void) AIUI_cJSON_Minify(char *json);

/* Macros for creating things quickly. */
#define AIUI_cJSON_AddNullToObject(object,name) AIUI_cJSON_AddItemToObject(object, name, AIUI_cJSON_CreateNull())
#define AIUI_cJSON_AddTrueToObject(object,name) AIUI_cJSON_AddItemToObject(object, name, AIUI_cJSON_CreateTrue())
#define AIUI_cJSON_AddFalseToObject(object,name) AIUI_cJSON_AddItemToObject(object, name, AIUI_cJSON_CreateFalse())
#define AIUI_cJSON_AddBoolToObject(object,name,b) AIUI_cJSON_AddItemToObject(object, name, AIUI_cJSON_CreateBool(b))
#define AIUI_cJSON_AddNumberToObject(object,name,n) AIUI_cJSON_AddItemToObject(object, name, AIUI_cJSON_CreateNumber(n))
#define AIUI_cJSON_AddStringToObject(object,name,s) AIUI_cJSON_AddItemToObject(object, name, AIUI_cJSON_CreateString(s))
#define AIUI_cJSON_AddRawToObject(object,name,s) AIUI_cJSON_AddItemToObject(object, name, AIUI_cJSON_CreateRaw(s))

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define AIUI_cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the AIUI_cJSON_SetNumberValue macro */
AIUI_CJSON_PUBLIC(double) AIUI_cJSON_SetNumberHelper(AIUI_cJSON *object, double number);
#define AIUI_cJSON_SetNumberValue(object, number) ((object != NULL) ? AIUI_cJSON_SetNumberHelper(object, (double)number) : (number))

/* Macro for iterating over an array */
#define AIUI_cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

#ifdef __cplusplus
}
#endif

#endif
