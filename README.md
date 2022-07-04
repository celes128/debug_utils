# debug_utils


This is a Windows-only library. For now, it only has a console along with some helper classes.

## Console

The console in debug_utils/ contains the logic and input handling for a console that behaves like a Windows console or Linux terminal.

It has a history with fixed capacity that you can browse with the up and down arrow keys.

The command line editing is very limited: only the Backspace, Delete, Home and End keys are supported.



### Demo

There is a Direct2D/DirectWrite program that shows how to use the console. It is in the DemoConsole/ folder.