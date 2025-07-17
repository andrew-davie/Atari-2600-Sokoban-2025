###############################################################################
# File: Makefile
# Description: CDFJ Test Makefile
# (C) Copyright 2017 - Chris Walton, Fred Quimby, Darrell Spice Jr
# Modifications for Sokoban by Andrew Davie
# used, modified, and distriuted with permission (April 2022)
###############################################################################

PROJECT=Sokoban

EMULATOR=gopher
#EMULATOR=stella
#DATESTAMP_ROM


DASM_TO_C=defines_from_dasm_for_c.h


# Tool names
TOOLPATH = ./tools
TOOLCHAIN= ~/Documents/software/gcc-arm-none-eabi/bin/arm-none-eabi

CC=$(TOOLCHAIN)-gcc
AS=$(TOOLCHAIN)-as
LD=$(TOOLCHAIN)-ld
OBJCOPY=$(TOOLCHAIN)-objcopy
SIZE=$(TOOLCHAIN)-size
DASM=$(TOOLPATH)/dasm

# Search path
VPATH += $(BASE):$(SRC)
SERIAL_DATE := $(shell date +"const int serialNumberDate = %y%m%d;")
SERIAL_TIME := $(shell date +"const int serialNumberime = %H%M%S;")
TARGET := $(shell date +"Sokoban_%Y%m%d@%H:%M:%S" | tr ' :' '__')
SERIAL2 := $(shell date +"%m%d%H%M")


ifeq ($(EMULATOR),gopher)
RUN=@zsh -c "nohup ../Gopher2600/gopher2600_darwin_arm64 -tv NTSC -right savekey ./$(PROJECT).bin &"
KILL=@killall -INT gopher2600_darwin_arm64 || true > silent
else
RUN=@open -a /Applications/Stella.app ./$(PROJECT).bin
KILL=@osascript -e 'quit app "Stella"'
endif


BASE = main
SRC = $(BASE)/custom
BIN = $(BASE)/bin
VPATH += $(BASE):$(SRC)

SRCS =  animations.c \
		atarivox.c \
		attribute.c \
		characterset.c \
		colour.c \
		custom.c \
		drawplayer.c \
		drawscreen.c \
		joystick.c \
		main.c \
		menu.c \
		player.c \
		random.c \
		rooms.c \
		man.c \
		score.c \
		scroll.c \
		sound.c \
		swipe.c

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)


CFLAGS = -g3 -gdwarf-4 -gstrict-dwarf -mcpu=arm7tdmi -march=armv4t -mthumb \
         -Wall -Wextra -Wunused-macros -ffunction-sections -Os \
        -Wl,--print-memory-usage,--build-id=none -flto -fno-builtin \
		-mno-thumb-interwork -fextended-identifiers \
        -MMD -MP


# Default target
default: armcode
	$(RUN)
	@sleep 1

.PHONY: killEmulator
killEmulator:
	$(KILL)

armcode_defines: gfx $(asmSrcs)
	@echo "Step 1/3 - Create $(DASM_TO_C)"
	$(DASM) $(PROJECT).asm -p100 -f3 -v0 -s$(PROJECT).sym -l$(PROJECT).lst -o$(PROJECT).bin
	@echo "// Do not change this file. It is auto-generated during the make process" > main/$(DASM_TO_C)
	awk '$$0 ~ /^_/ {printf "#define %-25s 0x%s\n", $$1, $$2}' $(PROJECT).sym >> main/$(DASM_TO_C)

	@echo "Step 2/3 - Create ARM BIN"

armcode_atari:
	$(DASM) $(PROJECT).asm -p100 -f3 -v0 -s$(PROJECT).sym -l$(PROJECT).lst -o$(PROJECT).bin
	@cp $(PROJECT).bin ROMs/$(TARGET).bin
	#@echo "Exporting BASE_6507_START" for linker...

armcode_list:

	@echo "Step 3/3 - Create BIN"
	$(DASM) $(PROJECT).asm -p100 -f3 -v0 -o$(PROJECT).bin -l$(PROJECT).lst -s$(PROJECT).sym

armcode: killEmulator armcode_defines armcode_arm armcode_list armcode_atari


gfx: iCC copyright.asm

copyright.asm: gfx/copyright.png
	@echo "processing copyright image"
	python3 $(TOOLPATH)/logo.py gfx/copyright.png

iCC: gfx/iCC_title.gif
	(cd gfx & python3 gfx/grid.py)


#flash:
#	lpc21isp -bin -wipe -verify -control -controlswap $(PROJECT).bin /dev/ttyUSB0 38400 10000

############################# CUSTOM C ##############################

CUSTOMNAME = armcode
CUSTOMELF = $(BIN)/$(CUSTOMNAME).elf
CUSTOMBIN = $(BIN)/$(CUSTOMNAME).bin
CUSTOMMAP = $(BIN)/$(CUSTOMNAME).map
CUSTOMLST = $(BIN)/$(CUSTOMNAME).lst
CUSTOMLINK = $(SRC)/custom.boot.lds

CUSTOMDEPENDS = sound.h main.h attribute.h
CUSTOMTARGETS = $(CUSTOMELF) $(CUSTOMBIN)


sokoban.asm : copyright.asm menuKernel.asm gameKernel.asm vcs.h macro.h cdfj.h atarivox/speakjet.h atarivox/speechData.asm

armcode_arm: $(CUSTOMTARGETS)
	@ls -l $(CUSTOMBIN)

$(CUSTOMELF): $(asmSrcs) $(OBJS) $(CUSTOMDEPENDS) Makefile
	$(CC) $(CFLAGS) -o $(CUSTOMELF) $(OBJS) -T $(CUSTOMLINK) -nostartfiles -Wl,-Map,$(CUSTOMMAP)
	$(TOOLCHAIN)-objdump -Sdrl $(CUSTOMELF) > $(BIN)/$(CUSTOMNAME).obj
#	,--gc-sections
$(CUSTOMBIN): $(CUSTOMELF)
	$(OBJCOPY) -O binary -S $(CUSTOMELF) $(CUSTOMBIN)
	$(SIZE) $(OBJS) $(CUSTOMELF)


-include $(DEPS)



clean:
	rm -f $(CUSTOMOBJS) *.i *.s $(BIN)/*.* $(PROJECT).bin $(PROJECT).lst $(PROJECT).sym


# EOF

