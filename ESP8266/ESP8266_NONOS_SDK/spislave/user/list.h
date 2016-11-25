
//#define INLINE inline

#ifndef LIST_H_
#define LIST_H_

#ifndef INLINE
#define INLINE
#endif

struct list_head  
{  
	struct list_head *prev;  
    struct list_head *next;  
};

#define list_entry(ptr, type, member) \
	((type*)((char*)(ptr)-(unsigned long)(&((type*)0)->member)))

#define __list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

static INLINE int list_empty(const struct list_head *head)  
{  
	return head->next == head;  
}
static INLINE void __list_del(struct list_head * prev, struct list_head * next)  
{
        next->prev = prev;  
        prev->next = next;  
}
static INLINE void list_del(struct list_head *entry)  
{
    __list_del(entry->prev, entry->next);
    entry->next = 0;
    entry->prev = 0;
}
static INLINE void __list_add(struct list_head *_new, struct list_head *prev, struct list_head *next)  
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}
static INLINE void list_add(struct list_head *_new, struct list_head *head)  
{
	__list_add(_new, head, head->next);
}
static INLINE void list_add_tail(struct list_head *_new, struct list_head *head)  
{
	__list_add(_new, head->prev, head);
}
static INLINE void INIT_LIST_HEAD(struct list_head *list)  
{
	list->next = list;
    list->prev = list;
}

static INLINE void list_del_init(struct list_head *entry)  
{
	INIT_LIST_HEAD(entry);
}

#endif
