; Sokoban 2025
; (c) 2025 Andrew Davie

; REMEMBER cannot use lda # as this is overloaded for datastream access!!
; use ldx # or ldy # instead

; Note: mode changes (normal/menu) in kernel occur at START of overscan
; and continue immediately with new kernel jump


        PROCESSOR 6502
        include "vcs.h"
        include "macro.h"
        include "cdfj.h"


__ENABLE_ATARIVOX       = 0         ;  415 bytes?


BASE_6507_START SET $70FD

; Note: when adjusting BASE_6507_START if we want C-code to take the beneath space in
; bank $7000 (i.e., up to BASE_6507_START) then we need to modify custom.boot.lds to
; set the sizing of the C area correctly. DO NOT USE THE ABOVE VALUE; USE THE OUTPUT VALUE
; ON BUILD, AS IT IS ADJUSTED BELOW


    IF __ENABLE_ATARIVOX
        include "atarivox/speakjet.h"
    ENDIF


; Symbols prefixed with _ are exported to 'main/defines_from_dasm_for_c.h'
; This is done via 'awk' in the makefile.


; which ARM function to run
; also must update runFunc table in main.c with same order

_FN_SYSTEM_RESET        = 0         ; SystemReset()
_FN_MENU_OS             = 1         ; MenuOverScan()
_FN_MENU_VB             = 2         ; MenuVerticalBlank()
_FN_MENU_IDLE           = 3         ; SchedulerMenu()
_FN_INIT_GAME_KERNEL    = 4         ; InitializeGameKernel()
_FN_GAME_OS             = 5         ; GameOverscan()
_FN_GAME_VB             = 6         ; GameVerticalBlank()
_FN_GAME_IDLE           = 7         ; Scheduler()


_DS_COLUP0      = DS0DATA
_DS_COLUP1      = DS1DATA

_DS_SPEECH      = DS2DATA

; datastream usage for Game Kernel
_DS_PF0_LEFT    = DS4DATA
_DS_PF1_LEFT    = DS5DATA
_DS_PF2_LEFT    = DS6DATA
_DS_PF0_RIGHT   = DS7DATA
_DS_PF1_RIGHT   = DS8DATA
_DS_PF2_RIGHT   = DS9DATA

_DS_CTRLPF      = DS10DATA

_DS_GRP0a       = DS11DATA
_DS_GRP1a       = DS12DATA

_DS_COLUBK      = DS13DATA

_DS_AUDV0       = DS14DATA
_DS_AUDC0       = DS15DATA
_DS_AUDF0       = DS16DATA

_DS_GRP0b       = DS17DATA
_DS_GRP1b       = DS18DATA
_DS_GRP0c       = DS19DATA
_DS_GRP1c       = DS20DATA

_DS_COLUPF      = DS21DATA


__BOARD_WIDTH = 40
__BOARD_DEPTH = 22
__BOARD_SIZE = (__BOARD_DEPTH * __BOARD_WIDTH)


; timer values
VB_TIM64T = 43
OS_TIM64T = 29

; ; color values
; _BLACK           = $00
; _WHITE           = $0E
; _GREY            = $00
; _YELLOW          = $10
; _ORANGE          = $20
; _BROWN           = $30
; _RED             = $40
; _PURPLE          = $50
; _VIOLET          = $60
; _INDIGO          = $70
; _BLUE            = $80
; _BLUE2           = $90
; _TURQUOISE       = $A0
; _CYAN            = $B0
; _GREEN           = $C0
; _YELLOW_GREEN    = $D0
; _OCHRE_GREEN     = $E0
; _OCHRE           = $F0

;_HAIR            = $F4
;_FACE            = $4C

; controls spacing in main menu

_ARENA_SCANLINES    = 198   ; number of scanlines for the arena
_ICC_SCANLINES = _ARENA_SCANLINES/3
ARENA_BUFFER_SIZE   = 198    ; PF buffer size for largest arena
; _ARENA_SCANLINES2 = 396


;===============================================================================
; Define Zero Page RAM Usage
;----------------------------------------
;   ZP RAM variables can only be seen by the 6507 CPU
;   Likewise C variables can only be seen by the ARM CPU
;===============================================================================

        SEG.U VARS
        ORG $80

kernel          ds 1       ; 0 copyright, 1 menu, 2 game, 3 stats
currentKernel   ds 1

    IF __ENABLE_ATARIVOX
what            ds 1

highScoreSK         ds 3
startRoom           ds 1        ; room * 5
startLevel          ds 1
offsetSK            ds 1        ; for calculating the SK slot address

    ENDIF


        echo "----",($00FE - *) , "bytes of RAM left (space reserved for 2 byte stack)"



;===============================================================================
; Define Start of Cartridge
;----------------------------------------
;   CDFJ cartridges must start with the Harmony/Melody driver.  The driver is
;   the ARM code that emulates the CDFJ coprocessor.
;===============================================================================

                    SEG CODE
                    ORG 0

                    incbin "cdfdriver20190317.bin"

SIZEOF_CDFJ_DRIVER = *
        echo "CDFJ driver = $0 - ", *, "(",[*]d, "bytes)"

;===============================================================================
; ARM user code
; Banks 0 thru n
;----------------------------------------
;   The ARM code starts at $0800 and grows into bank 0+
;===============================================================================

;                    ORG $0800
ARM_CODE

                    incbin "main/bin/armcode.bin"

SIZEOF_ARM_CODE = * - ARM_CODE
        echo "C (ARM code) =", ARM_CODE, "-", *, "(",[SIZEOF_ARM_CODE]d, "bytes)"

    echo "(calculated free space = ", (BASE_6507_START - *)d,")"
    echo "(realistic free space ($7000 base) = ", ($7000 - *)d,")"

;===============================================================================
; ARM Indirect Data
;----------------------------------------
;   Data that the C code indirectly accesses can be stored immediately after the
;   custom ARM code.
;===============================================================================

;    echo "---->"
;    echo "ARM INDIRECT DATA starts at", *

SPEECH_DATA

    IF __ENABLE_ATARIVOX

        include "atarivox/speechData.asm"
SIZEOF_SPEECH_DATA = * - SPEECH_DATA
        echo "Speech data =", SPEECH_DATA, "-", *, "(",[SIZEOF_SPEECH_DATA]d, "bytes)"

    ENDIF


END_OF_INDIRECT_DATA
;    echo "ARM INDIRECT DIRECT DATA ends @", *
;    echo "<----"


;===============================================================================
; ARM Direct Data
;----------------------------------------
;   I find it easier, and more space efficient, to store some of the data the
;   C code needs to access in the 6507 code instead of the C code.  Because the
;   build process is:
;
;       1) assemble 6507 code to create defines_from_dasm_for_c.h
;       2) compile C code to create ARM routines
;       3) assemble 6507 to create final ROM
;
;   the ARM code could change size between steps 1 and 3, which would shift data
;   that immediately comes after it. So the data that C directly accesses needs
;   to be after an ORG to prevent it from moving.
;
;   The _IMAGE_GRAPHICS, _IMAGE_COLORS, etc data tables are directly access by
;   the C code so must be in the Direct Data area. The data they point to is
;   indirectly accessed by the C code, so they can go in the Indirect Data area.
;   Note the labels for the tables are prefixed by _ so they'll end up in the
;   defines_from_dasm_for_c.h file, while the labels for the data the tables
;   point to are not prefixed by _



; .DIRECT_DATA_SIZE .SET TITLESCREEN_DATA

; .DIRECT_DATA_SIZE .SET .DIRECT_DATA_SIZE + (__WORD_COUNT * 2)
; .DIRECT_DATA_SIZE .SET .DIRECT_DATA_SIZE + __COPYRIGHT_ROWS * 6
; .DIRECT_DATA_SIZE .SET .DIRECT_DATA_SIZE + (CHAR_COUNT * CHAR_SIZE)

;     echo .DIRECT_DATA_SIZE, "bytes of ARM_DIRECT_DATA expected, consisting of..."


; 6507 code lives at the end of the ROM, so we ORG it to ~$7FFF - its length
; It "grows" down - the bigger it is, the lower the address. We can exact-fit it
; so that the "kernel bytes available" is 0.  The C code grows UP to the ORG
; address here. The delta between end of the C code and this ORG is free space

    echo "FREE C-SPACE = ", [ARM_DIRECT_DATA - *]d, "bytes"

#if __ENABLE_ATARIVOX
BASE_6507_START SET BASE_6507_START - 84
#endif

    ORG BASE_6507_START

ARM_DIRECT_DATA

;    echo "CHAFF SPACE = ", [* - END_OF_INDIRECT_DATA]d, "bytes"

;    echo "ARM DIRECT DATA starts at", *



WASTE SET 0

    IF * < $7000
        echo "shifted 6502 as it was too early @", *
WASTE SET $7000 - *
        ORG $7000
    ELSE
    ENDIF

    echo "6502 origin is ", *
    echo "POTENTIAL c->6502 shift :", (* - $7000)d
    IF WASTE
        echo "  --> wastage", WASTE
    ENDIF

