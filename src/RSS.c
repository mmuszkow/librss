#include "RSS.h"

#include "RSS_Parser.h"
#include "RSS_Http.h"

time_t RSS_parse_RFC822_Date(const RSS_char* str, RSS_error_handler handler)
{
	RSS_char*	tmp;
	RSS_char		copy[25];
	struct tm	timeinfo;

	if (RSS_strlen(str) != 29)
	{
		if(handler)
			handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC822 Date, wrong length"), RSS_NO_POS_INFO);
		return 0;
	}

	tmp = RSS_strstr(str, RSS_text(", "));
	if(!tmp)
	{
		if(handler)
			handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC822 Date, no day information"), RSS_NO_POS_INFO);
		return 0;
	}

	tmp = &tmp[2];
	RSS_strcpy(copy, tmp);
	copy[2] = 0;
	copy[6] = 0;
	copy[11] = 0;
	copy[14] = 0;
	copy[17] = 0;
	copy[20] = 0;

	memset(&timeinfo, 0, sizeof(struct tm));
	timeinfo.tm_mday = RSS_atoi(&copy[0]);
	if(RSS_strcmp(&copy[3],RSS_text("Jan"))==0) /* parsing month name, english only */
		timeinfo.tm_mon = 0;
	else if(RSS_strcmp(&copy[3],RSS_text("Feb"))==0)
		timeinfo.tm_mon = 1;
	else if(RSS_strcmp(&copy[3],RSS_text("Mar"))==0)
		timeinfo.tm_mon = 2;
	else if(RSS_strcmp(&copy[3],RSS_text("Apr"))==0)
		timeinfo.tm_mon = 3;
	else if(RSS_strcmp(&copy[3],RSS_text("May"))==0)
		timeinfo.tm_mon = 4;
	else if(RSS_strcmp(&copy[3],RSS_text("Jun"))==0)
		timeinfo.tm_mon = 5;
	else if(RSS_strcmp(&copy[3],RSS_text("Jul"))==0)
		timeinfo.tm_mon = 6;
	else if(RSS_strcmp(&copy[3],RSS_text("Aug"))==0)
		timeinfo.tm_mon = 7;
	else if(RSS_strcmp(&copy[3],RSS_text("Sep"))==0)
		timeinfo.tm_mon = 8;
	else if(RSS_strcmp(&copy[3],RSS_text("Oct"))==0)
		timeinfo.tm_mon = 9;
	else if(RSS_strcmp(&copy[3],RSS_text("Nov"))==0)
		timeinfo.tm_mon = 10;
	else if(RSS_strcmp(&copy[3],RSS_text("Dec"))==0)
		timeinfo.tm_mon = 11;
	else /* wrong month name */
		return 0;
	timeinfo.tm_year = RSS_atoi(&copy[7]) - 1900;
	timeinfo.tm_hour = RSS_atoi(&copy[12]);
	timeinfo.tm_min = RSS_atoi(&copy[15]);
	timeinfo.tm_sec = RSS_atoi(&copy[18]);

	return mktime(&timeinfo);
}

time_t RSS_parse_RFC3339_Date(const RSS_char* str, RSS_error_handler handler)
{
	struct tm	timeinfo;
	RSS_char		copy[21] = {0};
	
	memset(&timeinfo, 0, sizeof(struct tm));
	if(RSS_strlen(str) != 20)
	{
		if(handler)
			handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC3339 Date, wrong length"), RSS_NO_POS_INFO);
		return 0;
	}
	
	RSS_strcpy(copy, str);
	copy[4] = 0; /* remove the unnecessary chars */
	copy[7] = 0;
	copy[10] = 0;
	copy[13] = 0;
	copy[16] = 0;
	copy[19] = 0;

	timeinfo.tm_year = RSS_atoi(&copy[0])-1900;
	timeinfo.tm_mon = RSS_atoi(&copy[5])-1;
	timeinfo.tm_mday = RSS_atoi(&copy[8]);
	timeinfo.tm_hour = RSS_atoi(&copy[11]);
	timeinfo.tm_min = RSS_atoi(&copy[14]);
	timeinfo.tm_sec = RSS_atoi(&copy[17]);

	return mktime(&timeinfo);
}

