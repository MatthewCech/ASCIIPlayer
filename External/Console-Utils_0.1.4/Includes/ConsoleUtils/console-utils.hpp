// This is a combination header file that accounts for all sorts of console drawing and whatnot,
// expanding functionality and ease of use of rlutil.h by Tapio Vierros.
//
// Version: 1.4
// Date: Nov. 2016
//
// Console Utils Implementation by:
// Reverie Wisp
// JoShadow



///////////////////////////////////////////////////////////////////////
//rlutil.h
///////////////////////////////////////////////////////////////////////
#pragma once
/**
 * File: rlutil.h
 *
 * About: Description
 * This file provides some useful utilities for console mode
 * roguelike game development with C and C++. It is aimed to
 * be cross-platform (at least Windows and Linux).
 *
 * About: Copyright
 * (C) 2010 Tapio Vierros
 *
 * About: Licensing (DWTFYW)
 * See <License>    (DWTFYW)
 *
 * Minor tweaks to this specific file: 2016 Reverie Wisp
 */


/// Define: RLUTIL_USE_ANSI
/// Define this to use ANSI escape sequences also on Windows
/// (defaults to using WinAPI instead).
#if 0
#define RLUTIL_USE_ANSI
#endif

/// Define: RLUTIL_STRING_T
/// Define/typedef this to your preference to override rlutil's string type.
///
/// Defaults to std::string with C++ and char* with C.
#if 0
#define RLUTIL_STRING_T char*
#endif

#ifdef __cplusplus
	/// Common C++ headers
	#include <iostream>
	#include <string>
	#include <sstream>
	/// Namespace forward declarations
	namespace rlutil {
		void locate(int x, int y);
	}
#else
	void locate(int x, int y); // Forward declare for C to avoid warnings
#endif // __cplusplus

#ifndef RLUTIL_INLINE
	#ifdef _MSC_VER
		#define RLUTIL_INLINE __inline
	#else
		#define RLUTIL_INLINE __inline__
	#endif
#endif

#ifdef _WIN32
	#include <windows.h>  // for WinAPI and Sleep()
	#define _NO_OLDNAMES  // for MinGW compatibility
	#include <conio.h>    // for getch() and kbhit()
	#define getch _getch
	#define kbhit _kbhit
#else
	#ifdef __cplusplus
		#include <cstdio> // for getch()
	#else // __cplusplus
		#include <stdio.h> // for getch()
	#endif // __cplusplus
	#include <termios.h> // for getch() and kbhit()
	#include <unistd.h> // for getch(), kbhit() and (u)sleep()
	#include <sys/ioctl.h> // for getkey()
	#include <sys/types.h> // for kbhit()
	#include <sys/time.h> // for kbhit()

