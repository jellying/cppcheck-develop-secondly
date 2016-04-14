#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
class cm
{
private:
    const int s;
    int p = 10;
    
public:
    int* getS()
    {
        const int m = 10;
        return &s;  //违背
    }

    const int* getp()
    {
        return &p;
    }

    const int* getconstS()
    {
        return &s;
    }
};
int main()
{
    cm C;
    int *a, b;

    a = C.getS();
    printf("%d\n", b);
    return 0;
}