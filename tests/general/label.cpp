// public domain

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <memory>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

DWORD align(DWORD size, DWORD aligned)
{
	return (size + aligned - 1) / aligned * aligned;
}

enum AddrType { Abs, RVA, Rel };
struct Address
{
	shared_ptr<DWORD> addr;
	AddrType type;
	Address() : type(Abs) {}
	Address(DWORD addr, AddrType type) :
		addr(shared_ptr<DWORD>(new DWORD(addr))), type(type) {}
	Address(shared_ptr<DWORD> addr, AddrType type) :
		addr(addr), type(type) {}
	Address(const Address &ad) :
		addr(ad.addr), type(ad.type) {}
};
bool operator!(const Address &ad)
{
	return !ad.addr.get();
}

template <typename T> struct Wrap
{
	T val;
	Wrap(T val) : val(val) {}
};
typedef Wrap< WORD> u2;
typedef Wrap<DWORD> u4;
typedef Wrap<Address> Ptr;

class Buffer
{
private:
	DWORD imgbase, start;
	vector<BYTE> buffer;
	vector<pair<DWORD, Address>> values;
	vector<pair<DWORD, shared_ptr<DWORD>>> addrs;

public:
	Buffer()
	{
		clear();
	}

	void clear()
	{
		imgbase = start = 0;
		buffer.clear();
		values.clear();
		addrs.clear();
	}

	inline size_t size() const
	{
		return buffer.size();
	}
	inline void resize(size_t size)
	{
		buffer.resize(size, 0);
	}
	inline void expand(size_t size)
	{
		resize(buffer.size() + size);
	}
	void align(size_t aligned)
	{
		resize(::align(size(), aligned));
	}

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

	Buffer &operator << (const Buffer &buf)
	{
		auto sz = buffer.size();
		expand(buf.size());
		memcpy(&buffer[sz], &buf.buffer[0], buf.size());
		for (auto v : buf.values)
			values.emplace_back(sz + v.first, v.second);
		for (auto v : buf.addrs)
			addrs.emplace_back(sz + v.first, v.second);
		return *this;
	}

	Buffer &operator << (const Address &f)
	{
		values.emplace_back(size(), f);
		expand(4);
		return *this;
	}

	Buffer &operator << (const char *s)
	{
		return add(s, strlen(s) + 1);
	}

	Buffer &operator << (const string &s)
	{
		return add(s.c_str(), s.size() + 1);
	}

	template <typename T> Buffer &operator << (const vector<T> &vec)
	{
		return add(&vec[0], sizeof(T) * vec.size());
	}

	template <typename T> Buffer &operator << (T *src)
	{
		return add(src, sizeof(T));
	}

	template <typename T> Buffer &operator << (const Wrap<T> &v)
	{
		return add(&v.val, sizeof(v.val));
	}

	Address addr(AddrType type = Abs)
	{
		if (!addrs.empty())
		{
			auto p = addrs.back().second;
			if (*p == size())
				return Address(p, type);
		}
		Address ret(size(), type);
		addrs.emplace_back(size(), ret.addr);
		return ret;
	}

	inline Address rva()
	{
		return addr(RVA);
	}

	void put(const Address &addr)
	{
		addrs.emplace_back(size(), addr.addr);
	}

	void reloc(DWORD imgbase, DWORD rva)
	{
		this->imgbase = imgbase;
		start = imgbase + rva;
		for (auto ad : addrs)
			*ad.second = start + ad.first;
	}

	void dump()
	{
		for (int i = 0; i < size(); i += 16)
		{
			string asc;
			printf("%08x ", start + i);
			for (int j = 0; j < 16; ++j)
			{
				if (i + j < size())
				{
					auto b = buffer[i + j];
					printf("%02x ", b);
					asc += 32 <= b && b < 127 ? b : '.';
				}
				else printf("   ");
			}
			printf("%s\n", asc.c_str());
		}
	}

	void write(FILE *f, size_t aligned = 1)
	{
		for (auto p : values)
		{
			auto ad = *p.second.addr;
			switch (p.second.type)
			{
			case RVA:
				ad -= imgbase;
				break;
			case Rel:
				ad -= start + p.first + 4;
				break;
			}
			*reinterpret_cast<LPDWORD>(&buffer[p.first]) = ad;
		}
		fwrite(&buffer[0], size(), 1, f);
		if (aligned > 1)
		{
			vector<BYTE> pad(::align(size(), aligned) - size());
			fwrite(&pad[0], 1, pad.size(), f);
		}
	}
};

