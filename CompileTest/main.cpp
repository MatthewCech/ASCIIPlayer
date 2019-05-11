#include <iostream>
#include "fmod.h"

const char* MakeAudioSystem() 
{
  FMOD_SYSTEM *system = nullptr;
  FMOD_System_Create(&system);
  return "It did a thing!";
}

int main(int argc, char** argv)
{
  std::cout << MakeAudioSystem() << std::endl;
  return 0;
}

