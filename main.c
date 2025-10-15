#include <stdbool.h>
#include "order.c"

Instruction times2(Instruction instruction)
{
    switch (instruction.type) {
        case ORDER: instruction.order.price *= 2;
        default:
        return instruction;
    }
    return instruction;
}

bool has_id(Instruction instruction, void *ctx)
{
    int *id = (int*)ctx;
    switch (instruction.type) {
        case ORDER: return instruction.order.id == *id;
        case CANCEL: return instruction.cancel.xid == *id;
        case CANCEL_REPLACE: return instruction.cancel_replace.xr_id == *id;
    }
}

InstructionList *filter_by_oid(InstructionList *instructions, int oid)
{
    return filter(has_id, instructions, (void*)&oid);
}

int main()
{
    InstructionList* il = 
        list((Instruction){.type = ORDER, .order = {.id = 1, .price = 50.0}}, 
        list((Instruction){.type = ORDER, .order = {.id = 2, .price = 50.0}}, 
        list((Instruction){.type = CANCEL, .cancel = {.xid = 1}},
        list((Instruction){.type = ORDER, .order = {.id = 3, .price = 50.0}}, 
        list((Instruction){.type = ORDER, .order = {.id = 4, .price = 50.0}}, 
        empty_list)))));

    print_list(il);

    il = filter_by_oid(il, 3); 
    il = map(times2, il);

    print_list(il);
}
