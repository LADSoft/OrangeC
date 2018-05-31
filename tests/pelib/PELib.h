#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <windows.h>

DWORD align(DWORD size, DWORD aligned);

enum AddrType { Abs, RVA, Rel};
struct Address
{
    std::shared_ptr<DWORD> addr;
    AddrType type;
    Address();
    Address(DWORD addr, AddrType type = Abs);
    Address(std::shared_ptr<DWORD> addr, AddrType type);
    Address(const Address &ad);
};
bool operator!(const Address &ad);

template <typename T> struct Wrap
{
    T val;
    Wrap(T val): val(val) {}
};
typedef Wrap< BYTE> u1;
typedef Wrap< WORD> u2;
typedef Wrap<DWORD> u4;
typedef Wrap<Address> Ptr;

class Buffer
{
private:
    DWORD imgbase, start;
    std::vector<BYTE> buffer;
    std::vector<std::pair<DWORD, Address>> values;
    std::vector<std::pair<DWORD, std::shared_ptr<DWORD>>> addrs;

public:
    Buffer();

    inline size_t size() const { return buffer.size(); }
    inline void resize(size_t size) { buffer.resize(size, 0); }
    inline void expand(size_t size) { resize(buffer.size() + size); }

    void clear();
    void align(size_t aligned);

    inline Buffer &add(const void *src, int size)
	{
        auto sz = buffer.size();
        expand(size);
        memcpy(&buffer[sz], src, size);
        return *this;
    }

    inline Buffer &operator << (BYTE b1)
	{
        buffer.push_back(b1);
        return *this;
    }

    Buffer &operator << (const Buffer &buf);
    Buffer &operator << (const Address &f);
    Buffer &operator << (const char *s);
    Buffer &operator << (const std::string &s);

    template <typename T> Buffer &operator << (const std::vector<T> &vec)
	{
        return add(&vec[0], sizeof(T) * vec.size());
    }

    template <typename T> Buffer &operator << (T *src)
	{
        return add(src, sizeof(T));
    }

    template <typename T> Buffer &operator << (const Wrap<T> &v) {
        return add(&v.val, sizeof(v.val));
    }

    Address addr(AddrType type = Abs);
    inline Address rva() { return addr(RVA); }
    void put(const Address &addr);
    void reloc(DWORD imgbase, DWORD rva);
    void dump();
    void write(std::FILE *f, size_t aligned = 1);
};

class Section: public Buffer
{
public:
    std::string name;
    IMAGE_SECTION_HEADER h;

    Section(const std::string &name, DWORD ch);
    bool bss();
};

extern Buffer *curtext;

class PE
{
private:
    IMAGE_DOS_HEADER dosh;
    Buffer stub;
    IMAGE_NT_HEADERS32 peh;
    IMAGE_FILE_HEADER *fh;
    IMAGE_OPTIONAL_HEADER32 *oph;
    std::vector<Section> sections;
    std::vector<Section *> sects;
    Section *text, *data, *bss, *rdata, *idata;
    std::map<std::string, std::map<std::string, Address>> imports;
    std::map<std::string, Address> syms;

public:
    PE();
    void init();
    Section *section(const std::string &name);
    void select();
    DWORD align(DWORD size);
    DWORD falign(DWORD size);
    Address sym(const std::string &s, bool create = false);
    Address str(const std::string &s);
    Address ptr(const std::string &s, const Address &ptr);
    Address alloc(const std::string &s, size_t size);
    Address dword(const std::string &s, DWORD val);
    void link();
    void write(std::FILE *f);
    Address import(const std::string &dll, const std::string &sym);

private:
    void mkidata();
};


enum reg64 { rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi,
             r8 , r9 , r10, r11, r12, r13, r14, r15 };
enum reg32 { eax, ecx, edx, ebx, esp, ebp, esi, edi };
enum reg16 {  ax,  cx,  dx,  bx,  sp,  bp,  si,  di };
enum reg8  {  al,  cl,  dl,  bl,  ah,  ch,  dh,  bh };

struct
{
    template <typename T> Wrap<T> operator[](T t)
	{
        return Wrap<T>(t);
    }
} ptr;

void nop();
void ret();
void leave();
void mov(reg32 r1, reg32 r2);
void mov(reg32 r, DWORD v);
void mov(reg32 r, Address ad);
void mov(reg32 r, Ptr p);
void mov(Ptr p, DWORD v);
void mov(Ptr p, Address ad);
void mov(Ptr p, reg32 r);
void add(reg32 r1, reg32 r2);
void add(reg32 r, DWORD v);
void add(reg32 r, Address ad);
void push(reg32 r);
void push(DWORD v);
void push(Address ad);
void push(Ptr p);
void push(Wrap<reg32> p);
void call(Ptr p);
void call(Address ad);
void jmp (Ptr p);
void jmp (Address ad);
void jc  (Address ad);
void jnc (Address ad);
void jz  (Address ad);
void jnz (Address ad);
void inc(reg32 r);
void cmp(reg32 r, DWORD v);
