#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct node node;

struct node
{
    node *next;
    int ind;
    HANDLE *elem;
};

typedef struct
{
    node *head;
    int size;
    node *last;
} h_list;

h_list *createHList()
{
    h_list *list = malloc(sizeof(h_list));
    list->size = 0;
    list->head = NULL;
    list->last = list->head;
}

void destroyNode(node *nd);

void destroyHList(h_list *lst)
{
    if (lst->head != NULL)
    {
        destroyNode(lst->head);
    }
    free(lst);
}

void destroyNode(node *nd)
{
    if (nd->next != NULL)
    {
        destroyNode(nd->next);
    }
    free(nd);
}

void add_h(h_list *lst, HANDLE *elem, int ind)
{
    node *new = malloc(sizeof(node));
    new->next = NULL;
    new->elem = elem;
    new->ind = ind;

    if (lst->last == NULL)
    {
        lst->head = new;
        lst->last = lst->head;
        lst->size++;
        return;
    }
    lst->last->next = new;
    lst->last = new;
    lst->size++;
}

int delete_h(h_list *lst, int pos)
{
    int cache;

    if (pos >= lst->size)
    {
        printf("Error: Index out of range");
        exit(-1);
    }

    if (pos == 0)
    {
        node *temp = lst->head;
        lst->head = temp->next;
        cache = temp->ind;
        free(temp->elem);
        free(temp);

        if (lst->head == NULL)
        {
            lst->last = NULL;
        }
        return cache;
    }

    node *curr = lst->head;
    for (int i = 0; i < pos; i++)
    {
        curr = curr->next;
    }

    node *temp = curr;

    curr->next = temp->next;
    cache = temp->ind;
    free(temp->elem);
    free(temp);
    return cache;
}

HANDLE *get_h(h_list *lst, int pos)
{
    if (pos >= lst->size)
    {
        printf("Error: Index out of range");
        exit(-1);
    }

    node *curr = lst->head;
    for (int i = 0; i < pos; i++)
    {
        curr = curr->next;
    }

    return curr->elem;
}

void printList(h_list *lst)
{
    node *elem = lst->head;
    while (elem != NULL)
    {
        printf("%d ", elem->elem);
        elem = elem->next;
    }
    printf("\n");
}