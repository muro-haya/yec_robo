# SPDX-License-Identifier: MIT
# Copyright (c) 2013, 2014 Damien P. George
# Copyright (c) 2019-2022 The Pybricks Authors

# ensure required git submodules checked out
ifeq ("$(wildcard ../../micropython/README.md)","")
$(info GIT cloning micropython submodule)
$(info $(shell cd ../.. && git submodule update --init micropython))
ifeq ("$(wildcard ../../micropython/README.md)","")
$(error failed)
endif
endif
ifeq ("$(wildcard ../../micropython/lib/axtls/README)","")
$(info GIT cloning axtls submodule)
$(info $(shell cd ../../micropython && git submodule update --init lib/axtls))
ifeq ("$(wildcard ../../micropython/lib/axtls/README)","")
$(error failed)
endif
endif
ifeq ("$(wildcard ../../micropython/lib/berkeley-db-1.xx/README)","")
$(info GIT cloning berkeley-db-1.xx submodule)
$(info $(shell cd ../../micropython && git submodule update --init lib/berkeley-db-1.xx))
ifeq ("$(wildcard ../../micropython/lib/berkeley-db-1.xx/README)","")
$(error failed)
endif
endif

# lets micropython make files work with external files
USER_C_MODULES = ../..

# Environment
-include mpconfigport.mk
include ../../micropython/py/mkenv.mk

# Frozen Python code
FROZEN_MANIFEST ?= manifest.py

# define main target
PROG = pybricks-micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS = ../_common/qstrdefs.h
QSTR_GLOBAL_DEPENDENCIES = brickconfig.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(TOP)/py/py.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I../../lib/contiki-core
INC += -I../../lib/ev3dev
INC += -I../../lib/ev3dev/include
INC += -I../../lib/lego
INC += -I../../lib/pbio
INC += -I../../lib/pbio/include
INC += -I../../lib/pbio/platform/ev3dev_stretch
INC += -I../..
INC += -I$(TOP)/ports/unix

# compiler settings
CFLAGS_WARN = -Wall -Werror -Wextra -Wno-unused-parameter -Wno-maybe-uninitialized
CFLAGS_WARN += -Wpointer-arith -Wdouble-promotion -Wfloat-conversion
CFLAGS += $(INC)-std=gnu99 -DUNIX $(CFLAGS_WARN) $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)

# Debugging/Optimization
ifdef DEBUG
COPT ?= -Og
else
COPT ?= -Os
COPT += -DNDEBUG
endif

# Remove unused sections.
COPT += -fdata-sections -ffunction-sections

# Always enable symbols -- They're occasionally useful, and don't make it into the
# final .bin/.hex/.dfu so the extra size doesn't matter.
CFLAGS += -g

ifndef DEBUG
# _FORTIFY_SOURCE is a feature in gcc/glibc which is intended to provide extra
# security for detecting buffer overflows. Some distros (Ubuntu at the very least)
# have it enabled by default.
#
# gcc already optimizes some printf calls to call puts and/or putchar. When
# _FORTIFY_SOURCE is enabled and compiling with -O1 or greater, then some
# printf calls will also be optimized to call __printf_chk (in glibc). Any
# printfs which get redirected to __printf_chk are then no longer synchronized
# with printfs that go through mp_printf.
#
# In MicroPython, we don't want to use the runtime library's printf but rather
# go through mp_printf, so that stdout is properly tied into streams, etc.
# This means that we either need to turn off _FORTIFY_SOURCE or provide our
# own implementation of __printf_chk. We've chosen to turn off _FORTIFY_SOURCE.
# It should also be noted that the use of printf in MicroPython is typically
# quite limited anyways (primarily for debug and some error reporting, etc
# in the unix version).
#
# Information about _FORTIFY_SOURCE seems to be rather scarce. The best I could
# find was this: https://securityblog.redhat.com/2014/03/26/fortify-and-you/
# Original patchset was introduced by
# https://gcc.gnu.org/ml/gcc-patches/2004-09/msg02055.html .
#
# Turning off _FORTIFY_SOURCE is only required when compiling with -O1 or greater
CFLAGS += -U _FORTIFY_SOURCE
endif

