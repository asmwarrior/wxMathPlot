# wxMathPlot

I have updated the mathplot component for wxWidgets 3.2.1.
I have made some improvements like :
- corrected some bugs (and probably add others lol )
- rewrote some methods
- added a config window (that you can have access with the right click)
- add "view as bar" for XY functions
- view fullscreen if plot is in a single frame
- add in the demo the samples of the original project
- add second Y axis management
- lot of things ...<br>
I keep the philosophy and the structure so it was easy to update existing project (I hope !).

![Demo](Demo.png "Demo")  

Enjoy
Lionel

# Change by Asmwarrior

If you are using Code::Blocks with MSYS2's prebuilt wxWidgets 3.2 library.
You can directly open the the file `MathPlotConfig/MathPlotConfig.cbp` and build it.
Note that you need `wx-config-msys2.exe` somewhere in your PATH, which is used to generate the correct compiler and linker options for the wxWidgets library from MSYS2.
More details can be found [eranif/wx-config-msys2: wx-config tool for MSYS2 based installation of wxWidgets using the mingw64 repository](https://github.com/eranif/wx-config-msys2),
You can build this tool from the command line shell in MSYS2.
