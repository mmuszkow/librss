#ifndef __RSS_Feed_H__
#define __RSS_Feed_H__

#include "RSS.h"
#include "RSS_Channel.h"

/** RSS 2.0 Feed */
typedef struct RSS_Feed
{
	/** Content of the feed */
	RSS_Channel	channel;

	/** Feed type (RSS/ATOM) */
	RSS_u32		type;

} RSS_Feed;

#endif
