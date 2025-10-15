#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

/* 
 * GENERIC IMMUTABLE LIST LIBRARY
 * ================================
 * 
 * Usage:
 * ------
 * DEFINE_LIST(Type, TypeName) generates a complete functional list implementation.
 * 
 * Example:
 *   DEFINE_LIST(int, Int)           // Creates IntList and all Int_* functions
 *   DEFINE_LIST(Instruction, Instruction)  // Creates InstructionList, etc.
 * 
 * Generated Types:
 * ----------------
 * - TypeNameList                    // The list structure
 * - empty_TypeNameList              // Singleton for empty list
 * - TypeNameMapFunc                 // Function pointer: Type -> Type
 * - TypeNameFilterFunc              // Function pointer: (Type, void*) -> bool
 * - TypeNameFoldFunc                // Function pointer: (void*, Type) -> void*
 * - TypeNameFlatMapFunc             // Function pointer: Type -> TypeNameList*
 * - TypeNamePredicateFunc           // Function pointer: (Type, void*) -> bool
 * - TypeNameZipWithFunc             // Function pointer: (Type, Type) -> Type
 * - TypeNamePartition               // Struct with passed and failed lists
 * 
 * Basic Operations:
 * -----------------
 * TypeName_cons(head, rest)         // Prepend element to list
 * TypeName_list(count, ...)         // Create list from varargs
 * TypeName_head(list)               // Get first element
 * TypeName_tail(list)               // Get rest of list
 * TypeName_is_empty(list)           // Check if empty
 * TypeName_length(list)             // Count elements
 * TypeName_nth(list, index, *result) // Get element at index (returns bool)
 * 
 * Building Lists:
 * ---------------
 * TypeName_append(list, value)      // Add element to end
 * TypeName_concat(list1, list2)     // Join two lists
 * TypeName_reverse(list)            // Reverse order
 * 
 * Transforming:
 * -------------
 * TypeName_map(list, func)          // Apply function to each element
 * TypeName_filter(list, func, ctx)  // Keep elements matching predicate
 * TypeName_flatmap(list, func)      // Map then flatten results
 * 
 * Folding/Reducing:
 * -----------------
 * TypeName_foldl(list, func, acc)   // Fold left (accumulate left-to-right)
 * TypeName_foldr(list, func, acc)   // Fold right (accumulate right-to-left)
 * 
 * Slicing:
 * --------
 * TypeName_take(list, n)            // First n elements
 * TypeName_drop(list, n)            // Skip first n elements
 * TypeName_takeWhile(list, pred, ctx) // Take while predicate true
 * TypeName_dropWhile(list, pred, ctx) // Skip while predicate true
 * 
 * Searching/Testing:
 * ------------------
 * TypeName_find(list, pred, ctx, *result)  // Find first match (returns bool)
 * TypeName_any(list, pred, ctx)     // True if any element matches
 * TypeName_all(list, pred, ctx)     // True if all elements match
 * 
 * Combining:
 * ----------
 * TypeName_zipWith(list1, list2, func)     // Combine two lists pairwise with func(a,b)->Type
 * TypeName_partition(list, pred, ctx)      // Split into passed/failed lists
 * 
 */

