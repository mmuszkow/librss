#ifndef __RSS_BUFFER_H__
#define __RSS_BUFFER_H__

#include "RSS.h"

typedef struct RSS_Buffer
{
	/** Text */
	RSS_char*	str;

	/** Length */
	size_t		len;

	/** Bytes reserved for text */
	size_t		reserved;

} RSS_Buffer;

#define	RSS_BUFFER_INITIAL_SIZE	512

/** Creates a buffer */
RSS_Buffer* RSS_create_buffer(void);
/** Reserves memory for buffer (2x previous reserved bytes) */
void RSS_reserve_buffer(RSS_Buffer* buffer);
/** Appends character to buffer */
void RSS_add_buffer(RSS_Buffer* buffer, RSS_char ch);
/** Clears content of the buffer */
void RSS_clear_buffer(RSS_Buffer* buffer);
/** Destroys the buffer */
void RSS_free_buffer(RSS_Buffer* buffer);

typedef enum RSS_Encoding
{
	/** No info about encoding */
	RSS_ENC_NO_INFO,
	/** Encoding not supported */
	RSS_ENC_UNSUPP,
	/** All values are Windows codepage numbers 
	  * http://msdn.microsoft.com/en-us/library/windows/desktop/dd317756%28v=vs.85%29.aspx
	  */
	RSS_ENC_UTF8 = 65001,
	/* Western European, Latin 1 */
	RSS_ENC_ISO8859_1 = 28591,
	RSS_ENC_WINDOWS_1252 = 1252,
	/* Central European */
	RSS_ENC_ISO8859_2 = 28592,
	RSS_ENC_WINDOWS_1250 = 1250,
	/* Latin 3 */
	RSS_ENC_ISO8859_3 = 28593,
	/* Baltic */
	RSS_ENC_ISO8859_4 = 28594,
	RSS_ENC_WINDOWS_1257 = 1257,
	/* Cyrylic */
	RSS_ENC_ISO8859_5 = 28595,
	RSS_ENC_WINDOWS_1251 = 1251,
	/* Arabic */
	RSS_ENC_ISO8859_6 = 28596,
	RSS_ENC_WINDOWS_1256 = 1256,
	/* Greek */
	RSS_ENC_ISO8859_7 = 28597,
	RSS_ENC_WINDOWS_1253 = 1253,
	/* Hebrew */
	RSS_ENC_ISO8859_8 = 28598,
	RSS_ENC_WINDOWS_1255 = 1255,
	/* Turkish */
	RSS_ENC_ISO8859_9 = 28599,
	RSS_ENC_WINDOWS_1254 = 1254,
	/* Vietnamese */
	RSS_ENC_WINDOWS_1258 = 1258
	
} RSS_Encoding;

/** Creates new char* (with malloc) from RSS_char*) */
char* RSS_str2char(const RSS_char* str);
/** Creates new wchar_t* (with malloc) from RSS_char*) */
RSS_char* char2RSS_str(const char* str, RSS_Encoding enc);

#endif
