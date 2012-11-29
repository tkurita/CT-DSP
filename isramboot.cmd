/* //////////////////////////////////////////////////////////

		sBOX(C6713) linker command file

		Inter RAM allocation
		only far variables are allocated in SDRAM (tkurita)
			File	: iramboot.cmd
			Date	: 2006. 6. 6
			Ver		: 1.1.0

			-- revision history --
				release 2004. 10. 18 	Ver.1.0.0
				ver.up	2005.  1. 22	Ver.1.0.1
				ver.up	2006.  6.  6	Ver.1.1.0	INRAM size 256K -> 192K

		Copyright(c) 2004-2006. Mtt Company DSP Lab. Kobe

////////////////////////////////////////////////////////// */

-c
-u_boot
-lrts6700.lib
-stack 1000h   /* alloction field 4K */
-heap  1000h   /* alloction field 4K */


MEMORY
{
	INRAM    : origin = 0x00000000 length = 0x00030000		/* 192K byte  */
	SDRAM    : origin = 0x80000000 length = 0x00800000		/* 8M byte   */
	FLASHRAM : origin = 0x90000000 length = 0x00040000		/* 256K byte  */
}

SECTIONS
{
	.boot  : load = 0h		/* BOOT							*/
	.intvec: {} > INRAM		/* INTERRUPT SERVICE TABLE		*/
	.text  : {} > INRAM		/* PROGRAM						*/
   	.cinit : {} > INRAM		/* INITIALIZATION TABLES		*/
	.bss   : {} > INRAM		/* VARIALBLES					*/
	.const : {} > INRAM		/* CONSTANTS					*/
	.far   : {} > SDRAM		/* VARIABLES DECLARED FAR       */
	.cio   : {} > INRAM		/* BUFFER  						*/
	.sysmem: {} > INRAM		/* DYNAMIC ALLOCATION MEMORY    */
	.stack : {} > INRAM		/* SYSTEM STACK					*/
}

