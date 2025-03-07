#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry = NULL;
    element_t *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add(&node->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *first = list_first_entry(head, element_t, list);

    list_del(&first->list);

    if (sp) {
        sp = strncpy(sp, first->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return first;
}


/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *last = list_last_entry(head, element_t, list);

    list_del(&last->list);
    if (sp) {
        sp = strncpy(sp, last->value, strlen(last->value));
        sp[strlen(last->value)] = '\0';
    }
    return last;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *node;
    int size = 0;
    list_for_each (node, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head **indir = &head->next;
    for (const struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        indir = &(*indir)->next;
    element_t *del = list_entry((*indir), element_t, list);
    list_del(&del->list);
    q_release_element(del);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    element_t *entry = NULL;
    element_t *safe = NULL;
    bool dup = false;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list != head && strcmp(entry->value, safe->value) == 0) {
            dup = true;
            list_del(&entry->list);
            q_release_element(entry);
        } else if (dup) {
            dup = false;
            list_del(&entry->list);
            q_release_element(entry);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node1, *node2;
    list_for_each_safe (node1, node2, head) {
        if (node2 == head)
            break;
        node1->prev->next = node2;
        node2->prev = node1->prev;
        node1->next = node2->next;
        node1->prev = node2;
        node2->next->prev = node1;
        node2->next = node1;

        node2 = node1->next;
        // https://leetcode.com/problems/swap-nodes-in-pairs/
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur = head->next;
    LIST_HEAD(store);
    while (cur != head) {
        struct list_head *end = cur;
        int count = 1;
        while (count < k && end->next != head) {
            end = end->next;
            count++;
        }

        struct list_head *next_start = end->next;

        if (count < k)
            break;
        LIST_HEAD(temp_head);
        list_cut_position(&temp_head, head, end);
        q_reverse(&temp_head);
        list_splice_tail(&temp_head, &store);
        cur = next_start;
    }
    list_splice(&store, head);
}

void mergeTwoLists(struct list_head *L1, struct list_head *L2, bool descend)
{
    if (!L1 || !L2)
        return;
    struct list_head head;
    INIT_LIST_HEAD(&head);
    while (!list_empty(L1) && !list_empty(L2)) {
        element_t *e1 = list_first_entry(L1, element_t, list);
        element_t *e2 = list_first_entry(L2, element_t, list);
        struct list_head *node = (descend ^ (strcmp(e1->value, e2->value) < 0))
                                     ? L1->next
                                     : L2->next;
        list_move_tail(node, &head);
    }
    list_splice_tail_init(list_empty(L1) ? L2 : L1, &head);
    list_splice(&head, L1);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}
/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    q_reverse(head);

    element_t *entry = NULL;
    element_t *safe = NULL;
    const element_t *max_node = list_first_entry(head, element_t, list);
    list_for_each_entry_safe (entry, safe, head, list) {
        if (strcmp(entry->value, max_node->value) > 0) {
            list_del(&entry->list);
            q_release_element(entry);
        } else {
            max_node = entry;
        }
    }
    q_reverse(head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    q_reverse(head);

    element_t *entry = NULL;
    element_t *safe = NULL;
    const element_t *max_node = list_first_entry(head, element_t, list);
    list_for_each_entry_safe (entry, safe, head, list) {
        if (strcmp(entry->value, max_node->value) < 0) {
            list_del(&entry->list);
            q_release_element(entry);
        } else {
            max_node = entry;
        }
    }
    q_reverse(head);
    return q_size(head);
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
}


/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