Buffer *curtext;

class Section : public Buffer
{
public:
	string name;
	IMAGE_SECTION_HEADER h;

	Section(const string &name, DWORD ch) : name(name)
	{
		memset(&h, 0, sizeof(h));
		memcpy(h.Name, name.c_str(), min((int)name.size(), 8));
		h.Characteristics = ch;
	}

	bool bss()
	{
		return h.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA;
	}
};

class PE
{
private:
	IMAGE_DOS_HEADER dosh;
	Buffer stub;
	IMAGE_NT_HEADERS32 peh;
	IMAGE_FILE_HEADER *fh;
	IMAGE_OPTIONAL_HEADER32 *oph;
	vector<Section> sections;
	Section *text, *data, *bss, *rdata, *idata;
	map<string, map<string, Address>> imports;
	map<string, Address> syms;

public:
	PE()
	{
		init();
	}

	void init()
	{
		sections.clear();
		stub.clear();
		imports.clear();
		syms.clear();

		sections.push_back(Section(".text",
			IMAGE_SCN_CNT_CODE | IMAGE_SCN_CNT_INITIALIZED_DATA |
			IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ));
		sections.push_back(Section(".data",
			IMAGE_SCN_CNT_INITIALIZED_DATA |
			IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE));
		sections.push_back(Section(".bss",
			IMAGE_SCN_CNT_UNINITIALIZED_DATA |
			IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE));
		sections.push_back(Section(".rdata",
			IMAGE_SCN_CNT_INITIALIZED_DATA |
			IMAGE_SCN_MEM_READ));
		sections.push_back(Section(".idata",
			IMAGE_SCN_CNT_INITIALIZED_DATA |
			IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE));

		text = section(".text");
		data = section(".data");
		bss = section(".bss");
		rdata = section(".rdata");
		idata = section(".idata");

		memset(&dosh, 0, sizeof(dosh));
		dosh.e_magic = *(LPWORD)"MZ";
		dosh.e_cblp = 0x90;
		dosh.e_cp = 3;
		dosh.e_cparhdr = 4;
		dosh.e_maxalloc = 0xffff;
		dosh.e_sp = 0xb8;
		dosh.e_lfarlc = 0x40;
		dosh.e_lfanew = 0x80;

		stub << 0x0e;               // push cs
		stub << 0x1f;               // pop ds
		stub << 0xba << u2(0x000e); // mov dx, 0x000e
		stub << 0xb4 << 9;          // mov ah, 9
		stub << 0xcd << 0x21;       // int 0x21
		stub << 0xb8 << u2(0x4c01); // mov ax, 0x4c01
		stub << 0xcd << 0x21;       // int 0x21
		stub << "This program cannot be run in DOS mode.\r\r\n$";

		memset(&peh, 0, sizeof(peh));
		peh.Signature = *(LPDWORD)"PE\0\0";

		fh = &peh.FileHeader;
		fh->Machine = 0x14c;
		fh->SizeOfOptionalHeader = sizeof(*oph);
		fh->Characteristics = 0x102;

		oph = &peh.OptionalHeader;
		oph->Magic = 0x10b;
		oph->MajorLinkerVersion = 4;
		oph->AddressOfEntryPoint = 0x1000;
		oph->ImageBase = 0x400000;
		oph->SectionAlignment = 0x1000;
		oph->FileAlignment = 0x200;
		oph->MajorOperatingSystemVersion = 4;
		oph->MinorOperatingSystemVersion = 0;
		oph->MajorSubsystemVersion = 4;
		oph->MinorSubsystemVersion = 0;
		oph->Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
		oph->SizeOfStackReserve = 0x100000;
		oph->SizeOfStackCommit = 0x1000;
		oph->SizeOfHeapReserve = 0x100000;
		oph->SizeOfHeapCommit = 0x1000;
		oph->NumberOfRvaAndSizes = 16;
	}

	Section *section(const string &name)
	{
		for (int i = 0; i < sections.size(); i++)
		{
			auto sec = &sections[i];
			if (sec->name == name) return sec;
		}
		return NULL;
	}

	void select()
	{
		curtext = text;
	}

	DWORD align(DWORD size)
	{
		return ::align(size, oph->SectionAlignment);
	}

