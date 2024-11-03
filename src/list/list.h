#ifndef list_h
#define list_h

typedef struct LinkedListNode_t {
    struct LinkedListNode_t* prev;
    struct LinkedListNode_t* next;
    void* value;
} LinkedListNode;

// Create a new linked list node.
LinkedListNode* NewLinkedListNode(void* value);

// Delete the linked list node
void DeleteLinkedListNode(LinkedListNode* node);

typedef struct LinkedList_t {
    LinkedListNode* head;
    LinkedListNode* tail;
    unsigned int length;
} LinkedList;

// Construct a new linked list.
LinkedList* NewLinkedList();

// Deletes a linked list.
void DeleteLinkedList(LinkedList* list);

// Append a new element to the tail of the list.
void Append(LinkedList* list, void* value);

// Function pointer type for providing values to a callback function.
typedef void (*ValueCallbackFunc)(void*);

// Iterates over the list, calling the callback function with each value.
void Iterate(LinkedList* list, ValueCallbackFunc callback);

#endif