#include "RSS.h"
#include <stdlib.h>
#include <string.h>

static int references = 0;

void RSS_init(void)
{
	if(references == 0)
		xmlInitParser();
	references++;
}

void RSS_deinit(void)
{
	if(references == 1)
		xmlCleanupParser();
	references--;
}

time_t RSS_parse_RFC822_Date(const xmlChar* str)
{
	xmlChar*	tmp;
	xmlChar		copy[25];
	struct tm	timeinfo;

	if (xmlStrlen(str) != 29) /* not enough information */
		return 0;
	tmp = (xmlChar*)xmlStrstr(str, (const xmlChar *)", ");
	if(!tmp) /* no day info */
		return 0;

	tmp = &tmp[2];
	RSS_xmlStrcpy_s(copy, 25, tmp);
	copy[2] = 0;
	copy[6] = 0;
	copy[11] = 0;
	copy[14] = 0;
	copy[17] = 0;
	copy[20] = 0;

	memset(&timeinfo, 0, sizeof(struct tm));
	timeinfo.tm_mday = RSS_xmlAtoi((const xmlChar*)&copy[0]);
	if(xmlStrcmp(&copy[3],(const xmlChar *)"Jan")==0) /* parsing month name, english only */
		timeinfo.tm_mon = 0;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Feb")==0)
		timeinfo.tm_mon = 1;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Mar")==0)
		timeinfo.tm_mon = 2;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Apr")==0)
		timeinfo.tm_mon = 3;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"May")==0)
		timeinfo.tm_mon = 4;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Jun")==0)
		timeinfo.tm_mon = 5;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Jul")==0)
		timeinfo.tm_mon = 6;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Aug")==0)
		timeinfo.tm_mon = 7;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Sep")==0)
		timeinfo.tm_mon = 8;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Oct")==0)
		timeinfo.tm_mon = 9;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Nov")==0)
		timeinfo.tm_mon = 10;
	else if(xmlStrcmp(&copy[3],(const xmlChar *)"Dec")==0)
		timeinfo.tm_mon = 11;
	else /* wrong month name */
		return 0;
	timeinfo.tm_year = RSS_xmlAtoi((const xmlChar*)&copy[7]) - 1900;
	timeinfo.tm_hour = RSS_xmlAtoi((const xmlChar*)&copy[12]);
	timeinfo.tm_min = RSS_xmlAtoi((const xmlChar*)&copy[15]);
	timeinfo.tm_sec = RSS_xmlAtoi((const xmlChar*)&copy[18]);

	return mktime(&timeinfo);
}

time_t RSS_parse_RFC3339_Date(const xmlChar* str)
{
	struct tm	timeinfo;
	xmlChar		copy[21] = {0};
	memset(&timeinfo, 0, sizeof(struct tm));
	if(xmlStrlen(str) != 20)
		return 0;
	RSS_xmlStrcpy_s(copy, 21, str);
	copy[4] = 0; /* remove the unnecessary chars */
	copy[7] = 0;
	copy[10] = 0;
	copy[13] = 0;
	copy[16] = 0;
	copy[19] = 0;

	timeinfo.tm_year = RSS_xmlAtoi((const xmlChar*)&copy[0])-1900;
	timeinfo.tm_mon = RSS_xmlAtoi((const xmlChar*)&copy[5])-1;
	timeinfo.tm_mday = RSS_xmlAtoi((const xmlChar*)&copy[8]);
	timeinfo.tm_hour = RSS_xmlAtoi((const xmlChar*)&copy[11]);
	timeinfo.tm_min = RSS_xmlAtoi((const xmlChar*)&copy[14]);
	timeinfo.tm_sec = RSS_xmlAtoi((const xmlChar*)&copy[17]);
	return mktime(&timeinfo);
}

RSS_char* RSS_create_string(const xmlChar* str)
{
#ifdef RSS_USE_WSTRING
# ifdef _WIN32
	int			len;
	wchar_t*	dst;
 
	len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if(len<=0) 
	{
		return NULL;
	}

	dst = (wchar_t*)malloc((len+1)<<1);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, dst, len);

	return dst;
