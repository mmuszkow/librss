#ifndef __RSS_NODE_H__
#define __RSS_NODE_H__

#include "RSS.h"

/** SGML attribute */
typedef struct RSS_Attribute RSS_Attribute;

struct RSS_Attribute
{
	/** Attribute name */
	RSS_char*		name;
	/** Attribute value */
	RSS_char*		value;
	/** Linked list - next item */
	RSS_Attribute*	next;
};

/** SGML Node */
typedef struct RSS_Node RSS_Node;

struct RSS_Node
{
	/** SGML node name */
	RSS_char*	name;

	/** SGML node text value */
	RSS_char*	value;

	/** Neighbours - first list element */
	RSS_Node*	neighbour;

	/** Neighbours - last list element */
	RSS_Node*	last_neighbour;

	/** Childrens */
	RSS_Node*	children;

	/** Attributes - first list element */
	RSS_Attribute*	attribute;

	/** Attributes - last list element */
	RSS_Attribute*	last_attribute;
};

/** Creates new SGML attribute */
RSS_Attribute* RSS_create_attribute(const RSS_char* name, const RSS_char* value);
/** Adds new attribute to node attribute list */
void RSS_add_attribute(RSS_Node* node, const RSS_char* name, const RSS_char* value);
/** Recursively free memory reserved for attributes */
void RSS_free_attribute(RSS_Attribute* attrib);

/** Creates SGML node */
RSS_Node* RSS_create_node(const RSS_char* name);
/** Creates new node neighbour (same level)*/
RSS_Node* RSS_node_neighbour_add(RSS_Node* node, const RSS_char* name);
/** Creates new node child (level below) */
RSS_Node* RSS_node_children_add(RSS_Node* node, const RSS_char* name);
/** Recursively free memory reserved for node, it's children and it's neighbours */
void RSS_free_node(RSS_Node* node);

#endif
