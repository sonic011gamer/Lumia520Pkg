ENTRY(_start);

SECTIONS
{
    _start = 0x98F00000;
    . = 0x98F00000;
    .data : {
        *(.data)
    }
}
