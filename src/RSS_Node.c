#include "RSS_Node.h"

RSS_Attribute* RSS_create_attribute(const RSS_char* name, const RSS_char* value)
{
	RSS_Attribute*	new_attrib;

	if(!name || !value)
		return NULL;

	new_attrib = (RSS_Attribute*)malloc(sizeof(RSS_Attribute));
	new_attrib->name = RSS_strdup(name);
	new_attrib->value = RSS_strdup(value);
	new_attrib->next = NULL;

	return new_attrib;
}

void RSS_add_attribute(RSS_Node* node, const RSS_char* name, const RSS_char* value)
{
	RSS_Attribute*	new_attrib;

	if(!node)
		return;

	new_attrib = RSS_create_attribute(name, value);
	if(node->last_attribute)
		node->last_attribute->next = new_attrib;
	else
		node->attribute = new_attrib;
	node->last_attribute = new_attrib;
}

void RSS_free_attribute(RSS_Attribute* attrib)
{
	RSS_Attribute*	prev;

	while(attrib)
	{
		free(attrib->name);
		free(attrib->value);
		prev = attrib;
		attrib = attrib->next;
		free(prev);
	}
}

RSS_Node* RSS_create_node(const RSS_char* name)
{
	RSS_Node* node;

	node = (RSS_Node*)malloc(sizeof(RSS_Node));
	node->name = RSS_strdup(name);
	node->value = NULL;
	node->children = NULL;
	node->neighbour = NULL;
	node->last_neighbour = NULL;
	node->attribute = NULL;
	node->last_attribute = NULL;

	return node;
}

RSS_Node* RSS_node_neighbour_add(RSS_Node* node, const RSS_char* name)
{
	RSS_Node*	new_node;

	new_node = RSS_create_node(name);
	if(node->last_neighbour)
		node->last_neighbour->neighbour = new_node;
	else
		node->neighbour = new_node;
	node->last_neighbour = new_node;

	return new_node;
}

RSS_Node* RSS_node_children_add(RSS_Node* node, const RSS_char* name)
{
	if(!node->children)
	{
		node->children = RSS_create_node(name);
		return node->children;
	}
	else
		return RSS_node_neighbour_add(node->children, name);
}

void RSS_free_node(RSS_Node* node)
{
	RSS_Node*	prev_neighbour;

	while(node)
	{
		free(node->name);
		if(node->value)
			free(node->value);
		if(node->attribute)
			RSS_free_attribute(node->attribute);
		if(node->children)
			RSS_free_node(node->children); /* TODO: stack overflow possible here */
		prev_neighbour = node;
		node = node->neighbour;
		free(prev_neighbour);
	}
}