;    IF * > $7000
;OVERUSE = * - $7000
;        echo "  --> 'EA' potential, overuse", (OVERUSE)d, "bytes"
;    ENDIF




START6502

; PLACE THESE EARLY IF POSSIBLE, SO THAT THE $EA-WAIT LOOP HAS PLENTY OF LEEWAY/TIME



; idleProcess first calls the OS/VB (passed in Y) and then the idle routine. It continues
; to call idle until the timer is approaching the auto-sync time. The less 6507 cycles
; used here, the more ARM code  can be executed, so high efficiency is crucial.
; ASSUMED we don't overflow time - otherwise INTIM will fail and we get scanline > 262

runARM              ldy #$FF                         ; FF = Run ARM code w/out digital audio interrupts
runARM2             sty CALLFN                       ; runs main() in the C code (6507 is $EA-locked)

    ; EA-wait resumes here

                    rts

idleProcess         ; y = ARM function ID to call


                    ldx #0
                    stx DSPTR
                    stx DSPTR
                    sty DSWRITE                     ; --> _RUN_FUNC (OS or VB)

                    jsr runARM
                    ; ldy #$FF                        ; Run ARM code w/out digital audio interrupts
                    ; sty CALLFN                      ; runs main() in the C code (6507 is $EA-locked)

    ; EA-wait resumes here

                    stx DSPTR
                    stx DSPTR

                    ldx #_FN_GAME_IDLE
                    stx DSWRITE                     ; --> _RUN_FUNC (IDLE)

                    jsr runARM2
                    ;                    sty CALLFN

    ; EA-wait resumes herew

safeTimerWait       lda INTIM
                    bpl safeTimerWait
                    rts


CallArmCode

    ; Y = function to run

                    ldx #<_DS_TO_ARM
                    stx DSPTR
                    stx DSPTR

                    sty DSWRITE                      ; --> _RUN_FUNC
                    ldx SWCHA
                    stx DSWRITE                      ; joysticks -> _SWCHA
                    ldx SWCHB
                    stx DSWRITE                      ; switches ->_SWCHB
                    ldx INPT4
                    stx DSWRITE                      ; left fire ->_INPT4
;                    ldx INPT5
;                    stx DSWRITE                      ; right fire ->_INPT5

                    jmp runARM


;    echo "early 6502 code (ARM calls) end at ", *


; SPEECH_TABLE

;      MAC WORDIS
;      .word SPEAK_{1}
;      ENDM

; __WORD_COUNT SET 0

;     IF __ENABLE_ATARIVOX

; __WORD_COUNT SET 9

; __WORD_NULL         = 0

; __WORD_XXX          = 1
; __WORD_WATCHOUT     = 2
; __WORD_GETREADY     = 3
; __WORD_AARGH        = 4
; __WORD_WET          = 5
; __WORD_TOO_LATE     = 6
; __WORD_LETSGOBABY   = 7
; __WORD_TRAP         = 8
; __WORD_DOSOMETHING  = 9
; __WORD_RAGEQUIT     = 10
; __WORD_GOODBYE      = 11
; __WORD_SOSAD        = 12
; __WORD_Room_ONE     = 14


; __SPEECH_ADDRESS

;     WORDIS NULL
;     WORDIS XXX
;     WORDIS WATCHOUT
;     WORDIS GETREADY
;     WORDIS AARGH
;     WORDIS WET
;     WORDIS TOO_LATE
;     WORDIS LETSGOBABY
;     WORDIS TRAP
;     WORDIS DOSOMETHING
;     WORDIS RAGEQUIT
;     WORDIS GOODBYE
;     WORDIS SOSAD
;     WORDIS Room_ONE

; ;    echo "   - SPEECH TABLE from", SPEECH_TABLE, "to", *, "(", (* - SPEECH_TABLE) , "bytes)"

;     ENDIF


__COPYRIGHT_START

    include "copyright.asm"

;    echo "   - COPYRIGHT DATA from", __COPYRIGHT_START, "to", *, "(", (* - __COPYRIGHT_START) , "bytes)"


;#if __ENABLE_DEMO

R = 0
L = 1
U = 2
D = 3
NONE = 4




__COLOUR_POOL

    dc 0x04, 0x36, 0x94, 0xB4  ; 00
    dc 0x16, 0xC4, 0x44, 0x64  ; 01
    dc 0x24, 0x96, 0xC6, 0x64  ; 02
    dc 0x24, 0xD4, 0x64, 0x86  ; 03
    dc 0x26, 0x46, 0xB8, 0x86  ; 04
    dc 0x34, 0x94, 0xA6, 0x76  ; 05
    dc 0x34, 0xD4, 0x74, 0x52  ; 06
    dc 0x36, 0xA4, 0x64, 0x44  ; 07
    dc 0x44, 0xA6, 0x96, 0xA6  ; 08
    dc 0x86, 0xD6, 0x34, 0x54  ; 09
    dc 0x76, 0x24, 0xC6, 0xB4  ; 10
    dc 0x96, 0x24, 0x86, 0xB6  ; 11
    dc 0x98, 0x24, 0x86, 0xC6  ; 12
    dc 0xA4, 0x24, 0x32, 0x52  ; 13
    dc 0xA4, 0xC4, 0x46, 0x22  ; 14
    dc 0xD4, 0x34, 0x24, 0x74  ; 15

__PALETTE_COUNT = (* - __COLOUR_POOL) / 4

    dc 0x28+1, 0x96+1, 0x0A+1, 0x06+1   ; 16 0   A0 was too dark
    dc 0x96+1, 0x26+1, 0x38+1, 0x34+1   ; 17 1   A1 up/down
    dc 0xE6+1, 0x66+1, 0xA6+1, 0xA2+1   ; 18 2   A2 up/down

    dc 0x88+1, 0x56+1, 0xA4+1, 0xA4+1   ; 19 3   A3 
    dc 0x28+1, 0x74+1, 0xB8+1, 0xB2+1   ; 20 4   A4 
    dc 0x78+1, 0x44+1, 0xB8+1, 0xB4+1   ; 21 5   B0 up/down + too dark
    dc 0xB6+1, 0x24+1, 0x96+1, 0x92+1   ; 22 6   B1 up/down
    dc 0x48+1, 0xB4+1, 0x68+1, 0x64+1   ; 23 7   B2 up/down
    dc 0x6A+1, 0x76+1, 0x58+1, 0x54+1   ; 24 8   B3 
    dc 0x3A+1, 0x64+1, 0x48+1, 0x44+1   ; 25 9   B4 up/down
    dc 0x26+1, 0xE4+1, 0xA6+1, 0xA2+1   ; 26 A   C0 up/down
    dc 0xEA+1, 0x46+1, 0x98+1, 0x94+1   ; 27 B   C1 up/down
    dc 0x26+1, 0x54+1, 0xA8+1, 0xA4+1   ; 28 C   C2 
    dc 0x66+1, 0x72+1, 0x96+1, 0x92+1   ; 29 D   C3 up/down

;    dc 0x96+1, 0x66+1, 0x96+1, 0x92+1   ; 30 E   C4 up/down
    dc 0x98+1, 0x68+1, 0x96+1, 0x92+1    ; 30E

;    dc 0x46+1, 0x94+1, 0xF8+1, 0xF4+1   ; 31 F   A0 up/down
    dc 0x48+1, 0x92+1, 0xF8+1, 0xF4+1    ; 31 F


CHAR_BLOCK_START

;CHARSET_START

____ = %00000000
___X = %00000001
__X_ = %00000010
__XX = %00000011
_X__ = %00000100
_X_X = %00000101
_XX_ = %00000110
_XXX = %00000111
X___ = %00001000
X__X = %00001001
X_X_ = %00001010
X_XX = %00001011
XX__ = %00001100
XX_X = %00001101
XXX_ = %00001110
XXXX = %00001111

_____ = %00000000
____X = %00000001
___X_ = %00000010
___XX = %00000011
__X__ = %00000100
__X_X = %00000101
__XX_ = %00000110
__XXX = %00000111
_X___ = %00001000
_X__X = %00001001
_X_X_ = %00001010
_X_XX = %00001011
_XX__ = %00001100
_XX_X = %00001101
_XXX_ = %00001110
_XXXX = %00001111
X____ = %00010000
X___X = %00010001
X__X_ = %00010010
X__XX = %00010011
X_X__ = %00010100
X_X_X = %00010101
X_XX_ = %00010110
X_XXX = %00010111
XX___ = %00011000
XX__X = %00011001
XX_X_ = %00011010
XX_XX = %00011011
XXX__ = %00011100
XXX_X = %00011101
XXXX_ = %00011110
XXXXX = %00011111

__ = %00000000
_X = %01000000
X_ = %10000000
XX = %11000000

__ = %00000000
_Y = %01000000
Y_ = %10000000
YY = %11000000

