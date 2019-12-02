PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)

TARGET = jetz
OBJS = src/ecs/ecs.o \
        src/ecs/systems/render_system.o \
		src/engine/camera.o \
		src/engine/engine.o \
        src/gpu/psp/psp_gpu.o \
		src/platforms/psp/psp_main.o \
		src/utl/utl_ringbuf.o

INCDIR   := $(INCDIR) . src

CFLAGS = -O2 -G0 -Wall -g -DJETZ_CONFIG_PLATFORM_PSP
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = -lpspgum_vfpu -lpspvfpu -lpspgu -lm
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Jetz PSP

include $(PSPSDK)/lib/build.mak

