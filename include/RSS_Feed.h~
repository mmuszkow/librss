#ifndef __RSS_FEED_H__
#define __RSS_FEED_H__

#include "RSS.h"
#include "RSS_Item.h"

/** Syndication type */
#define RSS_TYPE_RSS	1
#define RSS_TYPE_ATOM	2

/** For image tag */
typedef struct RSS_Image
{
	/** Required. The URL of a GIF, JPEG or PNG image that represents the channel  */
	RSS_char*	url;
	
	/** Required. Describes the image, it's used in the ALT attribute of the <img> tag */
	RSS_char*	title;
	
	/** Required. is the URL of the site, when the channel is rendered, the image is a link to the site */
	RSS_char*	link;
	
	/** Optional. Image width. Maximum value is 144, default value is 88 */
	RSS_u32		width;
	
	/** Optional. Image height. Maximum value is 400, default value is 31 */
	RSS_u32		height;
	
	/** Optional. contains text that is included in the TITLE attribute of the link formed around the image in the HTML rendering */
	RSS_char*	description;

} RSS_Image;

/** RSS 2.0/Atom 1.0 Feed */
typedef struct RSS_Feed
{
	/** Feed type (RSS/ATOM) */
	RSS_u32			type;

	/** RSS, Atom. Required. Defines the title of the channel */
	RSS_char*		title;

	/** RSS. Required. Defines the hyperlink to the channel */
	RSS_char*		link;

	/** RSS. Required. Describes the channel. Atom. Optional subtitle */
	RSS_char*		description;

	/** RSS, Atom. Required. Unique channel identifier */
	RSS_char*		id;

	/** Atom. Required. Author of the feed */
	RSS_Author*		author;

	/** RSS. Optional. Defines one or more categories for the feed */
	RSS_char*		category; 	

	/** RSS. Optional. Notifies about copyrighted material */
	RSS_char*		copyright; 	

	/** RSS. Optional. Specifies an URL to the documentation of the format used in the feed */
	RSS_char*		docs;

	/** RSS. Optional. Specifies the program used to generate the feed */
	RSS_char*		generator;

	/** RSS. Optional. Allows an image to be displayed when aggregators present a feed */
	RSS_Image*		image; 	

	/** RSS. Optional. Specifies the language the feed is written in */
	RSS_char*		language;

	/** RSS. Optional. Defines the last-modified date of the content of the feed */
	time_t			lastBuildDate; 

	/** RSS. Optional. Defines the e-mail address to the editor of the content of the feed */
	RSS_char*		managingEditor;

	/** RSS. Optional. Defines the last publication date for the content of the feed */
	time_t			pubDate;

	/** RSS. Optional. Specifies the number of minutes the feed can stay cached before refreshing it from the source */
	RSS_u32			ttl; 	

	/** RSS. Optional. Defines the e-mail address to the webmaster of the feed */
	RSS_char*		webMaster; 	

	/** Users data */
	void*			userData;

	/** Items */
	RSS_Item*		items;

} RSS_Feed;

#endif