___ = %00000000
__Y = %00100000
_Y_ = %01000000
_YY = %01100000
Y__ = %10000000
Y_Y = %10100000
YY_ = %11000000
YYY = %11100000



;CHARSET_END

;CHARSET_START
CHAR_BLOCK2_START

__CHAR_STEELWALL
__SMALLCHAR_STEELWALL

 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____
 dc _____
 dc %11111111
 dc _____



;    dc  XXXXX ;                      00 < 0
;    dc  _____ ;                      01
;    dc  XXXXX ;                      02
;    dc  XXXXX ;                   00 03 < 1 a
;    dc  X____ ;                   01 04
;    dc  XXXXX ;                   02 05
;    dc  XXXXX ;                00 03 06 < 2
;    dc  _____ ;                01 04 07
;    dc  _XXXX ;                02 05 08
;    dc  XXXXX ;             00 03 06 09 < 3
;    dc  _____ ;             01 04 07 10
;    dc  XXXXX ;             02 05 08 11
;    dc  XXXXX ;          00 03 06 09 12 < 4
;    dc  _____ ;          01 04 07 10 13
;    dc  XXXXX ;          02 05 08 11 14
;    dc  XXXXX ;       00 03 06 09 12 15 < 5 b
;    dc  ___X_ ;       01 04 07 10 13 16
;    dc  XXXXX ;       02 05 08 11 14 17
;    dc  XXXXX ;    00 03 06 09 12 15 18 < 6
;    dc  _____ ;    01 04 07 10 13 16 19
;    dc  XXX_X ;    02 05 08 11 14 17 20
;    dc  XXXXX ; 00 03 06 09 12 15 18 21 < 7
;    dc  _____ ; 01 04 07 10 13 16 19 22
;    dc  XXXXX ; 02 05 08 11 14 17 20 23
;
;    dc  XXXXX ;          02 05 08 11 14
;    dc  _____ ;          01 04 07 10 13
;    dc  XXXXX ;       00 03 06 09 12 15 < 5 b
;
;    dc  XXXXX ;          02 05 08 11 14
;    dc  _____ ;          01 04 07 10 13
;    dc  XXXXX ;       00 03 06 09 12 15 < 5 b

__CHAR_DIRT

;    dc| XXXX ; 00 <
    dc  ____ ; 01
    dc  ____ ; 02
    dc  XX_XX ; 03 <
    dc  ____ ; 04
    dc  ____ ; 05
    dc  XXXXX ; 06 <
    dc  ____ ; 07
    dc  ____ ; 08
    dc       XXX_X ; 09 <
    dc       ____ ; 10
    dc       ____ ; 11
    dc       XXXXX ; 12 <
    dc       ____ ; 13
    dc       ____ ; 14
    dc           X_XX ; 15 <
    dc           ____ ; 16
    dc           ____ ; 17
    dc           XXXX_ ; 18 <
    dc           ____ ; 19
    dc           ____ ; 20
    ; dc           XXXXX ; 21
    ; dc           ____ ; 22
    ; dc           ____ ; 23



__CHAR_DIRT2

    dc XX | YY | XXXX ; 00 <    21
    dc __ | __ | ____ ; 01      22
    dc __ | __ | ____ ; 02      23
    dc XX | YY | XXXX ; 03 <
    dc __ | __ | ____ ; 04
    dc __ | __ | ____ ; 05
    dc XX | YY | XXXX ; 06 <
    dc __ | __ | ____ ; 07
    dc __ | __ | ____ ; 08
    dc      YY | XX_X ; 09 <
    dc      __ | ____ ; 10
    dc      __ | ____ ; 11
    dc      YY | XXXX ; 12 <
    dc      __ | ____ ; 13
    dc      __ | ____ ; 14
    dc           XXXX ; 15 <
    dc           ____ ; 16
    dc           ____ ; 17
    dc           XXXX ; 18 <
    dc           ____ ; 19
    dc           ____ ; 20
    dc           X_XX ; 21
    dc           ____ ; 22
    dc           ____ ; 23


__CHAR_BRICKWALL


    dc  XXXX_ ; 00 <
    dc  XXXX_ ; 01
    dc  XXXXX ; 02
    dc  XXXX_ ; 03 <
    dc  XXXX_ ; 04
    dc  XXXXX ; 05
    dc  XXXX_ ; 06 <
    dc  XXXX_ ; 07
    dc  XXXXX ; 08
    dc  _____ ; 09 <
    dc  _____ ; 10
    dc  XXXXX ; 11
    dc  XX_XX ; 12 <
    dc  XX_XX ; 13
    dc  XXXXX ; 14
    dc  XX_XX ; 15 <
    dc  XX_XX ; 16
    dc  XXXXX ; 17
    dc  XX_XX ; 18 <
    dc  XX_XX ; 19
    dc  XXXXX ; 20
    dc  XX_XX ; 18 <
    dc  XX_XX ; 19
    dc  XXXXX ; 20
    dc  _____ ; 24 <
    dc  _____ ; 25
    dc  XXXXX ; 26

__SMALLCHAR_BRICKWALL

    dc %11011000
    dc %11011000
    dc %11111111
    dc %11011000
    dc %11011000
    dc %11111111
    dc %00000000
    dc %00000000
    dc %11111111

    dc %10110100
    dc %10110100
    dc %11111100
    dc %10110100
    dc %10110100
    dc %11111100
    dc %00000000
    dc %00000000
    dc %11111100




__CHAR_EXPLODETO_1

     dc __ | _Y | __X_ ; 00 <
     dc __ | _Y | X_X_ ; 01
     dc __ | _Y | X_X_ ; 02

__CHAR_EXPLODETO_4

     dc XX | Y_ | X___ ; 03 <
     dc XX | Y_ | ____ ; 04
     dc XX | Y_ | ____ ; 05

__CHAR_EXPLODETO_2

     dc __ | YY | __X_ ; 06 <
     dc __ | YY | __X_ ; 07
     dc __ | YY | __X_ ; 08

__CHAR_EXPLODETO_3

     dc XX | Y_ | ____ ; 00 <
     dc XX | Y_ | __X_ ; 01
     dc XX | Y_ | X_X_ ; 02

__CHAR_EXPLODETO_0

     dc __ | _Y | X_X_ ; 00 <
     dc __ | _Y | X___ ; 01
     dc __ | _Y | ___X ; 02
     dc XX | __ | ___X ; 03 <
     dc XX | __ | _X_X ; 04
     dc XX | __ | _X__ ; 05
     dc __ | Y_ | _X_X ; 06 <
     dc __ | Y_ | ___X ; 07
     dc __ | Y_ | ___X ; 08
     dc      __ | __X_ ; 09 <
     dc      __ | X_X_ ; 10
     dc      __ | X_X_ ; 11
     dc      YY | X___ ; 12 <
     dc      YY | ____ ; 13
     dc      YY | ____ ; 14
     dc           __X_ ; 15 <
     dc           X_X_ ; 16
     dc           X_X_ ; 17
     dc           __X_ ; 18 <
     dc           __X_ ; 19
     dc           __X_ ; 20
     dc           ____ ; 21 <
     dc           __X_ ; 22
     dc           X_X_ ; 23



__CHAR_PARALLAX_TEMPLATE        ; squished in here - careful about count above/below chars

     dc %00000000     ; 00




CHAR_BLOCK_SIZE = * - CHAR_BLOCK_START
    echo "Char block = ", CHAR_BLOCK_START, "-", *, "(", [CHAR_BLOCK_SIZE]d, "bytes)"
;CHARSET_END



;    echo "   - CHARACTER SET DATA from", CHAR_BLOCK_START, "to", *, "(", [(* - CHAR_BLOCK2_START)]d , " bytes, ", [(* - CHAR_BLOCK2_START) / CHAR_SIZE]d, "characters)"

