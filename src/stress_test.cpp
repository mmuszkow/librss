#ifdef _TEST

#include "RSS.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

void XMLCDECL error_handler_function2(void *ctx, xmlErrorPtr error)
{
// do nothing
}

void log_result(int correct[2], int all[2])
{
	FILE* f = NULL;
	fopen_s(&f, "results.txt", "w");
	if(f)
	{
		fprintf(f, "RSS: %d/%d, ATOM: %d/%d\n", correct[0], all[0], correct[1], all[1]);
		fclose(f);
	}
}

int main()
{
	//get_unique();
	//return 0;

	wchar_t			buff[2048];
	std::wstring	type, url;
	int				correct[2] = {0,0};
	int				all[2] = {0,0};
	RSS_Feed*		feed;
	std::wifstream	f("test_list_final.txt");

	while(f.getline(buff, 2048))
	{
		f >> type >> url;
		if(type == L"RSS")
		{
			feed = RSS_create_feed(url.c_str(), error_handler_function2, NULL);
			if(feed)
				correct[0]++;
			RSS_free_feed(feed);
			all[0]++;
		}
		else if(type == L"ATOM")
		{
			feed = RSS_create_feed(url.c_str(), error_handler_function2, NULL);
			if(feed)
				correct[1]++;
			RSS_free_feed(feed);
			all[1]++;
		}
		std::wcout << L'.';
		log_result(correct,all);
	}
	std::wcout << std::endl << 
		L"RSS " << correct[0] << L"/" << all[0] << 
		L" ATOM " << correct[1] << L"/" << all[1] << std::endl;
	f.close();
	return 0;
}

#endif