# On OSX, 'gcc' is a symlink to clang unless a real gcc is installed.
# The unix port of MicroPython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
ifeq ($(MICROPY_FORCE_32BIT),1)
CC = clang -m32
else
CC = clang
endif
# Use clang syntax for map file
LDFLAGS_ARCH = -Wl,-map,$@.map -Wl,-dead_strip
else
# Use gcc syntax for map file
LDFLAGS_ARCH = -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif
LDFLAGS += $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

# Flags to link with pthread library
LDFLAGS += -lpthread

ifeq ($(MICROPY_USE_READLINE),1)
INC += -I$(TOP)/shared/readline
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
SHARED_SRC_C_EXTRA += readline/readline.c
endif
ifeq ($(MICROPY_PY_TERMIOS),1)
CFLAGS_MOD += -DMICROPY_PY_TERMIOS=1
SRC_MOD += ports/unix/modtermios.c
endif
ifeq ($(MICROPY_PY_SOCKET),1)
CFLAGS_MOD += -DMICROPY_PY_SOCKET=1
SRC_MOD += ports/unix/modusocket.c
endif

ifeq ($(MICROPY_PY_FFI),1)
LIBFFI_CFLAGS_MOD := $(shell pkg-config --cflags libffi)
LIBFFI_LDFLAGS_MOD := $(shell pkg-config --libs libffi)

ifeq ($(UNAME_S),Linux)
LIBFFI_LDFLAGS_MOD += -ldl
endif

CFLAGS_MOD += $(LIBFFI_CFLAGS_MOD) -DMICROPY_PY_FFI=1
LDFLAGS_MOD += $(LIBFFI_LDFLAGS_MOD)
SRC_MOD += ports/unix/modffi.c
endif

ifeq ($(MICROPY_PY_JNI),1)
# Path for 64-bit OpenJDK, should be adjusted for other JDKs
CFLAGS_MOD += -I/usr/lib/jvm/java-7-openjdk-amd64/include -DMICROPY_PY_JNI=1
SRC_MOD += ports/unix/modjni.c
endif

CFLAGS_MOD += $(shell pkg-config --cflags libudev)
LDFLAGS_MOD += $(shell pkg-config --libs libudev)

CFLAGS_MOD += $(shell pkg-config --cflags glib-2.0)
LDFLAGS_MOD += $(shell pkg-config --libs glib-2.0)

CFLAGS_MOD += $(shell pkg-config --cflags gio-2.0)
LDFLAGS_MOD += $(shell pkg-config --libs gio-2.0)

CFLAGS_MOD += $(shell pkg-config --cflags grx-3.0)
LDFLAGS_MOD += $(shell pkg-config --libs grx-3.0)

# for pbsmbus
ifneq ($(shell $(CC) -print-file-name=libi2c.a),libi2c.a)
# in i2ctools v4, there is an acutal library and the header file has moved
CFLAGS_MOD += -DPB_HAVE_LIBI2C=1
LDFLAGS_MOD += -li2c
endif

# Sources and libraries common to all pybricks bricks
PBIO_PLATFORM = ev3dev_stretch

include ../_common/sources.mk

# Extra core MicroPython files

# NB: Since we are using MicroPython's build system, files in the micropython/
# directory have the micropython/ prefix excluded. It is very important to do
# it that way since there is special handling of certain files that will break
# if we don't do it this way. So we need to be very careful about name clashes
# between the top level directory and the micropython/ subdirectory.

PY_EXTRA_SRC_C += $(addprefix ports/unix/,\
	alloc.c \
	coverage.c \
	gccollect.c \
	input.c \
	main.c \
	modmachine.c \
	modtime.c \
	modufcntl.c \
	modummap.c \
	moduselect.c \
	mpthreadport.c \
	)

SRC_C += $(SRC_MOD)

