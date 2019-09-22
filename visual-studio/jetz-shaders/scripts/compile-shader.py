# =======================================================================================
# Usage:
#   compile-shader.py inputFile outputFile
#
# This script is used as a Custom Build Tool in the "Shaders" Visual Studio project.
#
# To add a new shader to the project:
#  - Add the shader file to the project.
#  - Right-click the shader file and choose "Properties".
#  - Under "Configuration Properties" -> "General" -> "Item Type" choose "Custom Build Tool".
#  - Save and close the properties window.
#
# To edit the Custom Build Tool properties:
#   - Right-click the "Shaders" project and choose "Properties".
#   - Edit properties in "Configuration Properties" -> "Custom Build Tool".
# =======================================================================================

# Imports
import os
import subprocess
import sys

# Check for required environment variables
if 'VULKAN_SDK' not in os.environ:
    print('ERROR: VULKAN_SDK environment variable not set.')
    sys.exit(1)

# Configuration
VULKAN_SDK = os.environ['VULKAN_SDK']
GLSLANG_EXE = os.path.join(VULKAN_SDK, 'Bin32', 'glslangValidator.exe')

# Get arguments
if len(sys.argv) != 3:
    print('ERROR: missing or unknown command line arguments.')
    sys.exit(1)

# Get input and output file paths
inputFilePath = sys.argv[1]
inputFileName = os.path.basename(inputFilePath)
outputFilePath = sys.argv[2]

if not os.path.isfile(inputFilePath):
    print('ERROR: specified input file not found: ' + inputFilePath)
    sys.exit(1)

# Call compiler
process = subprocess.Popen([GLSLANG_EXE,
                            '-V',
                            inputFilePath,
                            '-o',
                            outputFilePath],
                           stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE)

# Print std output from compiler process
for line in process.stdout:
    print(line)

# Print std error from compiler process
for line in process.stderr:
    print(line)

# Exit with compiler's error code
errcode = process.returncode
sys.exit(errcode)
