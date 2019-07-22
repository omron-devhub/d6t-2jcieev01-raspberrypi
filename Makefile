
.PHONY: d6t-1a d6t-8l d6t-44l d6t-32l

cpplint_flags:=--filter=-readability/casting,-build/include_subdir
ifeq (x$(cpplint),x)
cpplint := @echo lint with cpplint, option:
endif
ifeq (x$(cppcheck),x)
cppcheck := @echo lint with cppcheck, option:
endif

all: d6t-1a d6t-8l d6t-44l d6t-32l

d6t-1a: d6t-1a.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $^ -o $@

d6t-8l: d6t-8l.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $^ -o $@

d6t-44l: d6t-44l.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $^ -o $@

d6t-32l: d6t-32l.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $^ -o $@

