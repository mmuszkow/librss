#include "RSS.h"

#include "RSS_Parser.h"
#include "RSS_Http.h"
#include "RSS_Date.h"

time_t RSS_parse_RFC822_Date(const RSS_char* str, RSS_error_handler handler)
{
	RSS_char*	tmp;
	RSS_char*	timezone;
	RSS_char	copy[32];
	size_t		length;
	struct tm	timeinfo;
	int			i;
	size_t		year_offset;
	time_t		timezone_diff;

	if(!str)
		return 0;

	/* strip day name, useless */
	for(i=0; i<7; i++)
	{
	    /* double parentheses to avoid compiler warning */
		if((tmp = RSS_strstr(str, days_names[i])))
			break;
	}
	if(tmp) tmp += 3; else tmp = (RSS_char*)str;
	if(tmp[0] == RSS_text(',')) tmp++;
	if(tmp[0] == RSS_text(' ')) tmp++;

	if(RSS_strlen(tmp) > 31)
	{
		if(handler)
			handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC822 date, wrong length"), RSS_NO_POS_INFO);
		return 0;
	}

	RSS_strcpy(copy, tmp);
	length = RSS_strlen(copy);

	if(length < 20)
	{
		if(handler)
			handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC822 date, wrong length"), RSS_NO_POS_INFO);
		return 0;
	}

	copy[2] = 0; /* day */
	copy[6] = 0; /* named month */
	
	year_offset = 0;
	if(copy[9] != RSS_text(' ')) /* 2 or 4 digit year, not in standard but very popular */
		year_offset += 2;
	copy[9+year_offset] = 0; /* year */
	copy[12+year_offset] = 0; /* hour */
	copy[15+year_offset] = 0; /* min */
	copy[18+year_offset] = 0; /* sec */

	memset(&timeinfo, 0, sizeof(struct tm));
	timeinfo.tm_mday = RSS_atoi(&copy[0]);
	
	/* parsing month name */
	for(i=0; i<12; i++)
	{
		if(RSS_strcmp(&copy[3], months_names[i])==0) 
		{
			timeinfo.tm_mon = i;
			break;
		}
	}
	/* wrong month name */
	if(i == 12)
	{
		if(handler)
			handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC822 date, wrong month name"), RSS_NO_POS_INFO);
		return 0;
	}

	timeinfo.tm_year = RSS_atoi(&copy[7]) - 1900;
	timeinfo.tm_hour = RSS_atoi(&copy[10+year_offset]);
	timeinfo.tm_min = RSS_atoi(&copy[13+year_offset]);
	timeinfo.tm_sec = RSS_atoi(&copy[16+year_offset]);

	timezone_diff = 0;
	if(length > 19 + year_offset)
	{
		timezone = &(copy[19+year_offset]);

		if(timezone[0] == RSS_text('+') || timezone[0] == RSS_text('-'))
		{
			if(length >= 22 + year_offset)
			{
				RSS_char	hour[3], min[3];
				int			dh, dm;
				hour[0] = timezone[1]; hour[1] = timezone[2]; hour[2] = 0;
				min[0] = timezone[3]; min[1] = timezone[4]; min[2] = 0;
				dh = RSS_atoi(hour);
				dm = RSS_atoi(min);
				timezone_diff = dh * 3600 + dm * 60;
				if(timezone[0] == RSS_text('-'))
					timezone_diff = -timezone_diff;
			}
		}
		else
		{
			/* named difference */
			for(i=0; i<RSS_TIMEZONES_LEN; i++)
			{
				if(RSS_strcmp(timezone, timezones[i].name) == 0)
				{
					timezone_diff = timezones[i].diff * 3600;
					break;
				}
			}
			if(i == RSS_TIMEZONES_LEN)
			{
				if(handler)
					handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC822 date, wrong timezone name"), RSS_NO_POS_INFO);
				return 0;
			}
		}
	}

	return mktime(&timeinfo) + timezone_diff;
}

