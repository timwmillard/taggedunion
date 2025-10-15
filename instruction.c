#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

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

// Expanded from: DEFINE_LIST(Instruction, Instruction)

/* List structure */
typedef struct InstructionList {
    Instruction head;
    struct InstructionList* rest;
} InstructionList;

/* The empty list (shared singleton) */
InstructionList* empty_InstructionList = NULL;

/* Create a new list node (list construction operation) */
/* Returns a new list with value at the head, followed by rest */
InstructionList* Instruction_cons(Instruction head, InstructionList* rest) {
    InstructionList* list = (InstructionList*) malloc(sizeof(InstructionList));
    if (!list) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    list->head = head;
    list->rest = rest;
    return list;
}

/* Variadic list constructor */
InstructionList* Instruction_list(int count, ...) {
    InstructionList *l = NULL;
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
        Instruction value = va_arg(args, Instruction);
        l = Instruction_cons(value, l);
    }
    va_end(args);
    return l;
}

/* Get the head (first element) of the list */
Instruction Instruction_head(InstructionList* list) {
    if (!list) {
        fprintf(stderr, "head of empty list\n");
        exit(1);
    }
    return list->head;
}

/* Get the tail (rest of list after first element) */
InstructionList* Instruction_tail(InstructionList* list) {
    if (!list) {
        fprintf(stderr, "tail of empty list\n");
        exit(1);
    }
    return list->rest;
}

/* Check if list is empty */
bool Instruction_is_empty(InstructionList* list) {
    return list == NULL;
}

/* Get length of list */
int Instruction_length(InstructionList* list) {
    int count = 0;
    while (list) {
        count++;
        list = list->rest;
    }
    return count;
}

/* Append an element to the end (creates entirely new list) */
InstructionList* Instruction_append(InstructionList* list, Instruction value) {
    if (Instruction_is_empty(list)) {
        return Instruction_cons(value, empty_InstructionList);
    }
    return Instruction_cons(Instruction_head(list), Instruction_append(Instruction_tail(list), value));
}

/* Map function type */
typedef Instruction (*InstructionMapFunc)(Instruction);

/* Map a function over the list (creates new list) */
InstructionList* Instruction_map(InstructionList* list, InstructionMapFunc func) {
    if (Instruction_is_empty(list)) {
        return empty_InstructionList;
    }
    return Instruction_cons(func(Instruction_head(list)), Instruction_map(Instruction_tail(list), func));
}

/* Filter function type */
typedef bool (*InstructionFilterFunc)(Instruction, void*);

/* Filter list by predicate (creates new list) */
InstructionList* Instruction_filter(InstructionList* list, InstructionFilterFunc func, void *ctx) {
    if (Instruction_is_empty(list)) {
        return empty_InstructionList;
    }
    if (func(Instruction_head(list), ctx)) {
        return Instruction_cons(Instruction_head(list), Instruction_filter(Instruction_tail(list), func, ctx));
    }
    return Instruction_filter(Instruction_tail(list), func, ctx);
}

/* Reverse a list (creates new list) */
InstructionList* Instruction_reverse_acc(InstructionList* list, InstructionList* acc) {
    if (Instruction_is_empty(list)) {
        return acc;
    }
    return Instruction_reverse_acc(Instruction_tail(list), Instruction_cons(Instruction_head(list), acc));
}

InstructionList* Instruction_reverse(InstructionList* list) {
    return Instruction_reverse_acc(list, empty_InstructionList);
}

/* Fold/Reduce function type */
typedef void* (*InstructionFoldFunc)(void*, Instruction);

/* Fold left (reduce) - accumulate from left to right */
void* Instruction_foldl(InstructionList* list, InstructionFoldFunc func, void* acc) {
    if (Instruction_is_empty(list)) {
        return acc;
    }
    return Instruction_foldl(Instruction_tail(list), func, func(acc, Instruction_head(list)));
}

/* Fold right - accumulate from right to left */
void* Instruction_foldr(InstructionList* list, InstructionFoldFunc func, void* acc) {
    if (Instruction_is_empty(list)) {
        return acc;
    }
    return func(Instruction_foldr(Instruction_tail(list), func, acc), Instruction_head(list));
}

/* Take first n elements */
InstructionList* Instruction_take(InstructionList* list, int n) {
    if (n <= 0 || Instruction_is_empty(list)) {
        return empty_InstructionList;
    }
    return Instruction_cons(Instruction_head(list), Instruction_take(Instruction_tail(list), n - 1));
}

