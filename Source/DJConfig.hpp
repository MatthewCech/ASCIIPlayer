// Read from a file, designed to configure the DJ.
#pragma once
#include <string>



namespace ASCIIPlayer
{
  struct DJConfig
  {
    // Basic
    float        DefaultVolume = 1.0; // Default volume for the DJ to play at.
    unsigned int   ForwardSkip = 20; // Seconds to go forwards when skipping. 
    unsigned int BackwardsSkip = 20; // Seconds to go backwards when skipping.
    char          VisualizerID = 0; // ID of the visualizer to use.
    //std::string DefaultFolder; // Default folder to load music from.

    // Advanced
    unsigned int ChannelCount = 10; // The number of channels for the DJ to have.
  };
}