/// Function: getch
/// Get character without waiting for Return to be pressed.
/// Windows has this in conio.h
RLUTIL_INLINE int getch(void) {
	// Here be magic.
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

/// Function: kbhit
/// Determines if keyboard has been hit.
/// Windows has this in conio.h
RLUTIL_INLINE int kbhit(void) {
	// Here be dragons.
	static struct termios oldt, newt;
	int cnt = 0;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag    &= ~(ICANON | ECHO);
	newt.c_iflag     = 0; // input mode
	newt.c_oflag     = 0; // output mode
	newt.c_cc[VMIN]  = 1; // minimum time to wait
	newt.c_cc[VTIME] = 1; // minimum characters to wait for
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ioctl(0, FIONREAD, &cnt); // Read count
	struct timeval tv;
	tv.tv_sec  = 0;
	tv.tv_usec = 100;
	select(STDIN_FILENO+1, NULL, NULL, NULL, &tv); // A small time delay
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return cnt; // Return number of characters
}
#endif // _WIN32

#ifndef gotoxy
/// Function: gotoxy
/// Same as <rlutil.locate>.
RLUTIL_INLINE void gotoxy(int x, int y) {
	#ifdef __cplusplus
	rlutil::
	#endif
	locate(x,y);
}
#endif // gotoxy

#ifdef __cplusplus
/// Namespace: rlutil
/// In C++ all functions except <getch>, <kbhit> and <gotoxy> are arranged
/// under namespace rlutil. That is because some platforms have them defined
/// outside of rlutil.
namespace rlutil {
#endif

/**
 * Defs: Internal typedefs and macros
 * RLUTIL_STRING_T - String type depending on which one of C or C++ is used
 * RLUTIL_PRINT(str) - Printing macro independent of C/C++
 */

#ifdef __cplusplus
	#ifndef RLUTIL_STRING_T
		typedef std::string RLUTIL_STRING_T;
	#endif // RLUTIL_STRING_T

	inline void RLUTIL_PRINT(RLUTIL_STRING_T st) { std::cout << st; }

#else // __cplusplus
	#ifndef RLUTIL_STRING_T
		typedef char* RLUTIL_STRING_T;
	#endif // RLUTIL_STRING_T

	#define RLUTIL_PRINT(st) printf("%s", st)
#endif // __cplusplus

/**
 * Enums: Color codes
 *
 * BLACK - Black
 * BLUE - Blue
 * GREEN - Green
 * CYAN - Cyan
 * RED - Red
 * MAGENTA - Magenta / purple
 * BROWN - Brown / dark yellow
 * GREY - Grey / dark white
 * DARKGREY - Dark grey / light black
 * LIGHTBLUE - Light blue
 * LIGHTGREEN - Light green
 * LIGHTCYAN - Light cyan
 * LIGHTRED - Light red
 * LIGHTMAGENTA - Light magenta / light purple
 * YELLOW - Yellow (bright)
 * WHITE - White (bright)
 */
enum {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	GREY,
	DARKGREY,
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};

/**
 * Consts: ANSI color strings
 *
 * ANSI_CLS - Clears screen
 * ANSI_BLACK - Black
 * ANSI_RED - Red
 * ANSI_GREEN - Green
 * ANSI_BROWN - Brown / dark yellow
 * ANSI_BLUE - Blue
 * ANSI_MAGENTA - Magenta / purple
 * ANSI_CYAN - Cyan
 * ANSI_GREY - Grey / dark white
 * ANSI_DARKGREY - Dark grey / light black
 * ANSI_LIGHTRED - Light red
 * ANSI_LIGHTGREEN - Light green
 * ANSI_YELLOW - Yellow (bright)
 * ANSI_LIGHTBLUE - Light blue
 * ANSI_LIGHTMAGENTA - Light magenta / light purple
 * ANSI_LIGHTCYAN - Light cyan
 * ANSI_WHITE - White (bright)
 */
const RLUTIL_STRING_T ANSI_CLS = "\033[2J";
const RLUTIL_STRING_T ANSI_BLACK = "\033[22;30m";
const RLUTIL_STRING_T ANSI_RED = "\033[22;31m";
const RLUTIL_STRING_T ANSI_GREEN = "\033[22;32m";
const RLUTIL_STRING_T ANSI_BROWN = "\033[22;33m";
const RLUTIL_STRING_T ANSI_BLUE = "\033[22;34m";
const RLUTIL_STRING_T ANSI_MAGENTA = "\033[22;35m";
const RLUTIL_STRING_T ANSI_CYAN = "\033[22;36m";
const RLUTIL_STRING_T ANSI_GREY = "\033[22;37m";
const RLUTIL_STRING_T ANSI_DARKGREY = "\033[01;30m";
const RLUTIL_STRING_T ANSI_LIGHTRED = "\033[01;31m";
const RLUTIL_STRING_T ANSI_LIGHTGREEN = "\033[01;32m";
const RLUTIL_STRING_T ANSI_YELLOW = "\033[01;33m";
const RLUTIL_STRING_T ANSI_LIGHTBLUE = "\033[01;34m";
const RLUTIL_STRING_T ANSI_LIGHTMAGENTA = "\033[01;35m";
const RLUTIL_STRING_T ANSI_LIGHTCYAN = "\033[01;36m";
const RLUTIL_STRING_T ANSI_WHITE = "\033[01;37m";

/**
 * Consts: Key codes for keyhit()
 *
 * KEY_ESCAPE  - Escape
 * KEY_ENTER   - Enter
 * KEY_SPACE   - Space
 * KEY_INSERT  - Insert
 * KEY_HOME    - Home
 * KEY_END     - End
 * KEY_DELETE  - Delete
 * KEY_PGUP    - PageUp
 * KEY_PGDOWN  - PageDown
 * KEY_UP      - Up arrow
 * KEY_DOWN    - Down arrow
 * KEY_LEFT    - Left arrow
 * KEY_RIGHT   - Right arrow
 * KEY_F1      - F1
 * KEY_F2      - F2
 * KEY_F3      - F3
 * KEY_F4      - F4
 * KEY_F5      - F5
 * KEY_F6      - F6
 * KEY_F7      - F7
 * KEY_F8      - F8
 * KEY_F9      - F9
 * KEY_F10     - F10
 * KEY_F11     - F11
 * KEY_F12     - F12
 * KEY_NUMDEL  - Numpad del
 * KEY_NUMPAD0 - Numpad 0
 * KEY_NUMPAD1 - Numpad 1
 * KEY_NUMPAD2 - Numpad 2
 * KEY_NUMPAD3 - Numpad 3
 * KEY_NUMPAD4 - Numpad 4
 * KEY_NUMPAD5 - Numpad 5
 * KEY_NUMPAD6 - Numpad 6
 * KEY_NUMPAD7 - Numpad 7
 * KEY_NUMPAD8 - Numpad 8
 * KEY_NUMPAD9 - Numpad 9
 */
const int KEY_ESCAPE  = 0;
const int KEY_ENTER   = 1;
const int KEY_SPACE   = 32;

const int KEY_INSERT  = 2;
const int KEY_HOME    = 3;
const int KEY_PGUP    = 4;
const int KEY_DELETE  = 5;
const int KEY_END     = 6;
const int KEY_PGDOWN  = 7;

const int KEY_UP      = 14;
const int KEY_DOWN    = 15;
const int KEY_LEFT    = 16;
const int KEY_RIGHT   = 17;

const int KEY_F1      = 18;
const int KEY_F2      = 19;
const int KEY_F3      = 20;
const int KEY_F4      = 21;
const int KEY_F5      = 22;
const int KEY_F6      = 23;
const int KEY_F7      = 24;
const int KEY_F8      = 25;
const int KEY_F9      = 26;
const int KEY_F10     = 27;
const int KEY_F11     = 28;
const int KEY_F12     = 29;

const int KEY_NUMDEL  = 30;
const int KEY_NUMPAD0 = 31;
const int KEY_NUMPAD1 = 127;
const int KEY_NUMPAD2 = 128;
const int KEY_NUMPAD3 = 129;
const int KEY_NUMPAD4 = 130;
const int KEY_NUMPAD5 = 131;
const int KEY_NUMPAD6 = 132;
const int KEY_NUMPAD7 = 133;
const int KEY_NUMPAD8 = 134;
const int KEY_NUMPAD9 = 135;

/// Function: getkey
/// Reads a key press (blocking) and returns a key code.
///
/// See <Key codes for keyhit()>
///
/// Note:
/// Only Arrows, Esc, Enter and Space are currently working properly.
RLUTIL_INLINE int getkey(void) {
	#ifndef _WIN32
	int cnt = kbhit(); // for ANSI escapes processing
	#endif
	int k = getch();
	switch(k) {
		case 0: {
			int kk;
			switch (kk = getch()) {
				case 71: return KEY_NUMPAD7;
				case 72: return KEY_NUMPAD8;
				case 73: return KEY_NUMPAD9;
				case 75: return KEY_NUMPAD4;
				case 77: return KEY_NUMPAD6;
				case 79: return KEY_NUMPAD1;
				case 80: return KEY_NUMPAD4;
				case 81: return KEY_NUMPAD3;
				case 82: return KEY_NUMPAD0;
				case 83: return KEY_NUMDEL;
				default: return kk-59+KEY_F1; // Function keys
			}}
		case 224: {
			int kk;
			switch (kk = getch()) {
				case 71: return KEY_HOME;
				case 72: return KEY_UP;
				case 73: return KEY_PGUP;
				case 75: return KEY_LEFT;
				case 77: return KEY_RIGHT;
				case 79: return KEY_END;
				case 80: return KEY_DOWN;
				case 81: return KEY_PGDOWN;
				case 82: return KEY_INSERT;
				case 83: return KEY_DELETE;
				default: return kk-123+KEY_F1; // Function keys
			}}
		case 13: return KEY_ENTER;
#ifdef _WIN32
		case 27: return KEY_ESCAPE;
#else // _WIN32
		case 155: // single-character CSI
		case 27: {
			// Process ANSI escape sequences
			if (cnt >= 3 && getch() == '[') {
				switch (k = getch()) {
					case 'A': return KEY_UP;
					case 'B': return KEY_DOWN;
					case 'C': return KEY_RIGHT;
					case 'D': return KEY_LEFT;
				}
			} else return KEY_ESCAPE;
		}
#endif // _WIN32
		default: return k;
	}
}

/// Function: nb_getch
/// Non-blocking getch(). Returns 0 if no key was pressed.
RLUTIL_INLINE int nb_getch(void) {
	if (kbhit()) return getch();
	else return 0;
}

/// Function: getANSIColor
/// Return ANSI color escape sequence for specified number 0-15.
///
/// See <Color Codes>
RLUTIL_INLINE RLUTIL_STRING_T getANSIColor(const int c) {
	switch (c) {
		case 0 : return ANSI_BLACK;
		case 1 : return ANSI_BLUE; // non-ANSI
		case 2 : return ANSI_GREEN;
		case 3 : return ANSI_CYAN; // non-ANSI
		case 4 : return ANSI_RED; // non-ANSI
		case 5 : return ANSI_MAGENTA;
		case 6 : return ANSI_BROWN;
		case 7 : return ANSI_GREY;
		case 8 : return ANSI_DARKGREY;
		case 9 : return ANSI_LIGHTBLUE; // non-ANSI
		case 10: return ANSI_LIGHTGREEN;
		case 11: return ANSI_LIGHTCYAN; // non-ANSI;
		case 12: return ANSI_LIGHTRED; // non-ANSI;
		case 13: return ANSI_LIGHTMAGENTA;
		case 14: return ANSI_YELLOW; // non-ANSI
		case 15: return ANSI_WHITE;
		default: return "";
	}
}

/// Function: setColor
/// Change color specified by number (Windows / QBasic colors).
///
/// See <Color Codes>
RLUTIL_INLINE void setColor(int c) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)c);
#else
	RLUTIL_PRINT(getANSIColor(c));
