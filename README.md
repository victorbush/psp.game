
- make.bat, clean.bat, rebuild.bat
    - Batch files to make the project.
    - Uses https://github.com/pspdev/pspdev-docker
    - Must have already loaded the pspdev-docker image.

For Visual Studio:
- Set PSP_SDK_DIR path in user property sheet. Used to point visual studio to header files for intellisense.
    - Path should be path\to\pspsdk\src