B________ = 0
B_______X = 1
B______X_ = 2
B______XX = 3
B_____X__ = 4
B_____X_X = 5
B_____XX_ = 6
B_____XXX = 7
B____X___ = 8
B____X__X = 9
B____X_X_ = 10
B____X_XX = 11
B____XX__ = 12
B____XX_X = 13
B____XXX_ = 14
B____XXXX = 15
B___X____ = 16
B___X___X = 17
B___X__X_ = 18
B___X__XX = 19
B___X_X__ = 20
B___X_X_X = 21
B___X_XX_ = 22
B___X_XXX = 23
B___XX___ = 24
B___XX__X = 25
B___XX_X_ = 26
B___XX_XX = 27
B___XXX__ = 28
B___XXX_X = 29
B___XXXX_ = 30
B___XXXXX = 31
B__X_____ = 32
B__X____X = 33
B__X___X_ = 34
B__X___XX = 35
B__X__X__ = 36
B__X__X_X = 37
B__X__XX_ = 38
B__X__XXX = 39
B__X_X___ = 40
B__X_X__X = 41
B__X_X_X_ = 42
B__X_X_XX = 43
B__X_XX__ = 44
B__X_XX_X = 45
B__X_XXX_ = 46
B__X_XXXX = 47
B__XX____ = 48
B__XX___X = 49
B__XX__X_ = 50
B__XX__XX = 51
B__XX_X__ = 52
B__XX_X_X = 53
B__XX_XX_ = 54
B__XX_XXX = 55
B__XXX___ = 56
B__XXX__X = 57
B__XXX_X_ = 58
B__XXX_XX = 59
B__XXXX__ = 60
B__XXXX_X = 61
B__XXXXX_ = 62
B__XXXXXX = 63
B_X______ = 64
B_X_____X = 65
B_X____X_ = 66
B_X____XX = 67
B_X___X__ = 68
B_X___X_X = 69
B_X___XX_ = 70
B_X___XXX = 71
B_X__X___ = 72
B_X__X__X = 73
B_X__X_X_ = 74
B_X__X_XX = 75
B_X__XX__ = 76
B_X__XX_X = 77
B_X__XXX_ = 78
B_X__XXXX = 79
B_X_X____ = 80
B_X_X___X = 81
B_X_X__X_ = 82
B_X_X__XX = 83
B_X_X_X__ = 84
B_X_X_X_X = 85
B_X_X_XX_ = 86
B_X_X_XXX = 87
B_X_XX___ = 88
B_X_XX__X = 89
B_X_XX_X_ = 90
B_X_XX_XX = 91
B_X_XXX__ = 92
B_X_XXX_X = 93
B_X_XXXX_ = 94
B_X_XXXXX = 95
B_XX_____ = 96
B_XX____X = 97
B_XX___X_ = 98
B_XX___XX = 99
B_XX__X__ = 100
B_XX__X_X = 101
B_XX__XX_ = 102
B_XX__XXX = 103
B_XX_X___ = 104
B_XX_X__X = 105
B_XX_X_X_ = 106
B_XX_X_XX = 107
B_XX_XX__ = 108
B_XX_XX_X = 109
B_XX_XXX_ = 110
B_XX_XXXX = 111
B_XXX____ = 112
B_XXX___X = 113
B_XXX__X_ = 114
B_XXX__XX = 115
B_XXX_X__ = 116
B_XXX_X_X = 117
B_XXX_XX_ = 118
B_XXX_XXX = 119
B_XXXX___ = 120
B_XXXX__X = 121
B_XXXX_X_ = 122
B_XXXX_XX = 123
B_XXXXX__ = 124
B_XXXXX_X = 125
B_XXXXXX_ = 126
B_XXXXXXX = 127
BX_______ = 128
BX______X = 129
BX_____X_ = 130
BX_____XX = 131
BX____X__ = 132
BX____X_X = 133
BX____XX_ = 134
BX____XXX = 135
BX___X___ = 136
BX___X__X = 137
BX___X_X_ = 138
BX___X_XX = 139
BX___XX__ = 140
BX___XX_X = 141
BX___XXX_ = 142
BX___XXXX = 143
BX__X____ = 144
BX__X___X = 145
BX__X__X_ = 146
BX__X__XX = 147
BX__X_X__ = 148
BX__X_X_X = 149
BX__X_XX_ = 150
BX__X_XXX = 151
BX__XX___ = 152
BX__XX__X = 153
BX__XX_X_ = 154
BX__XX_XX = 155
BX__XXX__ = 156
BX__XXX_X = 157
BX__XXXX_ = 158
BX__XXXXX = 159
BX_X_____ = 160
BX_X____X = 161
BX_X___X_ = 162
BX_X___XX = 163
BX_X__X__ = 164
BX_X__X_X = 165
BX_X__XX_ = 166
BX_X__XXX = 167
BX_X_X___ = 168
BX_X_X__X = 169
BX_X_X_X_ = 170
BX_X_X_XX = 171
BX_X_XX__ = 172
BX_X_XX_X = 173
BX_X_XXX_ = 174
BX_X_XXXX = 175
BX_XX____ = 176
BX_XX___X = 177
BX_XX__X_ = 178
BX_XX__XX = 179
BX_XX_X__ = 180
BX_XX_X_X = 181
BX_XX_XX_ = 182
BX_XX_XXX = 183
BX_XXX___ = 184
BX_XXX__X = 185
BX_XXX_X_ = 186
BX_XXX_XX = 187
BX_XXXX__ = 188
BX_XXXX_X = 189
BX_XXXXX_ = 190
BX_XXXXXX = 191
BXX______ = 192
BXX_____X = 193
BXX____X_ = 194
BXX____XX = 195
BXX___X__ = 196
BXX___X_X = 197
BXX___XX_ = 198
BXX___XXX = 199
BXX__X___ = 200
BXX__X__X = 201
BXX__X_X_ = 202
BXX__X_XX = 203
BXX__XX__ = 204
BXX__XX_X = 205
BXX__XXX_ = 206
BXX__XXXX = 207
BXX_X____ = 208
BXX_X___X = 209
BXX_X__X_ = 210
BXX_X__XX = 211
BXX_X_X__ = 212
BXX_X_X_X = 213
BXX_X_XX_ = 214
BXX_X_XXX = 215
BXX_XX___ = 216
BXX_XX__X = 217
BXX_XX_X_ = 218
BXX_XX_XX = 219
BXX_XXX__ = 220
BXX_XXX_X = 221
BXX_XXXX_ = 222
BXX_XXXXX = 223
BXXX_____ = 224
BXXX____X = 225
BXXX___X_ = 226
BXXX___XX = 227
BXXX__X__ = 228
BXXX__X_X = 229
BXXX__XX_ = 230
BXXX__XXX = 231
BXXX_X___ = 232
BXXX_X__X = 233
BXXX_X_X_ = 234
BXXX_X_XX = 235
BXXX_XX__ = 236
BXXX_XX_X = 237
BXXX_XXX_ = 238
BXXX_XXXX = 239
BXXXX____ = 240
BXXXX___X = 241
BXXXX__X_ = 242
BXXXX__XX = 243
BXXXX_X__ = 244
BXXXX_X_X = 245
BXXXX_XX_ = 246
BXXXX_XXX = 247
BXXXXX___ = 248
BXXXXX__X = 249
BXXXXX_X_ = 250
BXXXXX_XX = 251
BXXXXXX__ = 252
BXXXXXX_X = 253
BXXXXXXX_ = 254
BXXXXXXXX = 255



