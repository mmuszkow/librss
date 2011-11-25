#include "RSS_Stack.h"

void RSS_init_stack(RSS_Stack* stack)
{
	stack->top = NULL;
	stack->size = 0;
}

void RSS_push_stack(RSS_Stack* stack, RSS_Node* node)
{
	RSS_Stack_elem* new_elem;

	new_elem = (RSS_Stack_elem*)malloc(sizeof(RSS_Stack_elem));
	new_elem->node = node;
	new_elem->next = NULL;
	new_elem->prev = stack->top;
	if(stack->top)
		stack->top->next = new_elem;
	stack->top = new_elem;
	stack->size++;
}

void RSS_pop_stack(RSS_Stack* stack)
{
	RSS_Stack_elem* old_elem;

	old_elem = stack->top;
	stack->top = stack->top->prev;
	if(stack->top)
		stack->top->next = NULL;
	stack->size--;

	free(old_elem);
}

void RSS_free_stack(RSS_Stack* stack)
{
	while(stack->top)
		RSS_pop_stack(stack);
}
