#include "../statemachine.h"


void statemachineReset(Statemachine * const statemachine, void * const data, StatemachineHandler const handler)
{
    statemachine->previousHandler(StatemachineStageDeinit, data);
    statemachine->previousHandler = &statemachineNoopHandler;

    statemachine->currentHandler = handler;
}
