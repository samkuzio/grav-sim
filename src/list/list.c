#include <stdlib.h>

#include "list.h"

LinkedListNode* NewLinkedListNode(void* value) {
    LinkedListNode* node = calloc(1, sizeof(LinkedListNode));
    node->value = value;
    return node;
}

// Delete the linked list node
void DeleteLinkedListNode(LinkedListNode* node) {
    free(node->value);
    free(node);
}

// Construct a new linked list.
LinkedList* NewLinkedList() {
    LinkedList* list = calloc(1, sizeof(LinkedList));
    return list;
}

// Deletes a linked list.
void DeleteLinkedList(LinkedList* list) {
    LinkedListNode* curr = list->head;
    while (curr != NULL) {
        LinkedListNode* next = curr->next;
        DeleteLinkedListNode(curr);
        curr = next;
    }
    free(list);
}

// Append a new element to the tail of the list.
void Append(LinkedList* list, void* value) {
    LinkedListNode* node = NewLinkedListNode(value);

    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        LinkedListNode* oldTail = list->tail;
        oldTail->next = node;
        list->tail = node;
    }

    list->length++;
}

// Iterates over the list, calling the callback function with each value.
void Iterate(LinkedList* list, ValueCallbackFunc callback) {
    LinkedListNode* curr = list->head;
    for(; curr != NULL; curr = curr->next) {
        callback(curr->value);
    }
}