__DIGIT_SHAPE

       dc B__X__X__
       dc B__X_XXX_
       dc B_XX_XXX_
       dc B_XX_XXX_
       dc B_XX_X_X_
       dc B_XX_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_XXX_
       dc B__X_XXX_
       dc B__X_XXX_
       dc B__X_XXX_
       dc B__X__X__
       dc B________


       dc B_X___X__
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BX_X_X_X_
       dc B__X___X_
       dc B__X___X_
       dc B_XX___X_
       dc B_X___XX_
       dc B_XX_XXX_
       dc B_XX_XX__
       dc B__X_X___
       dc B__X_X___
       dc BX_X_X_X_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc B_X__XXX_
       dc B________


       dc BXXX_X___
       dc BXXX_X___
       dc BXXX_X___
       dc BX_X_X_X_
       dc BX___X_X_
       dc BX___X_X_
       dc BXX__X_X_
       dc BXXX_X_X_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc B__X_XXX_
       dc B__X_XXX_
       dc BX_X_XXX_
       dc BX_X_X_X_
       dc BXXX___X_
       dc BXXX___X_
       dc BXXX___X_
       dc BXXX___X_
       dc B_X____X_
       dc B________


       dc BXXX__X__
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BX_X_X_X_
       dc B__X_X___
       dc B__X_XX__
       dc B__X_XXX_
       dc B__X_XXX_
       dc B_XX_XXX_
       dc B_XX_X_X_
       dc B_XX_X_X_
       dc B_X__X_X_
       dc B_X__X_X_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X___X__
       dc B________


       dc B_X___X__
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X__XX_
       dc BXXX__XX_
       dc BXXX_XX__
       dc B_XX_XX__
       dc B__X_X_X_
       dc B__X_X_X_
       dc BX_X_X_X_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc B_X___X__
       dc B________


    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________
    ;    dc B________


    ;    dc B_X___X__
    ;    dc B_X___X__
    ;    dc B_X___X__
    ;    dc B_X______
    ;    dc B_X__XXX_
    ;    dc B_X__XXX_
    ;    dc B_X__XXX_
    ;    dc B_X__XXX_
    ;    dc B_X___X__
    ;    dc B_X___X__
    ;    dc B_X___X__
    ;    dc B_____X__
    ;    dc B____X_X_
    ;    dc B____X_X_
    ;    dc B_X__X_X_
    ;    dc B_X__X_X_
    ;    dc B_X__X_X_
    ;    dc B________
    ;    dc B________
    ;    dc B________


       dc BX_X_____
       dc BX_X_____
       dc BX_X_____
       dc BX_X_____
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X__X__
       dc BX_X__X__
       dc BX_X__X__
       dc BX_X__X__
       dc B_____X__
       dc B____X_X_
       dc B____X_X_
       dc B____X_X_
       dc BX_X_X_X_
       dc BX_X_____
       dc BX_X_____
       dc BX_X_____

       dc BX____X__
       dc BX____X__
       dc BX____X__
       dc BXX___X__
       dc BXX___X__
       dc BXX___X__
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXX___X__
       dc BXX___X__
       dc BXX___X__
       dc BX____X__
       dc BX____X__
       dc BX____X__
       dc B________
       dc B________

       dc BXX___X__
       dc BXXX__X__
       dc BXXX__X__
       dc BXXX_XXX_
       dc BX_X_XXX_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BXXX_X_X_
       dc BXX__X_X_
       dc BXXX_X_X_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXX__X_X_
       dc B________


       dc BX____XX_
       dc BXX___XX_
       dc BXX__XXX_
       dc BXXX_XXX_
       dc BXXX_XX__
       dc BX_X_X___
       dc BX_X_X___
       dc BX_X_X___
       dc BX_X_X___
       dc BX_X_X___
       dc BX_X_X___
       dc BX_X_X___
       dc BX_X_X_X_
       dc BX_X_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXX___XX_
       dc B________


       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BX___X___
       dc BX___X___
       dc BX___XX__
       dc BX___XX__
       dc BXX__XX__
       dc BXX__XX__
       dc BXX__X___
       dc BXX__X___
       dc BX___X___
       dc BX___XXX_
       dc BX___XXX_
       dc BX___XXX_
       dc BX___XXX_
       dc BX___XXX_
       dc BX___XXX_
       dc B________


       dc BX_X__X__
       dc BX_X__XX_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X_X_X_
       dc BX_X_X___
       dc BX_X_X___
       dc BX_X_X___
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X__XX_
       dc BX_X__XX_
       dc B________

       dc B__X_XXX_
       dc B__X_XXX_
       dc B__X_XXX_
       dc B__X_XXX_
       dc B__X__X__
       dc B__X__X__
       dc B__X__X__
       dc B__X__X__
       dc B__X__X__
       dc B__X__X__
       dc B__X__X__
       dc B__X__X__
       dc BX_X__X__
       dc BX_X_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc B_X__XXX_
       dc B________

       dc BX___X_X_
       dc BX___X_X_
       dc BX___X_X_
       dc BX___X_X_
       dc BX___X_X_
       dc BX___X_X_
       dc BX___XXX_
       dc BX___XX__
       dc BX___XX__
       dc BX___XX__
       dc BX___XX__
       dc BX___XXX_
       dc BX_X_XXX_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc B________

       dc B__X_X_X_
       dc B__X_X_X_
       dc BX_X_X_X_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX___X_X_
       dc BX___X_X_
       dc BX___X_X_
       dc B________


       dc BXX___X__
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXX__X_X_
       dc BX___XXX_
       dc BX___XXX_
       dc BX___XXX_
       dc BX___XXX_
       dc BX____X__
       dc B________

       dc BXX___X__
       dc BXX__XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BXXX_X_X_
       dc BXX__X_X_
       dc BXX__X_X_
       dc BXXX_X_X_
       dc BXXX_X___
       dc BX_X_X_X_
       dc BX_X_X_XX
       dc BX_X_XXXX
       dc BX_X_XXXX
       dc BX_X_XX_X
       dc BX_X__X_X
       dc B________

       dc BXXX__XX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BXXX_X_X_
       dc BXXX_X___
       dc B_X__XX__
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X___XX_
       dc B_X____X_
       dc B_X____X_
       dc B_X____X_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XX__
       dc B________

       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X__XXX_
       dc B_X___X__
       dc B________

       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc B_X__X_X_
       dc B_X__X_X_
       dc B_X__X_X_
       dc BXXX_XXX_
       dc BXXX_XXX_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X_XXX_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc BX_X_X_X_
       dc B________

       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc BXXX_X_X_
       dc B__X_X_X_
       dc B__X_X_X_
       dc B__X_XXX_
       dc B__X_XXX_
       dc B_XX_XXX_
       dc BXXX_XXX_
       dc BXXX__X__
       dc BXX___X__
       dc BX____X__
       dc BX____X__
       dc BX____X__
       dc BX____X__
       dc BXXX__X__
       dc BXXX__X__
       dc BXXX__X__
       dc B________



; character set

__CHAR_A_TO_Z

   dc B________
   dc B________
   dc B________
   dc B________
   dc B________
   dc B________
   dc B________
   dc B________
   dc B________
   dc B________

   dc BXXX_____
   dc BXXX_____
   dc BXXX_____
   dc BXX______
   dc BXX______
   dc B________
   dc B________
   dc B________
   dc B________
   dc B________

   dc B___XXX__
   dc B__X___X_
   dc B_X_XX__X
   dc B_X_X_X_X
   dc B_X_X_X_X
   dc B_X_XX__X
   dc B_X_X_X_X
   dc B_X_X_X_X
   dc B__X___X_
   dc B___XXX__

   dc B__XXX___
   dc B_XXXXX__
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_

   dc BXXXXXX__
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXX__
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXX__

   dc B__XXXXX_
   dc B_XXXXXX_
   dc BXXXX____
   dc BXXX_____
   dc BXXX_____
   dc BXXX_____
   dc BXXXX____
   dc BXXXXXXX_
   dc B_XXXXXX_
   dc B__XXXXX_

   dc BXXXXX___
   dc BXXXXXX__
   dc BXXX_XXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX_XXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXX__

   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXX_____
   dc BXXXXX___
   dc BXXXXX___
   dc BXXX_____
   dc BXXX_____
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_

   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXX_____
   dc BXXX_____
   dc BXXXXX___
   dc BXXXXX___
   dc BXXXXX___
   dc BXXX_____
   dc BXXX_____
   dc BXXX_____

   dc B__XXXXX_
   dc B_XXXXXX_
   dc BXXXX____
   dc BXXX_____
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXX_XX_
   dc BXXXXXXX_
   dc B_XXXXXX_
   dc B__XXXXX_

   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_

   dc B_XXXXX__
   dc B_XXXXX__
   dc B_XXXXX__
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B_XXXXX__
   dc B_XXXXX__
   dc B_XXXXX__

   dc B____XXX_
   dc B____XXX_
   dc B____XXX_
   dc B____XXX_
   dc B____XXX_
   dc B____XXX_
   dc BXXX_XXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B_XXXXX__

   dc BXXX___X_
   dc BXXX__XX_
   dc BXXX_XXX_
   dc BXXXXXX__
   dc BXXXXX___
   dc BXXXXX___
   dc BXXXXXX__
   dc BXXXXXXX_
   dc BXXX_XXX_
   dc BXXX__XX_

   dc BXXX_____
   dc BXXX_____
   dc BXXX_____
   dc BXXX_____
   dc BXXX_____
   dc BXXX_____
   dc BXXX_____
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_

   dc BXX___XX_
   dc BXXX_XXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXX_X_XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_

   dc BXX___XX_
   dc BXXX__XX_
   dc BXXXX_XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXX_XXXX_
   dc BXXX_XXX_
   dc BXXX__XX_
   dc BXXX__XX_

   dc B_XXXXX__
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B_XXXXX__

   dc BXXXXX___
   dc BXXXXXX__
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXX__
   dc BXXX_____
   dc BXXX_____
   dc BXXX_____

   dc B_XXXXX__
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX_XXX_
   dc BXXXXXX__
   dc BXXXXXXXX
   dc B_XXX_XXX

   dc BXXXXX___
   dc BXXXXXX__
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX_XXX_
   dc BXXXXXX__
   dc BXXXXX___
   dc BXXXXXX__
   dc BXXX_XXX_
   dc BXXX__XX_

   dc B__XXXXX_
   dc B_XXXXXX_
   dc BXXX_____
   dc BXXX_____
   dc B_XXXXX__
   dc B___XXXX_
   dc B___XXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXX___

   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___

   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc B_XXXXXX_
   dc B__XXXX__

   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXX_XX_
   dc B_XXXXX__
   dc B_XXXXX__
   dc B__XXX___
   dc B__XXX___

   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXX_X_XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXX_XXX_
   dc BXX___XX_

   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX_XXX_
   dc BXXXXXX__
   dc B_XXXX___
   dc B__XXXX__
   dc B_XXXXXX_
   dc BXXX_XXX_
   dc BXX__XXX_
   dc BXX__XXX_

   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc B_XXXXX__
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___

   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B____XXX_
   dc B___XXXX_
   dc B__XXXX__
   dc B_XXXX___
   dc BXXXX____
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_

   dc B_XXXXX__
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B_XXXXX__

   dc B___XX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B___XX___
   dc B___XX___
   dc B___XX___
   dc B__XXXX__
   dc B__XXXX__
   dc B__XXXX__

   dc B_XXXXX__
   dc BXXXXXXX_
   dc BXXX_XXX_
   dc B____XXX_
   dc B___XXX__
   dc B__XXX___
   dc B_XXX____
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_

   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B____XXX_
   dc B___XXX__
   dc B__XXXX__
   dc B____XXX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B_XXXXX__

   dc B___XXX__
   dc B__XXXX__
   dc B_XXXXX__
   dc BXXX_XX__
   dc BXX__XX__
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B____XX__
   dc B____XX__

   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXX_____
   dc BXXXXXX__
   dc BXXXXXXX_
   dc B_____XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B_XXXXX__

   dc B_XXXXX__
   dc BXXXXXX__
   dc BXXX_____
   dc BXXXXXX__
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B_XXXXX__

   dc BXXXXXXX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B____XXX_
   dc B___XXX__
   dc B___XXX__
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___
   dc B__XXX___

   dc B_XXXXX__
   dc BXXXXXXX_
   dc BXXX__XX_
   dc B_XXXXX__
   dc BXXXXXXX_
   dc BXXX__XX_
   dc BXXX__XX_
   dc BXXXXXXX_
   dc BXXXXXXX_
   dc B_XXXXX__

   dc B_XXXXX__
   dc BXXXXXXX_
   dc BXX__XXX_
   dc BXX__XXX_
   dc BXX__XXX_
   dc BXXXXXXX_
   dc B_XXXXXX_
   dc B____XXX_
   dc B_XXXXX__
   dc B_XXXX___




