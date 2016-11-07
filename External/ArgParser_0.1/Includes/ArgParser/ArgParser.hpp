#pragma once
#include <vector>
#include <string>



class ArgParser
{
public:
  // Constructor
  ArgParser(int argc, char** argv);

  // Basic Member Functions
  std::string operator[](unsigned index);
  std::vector<std::string> StartsWith(std::string str);
  std::vector<std::string> Contains(std::string str);
  std::vector<std::string> DoesNotContain(std::string str);
  std::vector<std::string> DoesNotStartWith(std::string str);
  std::vector<std::string> GetAllArgs();
  unsigned int GetNumArgs();
  std::string GetProgramFullPath();

  // Advanced Member Functions
  bool ArgumentExists(std::string str);
  //!todo: Add callback for specific argument

private:
  // Private member functions
  bool startsWith(std::string toCheck, std::string startChars);
  int substrLoc(std::string toCheck, std::string substr);

  // Variables
  std::vector<std::string> args_;
  const static int NO_INDEX = -1;
};



  /////////////////////////////
 // Public member functions //
/////////////////////////////
// Argument parser constructor
inline ArgParser::ArgParser(int argc, char** argv)
{
  for (int i = 0; i < argc; ++i)
    args_.push_back(argv[i]);
}


// Subscript because yes.
inline std::string ArgParser::operator[](unsigned index)
{
  // Check bounds
  if (index > args_.size() - 1)
    return "";

  return args_[index];
}


// Returns arguments that start with the provided string.
inline std::vector<std::string> ArgParser::StartsWith(std::string str)
{
  std::vector<std::string> toRet;
  for (unsigned int i = 0; i < args_.size(); ++i)
  {
    if (startsWith(args_[i], str))
      toRet.push_back(args_[i]);
  }

  return toRet;
}


// Returns arguments that contain the provided string.
inline std::vector<std::string> ArgParser::Contains(std::string str)
{
  std::vector<std::string> toRet;
  for (unsigned int i = 0; i < args_.size(); ++i)
  {
    if (substrLoc(args_[i], str) != NO_INDEX)
      toRet.push_back(args_[i]);
  }

  return toRet;
}


// Returns arguments that don't start with the provided string.
inline std::vector<std::string> ArgParser::DoesNotStartWith(std::string str)
{
  std::vector<std::string> toRet;
  for (unsigned int i = 0; i < args_.size(); ++i)
  {
    if (startsWith(args_[i], str) == false)
      toRet.push_back(args_[i]);
  }

  return toRet;
}


// Returns the arguments that don't contain the following string.
inline std::vector<std::string> ArgParser::DoesNotContain(std::string str)
{
  std::vector<std::string> toRet;
  for (unsigned int i = 0; i < args_.size(); ++i)
  {
    if (substrLoc(args_[i], str) == NO_INDEX)
      toRet.push_back(args_[i]);
  }

  return toRet;
}


// Returns all arguments.
inline std::vector<std::string> ArgParser::GetAllArgs()
{
  return args_;
}


// Returns the number of arguments.
inline unsigned int ArgParser::GetNumArgs()
{
  return static_cast<unsigned int>(args_.size());
}


// Returns the name of the program.
inline std::string ArgParser::GetProgramFullPath()
{
  return args_[0];
}


// Checks to see if a specific argument exists
inline bool ArgParser::ArgumentExists(std::string str)
{
  std::vector<std::string> con = StartsWith(str);
  if (con.size() > 0)
    if (con[0].length() == str.length())
      return true;

  return false;
}

  /////////////////////////////
 // Private member functions//
/////////////////////////////
// Checks to see if a string starts with a specific substring
inline bool ArgParser::startsWith(std::string toCheck, std::string startChars)
{
  if (substrLoc(toCheck, startChars) == 0)
    return true;
  return false;
}


// Returns a bounds and argument verified location within the string,
// or the value NO_INDEX otherwise.
inline int ArgParser::substrLoc(std::string toCheck, std::string substr)
{
  if (substr.length() < 1)
    return NO_INDEX;
  if (toCheck.length() < 1)
    return NO_INDEX;

  std::size_t loc = toCheck.find(substr);
  if (loc == std::string::npos)
    return NO_INDEX;

  return static_cast<int>(loc);
}


#ifdef OVERLOAD_VECTOR
  /////////////////////////////////////////////////
 // Operator Overloads and other Global Methods //
/////////////////////////////////////////////////
// Vector of strings: Printing.
std::ostream & operator<<(std::ostream &os, std::vector<std::string> rhs)
{
  if (rhs.size() > 0)
    for (unsigned int i = 0; i < rhs.size(); ++i)
      os << "[" << i << "] " << rhs[i] << std::endl;

  return os;
}
#endif
