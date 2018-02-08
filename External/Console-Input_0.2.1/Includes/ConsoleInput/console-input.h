/*!**********************************************************************
@file    console-input.h
@author  Matthew C.
@author  JohannesMP
@author  R.W.

@date    2/8/2018

@brief
A wrapper allowing _getch and _kbhit windows-style functionality on
both windows and linux! Relevant functions are KeyHit() and GetKey().

Includes some utilities for reading paths vs single key inputs as well.
Depending on C vs C++ usage, either the InputParser class will be 
available for C++, or for C there is a HandleInput method. All take
callbacks to functions, passing the input as acquired.

@copyright (See LICENSE.md)
************************************************************************/
// Ease of use OS specific defines for compiling
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS) || defined(_WIN32_)
#define OS_WINDOWS
#else
#define OS_NON_WINDOWS
#endif
#ifdef __cplusplus
#define LANGUAGE_CPP
#else
#define LANGUAGE_C
#endif


/////////////////////
// Keycode Defines //
/////////////////////
// Accuracy not guarenteed. Use ASCII characters when possible instead.
// Keycodes based on ASCII Values, and ASCII Scan codes.
// http://www.asciitable.com/
//
// Provided main can be used to test as well.

// Main board keys
#define KEY_BACKSPACE 8
#define KEY_TAB       9
#define KEY_ENTER     13
#define KEY_RETURN    13
#define KEY_ESCAPE    27
#define KEY_SPACE     32

// Arrow Keys
#define KEY_LEFT      75 // WARNING: Also capital K
#define KEY_UP        72 // WARNING: Also capital H
#define KEY_DOWN      80 // WARNING: Also capital P
#define KEY_RIGHT     77 // WARNING: Also capital M

// Utility Keys:
#define KEY_INSERT    82 // WARNING: Also capital R
#define KEY_DELETE    83 // WARNING: Also capital S
#define KEY_HOME      71 // WARNING: Also capital G
#define KEY_END       79 // WARNING: Also captial O
#define KEY_PAGEUP    73 // WARNING: Also capital I
#define KEY_PAGEDOWN  81 // WARNING: Also captial Q

// Could not identify
//#define KEY_PRINT_SCREEN 43, when CTRL is pressed it should be 114
//#define KEY_SCROLL_LOCK  ??
//#define KEY_PAUSE        ??
//#define KEY_BREAK <would be same as pause>

// Number Input (both numpad and primary keyboard)
#define KEY_0         48
#define KEY_1         49
#define KEY_2         50
#define KEY_3         51
#define KEY_4         52
#define KEY_5         53
#define KEY_6         54
#define KEY_7         55
#define KEY_8         56
#define KEY_9         57
#define KEY_NUM_0     48
#define KEY_NUM_1     49
#define KEY_NUM_2     50
#define KEY_NUM_3     51
#define KEY_NUM_4     52
#define KEY_NUM_5     53
#define KEY_NUM_6     54
#define KEY_NUM_7     55
#define KEY_NUM_8     56
#define KEY_NUM_9     57





/////////////////////////
// Function Prototypes //
/////////////////////////
// Checks to see if a key was hit in the terminal.
// Returns truthy if a change was detected in the input
// queue (if a key was hit), falsy if not.
inline int KeyHit(void);

// Gets the last character changed in the terminal.
// Returns the value of the last character changed.
inline int GetChar(void);





////////////////////////////
// Windows Implementation //
////////////////////////////
#ifdef OS_WINDOWS
#define _NO_OLDNAMES   // for MinGW
#include <conio.h>     // getch and kbhit


// standard kbhit, returns if character change is queued.
inline int KeyHit(void) { return _kbhit(); }

// Uses getch as a sandard, supporting commonly typed console characters.
// Use wch to handle additional cases if you wish, tho know it changes codes.
inline int GetChar(void) { return _getch(); }


// C++ Specific additional functionality
#ifdef LANGUAGE_CPP
#include <string>     // std::string
#include <functional> // std::function

// Drag-in parser 
class InputParser
{
public:
  // Handle the input parsing and separation. This requires 
  // references as inputs for keeping track of keypresses.
  void HandleInput(std::function<void(char)> callbackSingleChar, std::function<void(std::string) > callbackFilepath)
  {
    int hit = KeyHit();
    
    // If there was a hit key, keep track of it.
    // If a combination shows up, handle that as well.
    if (hit)
      do
      {
        lastChar_ = GetChar();
        buffer_ += lastChar_;
      } while (hit = KeyHit());
    
    // If there is currently not a hit key but there was one 
    // last cycle when we checked...
    else
      if (lastChar_ != NoInput)
      {
        if (buffer_.size() > 1)
          callbackFilepath(buffer_);
        else
          callbackSingleChar(lastChar_);

        lastChar_ = NoInput;
        buffer_.clear();
      }
  }

