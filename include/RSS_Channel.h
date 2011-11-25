#ifndef __RSS_CHANNEL_H__
#define __RSS_CHANNEL_H__

#include "RSS.h"
#include "RSS_Item.h"

/** Represents RSS <channel> or ATOM <feed> Element */
typedef struct RSS_Channel
{
	/** Required. Defines the title of the channel */
	RSS_char*		title;

	/** Required. Defines the hyperlink to the channel */
	RSS_char*		link;

	/** Required. Describes the channel */
	RSS_char*		description;

	/** Optional. Defines one or more categories for the feed */
	RSS_char*		category; 	

	/** Optional. Register processes to be notified immediately of updates of the feed */
	RSS_char*		cloud; 	

	/** Optional. Notifies about copyrighted material */
	RSS_char*		copyright; 	

	/** Optional. Specifies an URL to the documentation of the format used in the feed */
	RSS_char*		docs;

	/** Optional. Specifies the program used to generate the feed */
	RSS_char*		generator;

	/** Optional. Allows an image to be displayed when aggregators present a feed */
	RSS_char*		image; 	

	/** Optional. Specifies the language the feed is written in */
	RSS_char*		language;

	/** Optional. Defines the last-modified date of the content of the feed */
	time_t			lastBuildDate; 

	/** Optional. Defines the e-mail address to the editor of the content of the feed */
	RSS_char*		managingEditor;

	/** Optional. Defines the last publication date for the content of the feed */
	time_t			pubDate;

	/** Optional. The PICS rating of the feed */
	RSS_char*		rating;

	/** Optional. Specifies the days where aggregators should skip updating the feed */
	RSS_char*		skipDays;

	/** Optional. Specifies the hours where aggregators should skip updating the feed */
	RSS_char*		skipHours; 	

	/** Optional. Specifies a text input field that should be displayed with the feed */
	RSS_char*		textInput; 	

	/** Optional. Specifies the number of minutes the feed can stay cached before refreshing it from the source */
	RSS_u32			ttl; 	

	/** Optional. Defines the e-mail address to the webmaster of the feed */
	RSS_char*		webMaster; 	

	/** Items */
	RSS_Item*		items;

} RSS_Channel;

#endif
