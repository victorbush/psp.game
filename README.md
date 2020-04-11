
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
- file conventions, naming conventions, etc.
- prefixed underscore in type names means (kind of) "private" -- TODO
- code generation?



- GPU window and GPU frame abstraction
- autoc - code generation
- Editor:
	- imgui integration
	- Open, save, and create world files
	- Add/move/delete geometry
	- Add/move/delete entities


