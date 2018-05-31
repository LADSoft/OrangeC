#include "PELib.h"

#include <cstdlib>
#include <cstring>

using namespace std;

DWORD align(DWORD size, DWORD aligned)
{
	return (size + aligned - 1) / aligned * aligned;
}

Address::Address(): type(Abs) {}
Address::Address(DWORD addr, AddrType type):
	addr(shared_ptr<DWORD>(new DWORD(addr))), type(type) {}
Address::Address(shared_ptr<DWORD> addr, AddrType type):
	addr(addr), type(type) {}
Address::Address(const Address &ad):
	addr(ad.addr), type(ad.type) {}

bool operator!(const Address &ad)
{
	return !ad.addr.get();
}

Buffer::Buffer()
{
	clear();
}

void Buffer::clear()
{
	imgbase = start = 0;
	buffer.clear();
	values.clear();
	addrs .clear();
}

void Buffer::align(size_t aligned)
{
	resize(::align(size(), aligned));
}

Buffer &Buffer::operator << (const Buffer &buf)
{
	auto sz = buffer.size();
	expand(buf.size());
	memcpy(&buffer[sz], &buf.buffer[0], buf.size());
	for (auto v: buf.values)
		values.emplace_back(sz + v.first, v.second);
	for (auto v: buf.addrs)
		addrs.emplace_back(sz + v.first, v.second);
	return *this;
}

Buffer &Buffer::operator << (const Address &f)
{
	values.emplace_back(size(), f);
	expand(4);
	return *this;
}

Buffer &Buffer::operator << (const char *s)
{
	return add(s, strlen(s) + 1);
}

Buffer &Buffer::operator << (const string &s)
{
	return add(s.c_str(), s.size() + 1);
}

Address Buffer::addr(AddrType type)
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

void Buffer::put(const Address &addr)
{
	addrs.emplace_back(size(), addr.addr);
}

void Buffer::reloc(DWORD imgbase, DWORD rva)
{
	this->imgbase = imgbase;
	start = imgbase + rva;
	for (auto ad: addrs)
		*ad.second = start + ad.first;
}

void Buffer::dump()
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
			} else printf("   ");
		}
		printf("%s\n", asc.c_str());
	}
}