#endif
}

/// Function: cls
/// Clears screen and moves cursor home.
RLUTIL_INLINE void cls(void) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	// TODO: This is cheating...
	system("cls");
#else
	RLUTIL_PRINT("\033[2J\033[H");
#endif
}

/// Function: locate
/// Sets the cursor position to 1-based x,y.
RLUTIL_INLINE void locate(int x, int y) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	COORD coord;
	coord.X = (SHORT)x-1;
	coord.Y = (SHORT)y-1; // Windows uses 0-based coordinates
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else // _WIN32 || USE_ANSI
	#ifdef __cplusplus
		std::ostringstream oss;
		oss << "\033[" << y << ";" << x << "H";
		RLUTIL_PRINT(oss.str());
	#else // __cplusplus
		char buf[32];
		sprintf(buf, "\033[%d;%df", y, x);
		RLUTIL_PRINT(buf);
	#endif // __cplusplus
#endif // _WIN32 || USE_ANSI
}

/// Function: hidecursor
/// Hides the cursor.
RLUTIL_INLINE void hidecursor(void) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsoleOutput;
	CONSOLE_CURSOR_INFO structCursorInfo;
	hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	GetConsoleCursorInfo( hConsoleOutput, &structCursorInfo ); // Get current cursor size
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo( hConsoleOutput, &structCursorInfo );
#else // _WIN32 || USE_ANSI
	RLUTIL_PRINT("\033[?25l");
#endif // _WIN32 || USE_ANSI
}

/// Function: showcursor
/// Shows the cursor.
RLUTIL_INLINE void showcursor(void) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsoleOutput;
	CONSOLE_CURSOR_INFO structCursorInfo;
	hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	GetConsoleCursorInfo( hConsoleOutput, &structCursorInfo ); // Get current cursor size
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo( hConsoleOutput, &structCursorInfo );
#else // _WIN32 || USE_ANSI
	RLUTIL_PRINT("\033[?25h");
#endif // _WIN32 || USE_ANSI
}

/// Function: msleep
/// Waits given number of milliseconds before continuing.
RLUTIL_INLINE void msleep(unsigned int ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	// usleep argument must be under 1 000 000
	if (ms > 1000) sleep(ms/1000000);
	usleep((ms % 1000000) * 1000);
#endif
}

/// Function: trows
/// Get the number of rows in the terminal window or -1 on error.
RLUTIL_INLINE int trows(void) {
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return -1;
	else
		return csbi.srWindow.Bottom - csbi.srWindow.Top + 1; // Window height
		// return csbi.dwSize.Y; // Buffer height
#else
#ifdef TIOCGSIZE
	struct ttysize ts;
	ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
	return ts.ts_lines;
#elif defined(TIOCGWINSZ)
	struct winsize ts;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
	return ts.ws_row;
#else // TIOCGSIZE
	return -1;
#endif // TIOCGSIZE
#endif // _WIN32
}

/// Function: tcols
/// Get the number of columns in the terminal window or -1 on error.
RLUTIL_INLINE int tcols(void) {
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return -1;
	else
		return csbi.srWindow.Right - csbi.srWindow.Left + 1; // Window width
		// return csbi.dwSize.X; // Buffer width
#else
#ifdef TIOCGSIZE
	struct ttysize ts;
	ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
	return ts.ts_cols;
#elif defined(TIOCGWINSZ)
	struct winsize ts;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
	return ts.ws_col;
#else // TIOCGSIZE
	return -1;
#endif // TIOCGSIZE
#endif // _WIN32
}	
	
// TODO: Allow optional message for anykey()?

/// Function: anykey
/// Waits until a key is pressed.
RLUTIL_INLINE void anykey(void) {
	getch();
}

#ifndef min
/// Function: min
/// Returns the lesser of the two arguments.
#ifdef __cplusplus
template <class T> const T& min ( const T& a, const T& b ) { return (a<b)?a:b; }
#else
#define min(a,b) (((a)<(b))?(a):(b))
#endif // __cplusplus
#endif // min