# else
	size_t		srclen;
	size_t		dstlen;
	size_t		converted;
	wchar_t*	newStr;
	iconv_t		conv;

	if(!str)
		return NULL;

	srclen = strlen(str);
	dstlen = srclen*sizeof(wchar_t);
	newStr = (wchar_t*)malloc(dstlen);

	conv = iconv_open("UTF-8", "UTF-16");
	converted = iconv(conv, (const char**)&str, &srclen, (char**)&newStr, &dstlen);
	iconv_close(conv);

	if(converted != -1)
		return newStr;
	else
	{
		free(newStr);
		return NULL;
	}
# endif /* ifndef _WIN32 */
#else
	if(!str)
		return NULL;

	return (RSS_char*)xmlStrdup(str);
#endif
}

RSS_Feed* RSS_parse_RSS(const xmlNode* node)
{
	RSS_Feed*	feed;
	RSS_Item*	item;
	RSS_Item*	last; /* temporary, for inserting items at the end */
	xmlNode*	itemNode;

	while(node)
	{
		if(node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"channel") == 0)
			break;
		node = node->next;
	}

	if(!node)
		return NULL;

	node = node->children;

	feed = (RSS_Feed*)malloc(sizeof(RSS_Feed));
	memset(feed, 0, sizeof(RSS_Feed));

	feed->type = RSS_TYPE_RSS;
	last = NULL;

	while(node)
	{
		if(node->type == XML_ELEMENT_NODE)
		{
			if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"title") == 0)
					feed->channel.title = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"description") == 0)
					feed->channel.description = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"link") == 0)
					feed->channel.link = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"category") == 0)
					feed->channel.category = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"cloud") == 0)
					feed->channel.cloud = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"copyright") == 0)
					feed->channel.copyright = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"docs") == 0)
					feed->channel.docs = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"generator") == 0)
					feed->channel.generator = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"image") == 0)
					feed->channel.image = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"language") == 0)
					feed->channel.language = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"lastBuildDate") == 0)
					feed->channel.lastBuildDate = RSS_parse_RFC822_Date(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"managingEditor") == 0)
					feed->channel.managingEditor = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"pubDate") == 0)
					feed->channel.pubDate = RSS_parse_RFC822_Date(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"rating") == 0)
					feed->channel.rating = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"skipDays") == 0)
					feed->channel.skipDays = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"skipHours") == 0)
					feed->channel.skipHours = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"textInput") == 0)
					feed->channel.textInput = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"ttl") == 0)
					feed->channel.ttl = RSS_xmlAtoi(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"webMaster") == 0)
					feed->channel.webMaster = RSS_create_string(node->children->content);

			else if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"item") == 0)
			{
				itemNode = node->children;

				item = (RSS_Item*)malloc(sizeof(RSS_Item));
				memset(item, 0, sizeof(RSS_Item));

				while(itemNode)
				{
					if(itemNode->type == XML_ELEMENT_NODE)
					{
						if(
							itemNode->children 
							&& (itemNode->children->type == XML_TEXT_NODE || itemNode->children->type == XML_CDATA_SECTION_NODE)
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"title") == 0)
								item->title = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& (itemNode->children->type == XML_TEXT_NODE || itemNode->children->type == XML_CDATA_SECTION_NODE)
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"description") == 0)
								item->description = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"link") == 0)
								item->link = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"author") == 0)
								item->author = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"category") == 0)
								item->category = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"comments") == 0)
								item->comments = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"enclosure") == 0)
								item->enclosure = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"guid") == 0)
								item->guid = RSS_create_string(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"pubDate") == 0)
								item->pubDate = RSS_parse_RFC822_Date(itemNode->children->content);

						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"source") == 0)
								item->source = RSS_create_string(itemNode->children->content);

						else
						{
							/* TODO: log unknown tag */
						}
					} /* itemNode->type == XML_ELEMENT_NODE */
					itemNode = itemNode->next;
				} /* while(itemNode) */

				if(last) /* if previous exists */
				{
					item->next = NULL;
					last->next = item;
					last = item;
				}
				else /* if first */
				{
					item->next = NULL;
					last = item;
					feed->channel.items = last;
				}
			}
			else
			{
				/* TODO: log unknown tag */
			}
		} /* if(node->type == XML_ELEMENT_NODE) */
		node = node->next;
	} /* while(node) */
	return feed;
}

