#ifndef LIST_H_H_
#define LIST_H_H_

#include <windows.h>

typedef struct h_list h_list;

h_list *createHList();

void destroyHList(h_list *lst);

void add_h(h_list *lst, HANDLE *elem, int ind);

int delete_h(h_list *lst, int pos);

HANDLE *get_h(h_list *lst, int pos);

#endif