#ifndef __RSS_H_
#define __RSS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#ifdef _WIN32
# include <windows.h>
#endif

/* Disable some stupid anti-POSIX warnings */
#ifdef _MSC_VER
# pragma warning(disable:4996)
#endif

#define	RSS_VERSION	0x0002

#ifdef RSS_USE_WSTRING /* wchar_t */
# define RSS_char		wchar_t
# define RSS_strcmp		wcscmp
# define RSS_strncmp	wcsncmp
# define RSS_strcpy		wcscpy
# define RSS_strstr		wcsstr
# define RSS_strlen		wcslen
# define RSS_strdup		wcsdup
# define RSS_printf		wprintf
# define RSS_ctime		_wctime
# define RSS_atoi		_wtoi
# define RSS_strrchr	wcsrchr
# define RSS_strncasecmp wcsnicmp
# define RSS_text(txt)	L##txt
#else /* char */
# define RSS_char		char
# define RSS_strcmp		strcmp
# define RSS_strncmp	strncmp
# define RSS_strcpy		strcpy
# define RSS_strstr		strstr
# define RSS_strlen		strlen
# define RSS_strdup		strdup
# define RSS_printf		printf
# define RSS_ctime		ctime
# define RSS_atoi		atoi
# define RSS_strrchr	strrchr
# ifdef _WIN32
#  define RSS_strncasecmp strnicmp
# else
#  define RSS_strncasecmp strncasecmp
# endif
# define RSS_text(txt)	txt
#endif /* RSS_USE_WSTRING */

#if defined(_WIN32) && defined(_MSC_VER) /* Visual WIN32 */
# define RSSCDECL __cdecl
# if defined(IN_LIBRSS) && !defined(LIBRSS_STATIC)
#  define RSSPUBFUN __declspec(dllexport)
# else
#  define RSSPUBFUN
# endif
#else /* Unix */
# define RSSCDECL
# define RSSPUBFUN
#endif

typedef unsigned int RSS_u32;

/** Error level - information */
#define RSS_EL_INFO		0
/** Error level - warning */
#define RSS_EL_WARNING	1
/** Error level - real error */
#define RSS_EL_ERROR	2

/** No information about position where error occured available */
#define RSS_NO_POS_INFO	-1

/** Parser error handler */
typedef void (RSSCDECL * RSS_error_handler)(RSS_u32 error_level, const RSS_char* error, size_t pos);

#include "RSS_Feed.h"

#define RSS_TYPE_RSS	1
#define RSS_TYPE_ATOM	2

#ifdef __cplusplus
extern "C" {
#endif

/** Date and Time Specification of RFC 822 format to time_t format */
RSSPUBFUN time_t RSSCDECL	RSS_parse_RFC822_Date(const RSS_char* str, RSS_error_handler handler);

/** Date and Time Specification of RFC 3339 format to time_t format */
RSSPUBFUN time_t RSSCDECL	RSS_parse_RFC3339_Date(const RSS_char* str, RSS_error_handler handler);

#ifndef RSS_NO_HTTP_SUPPORT
/** Creates parsed feed from URL */
RSSPUBFUN RSS_Feed* RSSCDECL	RSS_create_feed(const RSS_char* http_address, RSS_error_handler handler);
#endif

/** Creates parsed feed from document content */
RSSPUBFUN RSS_Feed* RSSCDECL	RSS_create_feed_from_str(const RSS_char* content, RSS_error_handler handler);

/** Releases created feed */
RSSPUBFUN void RSSCDECL		RSS_free_feed(RSS_Feed* feed);

#ifdef __cplusplus
}
#endif /* extern "C" { */

#endif /* __RSS_H_ */
