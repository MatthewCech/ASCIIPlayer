/*!***************************************************************************
@file    RTest.hpp
@author  Reverie Wisp
@par     Contact: reveriewisp@gmail.com
@date    6/4/2016

@brief
Super casual testing and debug tools to just throw in a project and use.
File contains basic timekeeping class and basic exception structure. Quickly
smashed the implementation into the header.
*****************************************************************************/
#pragma once
#include <string>
#include <ostream>
#include <chrono>



//Defines for asserts doing floating point math
#define RTest_NEAR_FLOAT  .0006
#define RTest_NEAR_DOUBLE .00006

//Turns on or off an assert. If there is an assert that fails, a custom exception type is thrown. 
//Place the following line in your project or compile with the following defined: #define RTest_ASSERT_ON
#ifndef RTest_ASSERT_OFF                                       
  #define RTest_ASSERT(a) do{ if(!(a)) { throw(RTest::RException("Assert Failed!")); } } while (0)                                                  
#endif






//Prototypes:
namespace RTest
{
  //A class for timing frames.
  class Timekeeper
  {
  public:
    //Static member functions
    static void StartFrame();
    static void EndFrame();
    static int GetLastTimeMS();
    static int GetAvgTimeMS();
    static void SetMaxSamples(unsigned int samples);

  private:
    //Frame time markers
    static std::chrono::milliseconds startTime_;
    static std::chrono::milliseconds endTime_;

    //Other data for averaging
    static double frameSamples_;
    static double frameAvg_;
    static long long lastTime_;
    static unsigned int maxFrameSamples_;
  };


  //Undefine the exception structure first to avoid expanding the macro.
  #undef RException
  struct RException
  {
    //Constructor
    RException(std::string file, unsigned int line, std::string message);

    //Member functions and Friends
    friend std::ostream &operator<<(std::ostream &os, const RException &rhs);

    //Data
    std::string File;
    unsigned int Line;
    std::string Message;
  };
  //Redefine macro for the exception
  #define RException(a) RException(__FILE__, __LINE__, a)
}






//Implementation:
namespace RTest
{
    /////////////////////////////////
   //Testing functions for asserts//
  /////////////////////////////////
  bool Near(double a, double b)
  {
    if (a + RTest_NEAR_DOUBLE > b && a - RTest_NEAR_DOUBLE < b)
      return true;
    return false;
  }

  bool Near(float a, float b)
  {
    if (a + RTest_NEAR_FLOAT > b && a - RTest_NEAR_FLOAT < b)
      return true;
    return false;
  }



    ////////////////////
   //Timekeeper Class//
  ////////////////////
  //Static time initialization
  std::chrono::milliseconds Timekeeper::startTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
  std::chrono::milliseconds Timekeeper::endTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
  double Timekeeper::frameSamples_ = 0;
  double Timekeeper::frameAvg_ = 0;
  long long Timekeeper::lastTime_ = 0;
  unsigned int Timekeeper::maxFrameSamples_ = 50;


  //Start frame marker. Should be called at the start of a single cycle of the program.
  void Timekeeper::StartFrame()
  {
    startTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch());
  }


  //End frame marker. Should be called at the end of a single cycle of the program.
  void Timekeeper::EndFrame()
  {
    endTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch());

    lastTime_ = (endTime_ - startTime_).count();
    frameAvg_ = (frameSamples_ * frameAvg_ + lastTime_) / static_cast<double>(frameSamples_ + 1);
    if (frameSamples_ < maxFrameSamples_)
      frameSamples_ += 1;
  }


  //Get the last frame time in MS as an int.
  int Timekeeper::GetLastTimeMS()
  {
    return static_cast<int>(lastTime_);
  }


  //Returns the average frame time that has been calculated.
  int Timekeeper::GetAvgTimeMS()
  {
    return static_cast<int>(frameAvg_);
  }


  //Sets the total number of previous frames to take into account when averaging.
  void Timekeeper::SetMaxSamples(unsigned int samples)
  {
    maxFrameSamples_ = samples;
    frameSamples_ = 0;
  }



    //////////////
   //RException//
  //////////////
  //Undefine the RException macro to prevent it expanding here.
  #undef RException

  //Handle constructor
  RException::RException(std::string file, unsigned int line, std::string message)
  : File(file)
  , Line(line)
  , Message(message)
  {  }

  //Define print formatting 
  std::ostream &operator<<(std::ostream &os, const RException &rhs)
  {
    os << "!! EXCEPTION: " << rhs.File << " line " << rhs.Line << ": " << rhs.Message;
    return os;
  }
  
  //Redefube macro to allow it to expand here.
  #define RException(a) RException(__FILE__, __LINE__, a)

}
