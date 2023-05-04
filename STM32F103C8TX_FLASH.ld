ENTRY(Reset_Handler);

MEMORY
{
	RAM	(rwx): ORIGIN = 0x20000000, LENGTH = 20K
	FLASH (rx) : ORIGIN = 0X08000000, LENGTH = 64K	
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{

	.isr_vector : 
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} >	FLASH
	.text :	
	{
		. = ALIGN(4);
		*(.text*)
		. = ALIGN(4);
		_etext = .; 
	} >	FLASH
	.rodata	: 
	{
		. = ALIGN(4);
		*(.rodata*)
		. = ALIGN(4);
	} >	FLASH
	
	_sidata = LOADADDR(.data);
	
	.data :
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} > RAM AT > FLASH

	. = ALIGN(4);

	.bss :
	{
		_sbss = .;
		*(.bss*)
		. = ALIGN(4);		
		_ebss = .;
	} > RAM

}