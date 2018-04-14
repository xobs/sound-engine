# Love-to-Code Sound Engine

This is a development module for a bytecode-based sound engine for the code- and memory-constrained Love-to-Code module.

On real hardware, audio is made by ganging two PWM channels together and driving them at opposite polarities.

On a development system, the program writes to stdout which shoud then be piped to `play` (part of the `sox` package).