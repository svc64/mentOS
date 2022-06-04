ENTRY(start)

. = 0x0;

SECTIONS
{
    .header : {
        LONG(0x73746e6d) // "mnts"
        LONG(start)
        LONG(_rela_start)
        LONG(_rela_end)
    }
    .text : {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t*) 
    }
    .rodata : { 
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }
    .rela.dyn : { // tnx marcan
        _rela_start = .;
        *(.rela)
        *(.rela.text)
        *(.rela.got)
        *(.rela.plt)
        *(.rela.bss)
        *(.rela.ifunc)
        *(.rela.text.*)
        *(.rela.data)
        *(.rela.data.*)
        *(.rela.rodata)
        *(.rela.rodata*)
        *(.rela.dyn)
        _rela_end = .;
    }
    . = ALIGN(4096);
    PROVIDE(_data = .);
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    . = ALIGN(4096);
    .data : {
         *(.data .data.* .gnu.linkonce.d*) 
         . = ALIGN(4096);
    }

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
   _end = .;
}
