/*!***************************************************************************
@file    RTest.hpp
@author  Reverie Wisp
@par     Contact: reveriewisp@gmail.com
@date    6/4/2016

@brief
Super casual testing and debug tools to just throw in a project and use.
File contains basic timekeeping class and basic exception structure.
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