void RSS_free_feed(RSS_Feed* feed)
{
	RSS_Item*	item;
	RSS_Item*	prev;

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

	if(feed->channel.title)
		free(feed->channel.title);

	if(feed->channel.textInput)
		free(feed->channel.textInput);

	if(feed->channel.webMaster)
		free(feed->channel.webMaster);

	if(feed->channel.skipHours)
		free(feed->channel.skipHours);

	/* Free channel items */
	item = feed->channel.items;
	while(item)
	{
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
		
		prev = item;
		item = item->next;
		free(prev);
	}

	free(feed);
}

RSS_Feed* RSS_parse_RSS(const RSS_Node* root, RSS_error_handler handler)
{
	RSS_Feed*	feed; /* Result */
	RSS_Node*	itemNode; /* For items loop */
	RSS_Node*	node; /* For channel loop */
	RSS_Item*	last; /* temporary, for inserting items at the end */

	/* Checking if first tag == channel */
	if(!root->children || RSS_strcmp(root->children->name, RSS_text("channel")))
	{
		if(handler)
			handler(RSS_EL_ERROR, RSS_text("No channel tag found"), RSS_NO_POS_INFO);
		return NULL;
	}

	/* Alloc memory */
	feed = (RSS_Feed*)malloc(sizeof(RSS_Feed));
	memset(feed, 0, sizeof(RSS_Feed));
	feed->type = RSS_TYPE_RSS;
	last = NULL;

	node = root->children->children;

	while(node)
	{
		if(node->value)
		{
			if(RSS_strcmp(node->name, RSS_text("title")) == 0 && !feed->channel.title)
				feed->channel.title = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("description")) == 0 && !feed->channel.description)
				feed->channel.description = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("link")) == 0 && !feed->channel.link)
				feed->channel.link = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("category")) == 0 && !feed->channel.category)
				feed->channel.category = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("cloud")) == 0 && !feed->channel.cloud)
				feed->channel.cloud = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("copyright")) == 0 && !feed->channel.copyright)
				feed->channel.copyright = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("docs")) == 0 && !feed->channel.docs)
				feed->channel.docs = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("generator")) == 0 && !feed->channel.generator)
				feed->channel.generator = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("image")) == 0 && !feed->channel.image)
				feed->channel.image = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("language")) == 0 && !feed->channel.language)
				feed->channel.language = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("lastBuildDate")) == 0)
				feed->channel.lastBuildDate = RSS_parse_RFC822_Date(node->value, handler);

			else if(RSS_strcmp(node->name, RSS_text("managingEditor")) == 0 && !feed->channel.managingEditor)
				feed->channel.managingEditor = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("pubDate")) == 0)
				feed->channel.pubDate = RSS_parse_RFC822_Date(node->value, handler);

			else if(RSS_strcmp(node->name, RSS_text("rating")) == 0 && !feed->channel.rating)
				feed->channel.rating = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("skipDays")) == 0 && !feed->channel.skipDays)
				feed->channel.skipDays = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("skipHours")) == 0 && !feed->channel.skipHours)
				feed->channel.skipHours = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("textInput")) == 0 && !feed->channel.textInput)
				feed->channel.textInput = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("ttl")) == 0)
				feed->channel.ttl = RSS_atoi(node->value);

			else if(RSS_strcmp(node->name, RSS_text("webMaster")) == 0 && !feed->channel.webMaster)
				feed->channel.webMaster = RSS_strdup(node->value);
		} /* if(node->value) */

		else if(RSS_strcmp(node->name, RSS_text("item")) == 0)
		{
			RSS_Item*	item;

			item = (RSS_Item*)malloc(sizeof(RSS_Item));
			memset(item, 0, sizeof(RSS_Item));

			itemNode = node->children;

			while(itemNode)
			{
				if(itemNode->value)
				{
					if(RSS_strcmp(itemNode->name, RSS_text("title")) == 0 && !item->title)
						item->title = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("description")) == 0 && !item->description)
						item->description = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("link")) == 0 && !item->link)
						item->link = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("author")) == 0	&& !item->author)
						item->author = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("category")) == 0 && !item->category)
						item->category = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("comments")) == 0 && !item->comments)
						item->comments = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("enclosure")) == 0 && !item->enclosure)
						item->enclosure = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("guid")) == 0 && !item->guid)
						item->guid = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(itemNode->name, RSS_text("pubDate")) == 0)
						item->pubDate = RSS_parse_RFC822_Date(itemNode->value, handler);
					else if(RSS_strcmp(itemNode->name, RSS_text("source")) == 0 && !item->source)
						item->source = RSS_strdup(itemNode->value);
					else
					{
						/* TODO: log unknown tag */
					}
				} /* if(itemNode->value) */
				itemNode = itemNode->neighbour;

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
		} /* item tag */
		else
		{
			/* TODO: log unknown tag */
		}
		node = node->neighbour;
	} /* while(node) */
	return feed;
}

