#include <stdexcept>
#include <stdio.h>

namespace std
{
/*
     template <class T, class U = typename remove_reference<T>::type>
     class __npbinder : public U, nested_exception
     {
        public:
          __npbinder(T& t) : U(forward<T>(t)) { }
     };
*/
}
class report
{
public:
	report(const char* a) : hold(a) { printf("%s\n", a); }
        ~report() { printf("~%s\n", hold); }
private:
	const char *hold;
};
class vclass
{
public:
	virtual ~vclass() { }
};
class uncaught_display
{
public:
	uncaught_display(const char * msg) : hold(msg) { }
	~uncaught_display()
	{
		printf("uncaught %s %d %s\n", hold, std::uncaught_exceptions(), std::uncaught_exception() ? "true" : "false");
	}
private:
	const char *hold;
};
class thrower_cls
{
public:
  	thrower_cls(const char *x) :hold(x) {}
	~thrower_cls()
	{
                printf("thrower_cls:%s\n", hold);
                uncaught_display ud1("destructing");
		try
		{
	                uncaught_display ud2("destructing2");
			dothrow();
		}
		catch(std::runtime_error &aa)
		{
	                uncaught_display ud3("destructing3");
			printf("%s\n", aa.what());
		}
	}
private:
	const char * hold;
        void dothrow()
        {
	        std::runtime_error s("thrower_1");
        	throw s;
        }

};
void basic_throw()
{
	std::runtime_error s("basic");
	throw s;
}
void basic_catch()
{
	try
	{
		basic_throw();
	}
	catch (std::runtime_error& aa)
	{
		printf("%s\n", aa.what());
	}
}
void self_catch()
{
	try
	{
    	    std::runtime_error s("self");
    	    throw s;
	}
	catch (std::runtime_error& aa)
	{
		printf("%s\n", aa.what());
	}
}

void unwind_0()
{
	report aa("unwind_0");
	std::runtime_error s("unwind");
	throw s;
}
void unwind_1()
{
	report aa("unwind_1");
	unwind_0();
}
void unwind_catch()
{
	report aa("unwind_catch_1");
	try
	{
		report bb("unwind_catch_2");
		unwind_1();
	}
	catch (std::runtime_error& aa)
	{
		report cc("unwind_catch_3");
		printf("%s\n", aa.what());
	}
}
void rethrow()
{
	std::runtime_error s("rethrow");
	throw s;
}
void rethrow_catch_1()
{
	try
	{
		rethrow();
	}
	catch (std::runtime_error& aa)
	{
		throw;
	}
}
void rethrow_catch()
{
	try
	{
		rethrow_catch_1();
	}
	catch (std::runtime_error& aa)
	{
		printf("%s\n", aa.what());
	}
}
void rethrow_exception_ptr()
{
	std::runtime_error s("rethrow_exception_ptr");
	throw s;
}
void rethrow_exception_ptr_catch_1()
{
	try
	{
		rethrow_exception_ptr();
	}
	catch (std::runtime_error& aa)
	{
		auto xx = std::current_exception();
                std::rethrow_exception(xx);
	}
}
void rethrow_exception_ptr_catch()
{
	try
	{
		rethrow_exception_ptr_catch_1();
	}
	catch (std::runtime_error& aa)
	{
		printf("%s\n", aa.what());
	}
}
void throw_nest()
{
	std::runtime_error s("throw_nest");
	throw s;
}
void throw_nest_catch()
{
        uncaught_display ud1("idle");
	try
	{
		thrower_cls thwr("throw_nest");
		throw_nest();
	}
	catch (std::runtime_error& aa)
	{
	        uncaught_display ud2("idle2");
		printf("%s\n", aa.what());
	}
}
void rethrow_make_exception_ptr()
{
	std::runtime_error s("rethrow");
	throw s;
}
void rethrow_make_exception_ptr_catch_1()
{
	try
	{
		rethrow_make_exception_ptr();
	}
	catch (std::runtime_error& aa)
	{
                std::rethrow_exception(std::make_exception_ptr<std::logic_error>(std::logic_error("make_exception_ptr")));
	}
}
void rethrow_make_exception_ptr_catch()
{
	try
	{
		rethrow_make_exception_ptr_catch_1();
	}
	catch (std::logic_error& aa)
	{
		printf("%s\n", aa.what());
	}
}
std::exception_ptr globalmake = std::make_exception_ptr<std::logic_error>(std::logic_error("make_exception_ptr_global"));
void rethrow_make_exception_ptr_global_catch_1()
{
	try
	{
		rethrow_make_exception_ptr();
	}
	catch (std::runtime_error& aa)
	{
                std::rethrow_exception(globalmake);
	}
}
void rethrow_make_exception_ptr_global_catch()
{
	try
	{
		rethrow_make_exception_ptr_global_catch_1();
	}
	catch (std::logic_error& aa)
	{
		printf("%s\n", aa.what());
	}
}
void rethrow_unwind()
{
	std::runtime_error s("rethrow_unwind");
	throw s;
}
void rethrow_catch_unwind_1()
{
	try
	{
		rethrow_unwind();
	}
	catch (std::runtime_error& aa)
	{
                report("rethrow_unwind_catch");
		throw;
	}
}
void rethrow_catch_unwind_catch()
{
	try
	{
		rethrow_catch_unwind_1();
	}
	catch (std::runtime_error& aa)
	{
		printf("%s\n", aa.what());
	}
}
void rethrow_if_nested()
{
	std::runtime_error s("rethrow_if_nested");
	throw s;

}
void rethrow_if_nested_catch_1()
{
	try
	{
		rethrow_if_nested();
	}
	catch (std::runtime_error& aa)
	{
		printf("first catch\n");
		std::__npbinder<std::runtime_error> bound(aa);
		std::rethrow_if_nested(bound);
	}
}
void rethrow_if_nested_catch_2()
{
	try
	{
		rethrow_if_nested_catch_1();
	}
	catch (std::nested_exception& e)
	{
                std::runtime_error*x = dynamic_cast<std::runtime_error*>(&e);
		printf("from nested: %s\n", x->what());
		std::rethrow_exception(e.nested_ptr());
	}
}
void rethrow_if_nested_catch()
{
	std::runtime_error xx("nothrow");
        std::rethrow_if_nested(xx);
        
	try
	{
		rethrow_if_nested_catch_2();
	}
	catch (std::runtime_error& aa)
	{
		printf("unnested: %s\n", aa.what());
	}
}
int rr()
try
{
	throw 5;
}
catch (int a)
{
	printf("%d\n", a);
}
catch (...)
{
	printf("caught");
}
int main()
{
printf(":basic\n");
	basic_catch();
printf(":self\n");
        self_catch();
printf(":unwind\n");
        unwind_catch();
printf(":rethrow\n");
        rethrow_catch();
printf(":rethrow_exception_ptr\n");
        rethrow_exception_ptr_catch();
printf(":throw_nest\n");
        throw_nest_catch();
printf(":rethrow_make_exception_ptr\n");
	rethrow_make_exception_ptr_catch();
printf(":rethrow_make_exception_ptr_global\n");
	rethrow_make_exception_ptr_global_catch();
printf(":rethrow_catch_unwind\n");
	rethrow_catch_unwind_catch();
printf(":rethrow_if_nested\n");
        rethrow_if_nested_catch();

rr();
}