#ifndef max
/// Function: max
/// Returns the greater of the two arguments.
#ifdef __cplusplus
template <class T> const T& max ( const T& a, const T& b ) { return (b<a)?a:b; }
#else
#define max(a,b) (((b)<(a))?(a):(b))
#endif // __cplusplus
#endif // max

// Classes are here at the end so that documentation is pretty.

#ifdef __cplusplus
/// Class: CursorHider
/// RAII OOP wrapper for <rlutil.hidecursor>.
/// Hides the cursor and shows it again
/// when the object goes out of scope.
struct CursorHider {
	CursorHider() { hidecursor(); }
	~CursorHider() { showcursor(); }
};

} // namespace rlutil
#endif

///////////////////////////////////////////////////////////////////////
//Colors.hpp
///////////////////////////////////////////////////////////////////////
#pragma once



namespace RConsole
{
  //Colors!
  enum Color
  {
    //Acquire rlutil info where possible--
    BLACK = rlutil::BLACK,
    BLUE = rlutil::BLUE,
    GREEN = rlutil::GREEN,
    CYAN = rlutil::CYAN,
    RED = rlutil::RED,
    MAGENTA = rlutil::MAGENTA,
    BROWN = rlutil::BROWN,
    GREY = rlutil::GREY,
    DARKGREY = rlutil::DARKGREY,
    LIGHTBLUE = rlutil::LIGHTBLUE,
    LIGHTGREEN = rlutil::LIGHTGREEN,
    LIGHTCYAN = rlutil::LIGHTCYAN,
    LIGHTRED = rlutil::LIGHTRED,
    LIGHTMAGENTA = rlutil::LIGHTMAGENTA,
    YELLOW = rlutil::YELLOW,
    WHITE = rlutil::WHITE,

    // Add custom values
    //DEFAULT = rlutil::DEFAULT, // BROKEN //Added custom to the rlutil header.
    PREVIOUS_COLOR
  };
}

///////////////////////////////////////////////////////////////////////
//Field2D.hpp
///////////////////////////////////////////////////////////////////////

// For strict unused variable warnings.
#define UNUSED(x) (void)(x)


/////// OS DEFINES /////////

// Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define OS_WINDOWS
#endif

// MacOSX
#if defined(__APPLE__)
#define OS_MACOSX
#endif

// Linux
#if defined(__linux__)
#define OS_LINUX
#endif

#if defined(OS_MACOSX) || defined(OS_LINUX)
#define OS_POSIX
#endif


/////// COMPILER DEFINES /////////

#if defined(_MSC_VER)
#define COMPILER_VS
#endif


/////// CONSOLE SETTINGS /////////

#define RConsole_CLIP_CONSOLE // Define we want console clipping
#define RConsole_NO_THREADING // Define we aren't threading- printf becomes unsafe, but faster.


#ifdef COMPILER_VS
// VS complains about unused functions.
#pragma warning(disable: 4505) //Unreferenced local function has been removed
#else
// g++/clang++ complain about unused functions.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif



namespace RFuncs
{

  // Absolute value of int.
  static int Abs(int x)
  {
    if (x < 0)
      return -x;
    return x;
  }
}



#ifdef COMPILER_VS
// restore VS warning
#pragma warning(default: 4505) //Unreferenced local function has been removed
#else
// restore g++/glang warnings
#pragma GCC diagnostic pop
#endif


namespace RConsole
{
  // Forward declare Field2D for use later.
  template <typename T>
  class Field2D;


  // A proxy class for the [] operator, allowing you to use the [] operator
  template <typename T>
  class Field2DProxy
  {
    // Mark the Field2D as my friend!
    friend Field2D<T>;

  public:
    // Operator Overload
    T &operator[](unsigned int y);

  private:
    // Private constructor, friends only!
    Field2DProxy(Field2D<T> *parentField, unsigned int xPos);

    // Variables
    Field2D<T> *field_;
    const int x_;
  };


  // A 2D way to represent a 1D line of continuous memory.
  // The 2D Field keeps track of the current index you are at in memory, allowing really
  // cheap O(K) reading if you have the spot selected, with a single add.
  // Note that this is not guarded- if you reach the "end" of the width, it will
  // let you freely step onto the next row of the 2D array you have set up.
  template <typename T>
  class Field2D
  {
    // Friensd can see ALL!
    friend Field2DProxy<T>;

  public:
    // Constructor
    Field2D(unsigned int w, unsigned int h);
    Field2D(unsigned int w, unsigned int h, const T defaultVal);
    Field2D &operator=(const Field2D &rhs);
    Field2D(const Field2D &rhs);
    ~Field2D();

	  // Structure Info
	  unsigned int Width() const;
	  unsigned int Height() const;
    unsigned int Length() const;

    // Member Functions - Complex Manipulation
    void Zero();
    void Set(const T &newItem);
    T &Get(unsigned int x, unsigned int y);
    Field2DProxy<T> operator[](unsigned int xPos);
    void GoTo(unsigned int x, unsigned int y);
    const T& Get(unsigned int x, unsigned int y) const;
    const T& Peek(unsigned int x, unsigned int y) const;
    const T& Peek(unsigned int index) const;
    void Fill(const T &objToUse);
    void Fill(const T &objToUse, unsigned int startIndex, unsigned int endIndex);

    // Basic Manipulation
    T &Get();
    T* GetHead() { return data_;}
    const T &Get() const;
    void IncrementX();
    void IncrementY();
    void DecrementX();
    void DecrementY();
    unsigned int GetIndex();
    void SetIndex(unsigned int index);

  private:
    // Variables
    unsigned int index_;
    unsigned int width_;
    unsigned int height_;
    T *data_;
  };
}


// Template implementations: Place in separate file:
namespace RConsole
{
    //////////////////////////////////
   // Field2DProxy Methods and Co. //
  //////////////////////////////////
  template <typename T>
  inline Field2DProxy<T>::Field2DProxy(Field2D<T> *parentField, unsigned int xPos)
    : field_(parentField)
    , x_(xPos)
  {  }


  // [] Operator Overload.
  // Note- This sets the current index!
  template <typename T>
  inline T &Field2DProxy<T>::operator[](unsigned int y)
  {
    field_->GoTo(x_, y);
    return field_->Get();
  }

    ////////////////////
   // Structure Info //
  ////////////////////
  // Gets the width of the Field2D
  template <typename T>
  inline unsigned int Field2D<T>::Width() const
  {
	  return width_;
  }

