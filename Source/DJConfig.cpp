#include <sstream>
#include <cstring>
#include "Defines.hpp"
#include "DJConfig.hpp"

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
      if (name == "DefaultVolume")
        DefaultVolume = stof(val);
      else if (name == "ForwardSkip")
        ForwardSkip = stoi(val);
      else if (name == "BackwardsSkip")
        BackwardsSkip = stoi(val);
      else if (name == "Looping")
        Looping = stoi(val) > 0 ? true : false;
      else if (name == "ChannelCount")
        ChannelCount = stoi(val);
      else if (name == "VisualizerID")
        VisualizerID = val;
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
    ss << "DefaultVolume:" << DefaultVolume << "\n"
      << "ForwardSkip:" << ForwardSkip << "\n"
      << "BackwardsSkip:" << BackwardsSkip << "\n"
      << "Looping:" << Looping << "\n"
      << "ChannelCount:" << ChannelCount << "\n"
      << "VisualizerID:" << VisualizerID << "\n";

    return ss.str();
  }
}
