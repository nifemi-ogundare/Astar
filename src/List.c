#include "List.h"
#include "status.h"

static Node* available = NULL;

/** Empty List creation by dynamic memory allocation (O(1)).
 * @param comp comparison function between elements (ala strcmp())
 * @param pr display function for list elements
 * @return a new (empty) list if memory allocation OK
 * @return 0 otherwise
 */
List*	newList	(compFun comp,prFun pr) {
	List* result = (List*)malloc(sizeof(List));
	result->nelts = 0;
	result->head = NULL;
	result->comp  = comp;
	result->pr = pr;

	return result;
}

/** destroy the list by deallocating used memory (O(N)).
 * @param l the list to destroy */
void 	delList	(List* list) {
	int i;
	Node *n = list->head;
	Node *tmp = NULL;

	for (i=0; i<list->nelts; i++) {
		tmp = n;
		n = n->next;
		free(tmp);
	}

	while (available) {
		tmp = available;
		available = available->next;
		free(tmp);
	}

	free(list);
}

/** get the Nth element of the list (O(N)).
 * @param l the list
 * @param n the index of the element in list
 * @param e (out) the searched element
 * @return OK if element found
 * @return INDER if index out of list bounds
 */
status 	nthInList	(List* list,int index,void** e) {
	int i;
	Node *n = list->head;

	if (index > list->nelts) return INDER;

	for (i=0;i<index;i++) {
		n = n->next;
	}

	*e = n->val;
	return OK;
}

/** Insert element at a given position in the list (O(N)).
 * @param l the list to store the element in
 * @param p the position of the insertion point
 * @param e the element to insert
 * @return INDER if position is out of list bounds
 * @return ERRALLOC if memory allocation failed
 * @return OK otherwise
 */
status 	addListAt	(List* list,int position,void* e) {
	int i;
	Node *tmp = NULL;

	if (position > list->nelts) return INDER;

	if (available) {
		tmp = available;
		available = available->next;
	}else{
		tmp = (Node*)malloc(sizeof(Node));
	}

	if (!tmp) return ERRALLOC;
	tmp->val = e;

	if (position == 0) {
		tmp->next = list->head;
	    list->head = tmp;
	    list->nelts+=1;
	    return OK;
	}

	Node *p = list->head;
	Node *n = p->next;

	for (i=0;i<position-1;i++) {
		n = n->next;
		p = p->next;
	}

	tmp->next = n;
	p->next = tmp;
	list->nelts+=1;
	return OK;
}

/** remove the element located at a given position in list (O(N)).
 * @param l the list to remove the element from
 * @param p the position of the element to remove
 * @param e (out) the removed element
 * @return INDER if position is out of list bounds
 * @return OK otherwise
 */
status 	remFromListAt	(List* list,int position,void**e) {
	int i;
	if (position > list->nelts) return INDER;

	Node *p = list->head;
	Node *n = p->next;

	if (position == 0) {
		*e = p->val;
	    list->head = n;
	    p->next = available;
	    available = p;
	    list->nelts-=1;
	    return OK;
	}

	for (i=0;i<position-1;i++) {
		p = p->next;
		n = n->next;
	}

	*e = n->val;
	p->next = n->next;
    n->next = available;
    available = n;
	list->nelts-=1;

	return OK;
}

/** remove given element from given list (O(N)).
 * @param l the list to remove the element from
 * @param e the element to remove
 * @return ERRABSENT if element is not found in list
 * @return OK otherwise
 */
status 	remFromList	(List* list,void* e) {

	Node *p = isInList(list,e);
	Node *tmp = NULL;

	if (p == 1) {
		tmp = list->head;
	    list->head = (list->head)->next;
	    tmp->next = available;
	    available = tmp;
	    list->nelts-=1;
	    return OK;
	}

	if (p == 0) {
		return ERRABSENT;
	}

	tmp = p->next;
	p->next = (p->next)->next;
    tmp->next = available;
    available = tmp;
	list->nelts-=1;
	return OK;
}

/** display list elements as "[ e1 ... eN ]" (O(N)).
 * The display of the element is delegated to the prFun function
 * @param l the list to display.
 */
void 	displayList	(List* list) {
	forEach(list, list->pr);
}

/** sequencially call given function with each element of given list (O(NxF)).
 * @param l the list
 * @param f the function
 */
void	forEach		(List* list, void(*func)(void*)) {
	int i;
	Node *n = list->head;

	for (i=0; i<list->nelts; i++) {
		func(n->val);
		n = n->next;
	}
}

/** compute and return the number of elements in given list (O(1)).
 * @param l the list
 * @return the number of elements in given list
 */
int	lengthList	(List* list) {
	return list->nelts;
}

/** add given element to given list according to compFun function (O(N)).
 * @param l the list (supposedly sorted according to compFun function)
 * @param e the element to add
 * @return ERRALLOC if memory allocation failed
 * @return OK otherwise
 */
status	addList	(List* list,void* e) {
	int i;
	Node *tmp = NULL;

	if (available) {
		tmp = available;
		available = available->next;
	}else{
		tmp = (Node*)malloc(sizeof(Node));
	}

	if (!tmp) return ERRALLOC;
	tmp->val = e;

	if (list->nelts == 0) {
	    list->head = tmp;
	    tmp->next = 0;
	    list->nelts+=1;
	    return OK;
	}

	Node *p = list->head;
	Node *n = p->next;

	if (list->comp(p->val, e) >= 0) {
	    tmp->next = list->head;
	    list->head = tmp;
	    list->nelts+=1;
	    return OK;
	}

	for (i=0;i<=list->nelts-1;i++) {
		if (i == list->nelts-1) {
			p->next = tmp;
			tmp->next = 0;
			list->nelts+=1;
			return OK;
		}
		if (list->comp(n->val, e) >= 0) {
			tmp->next = n;
			p->next = tmp;
			list->nelts+=1;
			return OK;
		}
		n = n->next;
		p = p->next;
	}

	return ERRALLOC;
}

/** tests whether the list contains given element (O(N)).
 * @param l the list
 * @param e the searched element
 * @return 0 if element is not found in list
 * @return 1 if element is at the head of the list (no predecessor)
 * @return (a pointer to) the predecessor of the search element otherwise
 */
Node*	isInList	(List* list,void* e) {
	int i;
	Node *p = (list->head);
	Node *n = (p->next);

	if (list->comp(p->val, e) == 0) {
		return 1;
	}

	for (i=0; i<list->nelts-1; i++) {
		if (list->comp(n->val, e) == 0) {
			return p;
		}
		n = n->next;
		p = p->next;
	}
	return 0;
}