  // Gets the height of the Field2D
  template <typename T>
  inline unsigned int Field2D<T>::Height() const
  {
	  return height_;
  }

  template <typename T>
  inline unsigned int Field2D<T>::Length() const
  {
    return width_ * height_;
  }


    /////////////////////////////
   // Field2D Methods and Co. //
  /////////////////////////////
  // Constructor
  // Defaults by setting everything to 0.
  template <typename T>
  inline Field2D<T>::Field2D(unsigned int w, unsigned int h)
    : index_(0)
    , width_(w)
    , height_(h)
  {
    data_ = new T[w * h];
    Zero();
  };


  // Sets all values to given default.
  template <typename T>
  inline Field2D<T>::Field2D(unsigned int w, unsigned int h, const T defaultVal)
    : index_(0)
    , width_(w)
    , height_(h)
    , data_(nullptr)
  {
    T* adsf = new T[w * h];
    data_ = adsf;
    for (unsigned int i = 0; i < w; ++i)
      for (unsigned int j = 0; j < h; ++j)
      {
        Set(defaultVal);
        IncrementX();
      }
    index_ = 0;
  }


  template <typename T>
  inline Field2D<T>::Field2D(const Field2D<T> &rhs)
  {
    if (data_)
      delete[] data_;

    data_ = new T[rhs.width_ * rhs.height_];
    width_ = rhs.width_;
    height_ = rhs.height_;
    index_ = rhs.index_;

    for (unsigned int i = 0; i < width_ * height_; ++i)
      data_[i] = rhs.data_[i];
  }

  // Assignment operator
  template <typename T>
  inline Field2D<T> & Field2D<T>::operator=(const Field2D<T> &rhs)
  {
    if (&rhs != this)
    {
      if (data_)
        delete[] data_;
      
      data_ = new T[rhs.width_ * rhs.height_];
      width_ = rhs.width_;
      height_ = rhs.height_;
      index_ = rhs.index_;

      for (unsigned int i = 0; i < width_ * height_; ++i)
        data_[i] = rhs.data_[i];
    }
    return *this;
  }


  // Destructor
  template <typename T>
  inline Field2D<T>::~Field2D()
  {
    delete[] data_;
  }


    ////////////////////////
   // Complex Operations //
  ////////////////////////
  // Get the item at the position X, Y.
  // Does not set the actual index of the Field!
  template <typename T>
  inline T &Field2D<T>::Get(unsigned int x, unsigned int y)
  {
    GoTo(x, y);
    return Get();
  }


  // Const version of get that returns const reference.
  template <typename T>
  inline const T &Field2D<T>::Get(unsigned int x, unsigned int y) const
  {
    return Get(x, y);
  }


  // Get the first part of a 2D array operator
  template <typename T>
  inline Field2DProxy<T> Field2D<T>::operator[](unsigned int xPos)
  {
    return Field2DProxy<T>(this, xPos);
  }


  // Set the value at the current index
  template <typename T>
  inline void Field2D<T>::Set(const T &newItem)
  {
    data_[index_] = newItem;
  }


  // Glance at a read-only version of a specified location. Does NOT set index.
  template <typename T>
  inline const T& Field2D<T>::Peek(unsigned int x, unsigned int y) const
  {
    return data_[x + y * width_];
  }


  // Get the value at the specified index.
  template <typename T>
  inline const T& Field2D<T>::Peek(unsigned int index) const
  {
    return data_[index];
  }


  // Chance selected index to specified point.
  template <typename T>
  inline void Field2D<T>::GoTo(unsigned int x, unsigned int y)
  {
    index_ = x + y * width_;
  }


  // Sets all memory to 0. Does NOT modify index!
  template <typename T>
  inline void Field2D<T>::Zero()
  {
    memset(data_, 0, sizeof(T) * width_ * height_);
  }


  // Sets all memory to whatever you want.
  template <typename T>
  inline void Field2D<T>::Fill(const T &objToUse)
  {
    Fill(objToUse, 0, Length());
  }
  

  // Fills a specific range to whatever I want, inclusive for start index and
  // excludes end index.
  template <typename T>
  inline void Field2D<T>::Fill(const T &objToUse, unsigned int startIndex, unsigned int endIndex)
  {
    unsigned int prevIndex = index_;
    index_ = startIndex;
    for (unsigned int i = startIndex; i < endIndex; ++i)
    {
      Set(objToUse);
      IncrementX();
    }
    index_ = prevIndex;
  }

    //////////////////////
   // Cheap operations //
  //////////////////////
  // Get the value at the current index.
  template <typename T>
  inline T &Field2D<T>::Get()
  {
    return data_[index_];
  }


  // Const get.
  template <typename T>
  inline const T& Field2D<T>::Get() const
  {
    return data_[index_];
  }


  // Increment X location by 1 in the 2D field
  template <typename T>
  inline void Field2D<T>::IncrementX()
  {
    ++index_;
  }


  // Increment Y location by 1 in the 2D field
  template <typename T>
  inline void Field2D<T>::IncrementY()
  {
    index_ += width_;
  }


  // Decrement X location by 1 in the 2D field
  template <typename T>
  inline void Field2D<T>::DecrementX()
  {
    --index_;
  }


  // Decrement Y location by 1 in the 2D Field
  template <typename T>
  inline void Field2D<T>::DecrementY()
  {
    index_ -= width_;
  }


  // Gets the index that the 2D Field currently has.
  template <typename T>
  inline unsigned int Field2D<T>::GetIndex()
  {
    return index_;
  }


  // Gets the index that the 2D Field currently has.
  template <typename T>
  inline void Field2D<T>::SetIndex(unsigned int index)
  {
    index_ = index;
  }
}

///////////////////////////////////////////////////////////////////////
//CanvasRaster.hpp
///////////////////////////////////////////////////////////////////////


namespace RConsole
{
  // The raster info struct, holds info on what is to be drawn at a location and the color.
  struct RasterInfo
  {
    RasterInfo();
    RasterInfo(const char val, Color col);
    bool operator ==(const RasterInfo &rhs) const;
    bool operator !=(const RasterInfo &rhs) const;
    char Value;
    Color C;
  };

  // Console raster class
  class Canvas;
  class CanvasRaster
  {
    friend Canvas;

