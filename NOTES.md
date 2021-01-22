# Random Notes

- make.bat, clean.bat, rebuild.bat
    - Batch files to make the project.
    - Uses https://github.com/pspdev/pspdev-docker
    - Must have already loaded the pspdev-docker image.
    - Use ppsspp for emulator.
    - Need to copy eboot to the game folder and launch from there for the asset file paths to be correct.

For Visual Studio:
- Set VULKAN_SDK environment 
- Set debug directory to /game/. Seems this is a per user setting?



(remove - not used anymore. use vs code for psp build) - Set PSP_SDK_DIR path in user property sheet. Used to point visual studio to header files for intellisense.
    - Path should be path\to\pspsdk\src



Development folder structure:
- docker
    - Not required to be here. Only need to have docker image loaded named 'pspdev-docker'.
- ppsspp
- projects
    - jetz
- pspsdk




Notes for compiler options:
JETZ_CONFIG_PLATFORM_PSP
JETZ_CONFIG_PLATFORM_GLFW




TODO
- create a string library - to allow common string copy functions for psp and others (can allow safe string copy too)
- create a memory library for malloc, free, etc - will allow for tracking memory, profiling, etc.

- prefixed underscore in type names means (kind of) "private" -- TODO
- code generation?
- move VMA to vulkan static lib?


- GPU window and GPU frame abstraction
- autoc - code generation
- Editor:
	- imgui integration
	- Open, save, and create world files
	- Add/move/delete geometry
	- Add/move/delete entities


- file conventions, naming conventions, etc.
	- include paths:
		- set include path to /src directory.
		- all #include paths are relative to /src. Include the path prefix in front.
			Example: #include "engine/kk_math.h"
		- for system includes or library includes that are relative to /src, put in the angle brackets:
			Example: #include <math.h>
	TODO
    
    
    
    
    
    
    
    
directory setup:
- /docker
- /ppsspp
- /projects
- /pspsdk 
    
    
 Docker setup:
 - Clone https://github.com/pspdev/pspdev-docker into /docker.
 - run `docker build -t pspdev-docker .`
 - this builds the docker image. it builds the psp toolchain.
 - you can save the docker image so you can transfer it to another PC or save. (see pspdev-docker repo readme).
 - building it takes a long time.
 - `docker load -i pspdev.docker` load from existingd
 
 
 
 
 
 
 other notes
 
 - incremental builds for psp are not always consistent. if you have weird errors, try a rebuild first.
 - debug settings (working directory, command line args, etc) are stored in visual studio user file so are checked into source control. Need to add to vcxproj.