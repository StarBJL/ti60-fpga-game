/*
 * game.ld - linker script for the SproutRun HDMI demo.
 * Same layout as bsp/.../linker/default.ld but the RAM region is enlarged:
 * render.c keeps a 640x480 RGB565 background (600 KB) plus the Game struct (~54 KB)
 * in .bss, which does not fit the default 124 KB.
 *
 * DDR map used by this app:
 *   0x0000_1000 .. 0x00F0_0FFF  program / data / bss / stack   (this script, 15 MB)
 *   0x0100_0000 .. 0x0109_5FFF  framebuffer 0  (640*480*2 = 614400 B)
 *   0x0120_0000 .. 0x0129_5FFF  framebuffer 1
 *   0x0140_0000 .. 0x0149_5FFF  framebuffer 2  (triple buffering)
 *   0x0160_0000 / 0x0180_0000 / 0x01A0_0000   demo pictures 1..3 (BitBlt image switching)
 *   0x01E0_0000 .. 0x01FC_1FFF  fourth picture (1280x720 RGB565), below sound bank
 *   0x01C0_0000 .. 0x01C9_5FFF  framebuffer 3  (band mode: one more frame is in the engine)
 *   0x0200_0000 .. 0x02FF_FFFF  sound bank (section .sound, tools/wav2bank.py; read by the audio mixer)
 *   0x0300_0000 .. 0x03B3_FFFF  band mode bins (engine scratch, up to 45 bands x 256 KB at 720p)
 *   0x0400_0000 .. 0x05FF_FFFF  .artbss (NOLOAD, not cleared at start): danmaku art for the 1.5x view (~9.6 MB),
 *                               since 2026-10-26 also the 2 MB command list, the squares fill tables (768 KB)
 *                               and the carrot locator outlines (2 MB) - the 15 MB program region was full;
 *                               danmaku art for the 1.5x (720p) view (section .artbss, NOLOAD:
 *                               not in the download, drawn by th_art_init; framebuffers at 720p
 *                               are 1280*720*2 = 1.8 MB, still 2 MB apart)
 *   0x0600_0000 .. 0x06FF_FFFF  compressed HD720 backgrounds (read-only ELF load)
 * The BitBlt engine (0xF810_1000) reads/writes these DDR addresses directly.
 */
OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  ram  (wxai!r) : ORIGIN = 0x00001000, LENGTH = 15M
  snd  (r)      : ORIGIN = 0x02000000, LENGTH = 16M
  art  (rw)     : ORIGIN = 0x04000000, LENGTH = 32M
  bg   (r)      : ORIGIN = 0x06000000, LENGTH = 16M
}

PHDRS
{
  ram PT_LOAD;
  snd PT_LOAD;
  bg PT_LOAD;
}

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 16K;

  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } >ram AT>ram :ram

  .text           :
  {
    *libc.a:*(.text .text.*)
    *(.text.main);
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
    *(.note.gnu.build-id)
  } >ram AT>ram :ram

  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >ram AT>ram :ram

  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);

  . = ALIGN(4);

  .preinit_array  :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >ram AT>ram :ram

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ram AT>ram :ram

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >ram AT>ram :ram

  .ctors          :
  {
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } >ram AT>ram :ram

  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } >ram AT>ram :ram

  .lalign         : ALIGN(4)
  {
    PROVIDE( _data_lma = . );
  } >ram AT>ram :ram

  .dalign         : ALIGN(4)
  {
    PROVIDE( _data = . );
  } >ram AT>ram :ram

  .data          :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x7F0 );
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
    . += 10; /* fix for linker false error message */
  } >ram AT>ram :ram

  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  .bss            :
  {
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(4);
  } >ram AT>ram :ram

  . = ALIGN(8);
  PROVIDE( _end = . );
  PROVIDE( end = . );

  .stack  : ALIGN(16)
  {
    PROVIDE( _heap_end = . );
    . = __stack_size;
    PROVIDE( _sp = . );
	__freertos_irq_stack_top = .;
  } >ram AT>ram :ram

  /* HDMI audio sound bank (sound/sound_bank.S, .incbin): loaded with the elf, read by the mixer.
     KEEP: nothing references it (the driver reads 0x02000000), --gc-sections would drop it. */
  .sound :
  {
    KEEP (*(.sound))
  } >snd AT>snd :snd

  /* HD720 compressed backgrounds: loaded by the ELF loader, decoded once.
     0x06000000..0x06ffffff is disjoint from framebuffers, audio, bins and art.
     The controller has a 28-bit byte address space (256 MiB DDR). */
  .bg_assets : ALIGN(64)
  {
    KEEP (*(.bg_assets .bg_assets.*))
  } >bg AT>bg :bg

  /* 1.5x danmaku art (th_art.c ART_BIG): big buffers above the band bins, not loaded */
  .artbss (NOLOAD) : ALIGN(64)
  {
    *(.artbss .artbss.*)
  } >art :NONE
}
