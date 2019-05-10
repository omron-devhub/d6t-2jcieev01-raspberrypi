
.PHONY: baro illm humi accl mmic

cpplint_flags:=--filter=-readability/casting,-build/include_subdir
ifeq (x$(cpplint),x)
cpplint := @echo lint with cpplint, option:
endif
ifeq (x$(cppcheck),x)
cppcheck := @echo lint with cppcheck, option:
endif
ifeq (x$(wiringpi),x)
wiringpi:=-lwiringPi
endif

all: baro illm humi accl mmic

baro: 2jcieev01-baro.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $(wiringpi) $^ -o 2jcieev01-baro

illm: 2jcieev01-illm.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $(wiringpi) $^ -o 2jcieev01-illm

humi: 2jcieev01-humi.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $(wiringpi) $^ -o 2jcieev01-humi

accl: 2jcieev01-accl.c
	$(cpplint) $(cpplint_flags) $^
	$(cppcheck) --enable=all $^
	gcc $(CFLAGS) $(wiringpi) $^ -o 2jcieev01-accl

mmic:
	@echo there is no sample for MEMS Microphone,
	@echo you can use  MEMS Microphone sensor from ALSA libraries or
	@echo basic `arecord` program.
	@echo please refer the README.

