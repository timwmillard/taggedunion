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
    return Instruction_filter(instructions, has_id, (void*)&oid);
}

int main()
{
    InstructionList *il = 
        Instruction_cons((Instruction){.type = ORDER, .order = {.id = 1, .price = 50.0}}, 
        Instruction_cons((Instruction){.type = ORDER, .order = {.id = 2, .price = 50.0}}, 
        Instruction_cons((Instruction){.type = CANCEL, .cancel = {.xid = 1}},
        Instruction_cons((Instruction){.type = ORDER, .order = {.id = 3, .price = 50.0}}, 
        Instruction_cons((Instruction){.type = ORDER, .order = {.id = 4, .price = 50.0}}, 
        empty_InstructionList)))));

    print_list("il", il);

    InstructionList *a = filter_by_oid(il, 3); 
    InstructionList *b = Instruction_map(il, times2);

    print_list("il", il);
    print_list("a", a);
    print_list("b", b);

    InstructionList *orders = Instruction_list(5,
        (Instruction){.type = ORDER, .order = {.id = 1, .price = 30.0}},
        (Instruction){.type = ORDER, .order = {.id = 2, .price = 40.0}},
        (Instruction){.type = CANCEL, .cancel = {.xid = 1}},
        (Instruction){.type = ORDER, .order = {.id = 3, .price = 50.0}},
        (Instruction){.type = ORDER, .order = {.id = 4, .price = 80.0}}
    );
    print_list("orders", orders);
    orders = filter_by_oid(orders, 3); 
    print_list("orders", orders);
}
