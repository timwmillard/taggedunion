#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int id;
    float price;
    int size;
} Order;

typedef struct {
    int xid;
} Cancel;

typedef struct {
    int xr_id;
    float new_price;
    int new_size;
} CancelReplace;

typedef enum {
    ORDER,
    CANCEL,
    CANCEL_REPLACE,
} Type;

typedef struct {
    Type type;
    union {
        Order order;
        Cancel cancel;
        CancelReplace cancel_replace;
    };
} Instruction;

// Immutable list
typedef struct InstructionList {
    Instruction value;
    struct InstructionList* next;
} InstructionList;

// The empty list (shared singleton)
InstructionList* empty_list = NULL;

// Create a new list node (list construction operation)
// Returns a new list with value at the head, followed by rest
InstructionList* list(Instruction value, InstructionList* rest) {
    InstructionList* list = (InstructionList*) malloc(sizeof(InstructionList));
    if (!list) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    list->value = value;
    list->next = rest;
    return list;
}

// Get the head (first element) of the list
Instruction head(InstructionList* list) {
    if (!list) {
        fprintf(stderr, "head of empty list\n");
        exit(1);
    }
    return list->value;
}

// Get the tail (rest of list after first element)
InstructionList* tail(InstructionList* list) {
    if (!list) {
        fprintf(stderr, "tail of empty list\n");
        exit(1);
    }
    return list->next;
}

// Check if list is empty
bool is_empty(InstructionList* list) {
    return list == NULL;
}

// Get length of list
int length(InstructionList* list) {
    int count = 0;
    while (list) {
        count++;
        list = list->next;
    }
    return count;
}

// Append an element to the end (creates entirely new list)
InstructionList* append(InstructionList* lst, Instruction value) {
    if (is_empty(lst)) {
        return list(value, empty_list);
    }
    return list(head(lst), append(tail(lst), value));
}
typedef Instruction (*MapFunc)(Instruction);

// Map a function over the list (creates new list)
InstructionList* map(MapFunc func, InstructionList* lst) {
    if (is_empty(lst)) {
        return empty_list;
    }
    return list(func(head(lst)), map(func, tail(lst)));
}
typedef bool (*FilterFunc)(Instruction, void*);

// Filter list by predicate (creates new list)
InstructionList* filter(FilterFunc func, InstructionList* lst, void *ctx) {
    if (is_empty(lst)) {
        return empty_list;
    }
    if (func(head(lst), ctx)) {
        return list(head(lst), filter(func, tail(lst), ctx));
    }
    return filter(func, tail(lst), ctx);
}

// Reverse a list (creates new list)
InstructionList* reverse_acc(InstructionList* lst, InstructionList* acc) {
    if (is_empty(lst)) {
        return acc;
    }
    return reverse_acc(tail(lst), list(head(lst), acc));
}

InstructionList* reverse(InstructionList* lst) {
    return reverse_acc(lst, empty_list);
}

int instruction_id(Instruction instruction)
{
    switch (instruction.type) {
        case ORDER: return instruction.order.id;
        case CANCEL: return instruction.cancel.xid;
        case CANCEL_REPLACE: return instruction.cancel_replace.xr_id;
    }
    return 0;
}

// Print list
void print_list(InstructionList* list) {
    printf("[");
    while (list) {
        printf("%d", instruction_id(list->value));
        if (list->value.type == ORDER) printf("($%.2f)", list->value.order.price);
        list = list->next;
        if (list) printf(", ");
    }
    printf("]\n");
}

