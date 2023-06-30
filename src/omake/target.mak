%.o: %.c .__BUILTIN
	${CC} ${CPPFLAGS} ${CFLAGS} -c $<

%.o: %.cc .__BUILTIN
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $<

%.o: %.cpp .__BUILTIN
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $<

%.o: %.s .__BUILTIN
	${AS} ${ASFLAGS} $<

%.exe: %.o .__BUILTIN
	${CC} ${LDFLAGS} $^ ${LOADLIBES} ${LDLIBS}

%.c: %.y .__BUILTIN
	${YACC} ${YFLAGS} $<

