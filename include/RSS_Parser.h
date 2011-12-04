#ifndef __RSS_PARSER_H__
#define __RSS_PARSER_H__

#include "RSS_Buffer.h"
#include "RSS_Node.h"
#include "RSS_Stack.h"

/** SGML parser state */
typedef enum RSS_Parser_state
{
	/** Finding "<" tag */
	FINDING_START_TAG,
	/** We are inside tag and we try to determine it's type (opening, DTD, closing, encoding) */
	TAG_TYPE,
	/** Saving opening tag name */
	OPENING_TAG_NAME,
	/** Text content inside tag */
	TAG_TEXT,
	/** Saving closing tag name */
	CLOSING_TAG_NAME,
	/** We are checking if there is an attribute or space after tag's name (there can be many spaces) */
	ATTRIBUTE_START,
	/** Reading attribute name */
	ATTRIBUTE_NAME,
	/** Checking if attribute value starts with " */
	FINDING_QUOTE,
	/** Reading attribute value in "" */
	INSIDE_QUOTES,
	/** Reading attribute value in '' */
	INSIDE_COMMAS,
	/** Comments starts, we are waiting for second "-" */
	COMMENT_START,
	/** Inside comment, skip all the text */
	INSIDE_COMMENT,
	/** Waiting for second -, if other character it means it was just a single - inside comment */
	COMMENT_END,
	/** Waiting for ">" */
	PROPER_END
} RSS_Parser_state;

/** Parses DTD inside SGML */
size_t RSS_parse_DTD(const RSS_char* sgml, size_t pos, size_t length, RSS_Buffer* tagText);

/** Parses encoding info inside SGML */
RSS_Parser_state RSS_parse_encoding(const RSS_char* sgml, size_t* pos);

/** Simple SGML parser */
RSS_Node* RSS_create_sgml_tree(const RSS_char* sgml, RSS_error_handler handler);

/** Determines encoding of the xml document content in char* */
RSS_Encoding RSS_determine_encoding(const char* sgml);

#endif /* __RSS_PARSER_H__ */
