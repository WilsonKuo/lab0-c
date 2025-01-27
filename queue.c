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
void q_free(struct list_head *l)
{
    if (!list_empty(l)) {
        element_t *entry, *safe;
        list_for_each_entry_safe (entry, safe, l, list) {
            list_del(&entry->list);
            q_release_element(entry);
        }
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *entry = malloc(sizeof(element_t));
    if (!entry)
        return false;
    int len = strlen(s) + 1;
    entry->value = malloc(len * sizeof(char));
    if (!entry->value) {
        free(entry);
        return false;
    }
    memcpy(entry->value, s, len);
    list_add(&entry->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *entry = malloc(sizeof(element_t));
    if (!entry)
        return false;
    int len = strlen(s) + 1;
    entry->value = malloc(len * sizeof(char));
    if (!entry->value) {
        free(entry);
        return false;
    }
    memcpy(entry->value, s, len);
    list_add_tail(&entry->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;
    element_t *entry = list_first_entry(head, element_t, list);
    list_del_init(&entry->list);
    if (sp) {
        strncpy(sp, entry->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;
    element_t *entry = list_last_entry(head, element_t, list);
    list_del_init(&entry->list);
    if (sp) {
        strncpy(sp, entry->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (list_empty(head))
        return false;
    struct list_head *slow, *fast;

    slow = head->next;
    fast = head->next;
    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (head != fast && head->next != fast);
    slow = slow->prev;

    element_t *rm = list_entry(slow, element_t, list);
    list_del(&rm->list);
    q_release_element(rm);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (list_empty(head) || list_is_singular(head))
        return false;
    bool flag = false;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list != head) {
            if (strcmp(entry->value, safe->value) == 0) {
                flag = true;
                list_del(&entry->list);
                q_release_element(entry);
            } else if (flag) {
                flag = false;
                list_del(&entry->list);
                q_release_element(entry);
            }
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (list_empty(head) || list_is_singular(head))
        return;
    struct list_head *prev_node, *curr_nodeA, *curr_nodeB, *next_node;
    prev_node = head;
    curr_nodeA = head->next;
    curr_nodeB = curr_nodeA->next;
    next_node = curr_nodeB->next;

    while (curr_nodeA != head && curr_nodeB != head) {
        prev_node->next = curr_nodeB;
        curr_nodeA->next = next_node;
        curr_nodeA->prev = curr_nodeB;
        curr_nodeB->next = curr_nodeA;
        curr_nodeB->prev = prev_node;

        prev_node = curr_nodeA;
        curr_nodeA = next_node;
        curr_nodeB = curr_nodeA->next;
        next_node = curr_nodeB->next;
    }

    head->prev = (head == curr_nodeA) ? prev_node : curr_nodeA;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;
    struct list_head *prev_node, *curr_node, *next_node;
    prev_node = head;
    curr_node = head->next;
    head->next = head->prev;
    head->prev = curr_node;
    next_node = curr_node->next;

    while (curr_node != head) {
        curr_node->next = prev_node;
        curr_node->prev = next_node;

        prev_node = curr_node;
        curr_node = next_node;
        next_node = next_node->next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *curr_node, *safe_node, tmp, *tmp_head = head;
    INIT_LIST_HEAD(&tmp);
    int i = 0;
    list_for_each_safe (curr_node, safe_node, head) {
        i++;
        if (i == k) {
            list_cut_position(&tmp, tmp_head, curr_node);
            q_reverse(&tmp);
            list_splice(&tmp, tmp_head);
            tmp_head = safe_node->prev;
            i = 0;
        }
    }
    return;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (list_empty(head) || list_is_singular(head))
        return 0;
    element_t *prev_entry = list_entry(head->next, element_t, list);
    element_t *entry, *safe;

    list_for_each_entry_safe (entry, safe, head, list) {
        if (&entry->list == head->next)
            continue;
        prev_entry = list_entry(entry->list.prev, element_t, list);
        if (strcmp(entry->value, prev_entry->value) < 0) {
            list_del(&entry->list);
            q_release_element(entry);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (list_empty(head) || list_is_singular(head))
        return 0;
    element_t *prev_entry = list_entry(head->next, element_t, list);
    element_t *entry, *safe;

    list_for_each_entry_safe (entry, safe, head, list) {
        if (&entry->list == head->next)
            continue;
        prev_entry = list_entry(entry->list.prev, element_t, list);
        if (strcmp(entry->value, prev_entry->value) > 0) {
            list_del(&entry->list);
            q_release_element(entry);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
