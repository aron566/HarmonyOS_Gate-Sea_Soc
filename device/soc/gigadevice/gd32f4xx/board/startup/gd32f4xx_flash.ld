/* memory map */
MEMORY
{
/*  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 3072K*/
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 3056K
  /*RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 192K*/
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 176K
}

ENTRY(Reset_Handler)

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 4K;
  
/* ISR vectors */
  .vectors :
  {
    . = ALIGN(4);
    KEEP(*(.vectors))
    . = ALIGN(4);
    __Vectors_End = .;
    /*__Vectors_Size = __Vectors_End - __gVectors;*/
    __Vectors_Size = __Vectors_End;
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7) 
    *(.glue_7t)
    *(.eh_frame)

    _hdf_drivers_start = .;
    KEEP(*(.hdf.driver))
    _hdf_drivers_end = .; /* define a section for hdf driver */

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    /* the symbol 窶論etext窶? will be defined at the end of code section */
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

    .zinitcall_array :
    {
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_core_start = .);
        KEEP (*(SORT(.zinitcall.core*)))
        KEEP (*(.zinitcall.core*))
        PROVIDE_HIDDEN (__zinitcall_core_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_device_start = .);
        KEEP (*(SORT(.zinitcall.device*)))
        KEEP (*(.zinitcall.device*))
        PROVIDE_HIDDEN (__zinitcall_device_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_bsp_start = .);
        KEEP (*(SORT(.zinitcall.bsp*)))
        KEEP (*(.zinitcall.bsp*))
        PROVIDE_HIDDEN (__zinitcall_bsp_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_sys_service_start = .);
        KEEP (*(SORT(.zinitcall.sys.service*)))
        KEEP (*(.zinitcall.sys.service*))
        PROVIDE_HIDDEN (__zinitcall_sys_service_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_app_service_start = .);
        KEEP (*(SORT(.zinitcall.app.service*)))
        KEEP (*(.zinitcall.app.service*))
        PROVIDE_HIDDEN (__zinitcall_app_service_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_sys_feature_start = .);
        KEEP (*(SORT(.zinitcall.sys.feature*)))
        KEEP (*(.zinitcall.sys.feature*))
        PROVIDE_HIDDEN (__zinitcall_sys_feature_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_app_feature_start = .);
        KEEP (*(SORT(.zinitcall.app.feature*)))
        KEEP (*(.zinitcall.app.feature*))
        PROVIDE_HIDDEN (__zinitcall_app_feature_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_run_start = .);
        KEEP (*(SORT(.zinitcall.run*)))
        KEEP (*(.zinitcall.run*))
        PROVIDE_HIDDEN (__zinitcall_run_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_test_start = .);
        KEEP (*(SORT(.zinitcall.test*)))
        KEEP (*(.zinitcall.test*))
        PROVIDE_HIDDEN (__zinitcall_test_end = .);
        . = ALIGN(0x4) ;
        PROVIDE_HIDDEN (__zinitcall_exit_start = .);
        KEEP (*(SORT(.zinitcall.exit*)))
        KEEP (*(.zinitcall.exit*))
        PROVIDE_HIDDEN (__zinitcall_exit_end = .);
    } > FLASH
   .ARM.extab :
  { 
     *(.ARM.extab* .gnu.linkonce.armextab.*) 
  } >FLASH
  
    .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    } >FLASH

  .ARM.attributes : { *(.ARM.attributes) } > FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* provide some necessary symbols for startup file to initialize data */
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    /* the symbol 窶論sdata窶? will be defined at the data section end start */
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    /* the symbol 窶論edata窶? will be defined at the data section end */
    _edata = .;
  } >RAM AT> FLASH

  .vector :
  {
    . = ALIGN(0x200);
    KEEP(*(.vector))
    KEEP (*(SORT(.vector)))
  }>RAM

  . = ALIGN(4);
  .bss :
  {
    /* the symbol 窶論sbss窶? will be defined at the bss section start */
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    /* the symbol 窶論ebss窶? will be defined at the bss section end */
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

 . = ALIGN(8);

  PROVIDE ( end = _ebss );
  PROVIDE ( _end = _ebss );

  . = ALIGN(8);
  __los_heap_addr_start__ = .;
  /* fixme: conflict with _estack? */
  __los_heap_addr_end__ = ORIGIN(RAM) + LENGTH(RAM)- __stack_size;

  .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
  {
    PROVIDE( _heap_end = . );
    . = __stack_size;
    PROVIDE( _sp = . );
  } >RAM AT>RAM

}

 /* input sections */
/*GROUP(libgcc.a libc.a libm.a libnosys.a)*/
/*GROUP(libgcc.a libc.a libm.a)*/