RSS_Feed* RSS_parse_ATOM(xmlNode* node)
{
	RSS_Feed*	feed;
	RSS_Item*	item;
	xmlNode*	itemNode;

	feed = (RSS_Feed*)malloc(sizeof(RSS_Feed));
	memset(feed, 0, sizeof(RSS_Feed));

	while(node)
	{
		if(node->type == XML_ELEMENT_NODE)
		{
			/* parsing general info */
			if(
				node->children 
				&& node->children->type == XML_TEXT_NODE
				&& xmlStrcmp(node->name, (const xmlChar *)"title") == 0)
					feed->channel.title = RSS_create_string(node->children->content);
			else if(xmlStrcmp(node->name, (const xmlChar *)"link") == 0
				&& node->properties)
			{
				xmlChar *rel;
				xmlChar *href;
				rel = xmlGetProp(node, (const xmlChar *)"rel");
				if(xmlStrcmp(rel, (const xmlChar *)"alternate") == 0)
				{
					href = xmlGetProp(node, (const xmlChar *)"href");
					if(href)
					{
						feed->channel.link = RSS_create_string(href);
						xmlFree(href);
					}
				}
				if(rel)
					xmlFree(rel);
			}

			/* parsing item info */
			else if(xmlStrcmp(node->name, (const xmlChar *)"entry") == 0)
			{
				itemNode = node->children;

				item = (RSS_Item*)malloc(sizeof(RSS_Item));
				memset(item, 0, sizeof(RSS_Item));

				while(itemNode)
				{
					if(itemNode->type == XML_ELEMENT_NODE)
					{
						if(
							itemNode->children 
							&& (itemNode->children->type == XML_TEXT_NODE || itemNode->children->type == XML_CDATA_SECTION_NODE)
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"title") == 0)
								item->title = RSS_create_string(itemNode->children->content);
						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"id") == 0)
								item->guid = RSS_create_string(itemNode->children->content);
						/*else if(iname == "author" && feedNode->children[i]->children[j]->children.size() == 1)
							item.author = toRSS_string(feedNode->children[i]->children[j]->children[0]->value, encoding);*/
						else if(
							xmlStrcmp(itemNode->name, (const xmlChar *)"link") == 0
							&& itemNode->properties)
						{
							xmlChar *rel;
							xmlChar *href;
							rel = xmlGetProp(itemNode, (const xmlChar *)"rel");
							if(xmlStrcmp(rel, (const xmlChar *)"alternate") == 0)
							{
								href = xmlGetProp(itemNode, (const xmlChar *)"href");
								if(href)
								{
									item->link = RSS_create_string(href);
									xmlFree(href);
								}
							}
							if(rel)
								xmlFree(rel);
						}
						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"updated") == 0)
								item->pubDate = RSS_parse_RFC822_Date(itemNode->children->content);
						else if(
							itemNode->children 
							&& itemNode->children->type == XML_TEXT_NODE
							&& xmlStrcmp(itemNode->name, (const xmlChar *)"content") == 0)
								item->description = RSS_create_string(itemNode->children->content);
					}

					itemNode = itemNode->next;
				} /* while(itemNode) */
				item->next = feed->channel.items;
				feed->channel.items = item;
			} /* xmlStrcmp(node->name, (const xmlChar *)"entry") == 0 */
		} /* node->type == XML_ELEMENT_NODE */
		node = node->next;
	}
	return feed;
}

