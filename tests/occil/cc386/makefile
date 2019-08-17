%.ilo: %.c
	..\..\release\occil /c /9 /E- /Di386 /Lkernel32 $^

list = AN386.ilo \
ANALYZE.ilo \
ARGS.ilo \
BROWSE.ilo \
CMAIN.ilo \
CONF386.ilo \
cppbltin.ilo \
DBG386.ilo \
DECL.ilo \
DECLASS.ilo \
einit.ilo \
ERROR.ilo \
EXPR.ilo \
EXT.ilo \
FATAL.ilo \
FLOAT.ilo \
floatexp.ilo \
FRIEND.ilo \
FUNC.ilo \
getdisk.ilo \
GETSYM.ilo \
GEXPR386.ilo \
GSTMT386.ilo \
INASM386.ilo \
INIT.ilo \
INLINE.ilo \
intexpr.ilo \
intr386.ilo \
INVOKE.ilo \
LIST.ilo \
mangle.ilo \
MEMMGT.ilo \
OBJ386.ilo \
OPTIMIZE.ilo \
OUTAS386.ilo \
outco386.ilo \
output.ilo \
Peep386.ilo \
PREPROC.ilo \
PSTR.ilo \
REG386.ilo \
rtti.ilo \
SEARCHKW.ilo \
SRCHPATH.ilo \
STMT.ilo \
SYMBOL.ilo \
TEMPLATE.ilo \
TYPES.ilo \
USAGE.ilo

q.exe: $(list)
	..\..\release\netlink /oq.exe @&&|
$(list)
|
