//Playlist class to hold any number of files and manage shuffling/looping.
#pragma once
#include <vector> //Underlying structure.
#include "AudioFile.hpp"



namespace ASCIIPlayer
{
	class Playlist
	{
  public:
    //Constructor
    Playlist();

    //Member Functions
    void Add(const AudioFile &file);
    void Remove(const AudioFile &file);
    const std::vector<const AudioFile *> PeekAll(); //Looks at items in other list as const pointers, no modifything it.
    bool Next(); //returns successfully advanced to next song.
    void Back(); //Goes back to last song if possible, otherwise stays at current.
    void SetLooping();
    AudioFile *GetCurrent();
    unsigned int GetPlaylistLength();

  private:
    //Variables
    std::vector<AudioFile> playlist_; //Holds songs to be played.
    unsigned int activeIndex_;        //Active index into the playlist.
    bool looping_;                    //Are we looping this playlist?
	};
}
