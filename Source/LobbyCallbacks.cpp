#include <Lobby.hpp>



namespace ASCIIPlayer
{
  // Interpret specific paths
  void Lobby::interpretPath(const std::string str)
  {
    std::string input = str;
    if (str[0] == '"' && str[str.size() - 1] == '"')
      input = str.substr(1, str.size() - 2);
    if (str[0] == '\\' && str[str.size() - 1] == '"')
      input = str.substr(2, str.size() - 4);

    AudioFile *test1 = new ASCIIPlayer::AudioFile(input);
    activeDJ_->AddSong(test1);
  }
}
