#include "RSS.h"
#include "RSS_Parser.h"
#include "RSS_Buffer.h"

/** Determines encoding of SGML file */
RSS_Encoding RSS_determine_encoding(const char* sgml)
{
	size_t	length, pos;

	length = strlen(sgml);
	pos = 0;

    while(  pos < length && 
            (sgml[pos] == ' ' || sgml[pos] == '\n' || 
            sgml[pos] == '\r' || sgml[pos] == '\t'))
        pos++;
        
    if(pos == length || pos + 1 == length)
        return RSS_ENC_NO_INFO;
        
	/** Check if there is an encoding header */
	if(sgml[pos] != '<' || sgml[pos+1] != '?')
		return RSS_ENC_NO_INFO;

	/* Find encoding= */
	pos = 2;
	while(pos < length && sgml[pos] != '>')
	{
		if(sgml[pos] == 'e')
		{
			/* encoding= */
			if(	pos + 9 < length && 
				(strncasecmp(&sgml[pos], "encoding=", 9) == 0))
			{
				/* Move to the string with encoding name */
				pos += 9;
				if(sgml[pos] == '"' || sgml[pos] == '\'')
					pos++;

				/* UTF-8 */
				if(pos + 5 < length && strncasecmp(&sgml[pos], "utf-8", 5) == 0)
					return RSS_ENC_UTF8;

				/* Iso-8859-? */
				if(pos + 10 < length && strncasecmp(&sgml[pos], "iso-8859-", 9) == 0)
				{
					pos += 9;
					switch(sgml[pos])
					{
					case '1': return RSS_ENC_ISO8859_1;
					case '2': return RSS_ENC_ISO8859_2;
					case '3': return RSS_ENC_ISO8859_3;
					case '4': return RSS_ENC_ISO8859_4;
					case '5': return RSS_ENC_ISO8859_5;
					case '6': return RSS_ENC_ISO8859_6;
					case '7': return RSS_ENC_ISO8859_7;
					case '8': return RSS_ENC_ISO8859_8;
					case '9': return RSS_ENC_ISO8859_9;
					default: return RSS_ENC_UNSUPP;
					}
				}

				/* Windows-? */
				if(pos + 12 < length && strncasecmp(&sgml[pos], "windows-125", 11) == 0)
				{
					pos += 11;
					switch(sgml[pos])
					{
					case '0': return RSS_ENC_WINDOWS_1250;
					case '1': return RSS_ENC_WINDOWS_1251;
					case '2': return RSS_ENC_WINDOWS_1252;
					case '3': return RSS_ENC_WINDOWS_1253;
					case '4': return RSS_ENC_WINDOWS_1254;
					case '5': return RSS_ENC_WINDOWS_1255;
					case '6': return RSS_ENC_WINDOWS_1256;
					case '7': return RSS_ENC_WINDOWS_1257;
					case '8': return RSS_ENC_WINDOWS_1258;
					default: return RSS_ENC_UNSUPP;
					}
				}

				return RSS_ENC_UNSUPP;
			}
		}
		pos++;
	}

	/* UTF-8 is default XML encoding (if no encoding= specified) */
	return RSS_ENC_UTF8;
}


/** Parses DTD inside tag */
void RSS_parse_DTD(const RSS_char* sgml, size_t* pos, size_t length, RSS_Buffer* tagText)
{
	int start_count;

	/* works only with  <![CDATA[text text text]]> */
	if(*pos < length - 8 && RSS_strncmp(&sgml[*pos], RSS_text("![CDATA["), 8) == 0)
	{
		(*pos) += 8;
		while(*pos < length && sgml[*pos] != RSS_text(']'))
		{
			RSS_add_buffer(tagText, sgml[*pos]);
			(*pos)++;
		}
	}

	/* skip rest */
				
	start_count = 1;
	while(*pos < length)
	{
		switch(sgml[*pos])
		{
		case RSS_text('<'):
			start_count++;
			break;
		case RSS_text('>'):
			start_count--;									
			break;
		}
		if(start_count == 0)
			break;
		(*pos)++;
	}
}

