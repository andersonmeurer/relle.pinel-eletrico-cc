Segger emWin v5.30 for NXP Platforms
=====================================

These emWin library files were built using:

- arm-none-eabi-gcc gcc version 4.9.3 (LPCXPresso 7.9.0).
- IAR ANSI C/C++ Compiler V7.40.3.8902 for ARM
- Keil ARM C/C++ Compiler 5.05 update 2 (build 169) [MDK-ARM Professional 5.15]
- Visual C++ 2010 Express edition

There is one set of library files for each of the 5 ARM cores and one for the x86 core.

Cortex-M4 libraries are compatible with applications generated using
 1. no-floating point
 2. software floating point
 3. hardware floating point with soft-ABI

Cortex-M4f_hard libraries are compatible only with application generated using
hardware floating point with hard-ABI.

Library files that include '_d5' are built with GUI_DEBUG_LEVEL=5. Linking with these
libraries will cause diagnostic messages for log entries, warnings, and entries to be produced.
The file GUI_Debug.h contains a description of these debug levels.
