#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stddef.h>


typedef enum
{
    StatemachineStageInit,
    StatemachineStageProcess,
    StatemachineStageDeinit
}
StatemachineStage;


// https://stackoverflow.com/questions/31482624/is-there-such-a-thing-as-a-generic-function-pointer-in-c-that-can-be-assigned-ca
typedef void (*FunctionPointerPrototype)(void);

typedef FunctionPointerPrototype (*StatemachineHandler)(StatemachineStage, void *);

FunctionPointerPrototype statemachineNoopHandler(StatemachineStage stage, void * data);


typedef struct
{
    StatemachineHandler currentHandler;
    StatemachineHandler previousHandler;
} Statemachine;


inline void statemachineInit(Statemachine * statemachine, StatemachineHandler handler)
{
    statemachine->previousHandler = &statemachineNoopHandler;
    statemachine->currentHandler = handler;
}

void statemachineProcess(Statemachine * statemachine, void * data);

void statemachineReset(Statemachine * statemachine, void * data, StatemachineHandler handler);


#endif // STATEMACHINE_H
