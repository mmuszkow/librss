#ifndef __RSS_STACK_H__
#define __RSS_STACK_H__

#include "RSS_Node.h"

/** SGML nodes stack */
typedef struct RSS_Stack_elem RSS_Stack_elem;

struct RSS_Stack_elem
{
	/** Content */
	RSS_Node*		node;

	/** Next element */
	RSS_Stack_elem*	next;

	/** Previous element */
	RSS_Stack_elem* prev;
};

typedef struct RSS_Stack
{
	/** Top element */
	RSS_Stack_elem*	top;

	/** How many elements on stack */
	size_t			size;
} RSS_Stack;

void RSS_init_stack(RSS_Stack* stack);
void RSS_push_stack(RSS_Stack* stack, RSS_Node* node);
void RSS_pop_stack(RSS_Stack* stack);
void RSS_free_stack(RSS_Stack* stack);

#endif
