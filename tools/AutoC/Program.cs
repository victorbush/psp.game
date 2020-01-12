using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace AutoC
{

    class Program
    {


        const string StartFlag = "//##";

        //const string EndFlag = "//##";
        const string EndFlag = "{"; // Go until the beginning brace of the function definition is found.

        //const string CacheFilePath = "cache.xml";

        const string ConfigFilePath = "config.xml";

        static Config Config;

        class Chunk
        { 
            public string Text { get; set; }
            public int StartIdx { get; set; }
            public int EndIdx { get; set; }

            public int NextIdx { get; set; }

            public bool Valid { get; set; }

            public string Param { get; set; }
        }



        
        static void Main(string[] args)
        {
            Config = LoadConfig();

            // Make sure output directory exists
            Directory.CreateDirectory(Config.OutputDirectory);

            // Process files in input dir
            foreach (var file in Directory.EnumerateFiles(Config.InputDirectory, "*.c", SearchOption.AllDirectories))
            {
                ProcessFile(file);
            }
        }

        static Config LoadConfig()
        {
            Config config = null;
            var xs = new XmlSerializer(typeof(Config));

            try
            {
                using (var r = new FileStream(ConfigFilePath, FileMode.Open))
                {
                    config = (Config)xs.Deserialize(r);
                }
            }
            catch
            {
            }

            if (config == null)
            {
                config = new Config();
            }

            return config;
        }


        //static Cache LoadCache()
        //{
        //    Cache cache;

        //    var xs = new XmlSerializer(typeof(Cache));
        //    using (var r = new FileStream(CacheFilePath, FileMode.Open))
        //    {
        //        cache = xs.Deserialize(r) as Cache;
        //    }

        //    // If no cache found (or invalid) create new one
        //    if (cache == null)
        //    {
        //        cache = new Cache();
        //    }

        //    return cache;
        //}

        //static void SaveCache(Cache cache)
        //{
        //    var xs = new XmlSerializer(typeof(Cache));
        //    using (var r = new FileStream(CacheFilePath, FileMode.Open))
        //    {
        //        xs.Serialize(r, cache);
        //    }
        //}

        static void ProcessFile(string filePath)
        {
            var fileName = Path.GetFileNameWithoutExtension(filePath);
            var src = File.ReadAllText(filePath);

            var publicHeaderPath = Path.Combine(Config.OutputDirectory, fileName + ".public.h");
            var publicHeaderText = "";

            var internalHeaderPath = Path.Combine(Config.OutputDirectory, fileName + ".internal.h");
            var internalHeaderText = "";

            var staticHeaderPath = Path.Combine(Config.OutputDirectory, fileName + ".static.h");
            var staticHeaderText = "";

            var chunk = GetNextChunk(src, 0);
            while (chunk.Valid)
            {
                if (chunk.Param == "static")
                {
                    staticHeaderText += chunk.Text;
                }
                else if (chunk.Param == "internal")
                {
                    internalHeaderText += chunk.Text;
                }
                else if (chunk.Param == "public")
                {
                    publicHeaderText += chunk.Text;
                }
                else
                {
                    Console.WriteLine("ERROR: Unknown access modifier: " + chunk.Param);
                }

                chunk = GetNextChunk(src, chunk.NextIdx);
            }

            // Write output text
            if (!string.IsNullOrWhiteSpace(publicHeaderText))
            {
                File.WriteAllText(publicHeaderPath, publicHeaderText);
            }

            if (!string.IsNullOrWhiteSpace(internalHeaderText))
            {
                File.WriteAllText(internalHeaderPath, internalHeaderText);
            }

            if (!string.IsNullOrWhiteSpace(staticHeaderText))
            {
                File.WriteAllText(staticHeaderPath, staticHeaderText);
            }
        }

        static Chunk GetNextChunk(string src, int startIdx)
        {
            var chunk = new Chunk();
            chunk.Valid = false;

            var startFlagIdx = src.IndexOf(StartFlag, startIdx);
            if (startFlagIdx < 0)
            {
                return chunk;
            }

            var endFlagIdx = src.IndexOf(EndFlag, startFlagIdx + 1);
            if (endFlagIdx < 0)
            {
                return chunk;
            }

            // Get parameters
            var paramStartIdx = startFlagIdx + StartFlag.Length;
            var paramEndIdx = src.IndexOf("\n", paramStartIdx);
            chunk.Param = src.Substring(paramStartIdx, paramEndIdx - paramStartIdx).Trim();

            // Get chunk
            chunk.StartIdx = paramEndIdx;
            chunk.EndIdx = endFlagIdx;
            chunk.Text = src.Substring(chunk.StartIdx, chunk.EndIdx - chunk.StartIdx);

            // Add trailing semicolon for function declarations
            chunk.Text += ";\n";

            chunk.NextIdx = endFlagIdx + EndFlag.Length;
            chunk.Valid = true;

            return chunk;
        }
    }
}