	DWORD falign(DWORD size)
	{
		return ::align(size, oph->FileAlignment);
	}

	Address sym(const string &s)
	{
		auto it = syms.find(s);
		return it == syms.end() ? Address() : it->second;
	}

	Address str(const string &s)
	{
		auto it = syms.find(s);
		if (it != syms.end()) return it->second;

		auto ret = rdata->addr();
		syms[s] = ret;
		*rdata << s;
		rdata->align(4);
		return ret;
	}

	Address ptr(const string &s, const Address &ptr)
	{
		auto it = syms.find(s);
		if (it != syms.end()) return it->second;

		auto ret = data->addr();
		syms[s] = ret;
		*data << ptr;
		return ret;
	}

	Address alloc(const string &s, size_t size)
	{
		auto it = syms.find(s);
		if (it != syms.end()) return it->second;

		auto ret = bss->addr();
		syms[s] = ret;
		bss->expand(::align(size, 4));
		return ret;
	}

	Address dword(const string &s, DWORD val)
	{
		auto it = syms.find(s);
		if (it != syms.end()) return it->second;

		auto ret = data->addr();
		syms[s] = ret;
		*data << &val;
		return ret;
	}

	void write(FILE *f)
	{
		mkidata();
		DWORD rva = oph->SectionAlignment;
		vector<Section *> sects;
		for (int i = 0; i < sections.size(); i++)
		{
			auto sect = &sections[i];
			auto size = sect->size();
			if (size > 0)
			{
				sects.push_back(sect);
				sect->h.Misc.VirtualSize = size;
				sect->h.VirtualAddress = rva;
				sect->reloc(oph->ImageBase, rva);
				rva += align(size);
			}
			else
			{
				sect->h.Misc.VirtualSize = 0;
				sect->h.VirtualAddress = 0;
			}
		}
		fh->NumberOfSections = sects.size();
		oph->BaseOfCode = text->h.VirtualAddress;
		oph->SizeOfCode = falign(text->size());
		oph->BaseOfData = data->h.VirtualAddress;
		oph->SizeOfInitializedData = falign(data->size());
		oph->SizeOfUninitializedData = falign(bss->size());
		oph->SizeOfImage = rva;
		oph->SizeOfHeaders = falign(
			dosh.e_lfanew + sizeof(peh)
			+ sizeof(IMAGE_SECTION_HEADER) * sects.size());
		oph->DataDirectory[1].VirtualAddress = idata->h.VirtualAddress;
		oph->DataDirectory[1].Size = idata->size();

		Buffer header;
		header << &dosh << stub;
		header.resize(dosh.e_lfanew);
		header << &peh;

		DWORD ptr = oph->SizeOfHeaders;
		for (auto sect : sects)
		{
			auto size = falign(sect->size());
			if (sect->bss())
			{
				sect->h.SizeOfRawData = 0;
				sect->h.PointerToRawData = 0;
			}
			else
			{
				sect->h.SizeOfRawData = size;
				sect->h.PointerToRawData = ptr;
			}
			header << &sect->h;
			ptr += sect->h.SizeOfRawData;
		}

		header.write(f, oph->FileAlignment);
		for (auto sect : sects)
			if (!sect->bss())
				sect->write(f, oph->FileAlignment);
	}

	Address import(const string &dll, const string &sym)
	{
		auto it = imports.find(dll);
		if (it == imports.end())
		{
			Address ret(0, Abs);
			imports[dll][sym] = ret;
			return ret;
		}
		auto it2 = it->second.find(sym);
		if (it2 == it->second.end())
		{
			Address ret(0, Abs);
			it->second[sym] = ret;
			return ret;
		}
		return it2->second;
	}

private:
	void mkidata()
	{
		idata->clear();
		if (imports.empty()) return;

		Buffer idt, ilt, iat, hn, name;
		for (auto dll : imports)
		{
			idt << ilt.rva() << u4(0) << u4(0) << name.rva() << iat.rva();
			name << dll.first;
			for (auto sym : dll.second)
			{
				iat.put(sym.second);
				ilt << hn.rva();
				iat << hn.rva();
				hn << u2(0) << sym.first;
				hn.align(2);
			}
			ilt << u4(0);
			iat << u4(0);
		}
		idt.expand(sizeof(IMAGE_IMPORT_DESCRIPTOR));
		*idata << idt << ilt << iat << hn << name;
	}
};


