// Read from a file, designed to configure the DJ.
#pragma once
#include <string>


namespace ASCIIPlayer
{
  struct DJConfig
  {
    // Basic
    float        VolumeDefault           = 0.9f;       // Default volume for the DJ to play at.
    float        VolumeChangeAmount      = 0.1f;       // By how much we change the volume every time.
    bool         DJLooping               = true;       // Does it loop?
    bool         DJOverlayAlwaysOn       = false;      // A request for having the UI on all the time.
    unsigned int DJPerLoopSleepMS        = 0;          // Specifies number of milliseconds to sleep each loop while not paused. Not more granular because it's just not worth it, and may as well use 'yield' at that point (which we do)
    unsigned int SkipForwardSeconds      = 15;         // Seconds to go forwards when skipping. 
    unsigned int SkipBackwardSeconds     = 15;         // Seconds to go backwards when skipping.
    unsigned int DJChannelCount          = 10;         // The number of channels for the DJ to have.
    std::string  DJVisualizerID          = "spectrum"; // ID of the visualizer to use.
    std::string  DJOverlayID             = "default";  // ID of the info overlay to use.

    //std::string DefaultFolder; // Default folder to load music from.

    // Public member functions for config management
    void ParseLine(std::string line);
    std::string ToString();

    // Static member funcs
    static DJConfig ReadString(std::string string);
    static DJConfig ReadFile(std::string path);
    static void Write(DJConfig d, std::string path);
  };
}
