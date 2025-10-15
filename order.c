#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "list.h"

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

DEFINE_LIST(Instruction, Instruction);

// // Immutable list
// typedef struct InstructionList {
//     Instruction head;
//     struct InstructionList* rest;
// } InstructionList;
//
// // The empty list (shared singleton)
// InstructionList* empty_list = NULL;
//
// // Create a new list node (list construction operation)
// // Returns a new list with value at the head, followed by rest
// InstructionList* cons(Instruction head, InstructionList* rest) {
//     InstructionList* list = (InstructionList*) malloc(sizeof(InstructionList));
//     if (!list) {
//         fprintf(stderr, "Out of memory\n");
//         exit(1);
//     }
//     list->head = head;
//     list->rest = rest;
//     return list;
// }
// InstructionList* list(int count, ...) {
//     InstructionList *l = NULL;
//     va_list args;
//     va_start(args, count);
//     for (int i = 0; i < count; i++) {
//         Instruction inst = va_arg(args, Instruction);
//         l = cons(inst, l);
//     }
//     va_end(args);
//     return l;
// }
//
// // Get the head (first element) of the list
// Instruction head(InstructionList* list) {
//     if (!list) {
//         fprintf(stderr, "head of empty list\n");
//         exit(1);
//     }
//     return list->head;
// }
//
// // Get the tail (rest of list after first element)
// InstructionList* tail(InstructionList* list) {
//     if (!list) {
//         fprintf(stderr, "tail of empty list\n");
//         exit(1);
//     }
//     return list->rest;
// }
//
// // Check if list is empty
// bool is_empty(InstructionList* list) {
//     return list == NULL;
// }
//
// // Get length of list
// int length(InstructionList* list) {
//     int count = 0;
//     while (list) {
//         count++;
//         list = list->rest;
//     }
//     return count;
// }
//
// // Append an element to the end (creates entirely new list)
// InstructionList* append(InstructionList* list, Instruction value) {
//     if (is_empty(list)) {
//         return cons(value, empty_list);
//     }
//     return cons(head(list), append(tail(list), value));
// }
// typedef Instruction (*MapFunc)(Instruction);
//
// // Map a function over the list (creates new list)
// InstructionList* map(MapFunc func, InstructionList* list) {
//     if (is_empty(list)) {
//         return empty_list;
//     }
//     return cons(func(head(list)), map(func, tail(list)));
// }
// typedef bool (*FilterFunc)(Instruction, void*);
//
// // Filter list by predicate (creates new list)
// InstructionList* filter(FilterFunc func, InstructionList* list, void *ctx) {
//     if (is_empty(list)) {
//         return empty_list;
//     }
//     if (func(head(list), ctx)) {
//         return cons(head(list), filter(func, tail(list), ctx));
//     }
//     return filter(func, tail(list), ctx);
// }
//
// // Reverse a list (creates new list)
// InstructionList* reverse_acc(InstructionList* list, InstructionList* acc) {
//     if (is_empty(list)) {
//         return acc;
//     }
//     return reverse_acc(tail(list), cons(head(list), acc));
// }
//
// InstructionList* reverse(InstructionList* list) {
//     return reverse_acc(list, empty_list);
// }

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
void print_list(char *name, InstructionList* list) {
    printf("%s [", name);
    while (list) {
        printf("%d", instruction_id(list->head));
        if (list->head.type == ORDER) printf("($%.2f)", list->head.order.price);
        list = list->rest;
        if (list) printf(", ");
    }
    printf("]\n");
}

