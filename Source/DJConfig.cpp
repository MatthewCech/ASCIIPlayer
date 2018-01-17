#include <sstream>
#include <cstring>
#include "Defines.hpp"
#include "DJConfig.hpp"

#define NAME_STR_LINE(var) #var << ": " << var << "\n"
#define PARSE_CONTINUE(var, toCompare, func) if(toCompare == #var) { var = func ; break; }

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
        PARSE_CONTINUE(DJLooping, name, stoi(val) > 0 ? true : false)
        PARSE_CONTINUE(VolumeDefault, name, stof(val))
        PARSE_CONTINUE(VolumeChangeAmount, name, stof(val))
        PARSE_CONTINUE(SkipForwardSeconds, name, stoi(val))
        PARSE_CONTINUE(SkipBackwardSeconds, name, stoi(val))
        PARSE_CONTINUE(DJChannelCount, name, stoi(val))
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
    ss << NAME_STR_LINE(DJLooping)
       << NAME_STR_LINE(VolumeDefault)
       << NAME_STR_LINE(VolumeChangeAmount)
       << NAME_STR_LINE(SkipForwardSeconds)
       << NAME_STR_LINE(SkipBackwardSeconds)
       << NAME_STR_LINE(DJChannelCount)
       << NAME_STR_LINE(DJVisualizerID)
       << NAME_STR_LINE(DJOverlayID)
			 << "Available Visualizers - default horizontalWaveform colorDefault centerVisualizer\n";
    return ss.str();
  }
}