enum reg64
{
	rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi,
	r8, r9, r10, r11, r12, r13, r14, r15
};
enum reg32 { eax, ecx, edx, ebx, esp, ebp, esi, edi };
enum reg16 { ax, cx, dx, bx, sp, bp, si, di };
enum reg8  { al, cl, dl, bl, ah, ch, dh, bh };

struct
{
	template <typename T> Wrap<T> operator[](T t)
	{
		return Wrap<T>(t);
	}
} ptr;

void ret()
{
	*curtext << 0xc3;
}
void nop()
{
	*curtext << 0x90;
}
void mov(reg32 r1, reg32 r2)
{
	*curtext << 0x89 << 0xc0 + r1 + (r2 << 3);
}
void mov(reg32 r, DWORD v)
{
	*curtext << 0xb8 + r << &v;
}
void mov(reg32 r, Address ad)
{
	*curtext << 0xb8 + r << ad;
}
void mov(reg32 r, Ptr p)
{
	if (r == eax) *curtext << 0xa1 << p.val;
	else *curtext << 0x8b << (r << 3) + 5 << p.val;
}
void mov(Ptr p, DWORD v)
{
	*curtext << 0xc7 << 0x05 << p.val << &v;
}
void mov(Ptr p, Address ad)
{
	*curtext << 0xc7 << 0x05 << p.val << ad;
}
void mov(Ptr p, reg32 r)
{
	if (r == eax) *curtext << 0xa3 << p.val;
	else *curtext << 0x89 << (r << 3) + 5 << p.val;
}
void add(reg32 r1, reg32 r2)
{
	*curtext << 0x01 << 0xc0 + r1 + (r2 << 3);
}
void add(reg32 r, DWORD v)
{
	if (v < 128) *curtext << 0x83 << 0xc0 + r << v;
	else *curtext << 0x81 << 0xc0 + r << &v;
}
void add(reg32 r, Address ad)
{
	*curtext << 0x81 << 0xc0 + r << ad;
}
void push(reg32 r)
{
	*curtext << 0x50 + r;
}
void push(DWORD v)
{
	*curtext << 0x68 << &v;
}
void push(Address ad)
{
	*curtext << 0x68 << ad;
}
void push(Ptr p)
{
	*curtext << 0xff << 0x35 << p.val;
}
void call(Ptr p)
{
	*curtext << 0xff << 0x15 << p.val;
}
void call(Address ad)
{
	*curtext << 0xe8 << Address(ad.addr, Rel);
}
void jmp(Address ad)
{
	*curtext << 0xe9 << Address(ad.addr, Rel);
}
void jc(Address ad)
{
	*curtext << 0x0f << 0x82 << Address(ad.addr, Rel);
}
void jnc(Address ad)
{
	*curtext << 0x0f << 0x83 << Address(ad.addr, Rel);
}
void jz(Address ad)
{
	*curtext << 0x0f << 0x84 << Address(ad.addr, Rel);
}
void jnz(Address ad)
{
	*curtext << 0x0f << 0x85 << Address(ad.addr, Rel);
}

// addition
void inc(reg32 r)
{
	*curtext << 0x40 + r;
}
void cmp(reg32 r, DWORD v)
{
	if (v < 128)
		*curtext << 0x83 << 0xf8 + r << v;
	else
	{
		if (r == eax) *curtext << 0x3d << &v;
		else *curtext << 0x81 << 0xf8 + r << &v;
	}
}


int main()
{
	auto f = fopen("label1.exe", "wb");
	if (!f) return 1;

	PE pe;
	pe.select();

	/*
	.intel_syntax
	mov ebx, 0
	jmp label2
	label1:
	push ebx
	push format
	call printf
	add esp, 8
	label2:
	inc ebx
	cmp ebx, 10
	jnz label1
	push 0
	call exit
	0:  jmp 0b
	.data
	format: .ascii "%d\10\0"
	*/
	mov(ebx, 0);
	Address label2(0, Abs);
	jmp(label2);
	auto label1 = curtext->addr();
	push(ebx);
	push(pe.str("%d\n"));
	call(ptr[pe.import("msvcrt.dll", "printf")]);
	add(esp, 8);
	curtext->put(label2);
	inc(ebx);
	cmp(ebx, 10);
	jnz(label1);
	push(0);
	call(ptr[pe.import("msvcrt.dll", "exit")]);
	jmp(curtext->addr());

	pe.write(f);
	fclose(f);
}
