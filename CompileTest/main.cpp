#include <iostream>
#include "fmod.hpp"

const char* MakeAudioSystem() 
{
  FMOD::System *system;
  FMOD::System_Create(&system);
  return "It did a thing!";
}

int main(int argc, char** argv)
{
  std::cout << MakeAudioSystem() << std::endl;
  return 0;
}

