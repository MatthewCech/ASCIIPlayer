//Read from a file, designed to configure the DJ.
#pragma once
#include <string>



namespace ASCIIPlayer
{
  struct DJConfig
  {
    float       DefaultVolume; //Default volume for the DJ to play at.
    int           ForwardSkip; //Seconds to go forwards when skipping. 
    int         BackwardsSkip; //Seconds to go backwards when skipping.
    char         VisualizerID; //ID of the visualizer to use.
    //std::string DefaultFolder; //Default folder to load music from.
  };
}