RSS_Feed* RSS_parse_ATOM(const RSS_Node* root, RSS_error_handler handler)
{
	RSS_Feed*	feed;
	RSS_Node*	itemNode;
	RSS_Node*	node;
	RSS_char*	without_namespace;
	RSS_Item*	last; /* temporary, for inserting items at the end */

	feed = (RSS_Feed*)malloc(sizeof(RSS_Feed));
	memset(feed, 0, sizeof(RSS_Feed));
	feed->type = RSS_TYPE_ATOM;
	last = NULL;

	node = root->children;

	while(node)
	{
		/* strip namespace */
		without_namespace = RSS_strrchr(node->name, RSS_text(':'));
		if(!without_namespace)
			without_namespace = node->name;
		else
			without_namespace += 1;

		if(node->value)
		{
			/* parsing general info */
			if(RSS_strcmp(without_namespace, RSS_text("title")) == 0 && !feed->channel.title)
			{
				RSS_Attribute*	attrib;
				int				alternate = 0;

				feed->channel.title = RSS_strdup(node->value);
				attrib = node->attribute;
				while(attrib)
				{
					if(RSS_strcmp(attrib->name, RSS_text("alternate")) == 0)
						alternate = 1;
					else if(RSS_strcmp(attrib->name, RSS_text("href")) == 0 && 
							alternate == 1 && !feed->channel.link)
						feed->channel.link = RSS_strdup(attrib->value);
					attrib = attrib->next;
				}
			}
		} /* if(node->value) */

		/* Parsing items info */
		if(RSS_strcmp(without_namespace, RSS_text("entry")) == 0)
		{
			RSS_Item*	item;

			item = (RSS_Item*)malloc(sizeof(RSS_Item));
			memset(item, 0, sizeof(RSS_Item));

			itemNode = node->children;
			while(itemNode)
			{
				if(itemNode->value)
				{
					without_namespace = RSS_strrchr(itemNode->name, RSS_text(':'));
					if(!without_namespace)
						without_namespace = itemNode->name;
					else
						without_namespace += 1;

					if(	RSS_strcmp(without_namespace, RSS_text("title")) == 0 && !item->title)
						item->title = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(without_namespace, RSS_text("id")) == 0 && !item->guid)
						item->guid = RSS_strdup(itemNode->value);
					else if(RSS_strcmp(without_namespace, RSS_text("updated")) == 0)
						item->pubDate = RSS_parse_RFC822_Date(itemNode->value, handler);
					else if(RSS_strcmp(without_namespace, RSS_text("content")) == 0 && !item->description)
						item->description = RSS_strdup(itemNode->value);
				} /* if(itemNode->value) */

				if(RSS_strcmp(without_namespace, RSS_text("link")) == 0 && !item->link)
				{
					RSS_Attribute*	attrib;
					int				alternate = 0;

					attrib = itemNode->attribute;
					while(attrib)
					{
						if(RSS_strcmp(attrib->name, RSS_text("alternate")) == 0)
							alternate = 1;
						else if(RSS_strcmp(attrib->name, RSS_text("href")) == 0 && alternate == 1)
							item->link = RSS_strdup(attrib->value);
						attrib = attrib->next;
					}
				}

				itemNode = itemNode->neighbour;
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

		} /* RSS_strcmp(without_namespace, RSS_text("entry") == 0 */

		node = node->neighbour;
	} /* while(node) */
	return feed;
}

