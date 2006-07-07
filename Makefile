####################################################
#                                                  #
# Copyright (c) 2000 by Michal 'Khorne' Rzechonek  #
# Copyright (c) 2006 by Seweryn Habdank-Wojewodzki #
#                                                  #
####################################################

# Target binaries
PROGS=knnl_demo

# Source files' extension
EXT=cpp

# Compiler
GCC=g++
 
## Libraries to link against
## example:
#LIBS+= -lm
LIBS+= -lboost_program_options-gcc
#LIBS+=-lboost_serialization-gcc
#LIBS+=-lboost_date_time-gcc
LIBS+= -lboost_filesystem-gcc
# LIBS+=$(shell pkg-config --libs gtkmm-2.4)

## Local include paths
INCLUDE=-I.

## Linker flags
LFLAGS+= -z combreloc 
#LFLAGS+= -static

## Compilation flags
CFLAGS+= -Wall -pedantic
CFLAGS+= -pipe 
CFLAGS+= -O2
#CFLAGS+= -march=i586
#CFLAGS+= -mtune=i686
#CFLAGS+= -ffloat-store 
CFLAGS+= -msse
CFLAGS+= $(shell ./gcccpuopt.sh)
## Use flag below for C code for C++ code better not (increase size and reduce speed).
## This flag is very good for optimising low level system functions.
#CFLAGS+= -fomit-frame-pointer
 
## example:
# CFLAGS+=$(shell pkg-config --cflags gtkmm-2.4)

## Debugging and profiling flags
#GFLAGS+= -p
#GFLAGS+= -pg
#GFLAGS+= -ggdb3
#GFLAGS+= -DFTDEBUG
#GFLAGS+= -DTDEBUG
#GFLAGS+= -fprofile-arcs
#GFLAGS+= -ftest-coverage

## To erase "assert"
#GFLAGS += -DNDEBUG

## To add "press any key section" to work with:
## pmap `pgrep ${PROGS}`
# GFLAGS += -DPMAP

######################################
# You shouldn't need to modify these #
######################################

CC=${GCC} ${INCLUDE} ${CFLAGS} ${GFLAGS} 
LINK=${GCC} ${GFLAGS} ${LFLAGS}
GENDEP=${GCC} ${GFLAGS} -MM ${INCLUDE} ${CFALGS}

OBJS:=$(patsubst %.${EXT},%.o, $(wildcard *.${EXT}))
DEPS:=$(patsubst %.${EXT},.%.d, $(wildcard *.${EXT}))

.PHONY: all
all: ${PROGS}
	@ echo Build complete
	@ echo LFLAGS = ${LFLAGS}
	@ echo CFLAGS = ${CFLAGS}
	@ echo GFLAGS = ${GFLAGS}
	@ echo LIBS = ${LIBS}

.%.d: %.${EXT}
	@ echo DEP $<
	@ ${GENDEP} $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

${PROGS}: ${DEPS} ${OBJS}
	@ echo LINK $@ 
	@ ${LINK} ${OBJS} ${LIBS} -o $@.debug
	@ strip -R .comment -R .note -R .note.ABI-tag -s $@.debug -o $@

${OBJS}: %.o: %.${EXT}
	@ echo CC $<
	@ ${CC} -c $<

REALDEPS:=$(wildcard .*.d)
ifneq ($(strip $(REALDEPS)),)
include ${REALDEPS}
endif

.PHONY: clean
clean:
	@ echo CLEAN ${PROGS}
	@ rm -f ${PROGS}
	@ rm -f ${PROGS}.debug

	@ echo CLEAN ${OBJS}
	@ rm -f ${OBJS}

	@ echo CLEAN ${DEPS}
	@ rm -f ${DEPS}