  public:
    // Constructors
    CanvasRaster(unsigned int width, unsigned int height);

    // Method Prototypes
    bool WriteChar(char toDraw, float x, float y, Color color = PREVIOUS_COLOR);
	  bool WriteString(const char *toWrite, size_t len, float x, float y, Color color = PREVIOUS_COLOR);
    const Field2D<RasterInfo>& GetRasterData() const;
    void Fill(const RasterInfo &ri);
    void Zero();

    // General
    unsigned int GetRasterWidth() const;
    unsigned int GetRasterHeight() const;

  private:
    // Private member functions
    Field2D<RasterInfo>& GetRasterData();

    // Variables
    unsigned int width_;
    unsigned int height_;
    Field2D<RasterInfo> data_;

  };
}


///////////////////////////////////////////////////////////////////////
//Canvas.hpp
///////////////////////////////////////////////////////////////////////


namespace RConsole
{
  class Canvas
  {
  public:
    // Init call
    static void ReInit(unsigned int width, unsigned int height);

    // Basic drawing calls
    static bool Update();
    static void FillCanvas(const RasterInfo &ri = RasterInfo(' ', WHITE));
    static void Draw(char toWrite, float x, float y, Color color = PREVIOUS_COLOR);
	  static void DrawString(const char* toDraw, float xStart, float yStart, Color color = PREVIOUS_COLOR);
    static void DrawAlpha(float x, float y, Color color, float opacity);
    static void Shutdown();

    // Advanced drawing calls
    static void DrawPartialPoint(float x, float y, Color color);
    static void DrawBox(char toWrite, float x1, float y1, float x2, float y2, Color color);
    static void SetCursorVisible(bool isVisible);
    static void DumpRaster(FILE *fp = stdout);
    static void CropRaster(FILE *fp = stdout, char toTrim = ' ');

    // Data related calls
    static unsigned int GetConsoleWidht();
    static unsigned int GetConsoleHeight();
  private:
    // Hidden Constructors- no instantiating publicly!
    Canvas() { };
    Canvas(const Canvas &rhs) { *this = rhs; }
    
    // Private methods.
    static void clearPrevious();
    static void fullClear();
    static void setColor(const Color &color);
    static bool writeRaster(CanvasRaster &r);
    static int  putC(int character, FILE * stream );
    static void setCloseHandler();

    // Any rasters we have. Could be expanded to have two, so you could "swap" them,
    // Although practicality of that is limited given the clearing technique.
    static CanvasRaster r_;
    static CanvasRaster prev_;

    // The tabs on what was last modified. This is important, because we will only update
    // what we care about.
    static bool hasLazyInit_;
    static bool isDrawing_;
    static unsigned int width_;
    static unsigned int height_;
    static Field2D<bool> modified_;
  };
}

///////////////////////////////////////////////////////////////////////
//CanvasRaster.cpp
///////////////////////////////////////////////////////////////////////



namespace RConsole
{
   /////////////////////////
   // Raster info object //
  ////////////////////////
  // 
  //constructor, no character and just the previous color.
  inline RasterInfo::RasterInfo() : Value(0), C(Color::PREVIOUS_COLOR)
  {  }

  
  // Non-Default constructor, specifies const character and color.
  inline RasterInfo::RasterInfo(const char val, Color col) : Value(val), C(col)
  {  }


  // Overloaded comparision operator that checks all fields.
  inline bool RasterInfo::operator ==(const RasterInfo &rhs) const
  {
    if (rhs.C == C && rhs.Value == Value)
      return true;
    return false;
  }


  // Overloaded comparison operator that checks all fields.
  inline bool RasterInfo::operator !=(const RasterInfo &rhs) const
  {
    return !(*this == rhs);
  }


    ///////////////////////////
   // Console Raster object //
  ///////////////////////////
  // Default constructor for the CanvasRaster- Zeros data and gets width and height.
  inline CanvasRaster::CanvasRaster(unsigned int width, unsigned int height)
    : width_(width)
    , height_(height)
    , data_(width, height, RasterInfo(' ', RConsole::WHITE))
  {  }


  // Draws a character to the screen. Returns if it was successful or not.
  inline bool CanvasRaster::WriteChar(char toDraw, float x, float y, Color color)
  {
    #ifdef RConsole_CLIP_CONSOLE

    if (x > width_) return false;
    if (y > height_) return false;

    #endif // RConsole_CLIP_CONSOLE

    data_.GoTo(static_cast<int>(x), static_cast<int>(y));
    data_.Set(RasterInfo(toDraw, color));
  
    //Everything completed correctly.
    return true;
  }


  // Writes a string to the field
  inline bool CanvasRaster::WriteString(const char *toWrite, size_t len, float x, float y, Color color)
  {
	  //Establish and check for a string of a usable size.
	  data_.GoTo(static_cast<int>(x), static_cast<int>(y));
	  for (unsigned int i = 0; i < len; ++i)
	  {
		  data_.Set(RasterInfo(toWrite[i], color));
		  data_.IncrementX();
	  }

	  //Return success.
	  return true;
  }


  // Writes a mass of spaces to the screen.
  inline void CanvasRaster::Fill(const RasterInfo &ri)
  {
    data_.Fill(ri);
  }
  

  // Clears out all of the data written to the raster. Does NOT move cursor to 0,0.
  inline void CanvasRaster::Zero()
  {
    data_.Zero();
  }


  // Get a constant reference to the existing raster.
  inline const Field2D<RasterInfo>& CanvasRaster::GetRasterData() const
  {
    return data_;
  }


  // Underlying raster exposing.
  inline Field2D<RasterInfo>& CanvasRaster::GetRasterData()
  {
    return data_;
  }
  

  // Gets console width.
  inline unsigned int CanvasRaster::GetRasterWidth() const
  { 
    return width_;
  }  


  // Get console height
  inline unsigned int CanvasRaster::GetRasterHeight() const
  { 
    return height_;
  } 
}

///////////////////////////////////////////////////////////////////////
//Canvas.cpp
///////////////////////////////////////////////////////////////////////
#include <cstdio>           // PutC
#include <iostream>         // ostream access
#include <csignal>          // Signal termination.
#include <chrono>           // Time related info for sleeping.
#include <thread>           // Sleep on exit to allow for update to finish.
#include <string>           // String for parsing.




