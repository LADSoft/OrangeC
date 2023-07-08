B := py $(ORANGEC)\lit\lit.py --echo-all-commands --config-prefix=hi


all: copyfiles
	$(B) std

current:
	$(B) std/algorithms
	$(B) std/atomics
	$(B) std/containers
	$(B) std/depr
	$(B) std/diagnostics
	$(B) std/iterators
	$(B) std/thread

algorithms: copyfiles
	$(B) std/algorithms

atomics: copyfiles
	$(B) std/atomics

containers: copyfiles
	$(B) std/containers

depr: copyfiles
	$(B) std/depr

diagnostics: copyfiles
	$(B) std/diagnostics

experimental: copyfiles
	$(B) std/experimental

input.output: copyfiles
	$(B) std/input.output

iterators: copyfiles
	$(B) std/iterators

language.support: copyfiles
	$(B) std/language.support

localization: copyfiles
	$(B) std/localization

numerics: copyfiles
	$(B) std/numerics

re:
	$(B) std/re

strings: copyfiles
	$(B) std/strings

thread: copyfiles
	$(B) std/thread

utilities: copyfiles
	$(B) std/utilities

copyfiles:
	copy $(ORANGEC)\src\clibs\cpp\stdinc_libcxx\exception $(ORANGEC)\src\clibs\cpp\libcxx\include