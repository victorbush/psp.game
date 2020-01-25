using System;
using System.Collections.Generic;

namespace AutoC
{
    [Serializable]
    public class Config
    {
        private const string DefaultInputDirectory = @"..\..\..\..\..\..\src\";

        private const string DefaultOutputDirectory = @"..\..\..\..\..\..\src\autogen\";

        private static readonly List<string> DefaultExcludeDirectories = new List<string>
        { 
            @"..\..\..\..\..\..\src\autogen\",
            @"..\..\..\..\..\..\src\thirdparty\",
        };

        public Config()
        {
            this.OutputDirectory = DefaultOutputDirectory;
            this.InputDirectory = DefaultInputDirectory;
            this.ExcludedDirectories = DefaultExcludeDirectories;

            // Purposely start this as null. On the first request to check if a directory is
            // excluded, this list will be generated.
            this.ExcludedDirectoriesFullPaths = null;
        }

        /// <summary>
        /// This is a list of directories to exclude from processing. These directories are relative
        /// to the exe startup path. Use <see cref="IsDirectoryExcluded"/> to properly check if a
        /// directory is excluded.
        /// </summary>
        public List<string> ExcludedDirectories { get; set; }

        /// <summary>
        /// The directory to process files in (recursively), relative to the exe startup path.
        /// </summary>
        public string InputDirectory { get; set; }

        /// <summary>
        /// The directory to output generated header files, relative to the exe startup path.
        /// </summary>
        public string OutputDirectory { get; set; }

        /// <summary>
        /// This is the list of excluded directories with their full directory paths. This list is
        /// generated from <see cref="ExcludedDirectories"/>.
        /// </summary>
        private List<string> ExcludedDirectoriesFullPaths { get; set; }

        /// <summary>
        /// Checks if a directory should be excluded from processing.
        /// </summary>
        /// <param name="path">The full directory path.</param>
        /// <returns>True if the directory should be excluded.</returns>
        public bool IsDirectoryExcluded(string path)
        {
            // Check if we need to compute the exlcuded directories full paths
            if (this.ExcludedDirectoriesFullPaths == null)
            {
                this.ExcludedDirectoriesFullPaths = new List<string>();

                foreach (var excludedPath in this.ExcludedDirectories)
                {
                    var fullPath = System.IO.Path.GetFullPath(excludedPath);
                    this.ExcludedDirectoriesFullPaths.Add(fullPath);
                }
            }

            // Check if requested directory is excluded
            foreach (var excludedPath in this.ExcludedDirectoriesFullPaths)
            {
                if (path.StartsWith(excludedPath.TrimEnd('\\')))
                {
                    // This directory is excluded
                    return true;
                }
            }

            // This directory is not excluded
            return false;
        }
    }
}