/* Drop first n elements */
InstructionList* Instruction_drop(InstructionList* list, int n) {
    if (n <= 0 || Instruction_is_empty(list)) {
        return list;
    }
    return Instruction_drop(Instruction_tail(list), n - 1);
}

/* Concatenate two lists */
InstructionList* Instruction_concat(InstructionList* list1, InstructionList* list2) {
    if (Instruction_is_empty(list1)) {
        return list2;
    }
    return Instruction_cons(Instruction_head(list1), Instruction_concat(Instruction_tail(list1), list2));
}

/* Flatten a list of lists */
typedef InstructionList* (*InstructionFlatMapFunc)(Instruction);

InstructionList* Instruction_flatmap(InstructionList* list, InstructionFlatMapFunc func) {
    if (Instruction_is_empty(list)) {
        return empty_InstructionList;
    }
    return Instruction_concat(func(Instruction_head(list)), Instruction_flatmap(Instruction_tail(list), func));
}

/* Find first element matching predicate */
typedef bool (*InstructionPredicateFunc)(Instruction, void*);

bool Instruction_find(InstructionList* list, InstructionPredicateFunc pred, void* ctx, Instruction* result) {
    if (Instruction_is_empty(list)) {
        return false;
    }
    if (pred(Instruction_head(list), ctx)) {
        *result = Instruction_head(list);
        return true;
    }
    return Instruction_find(Instruction_tail(list), pred, ctx, result);
}

/* Check if any element satisfies predicate */
bool Instruction_any(InstructionList* list, InstructionPredicateFunc pred, void* ctx) {
    if (Instruction_is_empty(list)) {
        return false;
    }
    if (pred(Instruction_head(list), ctx)) {
        return true;
    }
    return Instruction_any(Instruction_tail(list), pred, ctx);
}

/* Check if all elements satisfy predicate */
bool Instruction_all(InstructionList* list, InstructionPredicateFunc pred, void* ctx) {
    if (Instruction_is_empty(list)) {
        return true;
    }
    if (!pred(Instruction_head(list), ctx)) {
        return false;
    }
    return Instruction_all(Instruction_tail(list), pred, ctx);
}

/* For a proper zipWith that returns Instruction, the function should take (Instruction, Instruction) -> Instruction */
typedef Instruction (*InstructionZipWithFunc)(Instruction, Instruction);

InstructionList* Instruction_zipWith(InstructionList* list1, InstructionList* list2, InstructionZipWithFunc func) {
    if (Instruction_is_empty(list1) || Instruction_is_empty(list2)) {
        return empty_InstructionList;
    }
    Instruction combined = func(Instruction_head(list1), Instruction_head(list2));
    return Instruction_cons(combined, Instruction_zipWith(Instruction_tail(list1), Instruction_tail(list2), func));
}

/* Get element at index (0-based) */
bool Instruction_nth(InstructionList* list, int index, Instruction* result) {
    if (Instruction_is_empty(list) || index < 0) {
        return false;
    }
    if (index == 0) {
        *result = Instruction_head(list);
        return true;
    }
    return Instruction_nth(Instruction_tail(list), index - 1, result);
}

/* Partition list into two based on predicate */
typedef struct { InstructionList* passed; InstructionList* failed; } InstructionPartition;

InstructionPartition Instruction_partition(InstructionList* list, InstructionPredicateFunc pred, void* ctx) {
    InstructionPartition result = { empty_InstructionList, empty_InstructionList };
    if (Instruction_is_empty(list)) {
        return result;
    }
    InstructionPartition rest = Instruction_partition(Instruction_tail(list), pred, ctx);
    if (pred(Instruction_head(list), ctx)) {
        result.passed = Instruction_cons(Instruction_head(list), rest.passed);
        result.failed = rest.failed;
    } else {
        result.passed = rest.passed;
        result.failed = Instruction_cons(Instruction_head(list), rest.failed);
    }
    return result;
}

/* Take elements while predicate is true */
InstructionList* Instruction_takeWhile(InstructionList* list, InstructionPredicateFunc pred, void* ctx) {
    if (Instruction_is_empty(list) || !pred(Instruction_head(list), ctx)) {
        return empty_InstructionList;
    }
    return Instruction_cons(Instruction_head(list), Instruction_takeWhile(Instruction_tail(list), pred, ctx));
}

/* Drop elements while predicate is true */
InstructionList* Instruction_dropWhile(InstructionList* list, InstructionPredicateFunc pred, void* ctx) {
    if (Instruction_is_empty(list)) {
        return empty_InstructionList;
    }
    if (pred(Instruction_head(list), ctx)) {
        return Instruction_dropWhile(Instruction_tail(list), pred, ctx);
    }
    return list;
}

