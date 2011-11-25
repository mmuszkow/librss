#ifndef __RSS_HTTP_H__
#define __RSS_HTTP_H__

#include "RSS.h"
#include "RSS_Buffer.h"

/** Used to split hostname and path
	Format: uri hostname path request, uri is not obligatory
	Path includes path variable includes request
	*/
typedef struct RSS_Url
{
	char*	host;
	char*	path;
} RSS_Url;

#define RSS_HTTP_HEADER1  \
		"GET "
#define RSS_HTTP_HEADER2 \
		" HTTP/1.0\r\n" \
		"Host: "
#define RSS_HTTP_HEADER3 \
		"\r\n" \
		"Connection: close\r\n" \
		"Accept: text/xml\r\n" \
		"User-Agent: Mozilla/5.0\r\n" \
		"\r\n"

/** HTTP errors */
typedef enum RSS_Http_error 
{
		/** Everything OK */
		RSS_HTTP_OK,
		/** Windows only, could not initialize winsock */
		RSS_HTTP_WSASTARTUP,
		/** Could not create socket */
		RSS_HTTP_SOCKET,
		/** Could not resolve host name */
		RSS_HTTP_GETHOSTBYNAME,
		/** Received adress is not in IPv4 format */
		RSS_HTTP_NOT_IPV4,
		/** Could not connect */
		RSS_HTTP_CONN_FAILED,
		/** Could not send */
		RSS_HTTP_SEND_FAILED,
		/** HTTP response code different than 200 */
		RSS_HTTP_NOT_200,
		/** No HTTP data found */
		RSS_HTTP_NO_DATA,
		/** Bad function arguments */
		RSS_HTTP_BAD_ARG,
		/** Requested page was to big to allocate memory for it */
		RSS_HTTP_PAGE_TOO_BIG
} RSS_Http_error;

/** Initial size for buffer used to retrieve page, not less than 4KB! */
#define RSS_HTTP_INITIAL_BUFFER_SIZE	16384

/** Saves page content from url to buffer
  * Buffer don't need to be preallocated
  * Why not to file? Computers now have > 2GB RAM
  */
RSS_Http_error RSS_http_get_page(const RSS_Url* url, char** buff);

/** Creates RSS_Url from string url */
RSS_Url*	RSS_create_url(const RSS_char* url);

/** Releases created RSS_Url*/
void		RSS_free_url(RSS_Url* url);

#endif
