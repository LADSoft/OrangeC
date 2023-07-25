%.o: %.c .__BUILTIN
	${CC} ${CPPFLAGS} ${CFLAGS} -o $@ -c $<

%.o: %.cc .__BUILTIN
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -o $@ -c $<

%.o: %.cpp .__BUILTIN
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -o $@ -c $<

%.o: %.s .__BUILTIN
	${AS} ${ASFLAGS} -o $@ $<

%.exe: %.o .__BUILTIN
	${CC} -o $@ ${LDFLAGS} $^ ${LOADLIBES} ${LDLIBS}

%.c: %.y .__BUILTIN
	${YACC} ${YFLAGS} $<

