#include <stdio.h>

class Base
{
public:
    Base();
    virtual ~Base();
    virtual void Func1();
    virtual float Func2(int arg1);

    int a;
    float b;
};

Base::Base()
{
    printf("Base::Construct\n");
    a = 0;
    b = 0.f;
}

Base::~Base()
{
    printf("Base::Destruct\n");
}

void Base::Func1()
{
    printf("Base::Func1\n");
}

float Base::Func2(int arg1)
{
    printf("Base::Func2 arg1=%d\n", arg1);
    return 1.0f;
}


class ChildOne : public Base
{
public:
    ChildOne();
    virtual ~ChildOne();
    virtual void Func1();

    char c;
};

ChildOne::ChildOne()
{
    printf("ChildOne::Construct\n");
    c = 0;
}

ChildOne::~ChildOne()
{
    printf("ChildOne::Destruct\n");
}

void ChildOne::Func1()
{
    printf("ChildOne::Func1\n");
}


class ChildTwo : public ChildOne
{
public:
    ChildTwo();
    virtual ~ChildTwo();
    virtual float Func2(int arg1);

    short c;
    short d;
};

ChildTwo::ChildTwo()
{
    printf("ChildTwo::Construct\n");
    c = 0;
    d = 0;
}

ChildTwo::~ChildTwo()
{
    printf("ChildTwo::Destruct\n");
}

float ChildTwo::Func2(int arg1)
{
    printf("ChildTwo::Func2 arg1=%d\n", arg1);
    return 1.0f;
}


int main()
{
    Base* pBase;

    pBase = new Base();
    pBase->Func1();
    pBase->Func2(11);
    delete pBase;

    printf("\n");
    
    pBase = new ChildOne();
    pBase->Func1();
    pBase->Func2(12);
    delete pBase;

    printf("\n");

    pBase = new ChildTwo();
    pBase->Func1();
    pBase->Func2(13);
    delete pBase;
}
