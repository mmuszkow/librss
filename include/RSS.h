#ifndef __RSS_H_
#define __RSS_H_

#include <time.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <iconv.h> /* iconv is not working with wchar_t and UTF-16 on Windows */
#endif

#define	RSS_VERSION	0x0001
#ifdef _WIN32
# define RSS_xmlStrcpy_s(dst, s, src) strcpy_s(dst, s, src)
#else
# define RSS_xmlStrcpy_s(dst, s, src) strncpy((char*)dst, (const char*)src, s)
#endif
#define RSS_xmlAtoi(x)	atoi((const char*)x)

#ifdef RSS_USE_WSTRING
# define RSS_char		wchar_t
# define RSS_strcmp		wcscmp
# define RSS_strstr		wcsstr
# define RSS_strlen		wcslen
# define RSS_printf		wprintf
# define RSS_text(txt)	L##txt
# define XML_UNICODE_WCHAR_T 1
#else
# define RSS_char		char
# define RSS_strcmp		strcmp
# define RSS_strstr		strstr
# define RSS_strlen		strlen
# define RSS_printf		printf
# define RSS_text(txt)	txt
# ifdef XML_UNICODE_WCHAR_T
#  undef XML_UNICODE_WCHAR_T
# endif
#endif /* RSS_USE_WSTRING */

#ifndef _WIN32
# define __cdecl
# define RSS_EXPORT	extern
# define RSS_IMPORT extern
#else
# define RSS_EXPORT	__declspec(dllexport)
# define RSS_IMPORT __declspec(dllimport) extern
#endif

#ifdef RSS_ASDLL
# define RSS_FUNC RSS_EXPORT
# include <libxml/tree.h>
# include <libxml/parser.h>
#else
# define RSS_FUNC RSS_IMPORT
# ifndef __XML_ERROR_H__
# include "XML_errors.h"
# endif
# ifndef xmlChar
#  define xmlChar unsigned char
# endif
#endif

typedef unsigned int RSS_u32;

#include "RSS_Feed.h"

#define RSS_TYPE_RSS	1
#define RSS_TYPE_ATOM	2

#ifdef __cplusplus
extern "C" {
#endif

/** Should be called once in the main thread (to ensure libxml2 Thread safety)
	before using any of librss functions 
	*/
RSS_FUNC void	__cdecl		RSS_init(void);

/** Should be called once, right before exiting */
RSS_FUNC void	__cdecl		RSS_deinit(void);

/** Date and Time Specification of RFC 822 format to time_t format */
RSS_FUNC time_t __cdecl		RSS_parse_RFC822_Date(const xmlChar* str);

/** Date and Time Specification of RFC 3339 format to time_t format */
RSS_FUNC time_t __cdecl		RSS_parse_RFC3339_Date(const xmlChar* str);

/** Creates parsed feed from URL 
	\param url feed address
	\param error_handler xml and http error handler, NULL if deafult
	\param ctx param passed to error handler function
	*/
RSS_FUNC RSS_Feed* __cdecl	RSS_create_feed(const RSS_char* url, xmlStructuredErrorFunc error_handler, void* ctx);

/** Releases created feed */
RSS_FUNC void __cdecl		RSS_free_feed(RSS_Feed* feed);

#ifdef __cplusplus
}
#endif /* extern "C" { */

#endif /* __RSS_H_ */
