
ifneq ($(CC), emcc)
  CC = gcc
endif
.SUFFIXES=.c .o

#==============================================================================
# Define compilation target
#==============================================================================
  TARGET := HerosQuest
#==============================================================================

#==============================================================================
# Define every object required by compilation
#==============================================================================
  OBJS =                             \
          $(OBJDIR)/blastate.o       \
          $(OBJDIR)/collision.o      \
          $(OBJDIR)/introstate.o     \
          $(OBJDIR)/main.o           \
          $(OBJDIR)/playstate.o      \
          $(OBJDIR)/mob.o            
#==============================================================================

#==============================================================================
# Set OS flag
#==============================================================================
  OS := $(shell uname)
  ifeq ($(OS), MINGW32_NT-6.1)
    OS := Win
#   Also, set the icon
    ICON = $(WINICON)
  endif
  ifeq ($(CC), emcc)
    OS := emscript
  endif
#==============================================================================

#==============================================================================
# Define CFLAGS (compiler flags)
#==============================================================================
# Add all warnings and default include path
  CFLAGS := -Wall -I"./include/"
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -I"$(EMSCRIPTEN)/system/include/"
  endif
# Add architecture flag
  ARCH := $(shell uname -m)
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -m32
  else
    ifeq ($(ARCH), x86_64)
      CFLAGS := $(CFLAGS) -m64
    else
      CFLAGS := $(CFLAGS) -m32
    endif
  endif
# Add debug flags
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -O2
  else
    ifneq ($(RELEASE), yes)
      CFLAGS := $(CFLAGS) -g -O0 -DDEBUG
    else
      CFLAGS := $(CFLAGS) -O3
    endif
  endif
# Set flags required by OS
  ifeq ($(OS), Win)
    CFLAGS := $(CFLAGS) -I"/d/windows/mingw/include" -I/c/GFraMe/include
  else
    CFLAGS := $(CFLAGS) -fPIC
  endif
# Set a flag so we know it's compiling for EMCC
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -DEMSCRIPT
  endif
#==============================================================================

#==============================================================================
# Define LFLAGS (linker flags)
#==============================================================================
# Add the framework
  ifeq ($(RELEASE), yes)
    LFLAGS := -lGFraMe
  else
    LFLAGS := -lGFraMe_dbg
  endif
# Append SDL2
  LFLAGS := $(LFLAGS) -lSDL2
# Add libs and paths required by an especific OS
  ifeq ($(OS), Win)
    LFLAGS := -mwindows -lmingw32 $(LFLAGS) -lSDL2main
    LFLAGS := -L/d/windows/mingw/mingw32/lib $(LFLAGS)
# Prepend the framework search path
    LFLAGS := -L/c/GFraMe/lib/ $(LFLAGS)
# TODO Add OpenGL
  else
# Prepend the framework search path
    LFLAGS := -L/usr/lib/GFraMe/ $(LFLAGS)
# TODO Add OpenGL
  endif
#==============================================================================

#==============================================================================
# Define where source files can be found and where objects & binary are output
#==============================================================================
 VPATH := src
 OBJDIR := obj/$(OS)
 BINDIR := bin/$(OS)
#==============================================================================

#==============================================================================
# Define the generated icon
#==============================================================================
 WINICON := assets/icon.o
#==============================================================================

#==============================================================================
# Make the objects list constant (and the icon, if any)
#==============================================================================
 OBJS := $(OBJS)
#==============================================================================

#==============================================================================
# Define default compilation rule
#==============================================================================
all: MAKEDIRS $(BINDIR)/$(TARGET)
	date
#==============================================================================

#==============================================================================
# Rule for building a object file for emscript
#==============================================================================
emscript:
	# Ugly solution: call make with the correct params
	make RELEASE=yes CC=emcc bin/emscript/$(TARGET).bc
	make RELEASE=yes CC=emcc emscript_pkg
#==============================================================================

#==============================================================================
# Stupid rule for cleaning emscript build... gotta fix this at some point
#==============================================================================
emscript_clean:
	make CC=emcc clean
#==============================================================================

#==============================================================================
# Rule that should package the generated binary into a web page... hopefully
#==============================================================================
emscript_pkg: 
	rm -rf $(BINDIR)/pkg
	mkdir -p $(BINDIR)/pkg
	$(CC) --emrun -m32 -s TOTAL_STACK=5242880 -s TOTAL_MEMORY=33554432 -s USE_SDL=2 \
		-O2 $(BINDIR)/$(TARGET).bc                        \
		--preload-file assets/map.gfm@/map.gfm                    \
		--preload-file assets/atlas.bmp@/atlas.bmp                \
		--preload-file assets/expl.wav@/expl.wav                  \
		--preload-file assets/mysong.wav@/mysong.wav              \
		--preload-file assets/player_death.wav@/player_death.wav  \
		--preload-file assets/player_hit.wav@/player_hit.wav      \
		--preload-file assets/slime_death.wav@/slime_death.wav    \
		--preload-file assets/slime_hit.wav@/slime_hit.wav        \
		--preload-file assets/song.wav@/song.wav                  \
		--preload-file assets/wall_hit.wav@/wall_hit.wav          \
		-o $(BINDIR)/pkg/$(TARGET).html
#==============================================================================

#==============================================================================
# Define a rule to generated the icon
#==============================================================================
$(WINICON):
	windres assets/icon.rc $(WINICON)
#==============================================================================

#==============================================================================
# Rule for actually building the game
#==============================================================================
$(BINDIR)/$(TARGET): MAKEDIRS $(OBJS)  $(ICON)
	gcc $(CFLAGS) -o $@ $(OBJS) $(ICON) $(LFLAGS)
#==============================================================================

#==============================================================================
# Rule for building the game with emscript
#==============================================================================
$(BINDIR)/$(TARGET).bc: MAKEDIRS $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) ./libGFraMe.bc
#==============================================================================

#==============================================================================
# Rule for compiling any .c in its object
#==============================================================================
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
#==============================================================================

#==============================================================================
# Rule for creating every directory
#==============================================================================
MAKEDIRS: | $(OBJDIR)
#==============================================================================

#==============================================================================
# Rule for actually creating every directory
#==============================================================================
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
#==============================================================================

.PHONY: clean mostlyclean
clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)/$(TARGET)

mostlyclean: clean
	rmdir $(OBJDIR)
	rmdir $(BINDIR)

