/* //////////////////////////////////////////////////////////

		sBOX(C6713) linker command file

		INRAM location, Flash boot
		only far variables are allocated in SDRAM (tkurita)

			File	: flashboot.cmd
			Date	: 2006.  6.  6
			Ver		: 1.1.0

			-- revision history --
				release 2004. 10. 18 	Ver.1.0.0
				Ver. up 2006.  6.  6 	Ver.1.1.0	INRAM 256K -> 192K

		Copyright(c) 2004-2006. Mtt Company DSP Lab. Kobe

////////////////////////////////////////////////////////// */

-c
-u_boot_start
-lrts6700.lib
-stack 1000h   /* allocation field 4K */
-heap  1000h   /* allocation field 4K */


MEMORY
{
    BOOTLD	: origin = 0x90000000, length = 0x00000400
    FLASH   : origin = 0x90000400, length = 0x0003FC00		/* 256K - 1K byte */
	INRAMLD : origin = 0x00000000, length = 0x00000400
	INRAM   : origin = 0x00000400, length = 0x0002FC00		/* 192K byte  */
	SDRAM   : origin = 0x80000000, length = 0x00800000		/* 8M byte   */
}

SECTIONS
{
	.bootld: load = BOOTLD,  run = 0h
	.intvec: load = FLASH,   run = INRAM
    .text:   load = FLASH,   run = INRAM
    .cinit:  load = FLASH,   run = INRAM
    .const:  load = FLASH

	.bss   : load = INRAM	/* VARIALBLES					*/
	.far   : load = SDRAM	/* VARIABLES DECLARED FAR       */
	.cio   : load = INRAM	/* BUFFER  						*/
	.sysmem: load = INRAM	/* DYNAMIC ALLOCATION MEMORY    */
	.stack : load = INRAM	/* SYSTEM STACK					*/
}

