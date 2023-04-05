#include <stdio.h>
#include <malloc.h>
typedef void Demo;
typedef void (*PFnSetI)(Demo*, int);
typedef struct
{
	int i;
	int j;
	PFnSetI pf_seti;
	PFnSetI pf_setj;
}ClassDemo;

int ClassDemo_GetI(Demo* ptr)
{
	return ((ClassDemo*)ptr)->i;
}
int ClassDemo_GetJ(Demo* ptr)
{
	return ((ClassDemo*)ptr)->j;
}
void ClassDemo_SetI(Demo* ptr, int i)
{
	((ClassDemo*)ptr)->i = i;
}
void ClassDemo_SetJ(Demo* ptr, int j)
{
	((ClassDemo*)ptr)->j = j;
}

ClassDemo* ClassDemo_Create(int i, int j)
{
	ClassDemo* ptr = malloc(sizeof(ClassDemo));
	if (ptr == NULL)
		return NULL;
	ptr->i = i;
	ptr->j = j;
	ptr->pf_seti = &ClassDemo_SetI;
	ptr->pf_setj = &ClassDemo_SetJ;

	return ptr;
}

int main()
{
	printf("hello world\n");
	Demo* ptr = ClassDemo_Create(1, 2);
	int i = ClassDemo_GetI(ptr);
	int j = ClassDemo_GetJ(ptr);
	printf("ClassDemo_Create, ptr=%p, i=%d, j=%d\n", ptr, i, j);
	
	//ClassDemo_SetI(ptr, i + 1);
	//ClassDemo_SetJ(ptr, j + 1);
	ClassDemo* inst = ptr;
	inst->pf_seti(inst, i + 2);
	inst->pf_setj(inst, j + 2);

	i = ClassDemo_GetI(ptr);
	j = ClassDemo_GetJ(ptr);
	printf("ClassDemo_Set, ptr=%p, i=%d, j=%d\n", ptr, i, j);

	return 0;
}