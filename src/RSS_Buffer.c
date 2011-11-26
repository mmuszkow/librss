#include "RSS.h"
#include "RSS_Buffer.h"

#ifndef _WIN32
# include <iconv.h> /* iconv is not working with wchar_t and UTF-16 on Windows */
#endif

RSS_Buffer* RSS_create_buffer()
{
	RSS_Buffer* buffer;

	buffer = (RSS_Buffer*)malloc(sizeof(RSS_Buffer));
	buffer->str = (RSS_char*)malloc(RSS_BUFFER_INITIAL_SIZE * sizeof(RSS_char));
	buffer->str[0] = 0;
	buffer->reserved = RSS_BUFFER_INITIAL_SIZE;
	buffer->len = 0;

	return buffer;
}

void RSS_reserve_buffer(RSS_Buffer* buffer)
{
	RSS_char* tmp;

	if(buffer->len > 0)
	{
		tmp = buffer->str;
		buffer->str = (RSS_char*)malloc((buffer->reserved << 1) * sizeof(RSS_char));
		memcpy(buffer->str, tmp, buffer->len * sizeof(RSS_char));
		free(tmp);
		buffer->reserved <<= 1;
	}
	else
	{
		free(buffer->str);
		buffer->str = (RSS_char*)malloc((buffer->reserved<<1) * sizeof(RSS_char));
		buffer->reserved <<= 1;
		buffer->str[0] = 0;
	}
}

void RSS_add_buffer(RSS_Buffer* buffer, RSS_char ch)
{
	if(buffer->len >= buffer->reserved - 1)
		RSS_reserve_buffer(buffer);

	buffer->str[buffer->len] = ch;
	buffer->str[++(buffer->len)] = 0;
}

void RSS_clear_buffer(RSS_Buffer* buffer)
{
	buffer->len = 0;
	buffer->str[0] = 0;
}

void RSS_free_buffer(RSS_Buffer* buffer)
{
	free(buffer->str);
	free(buffer);
}

char* RSS_str2char(const RSS_char* str)
{
#ifdef RSS_USE_WSTRING
# ifdef _WIN32
	char*	ret;
	int		len;

	len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, 0, 0);
	ret = (char*)malloc(len + 1);	
	WideCharToMultiByte(CP_ACP, 0, str, -1, ret, len, 0, 0);
	return ret;
# else
#  error "Not implemented"
# endif
#else
	return strdup(str);
#endif
}

RSS_char* char2RSS_str(const char* str, RSS_Encoding enc)
{
#ifdef RSS_USE_WSTRING
# ifdef _WIN32
	wchar_t*	ret;
	int			len;

	len = MultiByteToWideChar((UINT)enc, 0, str, -1, NULL, 0);
	ret = (wchar_t*)malloc((len + 1)* sizeof(wchar_t));
	MultiByteToWideChar(enc, 0, str, -1, ret, len);
	return ret;
# else
#  error "Not implemented"
# endif	
#else
# ifdef RSS_USE_WSTRING
#  error "Not implemented"
# else
    iconv_t conv_desc;
    char*   enc_name;
    char*   inpos, *outpos;
    char*   utf8;
    size_t  len;
    size_t  utf8len;

    len = strlen(str);
    if(len == 0)
        return NULL;
    
    if(enc == RSS_ENC_UTF8)
        return strdup(str);
    
    if((enc_name = RSS_get_encoding_name(enc)) == NULL)
        return NULL;
        
    conv_desc = iconv_open("UTF-8", enc_name);
    if ((int) conv_desc == -1)
        return NULL;
        
    utf8len = len << 1;
    utf8 = (char*)malloc(utf8len);
    
    inpos = (char*)str;
    outpos = utf8;

    if(iconv(conv_desc, &inpos, &len, &outpos, &utf8len) == (size_t) -1)
    {
        free(utf8);
        iconv_close(conv_desc);
        return NULL;
    }
    *outpos = 0;
    
    iconv_close(conv_desc);
    
    return utf8;
# endif
#endif
}

RSS_char* RSS_get_encoding_name(RSS_Encoding enc)
{
    switch(enc)
    {
        case RSS_ENC_UTF8: return RSS_text("UTF-8");
    	case RSS_ENC_ISO8859_1: return RSS_text("ISO88591");
        case RSS_ENC_WINDOWS_1252: return RSS_text("WINDOWS-1252");
        case RSS_ENC_ISO8859_2: return RSS_text("ISO88592");
        case RSS_ENC_WINDOWS_1250: return RSS_text("WINDOWS-1250");
        case RSS_ENC_ISO8859_3: return RSS_text("ISO88593");
        case RSS_ENC_ISO8859_4: return RSS_text("ISO88594");
        case RSS_ENC_WINDOWS_1257: return RSS_text("WINDOWS-1257");
        case RSS_ENC_ISO8859_5: return RSS_text("ISO88595");
        case RSS_ENC_WINDOWS_1251: return RSS_text("WINDOWS-1251");
        case RSS_ENC_ISO8859_6: return RSS_text("ISO88596");
        case RSS_ENC_WINDOWS_1256: return RSS_text("WINDOWS-1256");
        case RSS_ENC_ISO8859_7: return RSS_text("ISO88597");
        case RSS_ENC_WINDOWS_1253: return RSS_text("WINDOWS-1253");
        case RSS_ENC_ISO8859_8: return RSS_text("ISO88598");
        case RSS_ENC_WINDOWS_1255: return RSS_text("WINDOWS-1255");
        case RSS_ENC_ISO8859_9: return RSS_text("ISO88599");
        case RSS_ENC_WINDOWS_1254: return RSS_text("WINDOWS-1254");
        case RSS_ENC_WINDOWS_1258: return RSS_text("WINDOWS-1258");
        default: return NULL;
    }
}

char* RSS_my_strdup(const char* str)
{
    size_t  n;
    char*   dup;
    
    n = strlen(str) + 1;
    dup = (char*)malloc(n);
    
    if(dup)
        strcpy(dup, str);
    
    return dup;
}

int RSS_my_strncasecmp(const char* s1, const char* s2, size_t n)
{
    unsigned char   c1,c2;
    size_t          i;
    
    i = 0;
    do {
        c1 = *s1++;
        c2 = *s2++;
        c1 = (unsigned char)tolower((unsigned char)c1);
        c2 = (unsigned char)tolower((unsigned char)c2);
    }
    while((c1 == c2) && (c1 != '\0') && (i++ < n));
    
    return (int)(c1-c2);
}

