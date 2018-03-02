// Written by reverie wisp, contact at reveriewisp@gmail.com
#include <string>  // Filepath
#include <vector>  // Storage
#include <fstream> // std FileIO


///////////////// FileIO.hpp //////////////////
namespace FileUtil
{
  // Proxy class for subscript operator shenanagins.
  class File;
  class FileProxy
  {
  public:
    FileProxy(unsigned int line, File &f);
    FileProxy &operator =(std::string str);
    operator std::string();
    friend std::ostream& operator<<(std::ostream &os, FileProxy &f);

  private:
    File &f_;
    const unsigned int line_;
  };


  // The actual file class.
  class File
  {
  public:
    // Read, write, and modify modes.
    enum ModifyMode { Replace = 1, Join = 2 };
    enum FileType { Text = 4, Binary = 8 };

    // Default modes for the flags being passed.
    // Note that the defaults are all the first of two options in enums.
    // This is important for parsing reasons.
    static const int Defaults = Text | Replace;

    // Constructor and Destructor
    File::File(std::string filepath, int mode = Defaults);
    ~File();

    // Member Functions
    std::string GetLine(unsigned int line) const;
    std::string operator[](unsigned int line) const;
    FileProxy operator[](unsigned int line);
    void Write(const void *data, size_t dataSizeBytes, int line = -1);
    void Write(std::string toWrite, int line = -1, const std::string newlineDelimiter = "\n");
    bool ReRead();
    void Clear();
    bool Save();
    bool SaveAs(std::string newPath, bool override = true);

    // Friend functions
    friend std::ostream& operator<<(std::ostream &os, File &f);

    // General info functions
    unsigned int GetLineCount() const;
    std::string  GetFileLocation() const;
    std::string  GetFileName() const;
    std::string  GetExtension() const;


  private:
    // Private member functions
    bool readCurrent();
    bool writeChangesToFile(bool writeOver);
    std::string parseFileLocation();
    std::string parseFileName();
    std::string parseExtension();

    // Variables
    std::string              filepath_;          // Constructor provide full filepath.
    FileType                 currentFileType_;   // Text or binary?
    ModifyMode               currentModifyMode_; // Write over or append to lines?
    std::string              fileName_;          // File name as we know it?
    std::string              fileExtension_;     // File extension as we know it?
    std::string              fileLocation_;      // File location as we know it?
    unsigned int             lastModifiedLine_;  // Last line modified when writing to file.
    std::vector<std::string> fileLines_;         // Vector of current lines in file. (1 if bin)
  };
}




///////////////// FileIO.cpp //////////////////
namespace FileUtil
{
  // Constructor for FileProxy
  inline FileProxy::FileProxy(unsigned int line, File &f) 
    : f_(f)
    , line_(line)
  {  }


  // Actually writes line(s) in question.
  inline FileProxy &FileProxy::operator =(std::string str)
  {
    f_.Write(str, line_);
    return *this;
  }


  // Attempt to access the line in question.
  // Will throw exception if line does not exist.
  inline FileProxy::operator std::string()
  {
    return f_.GetLine(line_);
  }


  // Printing the FileProxy
  inline std::ostream& operator<<(std::ostream &os, FileProxy &f)
  {
    os << f.f_.GetLine(f.line_);
    return os;
  }


  // Constructor.
  // Attempts to open the file in question, if no file was found,
  // automatically set to output.
  inline File::File(std::string filepath, int mode)
    : filepath_(filepath)
    , currentFileType_((mode & Binary) ? Binary : Text)
    , currentModifyMode_((mode & Join) ? Join : Replace) 
    , fileName_("")
    , fileExtension_("")
    , fileLocation_("")
    , lastModifiedLine_(0)
  {
    // Reads the current file.
    readCurrent();

    // Update information if possible.
    fileLocation_ = parseFileLocation();
    fileName_ = parseFileName();
    fileExtension_ = parseExtension();
  }


  // Destructor - Cleanup
  inline File::~File()
  {  }


  // Gets the specified line. No bounds checking performed.
  inline FileProxy File::operator[](unsigned int line)
  {
    return FileProxy(line, *this);
  }

  inline std::string File::GetLine(unsigned int line) const
  {
    return fileLines_[line];
  }

  // Gets the specified line. No bounds checking performed.
  inline std::string File::operator[](unsigned int line) const
  {
    return fileLines_[line];
  }


  // Writes arbitrary data to the file. Will disregard line if used in binary mode.
  inline void File::Write(const void *data, size_t dataSizeBytes, int line)
  {
    if (currentFileType_ == Binary)
    {
      // Resize if necessary
      if (fileLines_.size() < 1)
        fileLines_.resize(1);

      // Append if necessary.
      if (currentModifyMode_ == Join)
        fileLines_[0] = fileLines_[0] + std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
      else // Current mode is Replace
        fileLines_[0] = std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
    }
    else
    {
      if (line < 0)
        line = lastModifiedLine_;

      // Resize to accomodate for adding to the end.
      if (static_cast<size_t>(line) >= fileLines_.size())
        fileLines_.resize(line + 1);

      // Append if necessary.
      if (currentModifyMode_ == Join)
        fileLines_[line] = fileLines_[line] + std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
      else // Current mode is Replace
        fileLines_[0] = std::string(reinterpret_cast<const char *>(data), dataSizeBytes);
    }
  }


