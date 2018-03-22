#include <sstream>
#include <cstring>
#include <FileIO/FileIO.hpp>
#include "Defines.hpp"
#include "DJConfig.hpp"

#define NAME_STR_LINE(var) #var << ": " << var << "\n"
#define PARSE_CONTINUE(var, toCompare, func) if(toCompare == #var) { var = func ; break; }
#define CONFIG_FILE_NAME "ASCIIPlayer.conf"


namespace ASCIIPlayer
{
  std::string TrimWhitespace(std::string input)
  {
    while (input.length() > 0 && input[0] == ' ' || input[0] == '\t')
      input = input.substr(1);

    while (input.length() > 0 && input[input.length() - 1] == ' ' || input[input.length() - 1] == '\t')
      input = input.substr(0, input.length() - 1);

    return input;
  }

  // Attempts to parse a line of a file.
  void DJConfig::ParseLine(std::string line)
  {
    std::string name;
    std::string val;

    size_t pos = line.find_first_of(":");
    if (pos == std::string::npos)
      return;

    name = line.substr(0, pos);
    val = line.substr(pos + 1);

    try
    {
      do {
        // Floats
        PARSE_CONTINUE(VolumeDefault, name, stof(val))
        PARSE_CONTINUE(VolumeChangeAmount, name, stof(val))

        // Bools
        PARSE_CONTINUE(DJLooping, name, stoi(val) > 0 ? true : false)
        PARSE_CONTINUE(DJOverlayAlwaysOn, name, stoi(val) > 0 ? true : false)

        // Ints
        PARSE_CONTINUE(SkipForwardSeconds, name, stoi(val))
        PARSE_CONTINUE(SkipBackwardSeconds, name, stoi(val))
        PARSE_CONTINUE(DJChannelCount, name, stoi(val))

        // Strings
        PARSE_CONTINUE(DJVisualizerID, name, TrimWhitespace(val))
        PARSE_CONTINUE(DJOverlayID, name, TrimWhitespace(val))
      } while (false);
    }
    catch (...)
    {
      DEBUG_PRINT("Error parsing value from line for config.");
    }
  }


  // Converts object to string.
  std::string DJConfig::ToString()
  {
    std::stringstream ss;
    ss 
       // Floats
       << NAME_STR_LINE(VolumeDefault)
       << NAME_STR_LINE(VolumeChangeAmount)

       // Bools
       << NAME_STR_LINE(DJLooping)
       << NAME_STR_LINE(DJOverlayAlwaysOn)

       // Ints
       << NAME_STR_LINE(SkipForwardSeconds)
       << NAME_STR_LINE(SkipBackwardSeconds)
       << NAME_STR_LINE(DJChannelCount)

       // Strings
       << NAME_STR_LINE(DJVisualizerID)
       << NAME_STR_LINE(DJOverlayID)

       // Etc.
       << "\n\n"
       << "===[Notes]===\n"
			 << "Available Visualizers - default waveform wisp\n";
    return ss.str();
  }

  // Tries to open config file for the visualizer, generates one otherwise.
  // Uses the specified path to do so.
  DJConfig DJConfig::Read(std::string path)
  {
    std::string arg0 = path;
    size_t loc = arg0.find_last_of('\\');
    std::string filepath = "";
    if (loc != std::string::npos)
      filepath = arg0.substr(0, loc);

    FileUtil::File f(filepath + "\\" + CONFIG_FILE_NAME);

    // If there is no file...
    if (f.GetLineCount() == 0)
    {
      DJConfig def;
      std::string str = def.ToString();
      f.Write(str);
      f.Save();
      return def;
    }
    else
    {
      DJConfig newConf;
      for (unsigned int i = 0; i < f.GetLineCount(); ++i)
        newConf.ParseLine(f[i]);
      return newConf;
    }
  }


  void DJConfig::Write(DJConfig d, std::string path)
  {
    std::string arg0 = path;
    size_t loc = arg0.find_last_of('\\');
    std::string filepath = "";
    if (loc != std::string::npos)
      filepath = arg0.substr(0, loc);

    // If there is no line count, we couldn't find the file, so just write it.
    // We really don't care about hosing what's there if we're given the order
    // to write at this point and it's valid.
    FileUtil::File f(filepath + "\\" + CONFIG_FILE_NAME);
    f.Clear();
    f.Write(d.ToString());
    f.Save();
  }
}