namespace RConsole
{
  #define DEFAULT_WIDTH_SIZE (rlutil::tcols() - 1)
  #define DEFAULT_HEIGHT_SIZE rlutil::trows()

  //// Static initialization in non-guaranteed order.
  //CanvasRaster Canvas::r_         = CanvasRaster(DEFAULT_WIDTH_SIZE, DEFAULT_HEIGHT_SIZE);
  //CanvasRaster Canvas::prev_      = CanvasRaster(DEFAULT_WIDTH_SIZE, DEFAULT_HEIGHT_SIZE);
  //bool Canvas::hasLazyInit_       = false;
  //bool Canvas::isDrawing_         = true;
  //unsigned int Canvas::width_     = DEFAULT_WIDTH_SIZE;
  //unsigned int Canvas::height_    = DEFAULT_HEIGHT_SIZE;
  //Field2D<bool> Canvas::modified_ = Field2D<bool>(DEFAULT_WIDTH_SIZE, DEFAULT_HEIGHT_SIZE);


    /////////////////////////////
   // Public Member Functions //
  /////////////////////////////
  // Setup with width and height. Can be re-init
  inline void Canvas::ReInit(unsigned int width, unsigned int height)
  {
    width_ = width;
    height_ = height;
    r_ = CanvasRaster(width, height);
    prev_ = CanvasRaster(width, height);
    modified_ = Field2D<bool>(width, height);
  }


  // Clear out the screen that the user sees.
  // Note: More expensive than clearing just the previous spaces
  // but less expensive than clearing entire buffer with command.
  inline void Canvas::FillCanvas(const RasterInfo &ri)
  {
    r_.Fill(ri);
  }

  // Write the specific character in a specific color to a specific location on the console.
  inline void Canvas::Draw(char toWrite, float x, float y, Color color)
  {
    #ifdef RConsole_CLIP_CONSOLE

    if (x >= width_) return;
    if (y >= height_) return;
		if (x <= 0) return;
		if (y <= 0) return;

    #endif // RConsole_CLIP_CONSOLE

    modified_.GoTo(static_cast<int>(x), static_cast<int>(y));
    modified_.Set(true);
    r_.WriteChar(toWrite, x, y, color);
  }


  // Draw a string
  inline void Canvas::DrawString(const char* toDraw, float xStart, float yStart, Color color)
  {
	  size_t len = strlen(toDraw);
	  if (len <= 0) return;

    #ifdef RConsole_CLIP_CONSOLE

    // Bounds check.
    if (xStart > width_) return;
    if (yStart > height_) return;

	  // Set the memory we are using to modified.
	  modified_.GoTo(static_cast<int>(xStart), static_cast<int>(yStart));
	  unsigned int index = modified_.GetIndex();
    if (xStart > width_) return;

    // Checks the length and adjusts if it will be past.
    int writeLen = static_cast<int>(len);
    if (writeLen + index > modified_.Length())
      writeLen = modified_.Length() - index;

    // If our length plus the index we are at exceeds the end of the buffer,
    memset(modified_.GetHead() + index, true, writeLen);

    #else

    // Just blindly set modified for the length.
    modified_.GoTo(static_cast<int>(xStart), static_cast<int>(yStart));
    unsigned int index = modified_.GetIndex();
	  memset(modified_.GetHead() + index, true, len);

    #endif


	  // Write string
	  r_.WriteString(toDraw, len, xStart, yStart, color);
  }

  // Updates the current raster by drawing it to the screen.
  inline bool Canvas::Update()
  {
    if (!isDrawing_) return false;
    
    if (!hasLazyInit_)
    {
      setCloseHandler();
      hasLazyInit_ = true;
    }

    clearPrevious();
    writeRaster(r_);
    
    // Write and reset the raster.
    memcpy(prev_.GetRasterData().GetHead(), r_.GetRasterData().GetHead(), width_ * height_ * sizeof(RasterInfo));
    r_.Zero();

    rlutil::setColor(WHITE);

    return true;
  }


  // Draws a point with ASCII to attempt to represent alpha values in 4 steps.
  inline void Canvas::DrawAlpha(float x, float y, Color color, float opacity)
  {
    // All characters use represent alt-codes. 
    if (opacity < .25)
      Draw(static_cast<unsigned char>(176), x, y, color);
    else if (opacity < .5)
      Draw(static_cast<unsigned char>(177), x, y, color);
    else if (opacity < .75)
      Draw(static_cast<unsigned char>(178), x, y, color);
    else
      Draw(static_cast<unsigned char>(219), x, y, color);
  }


  // Stops the update loop.
  inline void Canvas::Shutdown()
  {
    isDrawing_ = false;
  }


  // Draws a point with ASCII to attempt to represent location in a square.
  inline void Canvas::DrawPartialPoint(float x, float y, Color color)
  {
    // Get first two decimal places from location.
    int xDec = static_cast<int>(x * 100) % 100;
    int yDec = static_cast<int>(x * 100) % 100;


    UNUSED(xDec);
    UNUSED(yDec);


    //If Y is closer to a border, use it for placement.
    if (RFuncs::Abs(50 - static_cast<int>(x)) < RFuncs::Abs(50 - static_cast<int>(y)))
    {
      if (x > 50)
        return Draw(static_cast<unsigned char>(222), x, y, color);
      return Draw(static_cast<unsigned char>(221), x, y, color);
    }
    //Otherwise, X is closer to a border.
    else
    {
      if (y > 50)
        return Draw(static_cast<unsigned char>(223), x, y, color);
      return Draw(static_cast<unsigned char>(220), x, y, color);
    }
  }

  // Drawing box
  inline void Canvas::DrawBox(char toWrite, float x1, float y1, float x2, float y2, Color color)
  {
    
    if(x1 > x2)
    {
      float temp = x1;
      x2 = x1;
      temp = x2;
    }
    if(y1 > y2)
    {
      float temp = y1;
      y2 = y1;
      temp = y2;
    }

    // At this point it can be assumed that x1 and y1 and lower than x2 and y2 respectively.

    for(unsigned int x = static_cast<unsigned int>(x1); x < x2; ++x)
    {
      for(unsigned int y = static_cast<unsigned int>(x1); y < y2; ++y)
      {
        Draw(toWrite, static_cast<float>(x), static_cast<float>(y), color);
      }
    }
  }


