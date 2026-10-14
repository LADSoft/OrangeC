// this tests both the decay copy of this, and inline member initializers without constructors

#include <type_traits>
#include <stdio.h>

#ifndef __ORANGEC__
namespace std
{
template <class _Tp>
inline
typename decay<_Tp>::type
__decay_copy(_Tp&& __t)
{
    return std::forward<_Tp>(__t);
}
}
#endif


struct aa
{
void disp(aa* b_, int n)
{
	printf("%d %d\n", b_->r, n);
}

template<class T, class U>
void xx(T&& r, U&&s)
{
	disp(std::__decay_copy(std::forward<T>(r)), std::__decay_copy(std::forward<U>(s)));
}
	int r = 77;
	void run(int p) { xx(this, p); }
};

	aa nn;
int main()
{
	int vv = 44;
	nn.run(vv);
}