// Playlist class to hold any number of files and manage shuffling/looping.
#pragma once
#include <vector>
#include "AudioFile.hpp"
#include "Defines.hpp"


namespace ASCIIPlayer
{
  template<typename T> class Playlist
	{
  public:
    // Constructor
    Playlist(void(T::*updateCallback)(), T* caller);

    // Member Functions
    void Add(AudioFile *file);
    void Remove(AudioFile *file);
    const std::vector<const AudioFile *> PeekAll();
    void GoToBeginning();
    void Next();
    void Back(AudioSystem &audioSystem);
    void Scramble();
    void SetLooping(bool isLooping);
    AudioFile *GetCurrent();
    size_t GetPlaylistLength();
    size_t GetPlaylistPos();

  private:
    // Private member functions
    void listUpdateCallback();
    
    // Variables
    std::vector<AudioFile*> playlist_; // Holds songs to be played.
    std::size_t activeIndex_;          // Active index into the playlist.
    bool looping_;                     // Are we looping this playlist?

    // Callback info.
    void(T::*toCallOnUpdate_)();
    T* caller_;
	};
}

//#include "Playlist.tpp"
#include <algorithm> // random shuffle
namespace ASCIIPlayer
{
  // Constructor
  template<typename T> Playlist<T>::Playlist(void(T::*updateCallback)(), T* caller)
    : playlist_()
    , activeIndex_(0)
    , looping_(true)
    , toCallOnUpdate_(updateCallback)
    , caller_(caller)
  {  }


  // Adds a new audiofile to a playlist.
  template<typename T> void Playlist<T>::Add(AudioFile *file)
  {
    playlist_.push_back(file);
    
    // Update to the added song and update the list
    activeIndex_ = playlist_.size() - 1;
    listUpdateCallback();
  }


  // Removes a song in the playlist.
  template<typename T> void Playlist<T>::Remove(AudioFile *file)
  {
    for (auto iter = playlist_.begin(); iter != playlist_.end(); ++i)
    {
      if (*(*iter) == *file)
      {
        playlist_.erase(iter);
        listUpdateCallback();
        return;
      }
    }
  }


  // Looks at items in other list as const pointers, no modifything it.
  // This is fairly expensive.
  template<typename T> const std::vector<const AudioFile *>  Playlist<T>::PeekAll()
  {
    std::vector<const AudioFile *> toRet;
    for (int i = 0; i < playlist_.size(); ++i)
      toRet.push_back(playlist_[i]);

    return toRet;
  }


  // returns active song to the beginning.
  template<typename T> void Playlist<T>::GoToBeginning()
  {
    activeIndex_ = 0;
    listUpdateCallback();
  }


  // If possible, will attempt to go to the next song.
  template<typename T> void Playlist<T>::Next()
  {
    // Handle empty playlist case
    if (playlist_.size() == 0)
      return;

    // Move forward to the next song regardless of our position in the current song.
    // If we're looping, go back to the start as necessary.
    if (activeIndex_ < playlist_.size() - 1)
    {
      ++activeIndex_;
      listUpdateCallback();
    }
    else 
    {
      if (looping_)
      {
        activeIndex_ = 0;
        listUpdateCallback();
      }
      else
        activeIndex_ = static_cast<unsigned int>(playlist_.size());
    }
  }


  // Head back in the playlist if possible.
  template<typename T> void Playlist<T>::Back(AudioSystem &audioSystem)
  {
    // Handle a lack of any content in the playlist.
    if (playlist_.size() == 0)
      return;

    // Specifically handle being within a few seconds of the start
    const double delay = 3;
    if (audioSystem.GetCurrentPosition(*GetCurrent()) > delay * 1000)
    {
      audioSystem.SetCurrentPosition(*GetCurrent(), 0);
      return;
    }

    // Return back to the last song otherwise, and loop if that's something we have enabled.
    if (activeIndex_ > 0)
    {
      --activeIndex_;
      listUpdateCallback();
    }
    else
    {
      if (looping_)
      {
        int index = static_cast<int>(playlist_.size()) - 1;

        // Handle edge case of 0 things
        if (index < 0)
          throw "This should not be happening: Looped to index -1 while going back.";

        activeIndex_ = static_cast<unsigned int>(index);
        listUpdateCallback();
      }
    }
  }


  // Scrambles the items in the list. Does not change the actively playing song,
  // but shuffles everything into the list behind it after making sure it's at
  // the front of the list.
  template<typename T> void Playlist<T>::Scramble()
  {
    if (playlist_.size() > 2)
    {
      if (activeIndex_ != 0)
      {
        std::swap(playlist_[0], playlist_[activeIndex_]);
        activeIndex_ = 0;
      }

      std::random_shuffle(playlist_.begin() + 1, playlist_.end());
    }
  }


  // Sets whether or not the playlist is looping.
  template<typename T> void Playlist<T>::SetLooping(bool isLooping)
  {
    looping_ = isLooping;
  }


  // Gets the current audio file in the playlist.
  template<typename T> AudioFile *Playlist<T>::GetCurrent()
  {
    if (activeIndex_ == playlist_.size())
      return nullptr;

    if(playlist_.size() > 0)
      return playlist_[activeIndex_];
    return nullptr;
  }


  // Get the legnth of the playlist. (song count)
  template<typename T> size_t Playlist<T>::GetPlaylistLength()
  {
    return static_cast<unsigned int>(playlist_.size());
  }


  // Get the current song's location in the playlist.
  template<typename T> size_t Playlist<T>::GetPlaylistPos()
  {
    return activeIndex_;
  }


  // Calls the function given that allows updates.
  template<typename T> void Playlist<T>::listUpdateCallback()
  {
    if (toCallOnUpdate_ && caller_)
      (caller_->*toCallOnUpdate_)();
  }
}
