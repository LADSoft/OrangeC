B := py $(ORANGEC)\lit\lit.py --echo-all-commands

all: copyfiles lit.site.cfg
	$(B) std

current: 
	$(B) std/algorithms
	$(B) std/atomics
	$(B) std/containers
	$(B) std/depr
	$(B) std/diagnostics
	$(B) std/iterators
	$(B) std/thread

algorithms: copyfiles lit.site.cfg
	$(B) std/algorithms

atomics: copyfiles lit.site.cfg
	$(B) std/atomics

containers: copyfiles lit.site.cfg
	$(B) std/containers

depr: copyfiles lit.site.cfg
	$(B) std/depr

diagnostics: copyfiles lit.site.cfg
	$(B) std/diagnostics

experimental: copyfiles lit.site.cfg
	$(B) std/experimental

input.output: copyfiles lit.site.cfg
	$(B) std/input.output

iterators: copyfiles lit.site.cfg
	$(B) std/iterators

language.support: copyfiles lit.site.cfg
	$(B) std/language.support

localization: copyfiles lit.site.cfg
	$(B) std/localization

numerics: copyfiles lit.site.cfg
	$(B) std/numerics

re:
	$(B) std/re

strings: copyfiles lit.site.cfg
	$(B) std/strings

thread: copyfiles lit.site.cfg
	$(B) std/thread

utilities: copyfiles lit.site.cfg
	$(B) std/utilities

lit.site.cfg:
	makelitcfg > lit.site.cfg

copyfiles: $(ORANGEC)\src\clibs\cpp\libcxx\include

$(ORANGEC)\src\clibs\cpp\libcxx\include: $(ORANGEC)\src\clibs\cpp\stdinc_libcxx\exception
	copy $(ORANGEC)\src\clibs\cpp\stdinc_libcxx\exception $(ORANGEC)\src\clibs\cpp\libcxx\include