/** Simple SGML parser */
RSS_Node* RSS_create_sgml_tree(const RSS_char* sgml, RSS_error_handler handler)
{
	/** Buffers for temporary values */
	RSS_Buffer* tagName, *tagText, *attribName, *attribValue;
	/** Position in text */
	size_t				pos;
	/** Text length */
	size_t				length;
	/** Parser state */
	RSS_Parser_state	state;
	/** Stack with sgml nodes */
	RSS_Stack			stack;
	/** Root node */
	RSS_Node*			root;

	/** Initialization */
	pos = 0;
	length = RSS_strlen(sgml);
	state = FINDING_START_TAG;
	root = RSS_create_node(RSS_text("root"));
	RSS_init_stack(&stack);
	tagName = RSS_create_buffer();
	tagText = RSS_create_buffer();
	attribName = RSS_create_buffer();
	attribValue = RSS_create_buffer();

	RSS_push_stack(&stack, root);
	while(pos < length)
	{
		/** We ommit these characters */
		switch(sgml[pos])
		{
			case RSS_text('\r'):
			case RSS_text('\n'):
			case RSS_text('\t'):
				pos++;
				continue;
		}

		switch(state)
		{
		case FINDING_START_TAG:
			switch(sgml[pos])
			{
			case RSS_text('<'):
				state = TAG_TYPE;
				break;
			case RSS_text('\r'):
			case RSS_text('\n'):
			case RSS_text('\t'):
			case RSS_text(' '):
				break;
			}
			break;
		case TAG_TYPE:
			switch(sgml[pos])
			{
				/** Closing tag */
				case RSS_text('/'):
					state = CLOSING_TAG_NAME; 
					break; 
				/** DTD or comment */
				case RSS_text('!'):
					pos++;
					if(sgml[pos]==RSS_text('-'))
						state = COMMENT_START;
					else
					{
						/* DTD - skip it */
						int start_count;
				
						start_count = 1;
						while(pos < length)
						{
							switch(sgml[pos])
							{
							case RSS_text('<'):
								start_count++;
								break;
							case RSS_text('>'):
								start_count--;									
								break;
							}
							if(start_count == 0)
								break;
							pos++;
						}
						state = FINDING_START_TAG;
					}
					break;
				/** Encoding - we already know it */
				case RSS_text('?'):
					while(sgml[pos] != RSS_text('>')) 
						pos++;
					state = FINDING_START_TAG;
					break;
				/** It's a normal tag, let's get its name */
				default:			
					state = OPENING_TAG_NAME; 					
					RSS_add_buffer(tagName, sgml[pos]); 
			}
			break;
		case TAG_TEXT:
			switch(sgml[pos])
			{
				case RSS_text('<'):
					if(pos + 1 < length && sgml[pos+1] == '!')
					{
						pos++;
						RSS_parse_DTD(sgml, &pos, length, tagText);
						state = TAG_TEXT;
						break;
					}
					else if(tagText->len > 0)
					{
						stack.top->node->value = RSS_strdup(tagText->str);
						RSS_clear_buffer(tagText);
					}
					state = TAG_TYPE;
					break;
				case RSS_text('>'): 
					if(handler)
						handler(RSS_EL_ERROR, RSS_text("\">\" in tag value"), pos);
					goto parse_error;
					break;
				default: 
					RSS_add_buffer(tagText, sgml[pos]);
			}
			break;
		case OPENING_TAG_NAME:
			switch(sgml[pos])
			{
				case RSS_text('>'): 
				{
					RSS_Node* child;

					state = TAG_TEXT;

					child = RSS_node_children_add(stack.top->node, tagName->str);
					RSS_push_stack(&stack, child);
					RSS_clear_buffer(tagName);
					break;
				}
				case RSS_text(' '): 
				{
					RSS_Node* child;
					state = ATTRIBUTE_START;
					child = RSS_node_children_add(stack.top->node, tagName->str);
					RSS_push_stack(&stack, child);
					RSS_clear_buffer(tagName);
					break;
				}
				case RSS_text('/'):
				{
					RSS_Node* child;
					child = RSS_node_children_add(stack.top->node, tagName->str);
					RSS_push_stack(&stack, child);
					if(++pos < length && sgml[pos] == RSS_text('>'))
						state = FINDING_START_TAG;
					else
					{
						if(handler)
							handler(RSS_EL_ERROR, RSS_text("Wrong no-end tag"), pos);
						goto parse_error;
					}
					break;
				}
				default: 
					RSS_add_buffer(tagName, sgml[pos]);
			}
			break;
		case CLOSING_TAG_NAME:
			switch(sgml[pos])
			{
				case RSS_text('>'): 
				{
					state = FINDING_START_TAG;
					if(RSS_strcmp(stack.top->node->name, tagName->str) != 0)
					{
						if(handler)
							handler(RSS_EL_ERROR, RSS_text("Tag closed in wrong order"), pos);
						goto parse_error;
					}
					RSS_pop_stack(&stack);
					RSS_clear_buffer(tagName);
					break;
				}
				case RSS_text(' '): 
					if(handler)
						handler(RSS_EL_ERROR, RSS_text("\" \" in closing tag"), pos);
					goto parse_error;
					break;
				default: 
					RSS_add_buffer(tagName, sgml[pos]);
			}
			break;
		case ATTRIBUTE_START:
			switch(sgml[pos])
			{
				case RSS_text(' '): \

				    break; /* we skip spaces */
				case RSS_text('>'): 
					state = TAG_TEXT; 
					break;
				case RSS_text('/'): /* tags that don't need closing */
					RSS_pop_stack(&stack);
					if(++pos < length && sgml[pos] == RSS_text('>'))
						state = FINDING_START_TAG;
					else
					{
						if(handler)
							handler(RSS_EL_ERROR, RSS_text("Wrong no-end tag"), pos);
						goto parse_error;
					}
					break;
				default: 
					RSS_clear_buffer(attribName);
					RSS_add_buffer(attribName, sgml[pos]);
					state = ATTRIBUTE_NAME;
			}
			break;
		case ATTRIBUTE_NAME:
			switch(sgml[pos])
			{
			case RSS_text('='):
				RSS_clear_buffer(attribValue);
				state = FINDING_QUOTE; 
				break;
			case RSS_text(' '): 
				state = ATTRIBUTE_START; 
				break;
			case RSS_text('>'): 
				state = FINDING_START_TAG; 
				break;
			default: 
				RSS_add_buffer(attribName, sgml[pos]);
			}
			break;
		case FINDING_QUOTE:
			switch(sgml[pos])
			{
				case RSS_text(' '): /* skip spaces */
					break;
				case RSS_text('\''):
					state = INSIDE_COMMAS;
					break;
				case RSS_text('"'):
					state = INSIDE_QUOTES;
					break;
				default: 
					if(handler)
						handler(RSS_EL_ERROR, RSS_text("Attribute value didn't start with quote"), pos);
					goto parse_error;
			}
			break;
		case INSIDE_QUOTES:
			switch(sgml[pos])
			{
			case RSS_text('>'): 
				if(handler)
					handler(RSS_EL_ERROR, RSS_text("Non-closed quote"), pos);
				goto parse_error;
			case RSS_text('"'):
				RSS_add_attribute(stack.top->node, attribName->str, attribValue->str);
				state = ATTRIBUTE_START; 
				break;
			default:
				RSS_add_buffer(attribValue, sgml[pos]);
			}
			break;
		case INSIDE_COMMAS:
			switch(sgml[pos])
			{
			/*case '>': TODO: warning: Non-closed quote */
			case RSS_text('\''): 
				RSS_add_attribute(stack.top->node, attribName->str, attribValue->str);
				state = ATTRIBUTE_START; 
				break;
			default:
				RSS_add_buffer(attribValue, sgml[pos]);
			}
			break;
		case COMMENT_START:
			if(sgml[pos]!=RSS_text('-'))
			{
				if(handler)
					handler(RSS_EL_ERROR, RSS_text("\'-\' in comment expected"), pos);
				goto parse_error;
			}
			else
				state = INSIDE_COMMENT;
			break;
		case INSIDE_COMMENT:
			if(sgml[pos]==RSS_text('-'))
				state = COMMENT_END;
			break;
		case COMMENT_END:
			if(sgml[pos]==RSS_text('-'))
				state = PROPER_END;
			else
				state = INSIDE_COMMENT; /* TODO: warning: - in comment expected */
			break;
		case PROPER_END:
			if(sgml[pos]==RSS_text('>'))
				state = FINDING_START_TAG;
			else
			{
				if(handler)
					handler(RSS_EL_ERROR, RSS_text("Comment tag not closed properly (\'>\' missing)"), pos);
				goto parse_error;
			}
			break;
		}
		pos++;
	}
	RSS_pop_stack(&stack);
	goto free_mem;

parse_error:
	/** Error, return NULL */
	RSS_free_node(root);
	root = NULL;
free_mem:
	/** Free memory */	
	RSS_free_buffer(tagName);
	RSS_free_buffer(tagText);
	RSS_free_buffer(attribName);
	RSS_free_buffer(attribValue);
	RSS_free_stack(&stack);

	return root;
}
