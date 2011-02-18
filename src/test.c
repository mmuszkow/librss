#ifdef _TEST

#include "RSS.h"

void test_time3339(const xmlChar* str_time)
{
	time_t t;
	char buff[128] = {0};

	t = RSS_parse_RFC3339_Date(str_time);
	ctime_s(buff, 128, &t);
	buff[strlen(buff)-1] = 0;
	if(t)
		printf("test_time3339[OK]: %s == %s\r\n", buff, str_time);
	else
		printf("test_time3339[FAIL]: %s\r\n", str_time);
}

void test_time822(const xmlChar* str_time)
{
	time_t t;
	char buff[128] = {0};

	t = RSS_parse_RFC822_Date(str_time);
	ctime_s(buff, 128, &t);
	buff[strlen(buff)-1] = 0;
	if(t)
		printf("test_time822[OK]: %s == %s\r\n", buff, str_time);
	else
		printf("test_time822[FAIL]: %s\r\n", str_time);
}

void test_feed(const wchar_t* str)
{
	RSS_Feed* feed;
	
	feed = RSS_create_feed(str, NULL, NULL);
	if(feed)
		wprintf(L"test_feed[OK]: url=%s\r\n", str);
	else
		wprintf(L"test_feed[FAIL]: url=%s\r\n", str);

	RSS_free_feed(feed);
}

int main()
{
	/*test_time3339((const xmlChar*)"2011-02-03T09:56:44Z");
	test_time3339((const xmlChar*)"2011-02-02T18:56:48Z");
	test_time3339((const xmlChar*)"2011-01-18T11:11:22Z");
	test_time3339((const xmlChar*)"2011-01-20T09:45:39Z");

	test_time822((const xmlChar*)"Sun, 06 Feb 2011 15:46:39 GMT");
	test_time822((const xmlChar*)"Sun, 06 Feb 2011 15:46:39");*/

	test_feed(L"http://feeds.feedburner.com/niebezpiecznik/");

	return 0;
}

#endif
