#include <iostream>
#include <FileIO/FileIO.hpp>
#include "Lobby.hpp"



int main(int argc, char** argv) try
{
  // Stop logging
  FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_NONE);

  // Run program
  ASCIIPlayer::Lobby lounge(argc, argv);
  lounge.Run();

  // Return normally
  return 0;
}
//Catch and dump any errors.
catch (std::exception &e)
{
  std::cerr << e.what() << std::endl;
  std::cout << e.what() << std::endl;
  throw e;
}