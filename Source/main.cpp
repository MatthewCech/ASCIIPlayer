#include <iostream>
#include <FileIO/fileio.hpp>
#include "Core/Lobby.hpp"



namespace ASCIIPlayer
{
  int Run(int argc, char** argv)
  {
    // Run program
    ASCIIPlayer::Lobby lounge(argc, argv);
    lounge.Run();

    // Return normally
    return 0;
  }
}

// Run main, catching and dumping any errors.
int main(int argc, char** argv) try
{
  return ASCIIPlayer::Run(argc, argv);
}
catch (std::exception &e)
{
  std::cerr << e.what() << std::endl;
  std::cout << e.what() << std::endl;
  throw e;
}