time_t RSS_parse_RFC3339_Date(const RSS_char* str, RSS_error_handler handler)
{
	struct tm	timeinfo;
	RSS_char		copy[21] = {0};

	if(!str)
		return 0;
	
	if(RSS_strlen(str) != 20)
	{
		if(handler)
			handler(RSS_EL_WARNING, RSS_text("Failed to parse RFC3339 Date, wrong length"), RSS_NO_POS_INFO);
		return 0;
	}
	
	memset(&timeinfo, 0, sizeof(struct tm));
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
	if(feed->author)
	{
		if(feed->author->name)
			free(feed->author->name);

		if(feed->author->uri)
			free(feed->author->uri);

		if(feed->author->email)
			free(feed->author->email);

		free(feed->author);
	}

	if(feed->category)
		free(feed->category);

	if(feed->copyright)
		free(feed->copyright);

	if(feed->description)
		free(feed->description);

	if(feed->docs)
		free(feed->docs);

	if(feed->generator)
		free(feed->generator);

	if(feed->id)
		free(feed->id);

	if(feed->image)
	{
		if(feed->image->url)
			free(feed->image->url);

		if(feed->image->title)
			free(feed->image->title);

		if(feed->image->link)
			free(feed->image->link);

		if(feed->image->description)
			free(feed->image->description);

		free(feed->image);
	}

	if(feed->language)
		free(feed->language);

	if(feed->link)
		free(feed->link);

	if(feed->managingEditor)
		free(feed->managingEditor);

	if(feed->title)
		free(feed->title);

	if(feed->webMaster)
		free(feed->webMaster);

	/* Free channel items */
	item = feed->items;
	while(item)
	{
		if(item->author)
		{
			if(item->author->name)
				free(item->author->name);

			if(item->author->uri)
				free(item->author->uri);

			if(item->author->email)
				free(item->author->email);

			free(item->author);
		}

		if(item->category)
			free(item->category);

		if(item->comments)
			free(item->comments);

		if(item->copyright)
			free(item->copyright);

		if(item->description)
			free(item->description);

		if(item->guid)
			free(item->guid);

		if(item->link)
			free(item->link);

		if(item->source)
			free(item->source);

		if(item->sourceUrl)
			free(item->sourceUrl);

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
			if(!feed->title && RSS_strcmp(node->name, RSS_text("title")) == 0)
				feed->title = RSS_strdup(node->value);

			else if(!feed->description && RSS_strcmp(node->name, RSS_text("description")) == 0)
				feed->description = RSS_strdup(node->value);

			else if(!feed->link && RSS_strcmp(node->name, RSS_text("link")) == 0)
				feed->link = RSS_url_decode(node->value);

			else if(!feed->category && RSS_strcmp(node->name, RSS_text("category")) == 0)
				feed->category = RSS_strdup(node->value);

			else if(!feed->copyright && RSS_strcmp(node->name, RSS_text("copyright")) == 0)
				feed->copyright = RSS_strdup(node->value);

			else if(!feed->docs && RSS_strcmp(node->name, RSS_text("docs")) == 0)
				feed->docs = RSS_strdup(node->value);

			else if(!feed->generator && RSS_strcmp(node->name, RSS_text("generator")) == 0)
				feed->generator = RSS_strdup(node->value);

			else if(!feed->image && RSS_strcmp(node->name, RSS_text("image")) == 0 && node->children)
			{
				RSS_Node*	child;

				feed->image = (RSS_Image*)malloc(sizeof(RSS_Image));
				memset(feed->image, 0, sizeof(RSS_Image));
				feed->image->width = 88;
				feed->image->height = 31;

				child = node->children;
				while(child)
				{
					if(RSS_strcmp(child->name, RSS_text("url")) == 0)
						feed->image->url = RSS_strdup(child->value);
					else if(RSS_strcmp(child->name, RSS_text("title")) == 0)
						feed->image->title = RSS_strdup(child->value);
					else if(RSS_strcmp(child->name, RSS_text("link")) == 0)
						feed->image->link = RSS_strdup(child->value);
					else if(child->value && RSS_strcmp(child->name, RSS_text("width")) == 0)
						feed->image->width = RSS_atoi(child->value);
					else if(child->value && RSS_strcmp(child->name, RSS_text("height")) == 0)
						feed->image->height = RSS_atoi(child->value);
					else if(RSS_strcmp(child->name, RSS_text("description")) == 0)
						feed->image->description = RSS_strdup(child->value);
					child = child->neighbour;
				}
			}

			else if(!feed->language && RSS_strcmp(node->name, RSS_text("language")) == 0)
				feed->language = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("lastBuildDate")) == 0)
				feed->lastBuildDate = RSS_parse_RFC822_Date(node->value, handler);

			else if(!feed->managingEditor && RSS_strcmp(node->name, RSS_text("managingEditor")) == 0)
				feed->managingEditor = RSS_strdup(node->value);

			else if(RSS_strcmp(node->name, RSS_text("pubDate")) == 0)
				feed->pubDate = RSS_parse_RFC822_Date(node->value, handler);

			else if(RSS_strcmp(node->name, RSS_text("ttl")) == 0)
				feed->ttl = RSS_atoi(node->value);

			else if(!feed->webMaster && RSS_strcmp(node->name, RSS_text("webMaster")) == 0)
				feed->webMaster = RSS_strdup(node->value);
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
					if(!item->title && RSS_strcmp(itemNode->name, RSS_text("title")) == 0)
						item->title = RSS_strdup(itemNode->value);
					else if(!item->description && RSS_strcmp(itemNode->name, RSS_text("description")) == 0)
						item->description = RSS_strdup(itemNode->value);
					else if(!item->link && RSS_strcmp(itemNode->name, RSS_text("link")) == 0)
						item->link = RSS_url_decode(itemNode->value);
					else if(!item->author && RSS_strcmp(itemNode->name, RSS_text("author")) == 0)
					{
						item->author = (RSS_Author*)malloc(sizeof(RSS_Author));
						memset(item->author, 0, sizeof(RSS_Author));
						item->author->email = RSS_strdup(itemNode->value);
					}
					else if(!item->category && RSS_strcmp(itemNode->name, RSS_text("category")) == 0)
						item->category = RSS_strdup(itemNode->value);
					else if(!item->comments && RSS_strcmp(itemNode->name, RSS_text("comments")) == 0)
						item->comments = RSS_strdup(itemNode->value);
					else if(!item->guid && RSS_strcmp(itemNode->name, RSS_text("guid")) == 0)
					{
						RSS_Attribute*	attrib;

						item->guid = RSS_strdup(itemNode->value);
						attrib = itemNode->attribute;
						while(attrib)
						{
							if(RSS_strcmp(attrib->name, RSS_text("isPermaLink")) == 0 && /* guid can be also a link */
								RSS_strcmp(attrib->value, RSS_text("true")) == 0)
							{
								if(!item->link && itemNode->value)
									item->link = RSS_url_decode(itemNode->value);
								break;
							}
							attrib = attrib->next;
						}
					}
					else if(RSS_strcmp(itemNode->name, RSS_text("pubDate")) == 0)
						item->pubDate = RSS_parse_RFC822_Date(itemNode->value, handler);
					else if(!item->source && RSS_strcmp(itemNode->name, RSS_text("source")) == 0)
					{
						RSS_Attribute*	attrib;

						item->source = RSS_strdup(itemNode->value);
						attrib = itemNode->attribute;
						while(attrib)
						{
							if(RSS_strcmp(attrib->name, RSS_text("url")) == 0)
							{
								if(!item->sourceUrl)
									item->sourceUrl = RSS_url_decode(attrib->value);
								break;
							}
							attrib = attrib->next;
						}
					}
					else if(RSS_strcmp(itemNode->name, RSS_text("pubDate")) == 0)
						item->pubDate = RSS_parse_RFC822_Date(itemNode->value, handler);

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
 				feed->items = last;
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

		/* parsing general info */
		if(!feed->title && RSS_strcmp(without_namespace, RSS_text("title")) == 0 && node->value)
		{
			RSS_Attribute*	attrib;
			int				alternate = 0;

			feed->title = RSS_strdup(node->value);
			attrib = node->attribute;
			while(attrib)
			{
				if(RSS_strcmp(attrib->name, RSS_text("alternate")) == 0)
					alternate = 1;
				else if(RSS_strcmp(attrib->name, RSS_text("href")) == 0 && 
						alternate == 1 && !feed->link)
					feed->link = RSS_url_decode(attrib->value);
				attrib = attrib->next;
			}
		}
		else if(!feed->id && RSS_strcmp(without_namespace, RSS_text("id")) == 0)
			feed->id = RSS_strdup(node->value);
		else if(!feed->description && RSS_strcmp(without_namespace, RSS_text("subtitle")) == 0)
			feed->description = RSS_strdup(node->value);
		else if(RSS_strcmp(without_namespace, RSS_text("updated")) == 0)
			feed->lastBuildDate = RSS_parse_RFC3339_Date(node->value, handler);
		else if(!feed->category && RSS_strcmp(without_namespace, RSS_text("category")) == 0)
			feed->category = RSS_strdup(node->value);
		else if(!feed->generator && RSS_strcmp(without_namespace, RSS_text("generator")) == 0)
			feed->generator = RSS_strdup(node->value);
		else if(!feed->image && RSS_strcmp(without_namespace, RSS_text("logo")) == 0)
		{
			feed->image = (RSS_Image*)malloc(sizeof(RSS_Image));
			memset(feed->image, 0, sizeof(RSS_Image));
			feed->image->url = RSS_strdup(node->value);
		}
		else if(!feed->author && RSS_strcmp(without_namespace, RSS_text("author")) == 0 && node->children)
		{
			RSS_Node*	child;

			feed->author = (RSS_Author*)malloc(sizeof(RSS_Author));
			memset(feed->author, 0, sizeof(RSS_Author));

			child = node->children;
			while(child)
			{
				if(RSS_strcmp(child->name, RSS_text("name")) == 0)
					feed->author->name = RSS_strdup(child->value);
				else if(RSS_strcmp(child->name, RSS_text("uri")) == 0)
					feed->author->uri = RSS_strdup(child->value);
				else if(RSS_strcmp(child->name, RSS_text("email")) == 0)
					feed->author->email = RSS_strdup(child->value);
				child = child->neighbour;
			}
		}
		else if(!feed->copyright && RSS_strcmp(without_namespace, RSS_text("rights")) == 0)
			feed->copyright = RSS_strdup(node->value);
		else if(!feed->category && RSS_strcmp(without_namespace, RSS_text("category")) == 0)
			feed->category = RSS_strdup(node->value);

		/* Parsing items info */
		else if(RSS_strcmp(without_namespace, RSS_text("entry")) == 0)
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

					if(!item->title && RSS_strcmp(without_namespace, RSS_text("title")) == 0)
						item->title = RSS_strdup(itemNode->value);
					else if(!item->guid && RSS_strcmp(without_namespace, RSS_text("id")) == 0)
						item->guid = RSS_strdup(itemNode->value);
					else if(item->pubDate == 0 && 
						(RSS_strcmp(without_namespace, RSS_text("updated")) == 0 || RSS_strcmp(without_namespace, RSS_text("published")) == 0))
						item->pubDate = RSS_parse_RFC822_Date(itemNode->value, handler);
					else if(!item->description && RSS_strcmp(without_namespace, RSS_text("content")) == 0)
						item->description = RSS_strdup(itemNode->value);
					else if(!item->author && RSS_strcmp(without_namespace, RSS_text("author")) == 0 && itemNode->children)
					{
						RSS_Node*	child;

						item->author = (RSS_Author*)malloc(sizeof(RSS_Author));
						memset(item->author, 0, sizeof(RSS_Author));

						child = itemNode->children;
						while(child)
						{
							if(RSS_strcmp(child->name, RSS_text("name")) == 0)
								item->author->name = RSS_strdup(child->value);
							else if(RSS_strcmp(child->name, RSS_text("uri")) == 0)
								item->author->uri = RSS_strdup(child->value);
							else if(RSS_strcmp(child->name, RSS_text("email")) == 0)
								item->author->email = RSS_strdup(child->value);
							child = child->neighbour;
						}
					}
					else if(!item->copyright && RSS_strcmp(without_namespace, RSS_text("rights")) == 0)
						item->copyright = RSS_strdup(itemNode->value);
					else if(!item->source && RSS_strcmp(without_namespace, RSS_text("source")) == 0)
						item->source = RSS_strdup(itemNode->value);
					else if(!item->category && RSS_strcmp(without_namespace, RSS_text("category")) == 0)
						item->category = RSS_strdup(itemNode->value);

				} /* if(itemNode->value) */

				if(!item->link && RSS_strcmp(without_namespace, RSS_text("link")) == 0)
				{
					RSS_Attribute*	attrib;
					int				alternate = 0;

					attrib = itemNode->attribute;
					while(attrib)
					{
						if(RSS_strcmp(attrib->name, RSS_text("alternate")) == 0)
							alternate = 1;
						else if(RSS_strcmp(attrib->name, RSS_text("href")) == 0 && alternate == 1)
							item->link = RSS_url_decode(attrib->value);
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
 				feed->items = last;
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
