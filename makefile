PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)

TARGET = comanche
OBJS = src/main.o src/gpu.o src/input.o src/terrain.o

CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = -lpspgum_vfpu -lpspvfpu -lpspgu -lm
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Comanche PSP

include $(PSPSDK)/lib/build.mak

