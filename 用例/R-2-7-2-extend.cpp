#include <stdio.h>

class myclassA
{
    int a;
};

class myclassB
{
    int b;
};

myclassB newB()
{
    return myclassB();
}


bool bf(int k)
{
    int s = 10;
    int a=5;
    if(k>5)
        return !(s==10 || a==2);
    else
        return a=10;    //违背
    //return !(s==10);   
}

unsigned int fun()
{
    double s = 1.2;
    if(s>1)
        return s=2;   //违背
    else
        return 2;
}
float f(int s)
{
    if(s>0)
        return f(s-1);   
    else
        return fun();   //违背
}

myclassB newB()
{
    myclassA a;
    return a;   //违背
}
char getchar()
{
    return fun();//违背
}
int main()
{
    int s = fun();
    s=f(10);
    s=bf(2);
    return 0;
}