SHARED_SRC_C += $(addprefix shared/,\
	runtime/gchelper_generic.c \
	runtime/pyexec.c \
	timeutils/timeutils.c \
	$(SHARED_SRC_C_EXTRA) \
	)

# Pybricks port core source files
PYBRICKS_SRC_C += \
	ev3dev_mphal.c \
	modbluetooth.c \
	modusignal.c \
	modmedia_ev3dev.c \
	pb_type_ev3dev_font.c \
	pb_type_ev3dev_image.c \
	pb_type_ev3dev_speaker.c \
	pbinit.c \
	pbsmbus.c \


EV3DEV_LIB_SRC_C = $(addprefix lib/,\
	ev3dev/src/ev3dev_stretch/lego_motor.c \
	ev3dev/src/ev3dev_stretch/lego_port.c \
	ev3dev/src/ev3dev_stretch/lego_sensor.c \
	ev3dev/src/ev3dev_stretch/nxtcolor.c \
	ev3dev/src/ev3dev_stretch/sysfs.c \
	pbio/platform/ev3dev_stretch/status_light.c \
	)

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(CONTIKI_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(EV3DEV_LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PBIO_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PY_EXTRA_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PYBRICKS_PYBRICKS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PYBRICKS_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C) $(PY_EXTRA_SRC_C) $(EXTMOD_SRC_C) $(PYBRICKS_SRC_C) $(PYBRICKS_PYBRICKS_SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

ifneq ($(FROZEN_MANIFEST),)
# To use frozen code create a manifest.py file with a description of files to
# freeze, then invoke make with FROZEN_MANIFEST=manifest.py (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMPZ_DIG_SIZE=16 # force 16 bits to work on both 32 and 64 bit archs
endif

ifneq ($(FROZEN_MANIFEST),)
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif

include $(TOP)/py/mkrules.mk

.PHONY: test

EV3DEV_TEST_DIRS = $(addprefix ../../tests/ev3dev/, \
	brick \
	experimental \
	geometry \
	media \
	messaging \
	motor \
	parameters \
	)

GRX_TEST_PLUGIN_OBJ := $(BUILD)/grx-plugin.o
GRX_TEST_PLUGIN_LIB := $(BUILD)/libgrx-3.0-vdriver-test.so

$(GRX_TEST_PLUGIN_OBJ): ../../tests/ev3dev/grx-plugin.c
	$(Q)$(CC) -c -fPIC -o $@ $^ $(CFLAGS)

$(GRX_TEST_PLUGIN_LIB): $(GRX_TEST_PLUGIN_OBJ)
	$(Q)$(CC) -shared -o $@ $^ $(LDFLAGS)

test-ev3dev: $(PROG) $(TOP)/tests/run-tests.py $(GRX_TEST_PLUGIN_LIB)
	cd $(TOP)/tests && MICROPY_MICROPYTHON="../../tests/ev3dev/test-wrapper.sh" \
		GRX_PLUGIN_PATH=$(realpath $(BUILD)) GRX_DRIVER=test \
		./run-tests.py --test-dirs $(EV3DEV_TEST_DIRS)

test: $(PROG) $(TOP)/tests/run-tests.py $(GRX_TEST_PLUGIN_LIB)
	$(eval DIRNAME=../bricks/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON="../../tests/ev3dev/test-wrapper.sh" \
		GRX_PLUGIN_PATH=$(realpath $(BUILD)) GRX_DRIVER=test \
		./run-tests.py

# Value of configure's --host= option (required for cross-compilation).
# Deduce it from CROSS_COMPILE by default, but can be overridden.
ifneq ($(CROSS_COMPILE),)
CROSS_COMPILE_HOST = --host=$(patsubst %-,%,$(CROSS_COMPILE))
else
CROSS_COMPILE_HOST =
endif

PREFIX = /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin

install: $(PROG)
	install -d $(BINDIR)
	install $(PROG) $(BINDIR)/$(PROG)

uninstall:
	-rm $(BINDIR)/$(PROG)