void Buffer::write(FILE *f, size_t aligned)
{
	for (auto p: values)
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

Buffer *curtext;

Section::Section(const string &name, DWORD ch) : name(name)
{
	memset(&h, 0, sizeof(h));
	memcpy(h.Name, name.c_str(), min((int)name.size(), 8));
	h.Characteristics = ch;
}

bool Section::bss()
{
	return h.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA;
}

PE::PE()
{
	init();
}

void PE::init() {
	sections.clear();
	sects.clear();
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

	text  = section(".text");
	data  = section(".data");
	bss   = section(".bss");
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

Section *PE::section(const string &name)
{
	for (int i = 0; i < sections.size(); i++)
	{
		auto sec = &sections[i];
		if (sec->name == name) return sec;
	}
	return NULL;
}

void PE::select() { curtext = text; }

DWORD PE::align(DWORD size)
{
	return ::align(size, oph->SectionAlignment);
}

DWORD PE::falign(DWORD size)
{
	return ::align(size, oph->FileAlignment);
}

Address PE::sym(const string &s, bool create)
{
	auto it = syms.find(s);
	if (it != syms.end())
		return it->second;
	if (!create)
		return Address();
	Address ret(0);
	syms[s] = ret;
	return ret;
}

Address PE::str(const string &s)
{
	auto it = syms.find(s);
	if (it != syms.end())
		return it->second;

	auto ret = rdata->addr();
	syms[s] = ret;
	*rdata << s;
	rdata->align(4);
	return ret;
}

Address PE::ptr(const string &s, const Address &ptr)
{
	auto it = syms.find(s);
	if (it != syms.end())
		return it->second;

	auto ret = data->addr();
	syms[s] = ret;
	*data << ptr;
	return ret;
}

Address PE::alloc(const string &s, size_t size)
{
	auto it = syms.find(s);
	if (it != syms.end())
		return it->second;

	auto ret = bss->addr();
	syms[s] = ret;
	bss->expand(::align(size, 4));
	return ret;
}

Address PE::dword(const string &s, DWORD val)
{
	auto it = syms.find(s);
	if (it != syms.end())
		return it->second;

	auto ret = data->addr();
	syms[s] = ret;
	*data << &val;
	return ret;
}

void PE::link()
{
	sects.clear();
	mkidata();
	DWORD rva = oph->SectionAlignment;
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
}

void PE::write(FILE *f)
{
	if (sects.empty()) link();

	Buffer header;
	header << &dosh << stub;
	header.resize(dosh.e_lfanew);
	header << &peh;

	DWORD ptr = oph->SizeOfHeaders;
	for (auto sect: sects)
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
	for (auto sect: sects)
		if (!sect->bss())
			sect->write(f, oph->FileAlignment);
}

Address PE::import(const string &dll, const string &sym)
{
	auto it = imports.find(dll);
	if (it == imports.end())
	{
		Address ret(0);
		imports[dll][sym] = ret;
		return ret;
	}
	auto it2 = it->second.find(sym);
	if (it2 == it->second.end())
	{
		Address ret(0);
		it->second[sym] = ret;
		return ret;
	}
	return it2->second;
}

void PE::mkidata()
{
	idata->clear();
	if (imports.empty()) return;

	Buffer idt, ilt, iat, hn, name;
	for (auto dll: imports)
	{
		idt << ilt.rva() << u4(0) << u4(0) << name.rva() << iat.rva();
		name << dll.first;
		for (auto sym: dll.second)
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


void nop() { *curtext << 0x90; }
void ret() { *curtext << 0xc3; }
void leave() { *curtext << 0xc9; }
void mov(reg32 r1, reg32 r2) { *curtext << 0x89 << 0xc0 + r1 + (r2 << 3); }
void mov(reg32 r, DWORD v) { *curtext << 0xb8 + r << &v; }
void mov(reg32 r, Address ad) { *curtext << 0xb8 + r << ad; }
void mov(reg32 r, Ptr p)
{
	if (r == eax) *curtext << 0xa1 << p.val;
	else *curtext << 0x8b << (r << 3) + 5 << p.val;
}
void mov(Ptr p, DWORD v) { *curtext << 0xc7 << 0x05 << p.val << &v; }
void mov(Ptr p, Address ad) { *curtext << 0xc7 << 0x05 << p.val << ad; }
void mov(Ptr p, reg32 r)
{
	if (r == eax) *curtext << 0xa3 << p.val;
	else *curtext << 0x89 << (r << 3) + 5 << p.val;
}
void add(reg32 r1, reg32 r2) { *curtext << 0x01 << 0xc0 + r1 + (r2 << 3); }
void add(reg32 r, DWORD v) {
	if (v < 128) *curtext << 0x83 << 0xc0 + r << v;
	else *curtext << 0x81 << 0xc0 + r << &v;
}
void add(reg32 r, Address ad) { *curtext << 0x81 << 0xc0 + r << ad; }
void push(reg32 r) { *curtext << 0x50 + r; }
void push(DWORD v) { *curtext << 0x68 << &v; }
void push(Address ad) { *curtext << 0x68 << ad; }
void push(Ptr p) { *curtext << 0xff << 0x35 << p.val; }
void push(Wrap<reg32> p)
{
	switch (p.val)
	{
	case esp: *curtext << 0xff << 0x34 << 0x24; break;
	case ebp: *curtext << 0xff << 0x75 << u1(0); break;
	default : *curtext << 0xff << 0x30 + p.val; break;
	}
}
void call(Ptr p) { *curtext << 0xff << 0x15 << p.val; }
void call(Address ad) { *curtext << 0xe8 << Address(ad.addr, Rel); }
void jmp (Ptr p) { *curtext << 0xff << 0x25 << p.val; }
void jmp (Address ad) { *curtext << 0xe9 << Address(ad.addr, Rel); }
void jc  (Address ad) { *curtext << 0x0f << 0x82 << Address(ad.addr, Rel); }
void jnc (Address ad) { *curtext << 0x0f << 0x83 << Address(ad.addr, Rel); }
void jz  (Address ad) { *curtext << 0x0f << 0x84 << Address(ad.addr, Rel); }
void jnz (Address ad) { *curtext << 0x0f << 0x85 << Address(ad.addr, Rel); }
void inc(reg32 r) { *curtext << 0x40 + r; }
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
