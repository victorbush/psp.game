
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