// Macro to define a generic immutable list type and its operations
#define DEFINE_LIST(Type, TypeName) \
\
/* List structure */ \
typedef struct TypeName##List { \
    Type head; \
    struct TypeName##List* rest; \
} TypeName##List; \
\
/* The empty list (shared singleton) */ \
TypeName##List* empty_##TypeName##List = NULL; \
\
/* Create a new list node (list construction operation) */ \
/* Returns a new list with value at the head, followed by rest */ \
TypeName##List* TypeName##_cons(Type head, TypeName##List* rest) { \
    TypeName##List* list = (TypeName##List*) malloc(sizeof(TypeName##List)); \
    if (!list) { \
        fprintf(stderr, "Out of memory\n"); \
        exit(1); \
    } \
    list->head = head; \
    list->rest = rest; \
    return list; \
} \
\
/* Variadic list constructor */ \
TypeName##List* TypeName##_list(int count, ...) { \
    TypeName##List *l = NULL; \
    va_list args; \
    va_start(args, count); \
    for (int i = 0; i < count; i++) { \
        Type value = va_arg(args, Type); \
        l = TypeName##_cons(value, l); \
    } \
    va_end(args); \
    return l; \
} \
\
/* Get the head (first element) of the list */ \
Type TypeName##_head(TypeName##List* list) { \
    if (!list) { \
        fprintf(stderr, "head of empty list\n"); \
        exit(1); \
    } \
    return list->head; \
} \
\
/* Get the tail (rest of list after first element) */ \
TypeName##List* TypeName##_tail(TypeName##List* list) { \
    if (!list) { \
        fprintf(stderr, "tail of empty list\n"); \
        exit(1); \
    } \
    return list->rest; \
} \
\
/* Check if list is empty */ \
bool TypeName##_is_empty(TypeName##List* list) { \
    return list == NULL; \
} \
\
/* Get length of list */ \
int TypeName##_length(TypeName##List* list) { \
    int count = 0; \
    while (list) { \
        count++; \
        list = list->rest; \
    } \
    return count; \
} \
\
/* Append an element to the end (creates entirely new list) */ \
TypeName##List* TypeName##_append(TypeName##List* list, Type value) { \
    if (TypeName##_is_empty(list)) { \
        return TypeName##_cons(value, empty_##TypeName##List); \
    } \
    return TypeName##_cons(TypeName##_head(list), TypeName##_append(TypeName##_tail(list), value)); \
} \
\
/* Map function type */ \
typedef Type (*TypeName##MapFunc)(Type); \
\
/* Map a function over the list (creates new list) */ \
TypeName##List* TypeName##_map(TypeName##List* list, TypeName##MapFunc func) { \
    if (TypeName##_is_empty(list)) { \
        return empty_##TypeName##List; \
    } \
    return TypeName##_cons(func(TypeName##_head(list)), TypeName##_map(TypeName##_tail(list), func)); \
} \
\
/* Filter function type */ \
typedef bool (*TypeName##FilterFunc)(Type, void*); \
\
/* Filter list by predicate (creates new list) */ \
TypeName##List* TypeName##_filter(TypeName##List* list, TypeName##FilterFunc func, void *ctx) { \
    if (TypeName##_is_empty(list)) { \
        return empty_##TypeName##List; \
    } \
    if (func(TypeName##_head(list), ctx)) { \
        return TypeName##_cons(TypeName##_head(list), TypeName##_filter(TypeName##_tail(list), func, ctx)); \
    } \
    return TypeName##_filter(TypeName##_tail(list), func, ctx); \
} \
\
/* Reverse a list (creates new list) */ \
TypeName##List* TypeName##_reverse_acc(TypeName##List* list, TypeName##List* acc) { \
    if (TypeName##_is_empty(list)) { \
        return acc; \
    } \
    return TypeName##_reverse_acc(TypeName##_tail(list), TypeName##_cons(TypeName##_head(list), acc)); \
} \
\
TypeName##List* TypeName##_reverse(TypeName##List* list) { \
    return TypeName##_reverse_acc(list, empty_##TypeName##List); \
} \
\
/* Fold/Reduce function type */ \
typedef void* (*TypeName##FoldFunc)(void*, Type); \
\
/* Fold left (reduce) - accumulate from left to right */ \
void* TypeName##_foldl(TypeName##List* list, TypeName##FoldFunc func, void* acc) { \
    if (TypeName##_is_empty(list)) { \
        return acc; \
    } \
    return TypeName##_foldl(TypeName##_tail(list), func, func(acc, TypeName##_head(list))); \
} \
\
/* Fold right - accumulate from right to left */ \
void* TypeName##_foldr(TypeName##List* list, TypeName##FoldFunc func, void* acc) { \
    if (TypeName##_is_empty(list)) { \
        return acc; \
    } \
    return func(TypeName##_foldr(TypeName##_tail(list), func, acc), TypeName##_head(list)); \
} \
\
/* Take first n elements */ \
TypeName##List* TypeName##_take(TypeName##List* list, int n) { \
    if (n <= 0 || TypeName##_is_empty(list)) { \
        return empty_##TypeName##List; \
    } \
    return TypeName##_cons(TypeName##_head(list), TypeName##_take(TypeName##_tail(list), n - 1)); \
} \
\
/* Drop first n elements */ \
TypeName##List* TypeName##_drop(TypeName##List* list, int n) { \
    if (n <= 0 || TypeName##_is_empty(list)) { \
        return list; \
    } \
    return TypeName##_drop(TypeName##_tail(list), n - 1); \
} \
\
/* Concatenate two lists */ \
TypeName##List* TypeName##_concat(TypeName##List* list1, TypeName##List* list2) { \
    if (TypeName##_is_empty(list1)) { \
        return list2; \
    } \
    return TypeName##_cons(TypeName##_head(list1), TypeName##_concat(TypeName##_tail(list1), list2)); \
} \
\
/* Flatten a list of lists */ \
typedef TypeName##List* (*TypeName##FlatMapFunc)(Type); \
\
TypeName##List* TypeName##_flatmap(TypeName##List* list, TypeName##FlatMapFunc func) { \
    if (TypeName##_is_empty(list)) { \
        return empty_##TypeName##List; \
    } \
    return TypeName##_concat(func(TypeName##_head(list)), TypeName##_flatmap(TypeName##_tail(list), func)); \
} \
\
/* Find first element matching predicate */ \
typedef bool (*TypeName##PredicateFunc)(Type, void*); \
\
bool TypeName##_find(TypeName##List* list, TypeName##PredicateFunc pred, void* ctx, Type* result) { \
    if (TypeName##_is_empty(list)) { \
        return false; \
    } \
    if (pred(TypeName##_head(list), ctx)) { \
        *result = TypeName##_head(list); \
        return true; \
    } \
    return TypeName##_find(TypeName##_tail(list), pred, ctx, result); \
} \
\
/* Check if any element satisfies predicate */ \
bool TypeName##_any(TypeName##List* list, TypeName##PredicateFunc pred, void* ctx) { \
    if (TypeName##_is_empty(list)) { \
        return false; \
    } \
    if (pred(TypeName##_head(list), ctx)) { \
        return true; \
    } \
    return TypeName##_any(TypeName##_tail(list), pred, ctx); \
} \
\
/* Check if all elements satisfy predicate */ \
bool TypeName##_all(TypeName##List* list, TypeName##PredicateFunc pred, void* ctx) { \
    if (TypeName##_is_empty(list)) { \
        return true; \
    } \
    if (!pred(TypeName##_head(list), ctx)) { \
        return false; \
    } \
    return TypeName##_all(TypeName##_tail(list), pred, ctx); \
} \
\
/* For a proper zipWith that returns Type, the function should take (Type, Type) -> Type */ \
typedef Type (*TypeName##ZipWithFunc)(Type, Type); \
\
TypeName##List* TypeName##_zipWith(TypeName##List* list1, TypeName##List* list2, TypeName##ZipWithFunc func) { \
    if (TypeName##_is_empty(list1) || TypeName##_is_empty(list2)) { \
        return empty_##TypeName##List; \
    } \
    Type combined = func(TypeName##_head(list1), TypeName##_head(list2)); \
    return TypeName##_cons(combined, TypeName##_zipWith(TypeName##_tail(list1), TypeName##_tail(list2), func)); \
} \
\
/* Get element at index (0-based) */ \
bool TypeName##_nth(TypeName##List* list, int index, Type* result) { \
    if (TypeName##_is_empty(list) || index < 0) { \
        return false; \
    } \
    if (index == 0) { \
        *result = TypeName##_head(list); \
        return true; \
    } \
    return TypeName##_nth(TypeName##_tail(list), index - 1, result); \
} \
\
/* Partition list into two based on predicate */ \
typedef struct { TypeName##List* passed; TypeName##List* failed; } TypeName##Partition; \
\
TypeName##Partition TypeName##_partition(TypeName##List* list, TypeName##PredicateFunc pred, void* ctx) { \
    TypeName##Partition result = { empty_##TypeName##List, empty_##TypeName##List }; \
    if (TypeName##_is_empty(list)) { \
        return result; \
    } \
    TypeName##Partition rest = TypeName##_partition(TypeName##_tail(list), pred, ctx); \
    if (pred(TypeName##_head(list), ctx)) { \
        result.passed = TypeName##_cons(TypeName##_head(list), rest.passed); \
        result.failed = rest.failed; \
    } else { \
        result.passed = rest.passed; \
        result.failed = TypeName##_cons(TypeName##_head(list), rest.failed); \
    } \
    return result; \
} \
\
/* Take elements while predicate is true */ \
TypeName##List* TypeName##_takeWhile(TypeName##List* list, TypeName##PredicateFunc pred, void* ctx) { \
    if (TypeName##_is_empty(list) || !pred(TypeName##_head(list), ctx)) { \
        return empty_##TypeName##List; \
    } \
    return TypeName##_cons(TypeName##_head(list), TypeName##_takeWhile(TypeName##_tail(list), pred, ctx)); \
} \
\
/* Drop elements while predicate is true */ \
TypeName##List* TypeName##_dropWhile(TypeName##List* list, TypeName##PredicateFunc pred, void* ctx) { \
    if (TypeName##_is_empty(list)) { \
        return empty_##TypeName##List; \
    } \
    if (pred(TypeName##_head(list), ctx)) { \
        return TypeName##_dropWhile(TypeName##_tail(list), pred, ctx); \
    } \
    return list; \
}

#endif // GENERIC_LIST_H
