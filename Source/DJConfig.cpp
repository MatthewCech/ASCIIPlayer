#include <sstream>
#include <cstring>
#include "Defines.hpp"
#include "DJConfig.hpp"

#define NAME_STR_LINE(var) #var << ": " << var << "\n"
#define PARSE_CONTINUE(var, toCompare, func) if(toCompare == #var) { var = func ; break; }

namespace ASCIIPlayer
{
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
        PARSE_CONTINUE(DJLooping, name, stoi(val) > 0 ? true : false)
        PARSE_CONTINUE(VolumeChangeAmount, name, stof(val))
        PARSE_CONTINUE(VolumeDefault, name, stof(val))
        PARSE_CONTINUE(SkipForwardSeconds, name, stoi(val))
        PARSE_CONTINUE(SkipBackwardSeconds, name, stoi(val))
        PARSE_CONTINUE(DJChannelCount, name, stoi(val))
        PARSE_CONTINUE(DJVisualizerID, name, val)
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
    ss << NAME_STR_LINE(DJLooping)
       << NAME_STR_LINE(VolumeDefault)
       << NAME_STR_LINE(SkipForwardSeconds)
       << NAME_STR_LINE(SkipBackwardSeconds)
       << NAME_STR_LINE(DJChannelCount)
       << NAME_STR_LINE(VolumeChangeAmount)
       << NAME_STR_LINE(DJVisualizerID)
			 << "Available Visualizers - default horizontalWaveform colorDefault centerVisualizer\n";
    return ss.str();
  }
}
