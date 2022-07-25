# ASCIIPlayer
ASCIIPlayer is a lightweight command-line audio visualizer that allows you to play all sorts of audio formats in your terminal. ASCIIPlayer uses FMOD EX which makes it useful for most audio file formats associated with game development. 

## Using ASCIIPlayer
- Download the installer from www.asciiplayer.com or download the standalone executable
- Run the application and either pass in the path to the the song to play, or start the application and drag a song in (on windows). Multiple paths to songs can be provided.

Looking for a stable build or for info about the latest changes? Head over to www.asciiplayer.com, or check out github releases. This repo is actively iterated on, and may not necessarily be functional so refer to release tags for additional information and stable changelists.

Want to provide feedback or feature suggestions? Feel free to file a request or bug report on GitHub at https://github.com/MatthewCech/ASCIIPlayer, or send me an email at contact(at)asciiplayer.com

## Developing ASCIIPlayer

Suggested for Windows Development:
- Visual Studio 2017/2019 - editor and debugging: https://visualstudio.microsoft.com/vs/
- Inno Installer - Building the installer: https://www.jrsoftware.org/isinfo.php
- 7zip, for zipping up portable version: https://www.7-zip.org (and added to path)

Suggested for \*nix Development
- GNU Compiler Collection - editor and debugging: `sudo apt install build-essential`
- Valgrind - memory debugging: `sudo apt install valgrind`

Once set up, you can build for your selected platform by running the `_build` prefixed script associated with your platform located in the repo root. This should output a folder for the platform.

You'll then want to compile the project. Relevant settings and resources should have been installed by the build script, but if not there's limited configuration or flags. Here's the recommended \*nix configuration:  `g++ -pthread -m32 main.cpp -L./lib/x86 -lfmodL -I./inc -o output`. For windows, the visual studio solution file should be automatically configured.

Once compiled, to package you can use the scripts within the platform specific dist folders. 

Windows:
- Ensure `ASCIIPlayer.exe` exists in a `bin_` folder in the repo root.
- Run `_build_win_resources.bat` in the repo root to assign the executable (icon, etc).
- Copy `ASCIIPlayer.exe` will need to be copied from whichever build folder you with to use into the `DistWindows` folder,
- Run the .iss script (with inno) to collect and package up the executable in the `Output` folder.

## License

See LICENSE.md. This license is intended to only apply to code within the project itself and not to any external libraries. External libraries (located within the 'External' folder) may have separate licenses.

Human Readable Version:
- It's free to explore and modify, but you should mention "Hey, I got asciiplayer from here!" with a link back to me in some capacity. Besides, you probably don't want to claim credit for some of this.
- Not my fault if it breaks or causes your machine to explode or something like that.

## FMOD

ASCIIPlayer is based on FMOD's low level API, and FMOD itself requires per-project licensing. For more information, head over to https://www.fmod.com/ and https://www.fmod.com/licensing

Made with FMOD Studio by Firelight Technologies Pty Ltd.

## How to package