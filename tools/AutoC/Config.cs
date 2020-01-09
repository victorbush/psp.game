using System;
using System.Collections.Generic;
using System.Text;

namespace AutoC
{
    [Serializable]
    public class Config
    {
        private const string DefaultInputDirectory = @"..\..\..\..\..\src\";

        private const string DefaultOutputDirectory = @"..\..\..\..\..\src\autogen\";

        public Config()
        {
            this.OutputDirectory = DefaultOutputDirectory;
            this.InputDirectory = DefaultInputDirectory;
        }

        public string OutputDirectory { get; set; }

        public string InputDirectory { get; set; }
    }
}
