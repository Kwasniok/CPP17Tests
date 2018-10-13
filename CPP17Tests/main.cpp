
// #include <vector> // remove comment to have fun!

namespace N
{
	class C
	{
	};
}

int operator+(int i, N::C)
{
	return i+1;
}

#include <numeric>
int main()
{
	N::C a[10];
	std::accumulate(a, a+10, 0);
}
