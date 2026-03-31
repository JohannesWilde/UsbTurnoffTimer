#include "../statemachine.h"

#include "../unused.h"


FunctionPointerPrototype statemachineNoopHandler(StatemachineStage const stage, void * const data)
{
    UNUSED(stage);
    UNUSED(data);
    return (FunctionPointerPrototype)&statemachineNoopHandler;
}

void statemachineProcess(Statemachine * const statemachine, void * const data)
{
    if (statemachine->previousHandler != statemachine->currentHandler)
    {
        statemachine->previousHandler(StatemachineStageDeinit, data);
        statemachine->currentHandler(StatemachineStageInit, data);
        statemachine->previousHandler = statemachine->currentHandler;
    }

    // Finish processing with the processing of the current state, so that
    // the result will be externally visible afterwards.
    statemachine->currentHandler = (StatemachineHandler)statemachine->currentHandler(StatemachineStageProcess, data);
}
