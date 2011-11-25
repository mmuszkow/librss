#include "RSS.h"
#include "RSS_Buffer.h"

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
# error "Not implemented"
#endif
}
