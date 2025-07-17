

; Speech Data
; add address to __SPEECH_ADDRESS table in sokoban.asm
; make sure __WORD_COUNT is correct

    MAC SPEAK_SPEED ;{1}
    ; 114 default
    ; 0-127 range

    dc 21
    dc {1}
    ENDM

SPEAK_NULL
    dc 0xFF

SPEAK_SOSAD
    dc 6, 6, 6, 6, 6, 6, 6, 6, 6,  191, 21,122, 139, 21,114, 139, 171, 132, 175
    dc 6,6,6, 3, 188, 137, 188, 132, 175, 21, 114
    dc $FF

SPEAK_GOODBYE
    dc 179, 21,119, 138, 175, 171, 21,121, 157, 128 
    dc 3, 195, 148, 139, 146,
    dc 147, 21,119, 151, 146, 175    
    dc $FF

SPEAK_RAGEQUIT
    dc 184, 136,
    dc 3, 184, 136,
    dc 3, 184, 136, 136,
    dc 3, 21, 121, 148, 130, 165, 21,114, 195, 147, 129, 191,
    dc $FF

SPEAK_DOSOMETHING
    dc 175, 139, 188, 134, 140, 190, 129, 144,
    dc $FF

SPEAK_LETSGOBABY

    dc 146, 131, 191, 188, 6, 179, 164, 171, 130, 171, 128,
    dc $FF

SPEAK_WET
    dc 163,163,
    dc $FF

SPEAK_AARGH
    dc 22,150       ; pitch
    dc 20,50        ; quieter
    dc 14           ; stress
    dc 161,182
    dc 31
    dc $FF


SPEAK_TOO_LATE
    dc 191, 139, 188, 8,146, 137 ; "too slow"
;    dc 191, 139, 146, 130, 191 ; "too late"
    dc $FF

SPEAK_GETREADY
    dc 179,131,191,148,131,175,129
    dc $FF

SPEAK_TRAP
    dc 191 ,148 ,132 ,199 ,169 ,129 ,133 ,140 ,128 ,171 ,133 ; "Trap the x"
    dc $FF

SPEAK_WATCHOUT
    SPEAK_SPEED 120
    dc 147, 135, 182, 161, 191
    dc $FF


; SPEAK_FIRE
;         dc.b    31,253
;         dc.b    $ff

STRESS = 14
SLOWER = 8
