#pragma once
#include <string>

#define STR_TYPE static const std::string

namespace Strings
{
  STR_TYPE STARTUP_PRIMARY_TEXT = "Waiting for songs";
  STR_TYPE STARTUP_SUBTEXT_LINE_SPACER = { static_cast<char>(250), ' ', static_cast<char>(250), ' ', static_cast<char>(250) };
  STR_TYPE STARTUP_SUBTEXT_LINE_1 = "Drag in a music file or";
  STR_TYPE STARTUP_SUBTEXT_LINE_2 = "press ESC for the menu!";
  STR_TYPE STARTUP_FMOD_ATTRIBUTION = "Made with FMOD by Firelight Technologies Pty Ltd.";

  STR_TYPE MODAL_HELP_DEFAULT = "Hmm, no message was set for this menu. You shouldn't see this!";
  STR_TYPE MODAL_HELP_GENERAL = "ASCIIPlayer is a command-line program navigated exclusively by keyboard. If you want to play a song, you can drag it in to add it to the playlist, or select multiple and pass them as command line aguments!";
  STR_TYPE MODAL_HELP_CONFIG = "Not all configurations can be edited by the menu. ASCIIPlayer.conf is located next to the program executable, and can be opened with your default text editor via the menu 'EDIT' menu or by hand in the program folder.";
  STR_TYPE MODAL_HELP_OPEN = "Opening files with a dialogue box is not currently supported, but you can drag and drop single files into ASCIIPlayer to add them to the current playlist!";
  STR_TYPE MODAL_HELP_COMMANDS = "    + : Volume Up\n    - : Volume Down\n  > ] : Next Song\n  < [ : Previous Song\n  i u : Toggle Info\n    d : Toggle Debug*\nspace : Toggle Paused*\n left : Jump Back*\nright : Jump Forward*\n\n(*navigates menu if open)";

  STR_TYPE ERROR_INPUT_NO_DJ = "You should have an active DJ to issue character commands";
  STR_TYPE ERROR_VISUALIZER_CONFIG_INVALID = "Invalid/incorrect visualizer config provided!";

  STR_TYPE DEBUG_PER_SECOND = "per second";
  STR_TYPE DEBUG_DELAY = "delay";
  STR_TYPE DEBUG_YIELDING = "yield";
}
  

