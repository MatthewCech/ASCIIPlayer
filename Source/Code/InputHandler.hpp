//Keeps tabs on various input from keyboard or dragging in files.
#pragma once


namespace ASCIIPlayer
{
  class InputHandler
  {

    //Not really sure how I want this structured yet...

  public:
    bool Update();
    bool IsPressed();
    bool IsTriggered();
    bool HasInterrupt();

  private:
    //Probably a vector of bool/bool pairs to keep tabs on if a key is down or not.
  };
}