  // A member-function supported version of the previous function.
  // Uses the same variables as the other HandleInput function.
  template <class T> void HandleInput(T *thisClass, void(T::*callbackSingleChar)(char), void(T::*callbackFilepath)(std::string))
  {
    int hit = KeyHit();
    if (hit)
      do
      {
        lastChar_ = GetChar();
        buffer_ += lastChar_;
      } while (hit = KeyHit());
    else
      if (lastChar_ != NoInput)
      {
        if (buffer_.size() > 1)
          (*thisClass.*callbackFilepath)(buffer_);
        else
          (*thisClass.*callbackSingleChar)(lastChar_);

        lastChar_ = NoInput;
        buffer_.clear();
      }
  }


private:
  // Variables
  const int NoInput = 0;    // A constant for defining a lack of input. 
  int lastChar_ = NoInput;  // The last character we read. NoInput by default.
  std::string buffer_ = ""; // So long as we recieve input without a break, we continue to store it here.

};
#endif // LANGUAGE_CPP

// Additional functionality for C
#ifdef LANGUAGE_C

// Global internal variables for C
int ci_internal_last_char_ = 0;
int ci_internal_buffer_pos_ = 0;
const int buffer_max = 255;
char ci_internal_buffer_[buffer_max] = {};

// Callback functions specified as necessary.
void HandleInput(void(callbackSingleChar)(char), void(callbackFilepath)(const char *, int))
{
  int hit = KeyHit();

  // If there was a hit key, keep track of it.
  // If a combination shows up, handle that as well.
  if (hit)
    do
    {
      ci_internal_last_char_ = GetChar();
      ci_internal_buffer_[ci_internal_buffer_pos_] = (char)ci_internal_last_char_;

      if (ci_internal_buffer_pos_ < buffer_max)
        ++ci_internal_buffer_pos_;

    } while (hit = KeyHit());

  // If there is currently not a hit key but there was one 
  // last cycle when we checked...
  else
    if (ci_internal_last_char_ != 0)
    {
      if (ci_internal_buffer_pos_ > 1)
        callbackFilepath(ci_internal_buffer_, ci_internal_buffer_pos_);
      else
        callbackSingleChar(ci_internal_last_char_);

      ci_internal_last_char_ = 0;
      ci_internal_buffer_pos_ = 0;
      memset(ci_internal_buffer_, 0, buffer_max);
    }
}
#endif // LANGUAGE_C
#endif // OS_WINDOWS





////////////////////////////////
// Non-Windows Implementation //
////////////////////////////////
#ifdef OS_NON_WINDOWS

// System includes, mostly kbhit emulation
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>

// GetCH emulation + time
#include <stdio.h>
#include <termios.h>
#include <unistd.h>



// termios reference: http://man7.org/linux/man-pages/man3/termios.3.html
// More readable termios reference: https://www.mkssoftware.com/docs/man5/struct_termios.5.asp
// select reference: http://man7.org/linux/man-pages/man2/select.2.html
inline int KeyHit(void)
{
  // Recall: Define variables at the top for C
  static struct termios oldTermios; // Save off for previous terminal settings
  static struct termios newTermios; // Configured new terminal settings
  struct timeval tv;                // Timeval struct for small delays.
  int charCount = 0;                // Character count

  // Set up the console.
  tcgetattr(STDIN_FILENO, &oldTermios);           // Get old settings
  newTermios = oldTermios;                        // Transfer previous settings
  newTermios.c_oflag = 0;                         // Output mode
  newTermios.c_lflag &= ~(ICANON | ECHO);         // Jupt straight to the queue and don't echo.
  newTermios.c_iflag = 0;                         // Input mode
  newTermios.c_cc[VMIN] = 1;                      // Minimum time to wait
  newTermios.c_cc[VTIME] = 1;                     // Minimum characters to wait for
  tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);  // Set newTermios

  // Not checking return values, because if you're having issues with 
  // accessing the stack or reading information from the stream, you
  // likely have bigger issues than this function not working.
  ioctl(0, FIONREAD, &charCount);

  // Check file stream with a small delay, then return to previous state.
  tv.tv_sec = 0;     // time to delay, in Seconds.
  tv.tv_usec = 100;  // time to delay, in Microseconds
  select(STDIN_FILENO + 1, NULL, NULL, NULL, &tv);
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
  return charCount;
}

// Makes use of the getchar function. Effectively getc(stdin), 
// with quick tweaks to the terminal to prevent oops.
// getchar documentation: https://linux.die.net/man/3/getchar
// If called without a character waiting for reading, 
// you should recieve EOF.
inline int GetChar(void)
{
  // Recall: Define variables at the top for C
  struct termios oldTermios; // Save off for previous terminal settings
  struct termios newTermios; // Configured new terminal settings
  int charVal;               // The key value (as int) from stream.

  // Configure newTermios
  tcgetattr(STDIN_FILENO, &oldTermios);
  newTermios = oldTermios;
  newTermios.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

  // Reset terminal and return
  charVal = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
  return charVal;
}

#endif // OS_NON_WINDOWS

