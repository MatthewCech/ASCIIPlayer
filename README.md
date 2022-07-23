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
- 7zip, for zipping up portable version: https://www.7-zip.org

Suggested for \*nix Development
- GNU Compiler Collection - editor and debugging: `sudo apt install build-essential`
- Valgrind - memory debugging: `sudo apt install valgrind`

## License

See LICENSE.md. This license is intended to only apply to code within the project itself and not to any external libraries. External libraries (located within the 'External' folder) may have separate licenses.

Human Readable Version:
- It's free to explore and modify, but you should mention "Hey, I got asciiplayer from here!" with a link back to me in some capacity. Besides, you probably don't want to claim credit for some of this.
- Not my fault if it breaks or causes your machine to explode or something like that.

## FMOD

ASCIIPlayer is based on FMOD's low level API, and FMOD itself requires per-project licensing. For more information, head over to https://www.fmod.com/ and https://www.fmod.com/licensing

Made with FMOD Studio by Firelight Technologies Pty Ltd.
