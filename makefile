PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)

TARGET = jetz
OBJS = src/ecs/ecs.o \
		src/ecs/components/ecs_static_model.o \
		src/ecs/components/ecs_transform.o \
		src/ecs/systems/player_system.o \
		src/ecs/systems/render_system.o \
		src/engine/camera.o \
		src/engine/engine.o \
		src/engine/world.o \
		src/geo/geo.o \
		src/geo/geo_plane.o \
		src/gpu/gpu.o \
		src/gpu/gpu_anim_model.o \
		src/gpu/gpu_material.o \
		src/gpu/gpu_plane.o \
		src/gpu/gpu_static_model.o \
		src/gpu/gpu_texture.o \
		src/gpu/pspgu/pspgu.o \
		src/gpu/pspgu/pspgu_material.o \
		src/gpu/pspgu/pspgu_plane.o \
		src/gpu/pspgu/pspgu_static_mesh.o \
		src/gpu/pspgu/pspgu_static_model.o \
		src/gpu/pspgu/pspgu_texture.o \
		src/log/log.o \
		src/lua/lua_script.o \
		src/platform/psp/psp_main.o \
		src/platform/psp/psp_misc.o \
		src/thirdparty/lua/lua.o \
		src/thirdparty/md5/md5anim.o \
		src/thirdparty/md5/md5mesh.o \
		src/thirdparty/rxi_map/src/map.o \
		src/thirdparty/stb/stb_image.o \
		src/thirdparty/tinyobj/tinyobj_loader.o \
		src/utl/utl_ringbuf.o

INCDIR   := $(INCDIR) . src

# Flags:
# -std=c11							Use C11 compiler mode
# JETZ_CONFIG_PLATFORM_PSP			Set Jetz build for PSP platform
# LLONG_MAX							Used for Lua configuration (luaconf.h)
CFLAGS = -O2 -G0 -Wall -g -std=c11 -DJETZ_CONFIG_PLATFORM_PSP -DLLONG_MAX
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR := $(LIBDIR) . src/thirdparty/lua/psp-5.3.4/lib
LIBS = -lpspgum_vfpu -lpspvfpu -lpspgu -lm -lpsprtc -llua
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Jetz PSP

include $(PSPSDK)/lib/build.mak

