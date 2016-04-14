#include <stdio.h>
class cm
{
private:
	int s;
public:
	int* getS()
	{
		return &s;		//Î¥±³
	}

	const int* getmyS()
	{
		return &s;
	}
};
int main()
{
	cm C;
	int *a, b;

	a = C.getS();
	*a = 10;
	b = *C.getS();
	printf("%d\n", b);
	return 0;
}