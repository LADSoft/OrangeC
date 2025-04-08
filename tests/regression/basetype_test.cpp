#include <assert.h>
template <class _Tp, _Tp __v>
struct  integral_constant
{
  static constexpr const _Tp      value = __v;
  typedef _Tp               value_type;
  typedef integral_constant type;
  
  constexpr operator value_type() const noexcept {return value;}
  constexpr value_type operator ()() const noexcept {return value;}
};
template <class _Bp, class _Dp>
struct  is_base_of
    : public integral_constant<bool, __is_base_of(_Bp, _Dp)> {};
struct A
{
    char a1() {return 5;}
    short a2(int i) {return short(i+1);}
    int a3() const {return 1;}
    double a4(unsigned i) const {return i-1;}
};
template <class _Arg, class _Result>
struct unary_function
{
    typedef _Arg    argument_type;
    typedef _Result result_type;
};
template <class _Sp, class _Tp>
class const_mem_fun_t
    : public unary_function<const _Tp*, _Sp>
{
    _Sp (_Tp::*__p_)() const;
public:
    explicit const_mem_fun_t(_Sp (_Tp::*__p)() const)
        : __p_(__p) {}
    _Sp operator()(const _Tp* __p) const
        {return (__p->*__p_)();}
};

int main(int, char**)
{
    typedef const_mem_fun_t<int, A> F;
    static_assert((is_base_of<unary_function<const A*, int>, F>::value), "");
    const F f(&A::a3);
    const A a = A();
    assert(f(&a) == 1);

  return 0;
}
 