#define MAX_ELEMENTS	50
static int stack[MAX_ELEMENTS];
static int index;
void stackInit(void)
{
	index = 0;
}
int stackNumElems(void)
{
	return index;
}
unsigned int stackTop(void)
{
	return (stack[index - 1]);
}
void stackPush(unsigned int value)
{
	stack[index] = value;
	index++;
}
unsigned int stackPop(void)
{
	unsigned int value;
	index--;
	value = stack[index];
	return value;
}
