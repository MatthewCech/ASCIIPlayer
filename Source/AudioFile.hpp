// Audio file object for path and associated data required for streaming.
#pragma once
#include <string>
#include "Defines.hpp"
#include <unordered_map>



namespace ASCIIPlayer
{
  class AudioSystem;

  struct AudioHandleWrapper
  {
    AudioHandleWrapper() : LoadedObject(nullptr), BoundToChannel(false) { }
    AudioHandle *LoadedObject;
    bool BoundToChannel;
  };

	class AudioFile
	{
    // Mark as friend to manage handles.
    friend class AudioSystem;

	public:
    // Constructor and Destructor
    AudioFile(std::string Path);
    ~AudioFile();

    // Operator overloads
    bool operator==(const AudioFile &rhs);

	private:
    // Private methods
    AudioHandleWrapper *get(APUnique apu);

    // Variables
    std::string path_;                                                 // Filepath of actual song
    APUnique fileID_;                                                  // Unique song ID.
    std::unordered_map<APUnique, AudioHandleWrapper> loadedObjects_;   // Handle to the loaded object.

    // Static Variables
    static APUnique uniqueID_; // ID for lookup in the audio system
	};
}
