#include <iostream>
#include "RTest.hpp"



int main(int argc, char** argv) try
{
  // Parse command line. This will have both song names and flags to be interpreted. Consider getopt or similar for cross-platform stuff.
  // Parse config file. Just reinterpret_cast the data to the DJConfig structure.
  // Construct DJ object. One-liner.
  // Feed DJ songs from command line or folder that command line specifies (note: NOT DJ config, but existing logic)
  // Start Lobby - Blocking IO is totally good here.
    //// == Inside Lobby == ////
    // While loop for DJ updating //while(DJ.update()) or something.
    // Input Handler Update -- probably offload input parsing to another function? Nonblocking IO here.
    // Update DJ, which by extension gets everything.

  return 0;
}
//Catch and dump any errors.
catch (RTest::RException e)
{
  std::cerr << e << std::endl;
  std::cout << e << std::endl;
  throw e;
}