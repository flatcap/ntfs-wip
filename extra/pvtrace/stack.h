#ifndef __STACK_H
#define __STACK_H

void stackInit(void);

int stackNumElems(void);

unsigned int stackTop(void);

void stackPush(unsigned int value);

unsigned int stackPop(void);

#endif