RSS_Feed* RSS_create_feed(const RSS_char* url, xmlStructuredErrorFunc error_handler, void* ctx)
{
	xmlDoc*		doc;
	xmlNode*	root;
	RSS_Feed*	feed;

#ifdef RSS_USE_WSTRING
# ifdef _WIN32
	/* iconv sucks on Windows, so we must use native WINAPI */
	int			len;
	xmlChar*	dst;

	len = WideCharToMultiByte(CP_UTF8, 0, url, -1, NULL, 0, 0, 0);
	if(len<=0) 
	{
		return NULL;
	}
	dst = (xmlChar*)malloc(len+1);
	WideCharToMultiByte(CP_UTF8, 0, url, -1, dst, len, 0, 0);

	if(error_handler)
		xmlSetStructuredErrorFunc(ctx, error_handler);

	doc = xmlReadFile(dst, NULL, 0);

	free(dst);
# else
	size_t		srclen;
	size_t		dstlen;
	size_t		converted;
	xmlChar*	utf8url;
	iconv_t		conv;
	char*		dstptr; /* copy of utf8url, to keep it const */
	
	if(!url)
		return NULL;

	dstlen = wcslen(url);
	srclen = dstlen * sizeof(RSS_char);
	utf8url = (xmlChar*)malloc(dstlen+1);
	dstptr = utf8url;

	conv = iconv_open("UTF-32", "UTF-8");
	converted = iconv(conv, (const char**)&url, &srclen, (char**)&dstptr, &dstlen);
	iconv_close(conv);

	if(error_handler)
		xmlSetStructuredErrorFunc(ctx, error_handler);

	if(converted != -1)
		doc = xmlReadFile(utf8url, NULL, 0);
	else
		doc = NULL;

	free(utf8url);
# endif /* ifndef _WIN32 */
#else
	if(error_handler)
		xmlSetStructuredErrorFunc(ctx, error_handler);
	doc = xmlReadFile(url, NULL, 0);
#endif /* ifndef RSS_USE_WSTRING */

	if(!doc)
	{
		return NULL;
	}

	root = xmlDocGetRootElement(doc);
	if(!root)
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NULL;
	}

	feed = NULL;

	/* root mustn't have more than 1 child */
	if(root->next == NULL && root->type == XML_ELEMENT_NODE)
	{
		if(xmlStrcmp(root->name,(const xmlChar *)"rss") == 0)
			feed = RSS_parse_RSS(root->children);
		else if(xmlStrcmp(root->name,(const xmlChar *)"feed") == 0)
			feed = RSS_parse_ATOM(root->children);
	}

	xmlFreeDoc(doc);
	return feed;
}

void RSS_free_feed(RSS_Feed* feed)
{
	RSS_Item* item;

	if(!feed)
		return;

	/* Free the channel info */
	if(feed->channel.category)
		free(feed->channel.category);

	if(feed->channel.cloud)
		free(feed->channel.cloud);

	if(feed->channel.copyright)
		free(feed->channel.copyright);

	if(feed->channel.description)
		free(feed->channel.description);

	if(feed->channel.docs)
		free(feed->channel.docs);

	if(feed->channel.generator)
		free(feed->channel.generator);

	if(feed->channel.image)
		free(feed->channel.image);

	if(feed->channel.language)
		free(feed->channel.language);

	if(feed->channel.link)
		free(feed->channel.link);

	if(feed->channel.managingEditor)
		free(feed->channel.managingEditor);

	if(feed->channel.rating)
		free(feed->channel.rating);

	if(feed->channel.skipDays)
		free(feed->channel.skipDays);

	if(feed->channel.skipHours)
		free(feed->channel.skipHours);

	if(feed->channel.textInput)
		free(feed->channel.textInput);

	if(feed->channel.webMaster)
		free(feed->channel.webMaster);

	if(feed->channel.skipHours)
		free(feed->channel.skipHours);

	/* Free channel items */
	while(feed->channel.items)
	{
		item = feed->channel.items;

		if(item->author)
			free(item->author);

		if(item->category)
			free(item->category);

		if(item->comments)
			free(item->comments);

		if(item->description)
			free(item->description);

		if(item->enclosure)
			free(item->enclosure);

		if(item->guid)
			free(item->guid);

		if(item->link)
			free(item->link);

		if(item->source)
			free(item->source);

		if(item->title)
			free(item->title);

		/* swap and free */
		item = feed->channel.items->next;
		free(feed->channel.items);
		feed->channel.items = item;
	}

	free(feed);
}