  // Write to a specific line in the file.
  // Note that line and newlineDelimiter are completely disregarded for binary files.
  inline void File::Write(std::string toWrite, int line, const std::string newlineDelimiter)
  {
    // Handle binary case
    if (currentFileType_ == Binary)
    {
      // Resize if necessary
      if(fileLines_.size() < 1)
        fileLines_.resize(1);

      // Append if necessary.
      if (currentModifyMode_ == Join)
        fileLines_[0] = fileLines_[0] + toWrite;
      else // Current mode is Replace
        fileLines_[0] = toWrite;
    }
    else
    {
      // Prep location for new line.


      // Holds split lines.
      std::vector<std::string> splitLines;

      // Parse out at newline character.
      size_t pos;
      while ((pos = toWrite.find(newlineDelimiter)) != toWrite.npos)
      {
        splitLines.push_back(toWrite.substr(0, pos));
        toWrite.erase(0, pos + newlineDelimiter.size());
      }
      splitLines.push_back(toWrite);

      // Update line if necessary.
      if (line < 0)
        line = lastModifiedLine_;

      // Resize to accomodate for adding to the end.
      unsigned int newSize = static_cast<unsigned int>(splitLines.size() + line);
      if (fileLines_.size() < newSize)
        fileLines_.resize(newSize);

      // Now that we have all the lines split, we either join toor write over  existing lines.
      int lineWithOffset = line;
      for (std::size_t i = 0; i < splitLines.size(); ++i)
      {
        if (currentModifyMode_ == Join)
          fileLines_[lineWithOffset] = fileLines_[line + i] + splitLines[i];
        else // Current mode is Replace
          fileLines_[lineWithOffset] = splitLines[i];
        lineWithOffset++;
      }

      // Update last modified.
      lastModifiedLine_ = static_cast<unsigned int>(splitLines.size() - 1);
    }
  }


  // Writes file to output stream.
  inline std::ostream& operator<<(std::ostream &os, File &f)
  {
    for (size_t i = 0; i < f.fileLines_.size(); ++i)
      os << f.fileLines_[i] << std::endl;

    return os;
  }


  // Write the file changes
  inline bool File::writeChangesToFile(bool writeOver)
  {
    // Check to see if we will be writing over.
    std::fstream fTest(filepath_, std::ios::in);
    if (fTest.is_open())
    {
      //If we are not overriding, leave. Close then.
      if (!writeOver)
      {
        fTest.close();
        return false;
      }
    }

    // Establish the file mode flags
    int mode = std::ios::out;
    mode |= std::ios::trunc;
    if (currentFileType_ == Binary)
      mode |= std::ios::binary;

    // Create file object
    std::fstream fileObject(fileLocation_ + fileName_, mode);

    // See if we opened the file successfully.
    if (fileObject.is_open())
    {
      if (currentFileType_ == Binary)
      {
        for (std::size_t i = 0; i < fileLines_.size(); ++i)
          fileObject << fileLines_[i];
      }
      else
      {
        for (std::size_t i = 0; i < fileLines_.size(); ++i)
          fileObject << fileLines_[i] << std::endl;
      }
      fileObject.close();

      // We were successful
      return true;
    }

    // Return write error
    return false;
  }


  // ReReads the file into memory from the current filepath.
  // WARNING: ERASES ALL LOCAL MODIFICATIONS AND OVERRIDES CHANGES IF UNWRITTEN
  inline bool File::ReRead()
  {
    if (readCurrent())
    {
      // Update information if possible.
      fileLocation_ = parseFileLocation();
      fileName_ = parseFileName();
      fileExtension_ = parseExtension();

      // We read and updated stuff!
      return true;
    }

    // Could not re-read, did not update info.
    return false;
  }


  // Reads from scratch the file in question, resetting the object.
  inline bool File::readCurrent()
  {
    // Check read style
    int mode = std::ios::in;
    if (currentFileType_ == Binary)
      mode |= std::ios::binary;

    // Create file object
    std::fstream fileObject(filepath_, mode);

    // See if we opened the file successfully.
    if (fileObject.is_open())
    {
      // See if we can read the file
      std::string buffer;
      while (std::getline(fileObject, buffer))
        fileLines_.push_back(buffer);
      fileObject.close();

      // We read the file in successfully
      return true;
    }

    // Could not open file for whatever reason.
    return false;
  }


  // Getters
  inline unsigned int File::GetLineCount() const   { return static_cast<unsigned int>(fileLines_.size()); }
  inline std::string File::GetFileLocation() const { return fileLocation_;     }
  inline std::string File::GetFileName() const     { return fileName_;         }
  inline std::string File::GetExtension() const    { return fileExtension_;    }


  // Clears contents of the read file, if it exists.
  inline void File::Clear()
  {
    fileLines_.clear();
    lastModifiedLine_ = 0;
  }


  // Saves changes to file.
  inline bool File::Save()
  {
    return writeChangesToFile(true);
  }


  // Save file with a different filepath and name, with bool to override or not.
  inline bool File::SaveAs(std::string newPath, bool override)
  {
    // Get new filepath.
    filepath_ = newPath;

    // Update information, if possible.
    fileLocation_ = parseFileLocation();
    fileName_ = parseFileName();
    fileExtension_ = parseExtension();

    // Write changes.
    return writeChangesToFile(override);
  }


  // Parses the file location, or folder, the file is in.
  inline std::string File::parseFileLocation()
  {
    std::string path = "";
    std::size_t location = filepath_.find_last_of("/\\");
    if (location != filepath_.npos)
      path = filepath_.substr(0, location + 1);
    return path;
  }


  // Gets the name of the file with path removed.
  inline std::string File::parseFileName()
  {
    std::string name = filepath_;
    std::size_t location = filepath_.find_last_of("/\\");
    if(location != filepath_.npos)
      name = filepath_.substr(location + 1);
    return name;
  }


  // Gets the extension of the file, if applicable. Otherwise, returns empty string.
  inline std::string File::parseExtension()
  {
    std::string extension = "";
    std::size_t location = filepath_.find_last_of(".");
    if (location != filepath_.npos)
      extension = filepath_.substr(location + 1);
    return extension;
  }
}
