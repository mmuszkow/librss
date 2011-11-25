#ifdef _TEST

#include "RSS.h"
#include "RSS_Buffer.h"
#include "RSS_Parser.h"
#include "RSS_Http.h"

#ifdef _DEBUG
# define CRTDBG_MAP_ALLOC
# include <stdlib.h>
# include <crtdbg.h>
#endif

#include <stdio.h>

void test_error_handler(RSS_u32 error_level, const RSS_char* error, size_t pos)
{
	if(error_level == RSS_EL_ERROR)
	{
		if(pos != RSS_NO_POS_INFO)
			RSS_printf(RSS_text("[%d] %s\n"), pos, error);
		else
			RSS_printf(RSS_text("%s\n"), error);
	}
}

void test_time3339(const RSS_char* str_time)
{
	time_t t;

	t = RSS_parse_RFC3339_Date(str_time, test_error_handler);
	if(t)
		RSS_printf(RSS_text("test_time3339 [OK]:\n%s==?\n%s\n"), RSS_ctime(&t), str_time);
	else
		RSS_printf(RSS_text("test_time3339 [FAIL]: %s\n"), str_time);
}

void test_time822(const RSS_char* str_time)
{
	time_t t;

	t = RSS_parse_RFC822_Date(str_time, test_error_handler);
	if(t)
		RSS_printf(RSS_text("test_time822 [OK]:\n%s==?\n%s\n"), RSS_ctime(&t), str_time);
	else
		RSS_printf(RSS_text("test_time822 [FAIL]: %s\n"), str_time);
}

void test_feed(const RSS_char* url)
{
	RSS_Feed* feed;
	
	feed = RSS_create_feed(url, test_error_handler);
	if(feed)
		RSS_printf(L"test_feed [OK]: %s\n", url);
	else
		RSS_printf(L"test_feed [FAIL]: %s\n", url);
	RSS_free_feed(feed);
}

void test_feed_from_str(const RSS_char* str)
{
	RSS_Feed* feed;
	
	feed = RSS_create_feed_from_str(str, test_error_handler);
	if(feed)
		RSS_printf(L"test_feed [OK]\n");
	else
		RSS_printf(L"test_feed [FAIL]\n");
	RSS_free_feed(feed);
}

void test_buffer()
{
	RSS_Buffer* buff;
	int i;

	buff = RSS_create_buffer();
	for(i=0; i<1300; i++)
		RSS_add_buffer(buff, RSS_text('m'));

	if(buff->len != 1300 || buff->reserved != (RSS_BUFFER_INITIAL_SIZE*4))
		RSS_printf(RSS_text("test_buffer [FAIL]\n"));
	else
		RSS_printf(RSS_text("test_buffer [OK]\n"));

	RSS_free_buffer(buff);
}

void test_http(RSS_char* http_address)
{
	char*			page;
	RSS_Url*		url;
	RSS_Http_error	res;

	url = RSS_create_url(http_address);
	if(!url)
	{
		RSS_printf(RSS_text("test_http [FAIL]: url not created\n"));
		return;
	}

	if((res = RSS_http_get_page(url, &page)) != RSS_HTTP_OK)
	{
		RSS_printf(RSS_text("test_http [FAIL]: http error: %d\n"), res);
		RSS_free_url(url);
		return;
	}
	
	free(page);
	RSS_free_url(url);
	RSS_printf(RSS_text("test_http [OK]\n"));
}

int main()
{
	RSS_char	filename[256];
	int			i;
	FILE*		f;

#if defined(_DEBUG) && defined(_MSC_VER)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	test_time3339(RSS_text("2011-02-03T09:56:44Z"));
	test_time3339(RSS_text("2011-02-02T18:56:48Z"));
	test_time3339(RSS_text("2011-01-18T11:11:22Z"));
	test_time3339(RSS_text("2011-01-20T09:45:39Z"));

	test_time822(RSS_text("Sun, 06 Feb 2011 15:46:39 GMT"));
	test_time822(RSS_text("Sun, 06 Feb 2011 15:46:39"));

	test_buffer();

	/*test_http(L"http://forum.k2t.eu/.xml/?type=rss");
	test_http(L"forum.k2t.eu/.xml/?type=rss");
	test_http(L"http://forum.k2t.eu/");
	test_http(L"http://forum.k2t.eu");
	test_http(L"forum.k2t.eu/");
	test_http(L"forum.k2t.eu");

	test_feed(L"http://forum.k2t.eu/.xml/?type=rss");
	test_feed(L"http://www.elektroda.pl/rtvforum/rss.php");
	test_feed(L"http://www.pinkbike.com/pinkbike_xml_feed.php");
	test_feed(L"http://www.filmweb.pl/feed/news/latest");
	test_feed(L"http://feeds.feedburner.com/niebezpiecznik/");*/

	for(i=1;i<=1000;i++) // max 156615, 14341 - stack overflow
	{
		swprintf_s(filename, 255, L"D:\\Projekty_w_C\\librss\\testing_xml\\%d.xml", i);
		_wfopen_s(&f, filename, L"rb");
		if(f) 
		{
			RSS_Encoding	encoding;
			char*			encoded;
			long			file_size;
			int				utf16len;
			wchar_t*		utf16;

			fseek(f, 0, SEEK_END);
			file_size = ftell(f);
			fseek(f, 0, SEEK_SET);

			encoded = (char*)malloc(file_size+1);
			fread(encoded, 1, file_size, f);
			fclose(f);
			encoded[file_size] = 0;

			printf("Testing %d.xml: ", i);
			encoding = RSS_determine_encoding(encoded);
			if(encoding != RSS_ENC_NO_INFO && encoding != RSS_ENC_UNSUPP)
			{
				utf16len = MultiByteToWideChar(encoding, 0, encoded, -1, NULL, 0);
				utf16 = (wchar_t*)malloc(2 * (utf16len+1));
				MultiByteToWideChar(CP_UTF8, 0, encoded, -1, utf16, utf16len);
				
				test_feed_from_str(utf16);
				free(utf16);
			}
			else
			{
				RSS_printf(RSS_text("test_buffer [FAIL]\n"));
				test_error_handler(RSS_EL_ERROR, RSS_text("Unknown encoding"), RSS_NO_POS_INFO);
			}
			free(encoded);
		}
	}

	getch();

	return 0;
}

#endif