__XLATE
    dc $00, $0, $20, $40, $60, $80, $A0, $C0
    dc $D0, $B0, $90, $70, $50, $30, $20, $40

__XLATESECAM
    dc $0, $E, $C, 4, 4, 6, 6, 2, 2, 2, 8, 8, 8, 8, $C, $C

__RENOTE
    dc 1, 4, 6, 12

__rebase
    dc 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 7, 1 << 6, 1 << 5, 1 << 4, 1 << 3, 1 << 2
    dc 1 << 1, 1 << 0, 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7

__shiftMode
    dc 16, 17, 31

__rowSize

    dc 27
    dc 15, 9

__scoreColour
    dc $46, $98, $D8, $46, $98, $28, $28, $28

__sinoid
    dc 0, 1, 1, 3, 5, 6, 6, 7, 7, 6, 6, 5, 3, 1, 1, 0


; Sound FX

CMD_LOOP = 254
CMD_STOP = 255

__sampleTick
    dc $F, $1F, 1, 2
    dc 0, 0, 0, 6
    dc $F, $1F, 1, 1
    dc 0, 0, 0, 40
    dc CMD_LOOP

__sampleDrip
    dc 12, 12, 4, 1
    dc CMD_STOP

__sampleDrip2
    dc 12, 6, 4, 1
    dc 12, 4, 4, 1
    dc CMD_STOP

__sampleMagic
    dc $C, $8, 3, 2
    dc CMD_LOOP

__sampleMagic2
    dc 5, 8, 5, 15
    dc CMD_STOP


__samplePick
    dc 9
    dc 12
    dc 8
    dc 1
    dc 9
    dc 20
    dc 15
    dc 1
    dc 9
    dc 31
    dc 12
    dc 1
    dc CMD_STOP

__sampleRock
    dc 8, 18, 5, 4
    dc 8, 18, 5, 3
    dc 8, 18, 4, 2
    dc 8, 18, 3, 1
    dc CMD_STOP

__sampleRock2
    dc 8, 18, 4, 4
    dc 8, 18, 3, 3
    dc 8, 18, 2, 3
    dc CMD_STOP

__sampleDirt
    dc 8, 31, 3, 10
    dc CMD_STOP

__sampleSpace
    dc 8, 2, 2, 2
    dc CMD_STOP

__sampleBlip
    dc 4, 18, 5, 1
    dc 4, 18, 4, 2
    dc 4, 18, 3, 4
    dc CMD_STOP

__sampleX
    dc $C, 8, 1, 2, CMD_LOOP

__sampleBubbler
    dc $E, 8, 1, 30
    dc CMD_STOP

__sampleExxtra
    dc 12, 5, 4, 10
    dc 12, 7, 4, 10
    dc 12, 5, 4, 10
    dc 12, 7, 4, 10
    dc 12, 5, 4, 10
    dc 12, 7, 4, 10
    dc CMD_STOP

__samplePush
    dc 8, 4, 2, 12
    dc CMD_STOP

; __sampleExplode
;     dc 8, 7, 9, 2
;     dc 8, 10, 8, 2
;     dc 8, 13, 7, 2
;     dc 8, 16, 7, 3
;     dc 8, 19, 6, 4
;     dc 8, 22, 5, 5
;     dc 8, 25, 4, 6
;     dc 8, 28, 3, 10
;     dc 8, 29, 2, 15
;     dc 8, 31, 1, 15
;     dc CMD_STOP

__sampleExplodeQuiet
    dc 8, 7, 7, 2
    dc 8, 13, 5, 2
    dc 8, 19, 4, 4
    dc 8, 25, 2, 6
    dc 8, 29, 1, 15
    dc CMD_STOP

; __sampleWhoosh2
;     dc 15, 7, 2, 2
;     dc 15, 10, 4, 2
;     dc 15, 13, 7, 10
;     dc 15, 16, 7, 6
;     dc 15, 19, 6, 5
;     dc 15, 22, 5, 4
;     dc 15, 25, 4, 3
;     dc 15, 28, 3, 2
;     dc 15, 31, 2, 2
;     dc 15, 31, 1, 2
;     dc CMD_STOP

; __sampleWhoosh
;     dc 15, 31, 1, 2*3
;     dc 15, 31, 2, 2*3
;     dc 15, 28, 3, 2*3
;     dc 15, 25, 4, 3*3
;     dc 15, 22, 5, 4*3
;     dc 15, 19, 6, 5*3
;     dc 15, 16, 7, 6*3
;     dc 15, 13, 7, 10*3
;     dc 15, 10, 4, 2*3
;     dc 15, 7, 2, 2*3
;     dc CMD_STOP


__sample10987654321
    dc 4, 10, 1, 1
    dc 4, 10, 3, 1
    dc 4, 10, 5, 1
    dc 4, 10, 6, 1
    dc 4, 10, 5, 15
    dc 4, 10, 3, 3
    dc 4, 10, 2, 3
    dc 4, 10, 1, 3
    dc CMD_STOP

__sampleNone
    dc CMD_STOP

; __sampleSFX
; ;    // C,F,V,LEN
;     dc 5, 14, 4, 1
;     dc 5, 12, 6, 1
;     dc 5, 11, 7, 1
;     dc 5, 3, 10, 2
;     dc 5, 4, 8, 1
;     dc 5, 5, 6, 1
;     dc 5, 6, 4, 1
;     dc 5, 7, 1, 1
;     dc CMD_STOP

__sampleExit
    dc 12, 16, 1, 1
    dc 12, 16, 4, 1
    dc 12, 16, 10, 1
    dc 12, 16, 8, 4
    dc 12, 16, 6, 1
    dc 12, 16, 4, 1
    dc 12, 16, 2, 1
    dc CMD_STOP


;    echo "   - 3PIX DIGIT SHAPES from", __DIGIT_SHAPE, "to", *, "(", [(* - __DIGIT_SHAPE)]d , " bytes"


;███████████████████████████████████████████████████████████████████████████████

;__TITLE_SCREEN

;    include "iCC_title.asm"

;    echo "   - TITLE SCREEN  from", __TITLE_SCREEN, "to", *, "(", (* - __TITLE_SCREEN) , "bytes)"


    IF __ENABLE_ATARIVOX

speakJet

; AtariVox Speech Synth Driver
;; By Alex Herbert, 2004
; Updated for CDFJ usage by Andrew Davie 2022


SERIAL_OUTMASK      equ     $01
SERIAL_RDYMASK      equ     $02

;                    ldy #0
;                    sty spoken


                    lda SWCHA
                    and #SERIAL_RDYMASK     ; buffer full?
                    beq .speechDone

                    lda #_DS_SPEECH         ; data stream phoneme
                    eor #$FF
                    beq .speechDone

;kk jmp kk
    ; output byte as serial data
    ; send 1 start bit, 8 data bits, 1 stop bit

                    ldy #10

                    sec                     ; start bit
.serialiseBit       tax                     ; 2 @2

                    lda SWACNT              ; 4 @6
                    and #$fe                ; 2 @8
                    adc #0                  ; 2 @10     C -> D0
                    sta SWACNT              ; 4 @14     (and C now clear)

                    dey                     ; 2 @16
                    beq .speechDone         ; 2 @18

                    lda #-6                 ; 2 @20
.wasteCycles        adc #1                  ; 2         C is clear!
                    bcc .wasteCycles        ; 3(2) =5*6 - 1 = 29 loop

                    SLEEP 6                 ; @55

                    txa                     ; 2 @57
                    lsr                     ; 2 @59     next bit --> C
                    bpl .serialiseBit       ; 3 @62     unconditional


