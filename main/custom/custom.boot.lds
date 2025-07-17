OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)


/* Entry Point  */
ENTRY(Custom)

/* LPC2103 memory areas  */

BASE_6507_START = 0x70FD;


/*     sdfasfad*/
MEMORY
{
    boot (RX)   : ORIGIN = 0x800     , LENGTH = 0x80    /* C-runtime booter */
    C_code (RX) : ORIGIN = 0x880     , LENGTH = BASE_6507_START - 0x880

    /* above:  technically 26K + whatever we overflow into $7000 before 6502 starts
        (adjust for BASE_6507_START if that is shifted)
        */


/* DISPLAYDATA = 0x40000800 -> 0x40001800 *normally*
   but we usurp some of its RAM by shifting the C RAM start from 0x40001800


    DISPLAYDATA_BASE  = 0x40000800
    CRAM_BASE = 0x4000165C  <--- adjust this in sokoban.asm and below
    LENGTH = 0x40002000-ram

         */

    ram         : ORIGIN = 0x4000165C, LENGTH = 0x9A4       /* 2468 bytes */

/*    ram         : ORIGIN = 0x40001800, LENGTH = 0x800 */   /* 2K free RAM */
/*    ram         : ORIGIN = 0x40001400, LENGTH = 0xC00 */  /* 3K free RAM, took 1K from Display Data */
/*    ram         : ORIGIN = 0x40001300, LENGTH = 0xD00 */  /* 3.25K free RAM, took 1.25K from Display Data */
/*    ram         : ORIGIN = 0x400012C0, LENGTH = 0xD40 */  /* 3392 bytes free RAM, took 1344 from Display Data */

}

/* Global symbol _stack_end */
_stack_end = 0x40001FDC;

/* Output sections  */
SECTIONS
{
    .bootcode :
    {
        _sboot = .;
        *(.boot)
        . = ALIGN(4);
        _eboot = .;
    } > boot

    .text : {
        .stext = .;
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >C_code

    .data : AT ( _etext ) {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } >ram

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >ram
}
end = .;
