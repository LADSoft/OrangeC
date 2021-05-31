#include <functional>

#ifdef __ORANGEC__
#define invoke __invoke
#endif

struct __nat
{
	~__nat() = delete;
};
template <class _Ret, class _Fp, class ..._Args>
struct __invokable_r
{
  template <class _XFp, class ..._XArgs>
  static auto __try_call(int) -> decltype(
    std::invoke(std::declval<_XFp>(), std::declval<_XArgs>()...));
  template <class _XFp, class ..._XArgs>
  static __nat __try_call(...);

  // FIXME: Check that _Ret, _Fp, and _Args... are all complete types, cv void,
  // or incomplete array types as required by the standard.
  using _Result = decltype(__try_call<_Fp, _Args...>(0));

  using type =
  typename std::conditional<
      !std::is_same<_Result, __nat>::value,
      typename std::conditional<
          std::is_void<_Ret>::value,
          std::true_type,
          std::is_convertible<_Result, _Ret>
      >::type,
      std::false_type
  >::type;
  static const bool value = type::value;
   
};

struct cc
{
	int operator()(int*, int*);
};

#ifdef __ORANGEC__
#define invoke __invoke
#endif

typedef int*pint;
int dd(int*, int*);
int main(int argc)
{
//	printf("%d\n", __invokable_r<int, cc, int*, int*>::value);
//	printf("%d\n", __invokable_r<int, cc, int, int>::value);
//	printf("%d\n", __invokable_r<void, cc, int*, int*>::value);
	printf("%d\n", __invokable_r<void, cc, int, int>::value);
//	printf("%d\n", __invokable_r<int, decltype(dd), int*, int*>::value);
//	printf("%d\n", __invokable_r<int, decltype(dd), int, int>::value);
//	std::function<bool(int* , int*)> bb = [](int* a, int* b) { printf("hi"); return true; };
//	__invokable_r<int, decltype(dd), int, int>::_Result aa1;
//	bb(&argc, &argc);
//	cc aa;
//	invoke(aa, pint(), pint());
}