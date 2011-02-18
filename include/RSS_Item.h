#ifndef __RSS_Item_H__
#define __RSS_Item_H__

/** Represents RSS <item> or ATOM <entry> Element */
typedef struct RSS_Item
{
	/** Required. Defines the title of the item */
	RSS_char*		title;

	/** Required. Describes the item */
	RSS_char*		description;

	/** Required. Defines the hyperlink to the item */
	RSS_char*		link;

	/** Optional. Defines a unique identifier for the item */
	RSS_char*		guid;

	/** Optional. Defines the last-publication date for the item */
	time_t			pubDate;

	/** Optional. Specifies the e-mail address to the author of the item */
	RSS_char*		author;

	/** Optional. Defines one or more categories the item belongs to */
	RSS_char*		category;

	/** Optional. Allows an item to link to comments about that item */
	RSS_char*		comments;

	/** Optional. Allows a media file to be included with the item */
	RSS_char*		enclosure;

	/** Optional. Specifies a third-party source for the item */
	RSS_char*		source;

	/** Pointer to next item */
	struct RSS_Item*	next;

} RSS_Item;

#endif