.speechDone


    IF >.serialiseBit != >.
        ECHO "ERROR: page crossing in speech serialise loop"
        ERR
    ENDIF
                    rts

    ENDIF



    IF 0

;------------------------------------------------------------------------------

;
; i2c_v2.3.inc
;
;
; AtariVox EEPROM Driver
;
; By Alex Herbert, 2004
;
; Optimisations by Thomas Jentzsch, 2006/2008
;
; Fix for Speakjet noise during I2C_TXBIT, 2016


; I/O Constants

I2C_SDA_MASK    equ     $04
I2C_SCL_MASK    equ     $08


; Signalling Macros

;  MAC     I2C_SCL_0
;    ldx     #$00
;    stx     SWCHA
;  ENDM

;  MAC     I2C_SCL_1
;    ldx     #I2C_SCL_MASK
;    stx     SWCHA
;  ENDM

;  MAC     I2C_SDA_IN
;    ldx     #I2C_SCL_MASK
;    stx     SWACNT
;  ENDM

;  MAC     I2C_SDA_OUT
;    ldx     #I2C_SCL_MASK|I2C_SDA_MASK
;    stx     SWACNT
;  ENDM


  MAC     I2C_START
; I2C_SCL_1
    ldx     #(I2C_SCL_MASK|I2C_SDA_MASK)*2  ; 2         I2C_SCL_MASK
    stx     SWCHA                           ; 4
; I2C_SDA_OUT
    txa
    lsr                                     ; 2         I2C_SCL_MASK|I2C_SDA_MASK
    sta     SWACNT                          ; 4
; total: 12 cycles
  ENDM


  MAC     I2C_TXBIT
; I2C_SCL_0
    ldx     #$0                             ; 2
    stx     SWCHA                           ; 4
    txa
; set bit
    adc     #I2C_SCL_MASK >> 2              ; 2         %10 / %11
    asl                                     ; 2
    asl                                     ; 2
    sta     SWACNT                          ; 4         SDA = !C (a = %1000 / %1100)
; I2C_SCL_1
    ldx     #I2C_SCL_MASK                   ; 2
    stx     SWCHA                           ; 4
; total: 22 cycles
  ENDM


  MAC     I2C_TXNACK
; I2C_SCL_0
    ldx     #I2C_SCL_MASK*2                 ; 2         $00
    stx     SWCHA                           ; 4
    txa
; I2C_SDA_IN
    lsr                                     ; 2         I2C_SCL_MASK
    sta     SWACNT                          ; 4
; I2C_SCL_1
    nop                                     ; 2         required for timing!
    sta     SWCHA                           ; 4         I2C_SCL_MASK
; total: 18 cycles
  ENDM


; Subroutine Macros

  MAC     I2C_SUBS

i2c_startread
; use V to flag if previous byte needs ACK
    ldy     #%10100001                      ; 2         eeprom read command
    .byte   $2c                             ; 2
i2c_startwrite
    ldy     #%10100000                      ; 2         eeprom write command
    I2C_START                               ;12         start signal (clears V flag)
    tya                                     ; 2 = 16
i2c_txbyte
    eor     #$ff                            ; 2         invert data byte
    sec                                     ; 2
    rol                                     ; 2 =  6    shift loop bit into a
i2c_txbyteloop
    tay                                     ; 2
    I2C_TXBIT                               ;22         transmit
    tya                                     ; 2
    asl                                     ; 2         shift next bit into C
    bne     i2c_txbyteloop                  ; 2�= 30/31

; receive acknowledge bit

    beq     i2c_rxbit                       ;43 = 43
; i2c_txbyte: 296 cycles
;-------------------------------------------------------------------------------

i2c_rxbyte
    bvc     i2c_rxskipack                   ; 2�        previous byte needs acknowledge?
    jsr     i2c_txack                       ;24         transmit acknowledge bit

i2c_rxskipack
    bit     i2c_rxbyte                      ; 4         set V - next byte(s) require acknowledge

    ldy     #1                              ; 2
    tya
i2c_rxbyteloop
    tay                                     ; 2 =  2
i2c_rxbit:                                  ;           receive bit in C
    I2C_TXNACK                              ;18 = 18

    lda     SWCHA                           ; 4
    lsr                                     ; 2
    lsr                                     ; 2
    lsr                                     ; 2 = 10    C = SDA

    tya                                     ; 2
    rol                                     ; 2         rotate into Y
    bcc     i2c_rxbyteloop                  ; 2�=  6/7

; received byte in A
    rts                                     ; 6 =  6
; i2c_rxbit:  40 cycles
; i2c_rxbyte: 310/333 cycles
;-------------------------------------------------------------------------------

i2c_stopread
    bvc     i2c_stopwrite                   ; 2�
    ldy     #$80                            ; 2
    jsr     i2c_rxbit                       ;46         transmit no-acknowledge

i2c_stopwrite
    jsr     i2c_txack                       ;30

; return port to input mode
    ldx     #0                              ; 2         0
    stx     SWACNT                          ; 4
    rts                                     ; 6 = 12
; i2c_stopread:  45/92
; i2c_stopwrite: 42
;-------------------------------------------------------------------------------

i2c_txack
; I2C_SCL_0
    ldx     #0                              ; 2         $00
    stx     SWCHA                           ; 4
; I2C_SDA_OUT
    ldx     #(I2C_SCL_MASK|I2C_SDA_MASK)    ; 2         I2C_SCL_MASK|I2C_SDA_MASK
    stx     SWACNT                          ; 4
; I2C_SCL_1
    asl                                     ; 2         I2C_SCL_MASK
    sta     SWCHA                           ; 4

    rts                                     ; 6
; total: 24 cycles
;-------------------------------------------------------------------------------

  ENDM




    i2c_subs

HandleSaveKey

SAVEKEY_ADR     = $0600         ;           reserved address for Boulder Dash (64 bytes)

; SK Memory Map:
;               room    level
; $0600-$0602   A       1
; $0603-$0605   A       2
; $0606-$0608   A       3
; $0609-$060b   A       4
; $060c-$060e   A       5
; $060f-$0611   E       1
; $0612-$0614   E       2
; $0615-$0617   E       3
; $0618-$061a   unused          HIJACKED!
; $061b-$061d   unused          HIJACKED!
; $061e-$0620   I       1
; $0621-$0623   I       2
; $0624-$0626   I       3
; $0627-$0629   unused          HIJACKED!
; $062a-$062c   unused          HIJACKED!
; $062d-$062f   M       1
; $0630-$0632   M       2
; $0633-$0635   M       3
; $0636-$063f   unused          HIJACKED!


;------------------------------------------------------------------------------

ReadSaveKey ; = 2371

; assume no SaveKey found:
    lda     #$ff                ; 2         return $ff if no SaveKey found
    sta     highScoreSK+2       ; 3

; setup SaveKey:
;    lda     #0; //startRoom           ; 3         load start room*5 and level
;    ldx     #0; //level               ; 3
    jsr     SetupSaveKey        ;6+853
    bcc     NoSKfound           ; 2/3

    ;    lda     #$34
    ;    sta     COLUBK

; start read
    jsr     i2c_stopwrite       ;6+42       end of "fake" write
    jsr     i2c_startread       ;6+284      Start signal and $a1 command byte

; read high score:
    ldx     #3-1                ; 2 = 1187
.loopRead
    jsr     i2c_rxbyte          ;6+333      read byte from EEPROM
    cmp     #$ff                ; 2         EEPROM slot empty?
    bne     .skipEmpty          ; 2/3        no, skip clear
    lda     #0                  ; 2         clear EEPROM slot
.skipEmpty
    sta     highScoreSK,x       ; 4
    dex                         ; 2
    bpl     .loopRead           ; 2/3= 354

; stop read:
    jmp     i2c_stopread        ;3+92=95    terminate read

;------------------------------------------------------------------------------

WriteSaveKey ; = 1751

; check if new high score:
    ldx     highScoreSK         ; 3
    inx                         ; 2
    ;beq     NoSKfound           ; 2/3       no new high score, abort

; setup SaveKey:
    lda     startRoom           ; 3         load start room*5 and level
    ldx     startLevel          ; 3
    jsr     SetupSaveKey        ; 6+853
    bcc     NoSKfound           ; 2/3

    ;    lda     #$64
    ;    sta     COLUBK

; write high score:
    ldx     #3-1                ; 2 = 841
.loopWrite
    lda     highScoreSK,x       ; 4
    jsr     i2c_txbyte          ;6+264      transmit to EEPROM
    dex                         ; 2
    bpl     .loopWrite          ; 2/3= 837

; stop write:
    jmp     i2c_stopwrite       ; 3+42= 45  terminate write and commit to memory


;------------------------------------------------------------------------------

SetupSaveKey ; = 853

    ldx #<(SAVEKEY_ADR+24)                        ; 2 = 22
