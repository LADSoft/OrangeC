#include <stdlib.h>
#include <typeinfo>
#include <list>
#include <iostream>
#include <string>
// this isn't the best way to write a delegate class, use two linked lists for
// optimization.   I'm doing it this way to test the compiler a bit...
template <class RtnVal, class ... Params>
class delegate
{
private:
    struct _StorageBase
    {
        virtual RtnVal Execute(Params ...params) { }
    };
    template <class T>
    struct _MemberStorage : _StorageBase
    {
        
        _MemberStorage(T& instance, RtnVal (T::*memberFunc)(Params ...) ) :
                       MemberFunc(memberFunc), Instance(instance) { }
        RtnVal (T::*MemberFunc)(Params ...);
        T &Instance;
        virtual RtnVal Execute(Params ...params) 
        {
            if (MemberFunc)
                return (Instance.*MemberFunc)(params ...);
            else
                return RtnVal();
        }
    };
    struct _NonMemberStorage : _StorageBase
    {
        _NonMemberStorage( RtnVal (*nonMemberFunc)(Params ...) ) :
                          NonMemberFunc(nonMemberFunc) { }
        RtnVal (*NonMemberFunc)(Params ...);
        virtual RtnVal Execute(Params ... params) 
        {
            if (NonMemberFunc)
                return NonMemberFunc(params ...);
            else
                return RtnVal();                
        }
    };
public:
    delegate() { }
    ~delegate() 
    { 
        for (typename std::list<_StorageBase *>::iterator it = functions.begin(); it != functions.end(); ++it)
        {
            _StorageBase *funct = *it;
            delete funct;
        }
    }
    template<class T>
    void Attach(T& instance, RtnVal (T::*func)(Params ...) )
    {
        for (auto item:functions)
        {
            if (typeid(*item) == typeid(_MemberStorage<T>))
            {
                _MemberStorage<T> *i = (_MemberStorage<T> *)item;
                if ( i->MemberFunc == func && &i->Instance == &instance)
                    return;
            }
        }
        _MemberStorage<T> *next = new _MemberStorage<T>(instance, func);
        functions.push_back(next);
    }
    template <class T>
    void Detach(T& instance, RtnVal (T::*func)(Params ...) )
    {
        for (typename std::list<_StorageBase *>::iterator it = functions.begin(); it != functions.end(); ++it)
        {
            _StorageBase *item = *it;
            if (typeid(*item) == typeid(_MemberStorage<T>))
            {
                _MemberStorage<T> *i = (_MemberStorage<T> *)item;
                if ( i->MemberFunc == func && &i->Instance == &instance)
                {
                    functions.erase(it);
                    delete item;
                    break;
                }
            }
        }
    }
    void Attach( RtnVal (*func)(Params ...) )
    {
        for (auto item:functions)
        {
            if (typeid(*item) == typeid(_NonMemberStorage))
            {
                if (((_NonMemberStorage *)item)->NonMemberFunc == func)
                    return;
            }
        }
        _NonMemberStorage *next = new _NonMemberStorage(func);
        functions.push_back(next);
    }
    void Detach( RtnVal (*func)(Params ...))
    {
        for (typename std::list<_StorageBase *>::iterator it = functions.begin(); it != functions.end(); ++it)
        {
            _StorageBase *item = *it;
            if (typeid(*item) == typeid(_NonMemberStorage))
            {
                _NonMemberStorage *i = (_NonMemberStorage *)item;
                if ( i->NonMemberFunc == func)
                {
                    functions.erase(it);
                    delete item;
                    break;
                }
            }
        }
    }
    RtnVal operator() (Params ... params)
    {
        if (functions.size())
        {
            if (functions.size() == 1)
            {
                return functions.front()->Execute(params ...);
            }
            else
            {
                for (auto item:functions)
                {
                    item->Execute(params ...);
                }
            }
            return RtnVal();
        }
    }
private:
    delegate(const delegate &);
    delegate & operator =(const delegate &);
    
    std::list<_StorageBase *> functions;
};
class Fruit
{
public:    
    Fruit() { }
    virtual ~Fruit() { }
    virtual void Print(char *msg) { }
    static void DisplayAll(char *msg)
    {
        items(msg);
    }
protected:
    static delegate<void, char *> items;
};
delegate<void, char *> Fruit::items;
class Apple : public Fruit
{
public:
    Apple() { items.Attach(*this, &Apple::Print); }
    virtual ~Apple() { items.Detach(*this, &Apple::Print); }
    virtual void Print(char *msg)
    {
        std::cout << "Apple " << std::string(msg) << std::endl;
    }
};
class Orange : public Fruit
{
public:
    Orange() { items.Attach(*this, &Orange::Print); }
    virtual ~Orange() { items.Detach(*this, &Orange::Print); }
    virtual void Print(char *msg)
    {
        std::cout << "Orange " << std::string(msg) << std::endl;
    }
};
class Peach : public Fruit
{
public:
    Peach() { items.Attach(*this, &Peach::Print); }
    virtual ~Peach() { items.Detach(*this, &Peach::Print); }
    virtual void Print(char *msg)
    {
        std::cout << "Peach " << std::string(msg) << std::endl;
    }
};
class Pear : public Fruit
{
public:
    Pear() { items.Attach(*this, &Pear::Print); }
    virtual ~Pear() { items.Detach(*this, &Pear::Print); }
    virtual void Print(char *msg)
    {
        std::cout << "Pear " << std::string(msg) << std::endl;
    }
};

class aa
{
public:
    aa(char *m) :M(m) { }
    void print(char *x)
    {
        std::cout << M << x;
    }
    char *M;
};
void print(char *x)
{
    std::cout << x;
}
int main()
{
    delegate<void, char *> item;
    item.Attach(print);
    item("hi dave\n");
    aa zz("well ");
    delegate<void, char *> item2;
    item2.Attach(zz, &aa::print);
    item2("bye dave\n");

    Fruit *f = new Pear();
    f->Print(" is cute!");
    std::cout << std::endl;

    Apple *a1 = new Apple();
    Pear *p1 = new Pear();
    Peach *pe1 = new Peach();
    Apple *a2 = new Apple();
    Orange *o1 = new Orange();
    Peach *pe2 = new Peach();
    
    Fruit::DisplayAll("is a nice fruit!");
    delete a2;
    delete pe2;
    std::cout << std::endl;
    Fruit::DisplayAll("is a nice fruit!");
}