  //Set visibility of cursor to specified bool.
  inline void Canvas::SetCursorVisible(bool isVisible)
  {
    if (!isVisible)
      rlutil::hidecursor();
    else
      rlutil::showcursor();
  }


  // Gets the width of the console
  inline unsigned int Canvas::GetConsoleWidht()
  {
    return width_;
  }


  // Gets the height of the console
  inline unsigned int Canvas::GetConsoleHeight()
  {
    return height_;
  }

    //////////////////////////////
   // Private Member Functions //
  //////////////////////////////
  // Clears out the screen based on the previous items written. Clear character is a space.
  inline void Canvas::clearPrevious()
  {
    // Walk through, write over only what was modified.
    modified_.SetIndex(0);
    unsigned int maxIndex = width_ * height_;
    unsigned int index = 0;
    while (index < maxIndex)
    {
      index = modified_.GetIndex();
      const RasterInfo &curr = r_.GetRasterData().Peek(index);
      const RasterInfo &prev = prev_.GetRasterData().Peek(index);

      // If we have not modified the space,
      // and we don't have the same character as last time,
      // and we don't have the same color.
      if (!modified_.Get() && curr != prev)
      {
        // Compute X and Y location
        unsigned int xLoc = (index % width_) + 1;
        unsigned int yLoc = (index / width_) + 1;

        // locate on screen and set color
        rlutil::locate(xLoc, yLoc);

        putC(' ', stdout);
      }
      modified_.IncrementX();
    }

    // Set things back to zero.
    modified_.Zero();
  }


  // Explicitly clears every possible index. This is expensive! 
  inline void Canvas::fullClear()
  {
    rlutil::cls();
  }

  
  // Set the color in the console using utility, if applicable.
  inline void Canvas::setColor(const Color &color)
  {
    if (color != PREVIOUS_COLOR)
      rlutil::setColor(color);
  }


  // Write the raster we were attempting to write.
  inline bool Canvas::writeRaster(CanvasRaster &r)
  {
    // Set initial position.
    unsigned int maxIndex = width_ * height_;
    r.GetRasterData().SetIndex(0);
    unsigned int index = 0;
    while(index < maxIndex)
    {
      index = r.GetRasterData().GetIndex();
      const RasterInfo& ri = r.GetRasterData().Get();

      if (ri.Value != 0 && prev_.GetRasterData().Peek(index) != ri)
      {
        unsigned int xLoc = (index % width_) + 1;
        unsigned int yLoc = (index / width_) + 1;

        // Handle clipping the console if we define that tag.
      #ifdef RConsole_CLIP_CONSOLE
        // Handle X
        if (xLoc > width_)
          return false;

        // Handle Y
        if (yLoc > height_)
          return false;
      #endif


        // locate on screen and set color
        rlutil::locate(xLoc, yLoc);

        // Set color of cursor
        setColor(ri.C);

        // Print out to the console in the preferred fashion
        int retVal = 0;

        retVal = putC(ri.Value, stdout);

        if (!retVal)
          return false;
      }

      // Increment X location
      r.GetRasterData().IncrementX();
    }

    // Return we successfully printed the raster!
    return true;
  }

  // Cross-platform putc
  inline int Canvas::putC(int character, FILE * stream )
  {
    #if defined(RConsole_NO_THREADING) && defined(OS_WINDOWS)
      return _putc_nolock(character, stream);
    #else
      return putc(character, stream);
    #endif
  }


  // print out the formatted raster.
  // Note that because of console color formatting, we use the RLUTIL coloring option when
  // we are printing to the console, or have no file output specified.
  inline void Canvas::DumpRaster(FILE * fp)
  {
    // Dump only relevant part of stream.
    for (unsigned int i = 0; i < height_; ++i)
    {
      for (unsigned int j = 0; j < width_; ++j)
      {
        const RasterInfo &ri = r_.GetRasterData().Peek(j, i);
        if (fp == stdout)
        {
          rlutil::setColor(ri.C);
          std::cout << ri.Value;//fprintf(fp, "%c", ri.Value);
        }
        else
        {
          std::string line = rlutil::getANSIColor(ri.C) + ri.Value;
          fprintf(fp, "%s", line.c_str());
        }
      }

      std::cout << std::endl;
      //fprintf(fp, "\n");
    }

    // Set end color to white when we're done.
    if (fp == stdout)
      rlutil::setColor(WHITE);
  }


  // Crops all of the raster
  inline void Canvas::CropRaster(FILE *fp, char toTrim)
  {
    // Establish borders.
    unsigned int Xmin = width_;
    unsigned int Xmax = 0;
    unsigned int Ymin = height_;
    unsigned int Ymax = 0;

    for (unsigned int i = 0; i < width_; ++i)
    {
      for (unsigned int j = 0; j < height_; ++j)
      {
        if (r_.GetRasterData().Peek(i, j).Value != toTrim)
        {
          if (i < Xmin) Xmin = i;
          if (j < Ymin) Ymin = j;
          if (i > Xmax) Xmax = i;
          if (j > Ymax) Ymax = j;
        }
      }
    }

    // If we're trimming everything, don't even bother.
    if (Xmin > Xmax) return;
    if (Ymin > Ymax) return;

    // Dump only relevant part of stream.
    for (unsigned int j = Ymin; j <= Ymax; ++j)
    {
      for (unsigned int i = Xmin; i <= Xmax; ++i)
      {
        const RasterInfo &ri = r_.GetRasterData().Peek(i, j);
        if (fp == stdout)
        {
          rlutil::setColor(ri.C);
          fprintf(fp, "%c", ri.Value);
        }
        else
        {
          std::string line = rlutil::getANSIColor(ri.C) + ri.Value;
          fprintf(fp, "%s", line.c_str());
        }
      }

      fprintf(fp, "\n");
    }

    // Set end color to white when we're done.
    if (fp == stdout)
      rlutil::setColor(WHITE);
  }


  // Handle closing the window
  static void signalHandler(int signalNum)
  {
    Canvas::Shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int height = rlutil::trows();
    rlutil::locate(0, height);
    rlutil::setColor(WHITE);
    std::cout << std::endl;
    exit(signalNum);
  }
  inline void Canvas::setCloseHandler()
  {
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
  }
}
