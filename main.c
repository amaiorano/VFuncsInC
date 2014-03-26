#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

enum
{
    CLASS_BASE,
    CLASS_CHILDONE,
    CLASS_CHILDTWO,

    NUM_CLASSES
};

enum
{
    VFUNC_DESTRUCTOR,
    VFUNC_FUNC1,
    VFUNC_FUNC2,

    NUM_VFUNCS
};

typedef void (*Destructor)(void*);
typedef void (*Func1)();
typedef float (*Func2)(int arg1);

void*** g_allVTables = NULL;


// Base

typedef struct
{
    void** vtable;
    int a;
    float b;
} Base;

void Base_Construct(Base* pThis)
{
    printf("Base_Construct\n");
    pThis->a = 0;
    pThis->b = 0.f;
}

void Base_Destruct(Base* pThis)
{
    printf("Base_Destruct\n");
}

void Base_Func1()
{
    printf("Base_Func1\n");
}

float Base_Func2(int arg1)
{
    printf("Base_Func2 arg1=%d\n", arg1);
    return 1.0f;
}


// ChildOne

typedef struct
{
    Base base; // This is how we "inherit" from Base in C to have the same memory layout
    char c;
} ChildOne;

void ChildOne_Construct(ChildOne* pThis)
{
    Base_Construct((Base*)pThis); // Compiler would inject this here

    printf("ChildOne_Construct\n");
    pThis->c = 0;
}

void ChildOne_Destruct(ChildOne* pThis)
{
    printf("ChildOne_Destruct\n");
    
    Base_Destruct((Base*)pThis); // Compiler would inject this here
}

void ChildOne_Func1()
{
    printf("ChildOne_Func1\n");
}


// ChildTwo

typedef struct
{
    ChildOne base; // This is how we "inherit" from Base in C to have the same memory layout
    short c;
    short d;
} ChildTwo;

void ChildTwo_Construct(ChildTwo* pThis)
{
    ChildOne_Construct((ChildOne*)pThis); // Compiler would inject this here

    printf("ChildTwo_Construct\n");
    pThis->c = 0;
    pThis->d = 0;
}

void ChildTwo_Destruct(ChildTwo* pThis)
{
    printf("ChildTwo_Destruct\n");

    ChildOne_Destruct((ChildOne*)pThis); // Compiler would inject this here
}

float ChildTwo_Func2(int arg1)
{
    printf("ChildTwo_Func2 arg1=%d\n", arg1);
    return 1.0f;
}


// These are the functions we need to implement ourselves in C that's automatically handled by the C++ compiler

Base* NewBase()
{
    Base* pInstance = (Base*)malloc(sizeof(Base));
    pInstance->vtable = g_allVTables[CLASS_BASE];
    Base_Construct(pInstance);
    return pInstance;
}

ChildOne* NewChildOne()
{
    ChildOne* pInstance = (ChildOne*)malloc(sizeof(ChildOne));
    ((Base*)pInstance)->vtable = g_allVTables[CLASS_CHILDONE];
    ChildOne_Construct(pInstance);
    return pInstance;
}

ChildTwo* NewChildTwo()
{
    ChildTwo* pInstance = (ChildTwo*)malloc(sizeof(ChildTwo));
    ((Base*)pInstance)->vtable = g_allVTables[CLASS_CHILDTWO];
    ChildTwo_Construct(pInstance);
    return pInstance;
}

void DeleteBase(Base* pThis)
{
    if (pThis != 0)
    {
        ((Destructor)pThis->vtable[VFUNC_DESTRUCTOR])(pThis);
        free(pThis);
    }
}

void InitVTables()
{
    int i;

    // We need a pointer to a vtable per class
    g_allVTables = (void***)malloc( sizeof(void**) * NUM_CLASSES );
    
    // For each class, we allocate vtable - in our case, it's simple as we have a fixed number
    // of virtual functions for each class.
    for (i = 0; i < NUM_CLASSES; ++i)
    {
        g_allVTables[i] = (void**)malloc(sizeof(void*) * NUM_VFUNCS);
    }

    // Populate Base vtable entries
    g_allVTables[CLASS_BASE][VFUNC_DESTRUCTOR] = (void*)&Base_Destruct;
    g_allVTables[CLASS_BASE][VFUNC_FUNC1] = (void*)&Base_Func1;
    g_allVTables[CLASS_BASE][VFUNC_FUNC2] = (void*)&Base_Func2;

    // Populate ChildOne vtable entries
    g_allVTables[CLASS_CHILDONE][VFUNC_DESTRUCTOR] = (void*)&ChildOne_Destruct;
    g_allVTables[CLASS_CHILDONE][VFUNC_FUNC1] = (void*)&ChildOne_Func1;
    g_allVTables[CLASS_CHILDONE][VFUNC_FUNC2] = (void*)&Base_Func2;

    // Populate ChildTwo vtable entries
    g_allVTables[CLASS_CHILDTWO][VFUNC_DESTRUCTOR] = (void*)&ChildTwo_Destruct;
    g_allVTables[CLASS_CHILDTWO][VFUNC_FUNC1] = (void*)&ChildOne_Func1;
    g_allVTables[CLASS_CHILDTWO][VFUNC_FUNC2] = (void*)&ChildTwo_Func2;
}

void DeleteVTables()
{
    int i;

    for (i = 0; i < NUM_CLASSES; ++i)
    {
        free(g_allVTables[i]);
    }

    free(g_allVTables);
    g_allVTables = NULL;
}

int main()
{
    Base* pBase;

    // In C++, the vtables are built by the compiler and laid out in the executable already. In C,
    // we need to allocate and build them at runtime.
    InitVTables();

    pBase = NewBase();                          // pBase = new Base();
    ((Func1)pBase->vtable[VFUNC_FUNC1])();      // pBase->Func1();
    ((Func2)pBase->vtable[VFUNC_FUNC2])(11);    // pBase->Func2(11);
    DeleteBase(pBase);                          // delete pBase;

    printf("\n");
    
    pBase = (Base*)NewChildOne();               // pBase = new ChildOne();
    ((Func1)pBase->vtable[VFUNC_FUNC1])();      // pBase->Func1();
    ((Func2)pBase->vtable[VFUNC_FUNC2])(12);    // pBase->Func2(12);
    DeleteBase(pBase);                          // delete pBase;

    printf("\n");

    pBase = (Base*)NewChildTwo();               // pBase = new ChildTwo();
    ((Func1)pBase->vtable[VFUNC_FUNC1])();      // pBase->Func1();
    ((Func2)pBase->vtable[VFUNC_FUNC2])(13);    // pBase->Func2(13);
    DeleteBase(pBase);                          // delete pBase;

    DeleteVTables();
}
