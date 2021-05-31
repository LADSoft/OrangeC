#include <tuple>
int aaa = __has_builtin(__make_integer_seq);
#include <stdio.h>

template<class _Tp, _Tp... _Ip>
struct integer_sequence
{
    typedef _Tp value_type;
    static
    constexpr
    size_t
    size() noexcept { return sizeof...(_Ip); }
};

template <class T, T... i>
void print(integer_sequence<T, i...> int_seq)
{
	(printf("%d ",i)...);
        printf("\n" );
}

template <template<class T1, T1...>class tpl, class T, T e>
using tryme = __make_integer_seq<tpl, T, e>;

int main()
{
        print(tryme<integer_sequence, int, 7>{});
        print(tryme<integer_sequence, int, 3>{});
        print(tryme<integer_sequence, int, 7>{});
	print(integer_sequence<int, 4,3,2,1>{});
	print(integer_sequence<int, 1,2,3,4>{});
}