; detect SaveKey:
    jsr     i2c_startwrite      ;6+280
    bne     .exitSK             ; 2/3

; setup address:
    clv                         ; 2
    ldx     #>SAVEKEY_ADR       ; 2         upper byte of address
    txa
    jsr     i2c_txbyte          ;6+264
    txa                         ; 2         x = lower byte offset
    jmp     i2c_txbyte          ;3+264      returns C==1

.exitSK
    clc
    rts
NoSKfound
; jmp NoSKfound
     rts
  ENDIF


    ENDIF


;███████████████████████████████████████████████████████████████████████████████
;===============================================================================
; Bank 6 - 6507 code
;----------------------------------------
;   CDFJ will always start in bank 6 because banks 0-5 could contain ARM code
;===============================================================================

;    echo "regular 6502 code begins at", *


;  .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.
; / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \
;`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`



    include "menuKernel.asm"




;  .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.   .-.-.
; / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \ / / \ \
;`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`-'   `-`



                ;  NTSC       PAL           PAL-60        SECAM
;TimerOS         dc OS_TIM64T, OS_TIM64T+29, OS_TIM64T, OS_TIM64T ;+29
;TimerVB         dc VB_TIM64T, VB_TIM64T+30, VB_TIM64T, VB_TIM64T ;+30

;overscanInit    dc _FN_GAME_OS, _FN_GAME_OS2


InitSystem
                    cld


        ; Remarkable system clear by Omegamatrix
        ; SP=$FF, X=A=0

.clear_machine      ldx #$A                 ; ASL opcode = $0A
                    inx
                    txs
                    pha
                    bne .clear_machine+1    ; jump between operator and operand to do ASL

                    stx SETMODE             ; <-- x=0   "Fast Fetch" enable
                    ;stx kernel              ; "copyright" ==0 by sys clear


    ; #if __ENABLE_ATARIVOX
    ;     ldx #$FF
    ;     stx _BUF_SPEECH ;tmp
    ; #endif
                    ldy #_FN_SYSTEM_RESET
                    jsr CallArmCode

;                    jsr ReadSaveKey

    ; fall through

startAnyKernel      ldx kernel
                    stx currentKernel
                    lda KernelVectorHi,x
                    pha
                    lda KernelVectorLo,x
                    pha
                    rts



KernelVectorLo      .byte <(KernelCopyright-1)
                    .byte <(KernelMenu-1)
                    .byte <(KernelGame-1)
                    .byte <(KernelStatus-1)

KernelVectorHi      .byte >(KernelCopyright-1)
                    .byte >(KernelMenu-1)
                    .byte >(KernelGame-1)
                    .byte >(KernelStatus-1)



    include "gameKernel.asm"


;    echo "Kernels end @", * - 1




FREE_INDIRECT = ARM_DIRECT_DATA - END_OF_INDIRECT_DATA

 ;   echo "----", *, " (end of kernels)"

 ;   echo "----", [$7FED-*]d, " kernel bytes available"
 ;   echo "----", [FREE_INDIRECT]d, " indirect bytes available"

;    echo "TOTAL free ROM space =", [FREE_INDIRECT + ($7FEd - *)]d, "bytes"


FREE_SPACE = ARM_DIRECT_DATA_END - $8000 - *
    echo "FREE (6502) END-SPACE =", (FREE_SPACE)d, "bytes"

;===============================================================================

    ORG $7FED
    RORG $FFED

ARM_DIRECT_DATA_END

                    jmp InitSystem

                    ds 12, 0                         ; reserve space for CDFJ registers

                    .WORD InitSystem
                    .WORD InitSystem

;===============================================================================
; Display Data
;----------------------------------------
;   4K of RAM shared between the 6507 and ARM.
;
;   NOTE: anything prefixed with _ ends up in main/defines_from_dasm_for_c.h
;         so that the C code will have the same values as the 6507 code
;===============================================================================

    SEG.U DISPLAYDATA
    ORG $0000

_DS_TO_ARM

_RUN_FUNC           ds 1        ; function to run
_SWCHA              ds 1        ; joystick directions to ARM code
_SWCHB              ds 1        ; console switches to ARM code
_INPT4              ds 1        ; left firebutton state to ARM code
;_INPT5              ds 1        ; right firebutton state to ARM code

;_HIGHSCORE          ds 3


_DS_FROM_ARM

_P1_X               ds 1        ; position of player 1
_P0_X               ds 1        ; position of player 0
;_TV_TYPE            ds 1        ; 0 NTSC, 1 PAL, 2 PAL-60 3 SECAM
_KERNEL             ds 1        ; 0 = copyright, 1 = menu, 2 = game, 3 = stats

_ARENA_COLOUR       ds 1


_BUF_AUDV           ds 2
_BUF_AUDC           ds 2
_BUF_AUDF           ds 2

;_HIGHSCORE_NEW      ds 3

    align 4

_BUF_JUMP1          ds _ARENA_SCANLINES * 2 + 2         ; long-word align!!


_BOARD              ds __BOARD_SIZE + 4    ; extra for grab+1 in drawscreen "bug"

    IF __ENABLE_ATARIVOX
_BUF_SPEECH         ds 1
    ENDIF
;------------------------------------------------------------------------------

    ALIGN 4
_BUFFERS = *

    SEG.U BUFFER1
    ORG _BUFFERS

BUFN SET 0
    MAC DEFBUF ;name
_BUF_{1}             ds ARENA_BUFFER_SIZE
BUFN SET BUFN + 1
    ENDM

    DEFBUF COLUP0
    DEFBUF COLUP1
    DEFBUF COLUPF
    DEFBUF COLUBK

    ; Axiom: PF buffers contiguous (optimisation)
    DEFBUF PF0_LEFT
    DEFBUF PF1_LEFT
    DEFBUF PF2_LEFT
    DEFBUF PF0_RIGHT
    DEFBUF PF1_RIGHT
    DEFBUF PF2_RIGHT

    DEFBUF GRP0A
    DEFBUF GRP1A

_BUFFER_BLOCK_SIZE = * - _BUFFERS





;------------------------------------------------------------------------------

    SEG.U BUFFER2
    ORG _BUFFERS

    DEFBUF MENU_COLUPF
    DEFBUF MENU_COLUP0

    ; Order of these 4 important...
    DEFBUF MENU_PF1_LEFT
    DEFBUF MENU_PF2_LEFT
    DEFBUF MENU_PF1_RIGHT
    DEFBUF MENU_PF2_RIGHT

    DEFBUF MENU_GRP0A
    DEFBUF MENU_GRP1A
    DEFBUF MENU_GRP0B
    DEFBUF MENU_GRP1B
    DEFBUF MENU_GRP0C
    DEFBUF MENU_GRP1C

;    SEG.U OVERLAP
;    ORG _BUFFERS + _BUFFER_BLOCK_SIZE


;    ds 10 * 21

DISPLAY_DATA_SIZE = *
    echo "----DISPLAY DATA SIZE =",DISPLAY_DATA_SIZE,"bytes"



; OverlapDisplayDataRam:  ; mark the beginning of overlapped RAM
; ; Splash screen datastream buffers
; ;_BUF_SPLASH0:   ds 192
; ;_BUF_SPLASH1:   ds 192
; ;_BUF_SPLASH2:   ds 192
; ;_BUF_SPLASH3:   ds 192

;     echo "----",($1000 - *) , "Splash bytes of Display Data RAM left"
; ;----------------------------------------
; ; this ORG overlaps the Menu datastreams on top of the Splash datastreams
; ;----------------------------------------

;     ; Generic overlap variable usage
;     ORG OverlapDisplayDataRam
;     echo "----",($1000 - *) , "Menu bytes of Example Overlap RAM left"

; ;----------------------------------------
; ; this ORG overlaps the Game datastreams on top of the Splash and Menu datastreams
; ;----------------------------------------
;     ORG OverlapDisplayDataRam
; ; Game datastream buffers
; _EVERY_FRAME_ZERO_START:

;     align 4 ; need to be 4 byte aligned to use myMemsetInt
; _EVERY_FRAME_ZERO_COUNT=*-_EVERY_FRAME_ZERO_START   ; end of zeroed out data



    ; modify custom.boot.lds to adjust display data RAM
    ; ref: https://forums.atariage.com/topic/297759-part-1-cdfj-overview/
    ; ideally we want 0 bytes of display data free, all the rest available for CRAM

DISPLAYDATA_BASE SET $40000800
CRAM_BASE        SET $4000165C
CRAM_MALLOC      SET ($40002000 - CRAM_BASE)

    echo "----",(CRAM_MALLOC)d,"bytes allocated for RAM"
    echo "----",((CRAM_BASE - DISPLAYDATA_BASE) - *)d , "bytes of Display Data RAM avaialble"

#if (CRAM_BASE - DISPLAYDATA_BASE) < DISPLAY_DATA_SIZE
    echo "ERROR: MEMORY OVERLAP IN DISPLAYDATA/RAM"
    ERR
#endif