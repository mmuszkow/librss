#ifndef __RSS_ITEM_H__
#define __RSS_ITEM_H__

typedef struct RSS_Item RSS_Item;

/** For author tag */
typedef struct RSS_Author
{
	/** Required.  human-readable name for the person */
	RSS_char*	name;
	
	/** Optional. IRI associated with the person */
	RSS_char*	uri;
	
	/** Optional. E-mail address associated with the person */
	RSS_char*	email;

} RSS_Author;

/** Represents RSS <item> or ATOM <entry> Element */
struct RSS_Item
{
	/** RSS Required. Defines the title of the item */
	RSS_char*		title;

	/** RSS Required. Describes the item */
	RSS_char*		description;

	/** RSS Required. Defines the hyperlink to the item */
	RSS_char*		link;

	/** RSSOptional. Defines a unique identifier for the item */
	RSS_char*		guid;

	/** RSS Optional. Defines the last-publication date for the item */
	time_t			pubDate;

	/** RSS Optional. Specifies the e-mail address to the author of the item 
		Atom Required. Specifies info about author of the item */
	RSS_Author*		author;

	/** RSS Optional. Defines one or more categories the item belongs to */
	RSS_char*		category;

	/** RSS Optional. Allows an item to link to comments about that item */
	RSS_char*		comments;

	/** RSS Optional. Specifies a third-party source for the item */
	RSS_char*		source;

	/** RSS Optional. Specifies a third-party source URL for the item */
	RSS_char*		sourceUrl;

	/** Atom Optional. Notifies about copyrighted material */
	RSS_char*		copyright;

	/** RSS Users data */
	void*			userData;

	/** Next item */
	RSS_Item*		next;

};

#endif