RSS_Feed* RSS_create_feed(const RSS_char* http_address, RSS_error_handler handler)
{
	RSS_Url*		url;
	char*			page_content;
	RSS_char*		converted_page;
	RSS_Http_error	http_res;
	RSS_Encoding	enc;
	RSS_Feed*		feed;

	url = RSS_create_url(http_address);
	if(!url)
	{
		if(handler)
			handler(RSS_EL_ERROR, RSS_text("Feed url is not correct"), RSS_NO_POS_INFO);
		return NULL;
	}

    page_content = NULL;
	if((http_res = RSS_http_get_page(url, &page_content)) != RSS_HTTP_OK)
	{
		if(handler)
		{
			switch(http_res)
			{
			case RSS_HTTP_SOCKET:
				handler(RSS_EL_ERROR, RSS_text("HTTP: Could not create socket"), RSS_NO_POS_INFO);
				break;
			case RSS_HTTP_GETHOSTBYNAME:
				handler(RSS_EL_ERROR, RSS_text("HTTP: Could not resolve host"), RSS_NO_POS_INFO);
				break;
			case RSS_HTTP_CONN_FAILED:
				handler(RSS_EL_ERROR, RSS_text("HTTP: Connection failed"), RSS_NO_POS_INFO);
				break;
			case RSS_HTTP_NOT_200:
				handler(RSS_EL_ERROR, RSS_text("HTTP: Code different than 200 returned"), RSS_NO_POS_INFO);
				break;
			default:
				handler(RSS_EL_ERROR, RSS_text("HTTP: Other error"), RSS_NO_POS_INFO);
			}
		}

		RSS_free_url(url);
        if(page_content) /* page could be retrieved but for example http code was 403 */
            free(page_content);

		return NULL;
	}

	enc = RSS_determine_encoding(page_content);
	if(enc == RSS_ENC_NO_INFO || enc == RSS_ENC_UNSUPP)
	{
		if(handler)
			handler(RSS_EL_ERROR, RSS_text("Unknown or unsupported encoding"), RSS_NO_POS_INFO);
		
		RSS_free_url(url);
		free(page_content);
		
		return NULL;
	}

	converted_page = char2RSS_str(page_content, enc);
	if(!converted_page)
	{
	    if(handler)
	        handler(RSS_EL_ERROR, RSS_text("Encoding conversion error"), RSS_NO_POS_INFO);
	    feed = NULL;
    }
    else
    {
    	feed = RSS_create_feed_from_str(converted_page, handler);
       	free(converted_page);
    }
	
	RSS_free_url(url);
	free(page_content);

	return feed;
}

RSS_Feed* RSS_create_feed_from_str(const RSS_char* content, RSS_error_handler handler)
{
	RSS_Node*	root;
	RSS_Feed*	feed;

	root = RSS_create_sgml_tree(content, handler);
	/* Error during parsing */
	if(!root)
		return NULL;

	/* No children */
	if(!root->children)
	{
		if(handler)
			handler(RSS_EL_ERROR, RSS_text("Empty document"), RSS_NO_POS_INFO);
			
        RSS_free_node(root);
    	return NULL;
	}

	if(RSS_strcmp(root->children->name, RSS_text("rss")) == 0)
		feed = RSS_parse_RSS(root->children, handler);
	else if(RSS_strcmp(root->children->name, RSS_text("feed")) == 0)
		feed = RSS_parse_ATOM(root->children, handler);
	else
		feed = NULL;
	
	RSS_free_node(root);

	return feed;
}
