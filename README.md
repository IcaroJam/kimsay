# kimsay
###### Kind of like cowsay, but waaaay more disco.
Kimsay is a command line utility written in c++ that does more or less the same as cowsay, but is Disco Elysium themed.

_What is Disco Elysium you say?_  
If you are asking questions like that one do yourself a favour and go play it. It is a game. A great game.

![A screenshot of the program's output](demo.png)
## Installation
There are two main ways of installing kimsay:
- `sudo make install` will install for all users.
	- The binary will be put into `/usr/local/bin`
	- Other program files are put into `/usr/local/share/kimsay`
	- The manpage is put into `/usr/local/share/man/man6`
- `make install PREFIX=~/.local` will install for your user only.
	- The binary will be put into `~/.local/bin`. Make sure this is in your path if you choose this method.
	- Other program files are put into `~/.local/share/kimsay`
	- The manpage is put into `~/.local/share/man/man6`

You can further refine the installation by overriding `BINDIR` (path to the execurable), `DATADIR` (path to the portraits and dialog) and `MANDIR` (path to the manpage).

_The binary path should probably default to `/usr/local/games/` instead, but that would require adding it to the PATH during installation and I don't want to deal with all of that at the moment._
## Usage
If no arguments are provided, the program reads from standard input until EOF. Otherwise it concatenates the provided arguments, separated by a space.

| Flag | Default | Description | Example |
| --- | --- | --- | --- |
| -h | | Prints a help message and nothing else | `kimsay -h` |
| -r | | _As in Revachol._ The text printed is random in-game dialog said by the character. Beware of spoilers | `kimsay -r` |
| -F | | The portrait is painted frameless | `kimsay -F` |
| -u | | The text is printed without the disco format | `kimsay -u` |
| -c | kim | Instead of Kim, use the portrait and name of another character | `kimsay -c conceptualization` |
| -w | 42 | The text is wrapped at this amount of columns. Values under 12 are ignored | `kimsay -w 30` |
| -g | 2 | The amount of columns that separate the portrait from the text. Values under 0 are ignored | `kimsay -g 5` |
| -n |  | Override the name of the character | `kimsay -n "Finn the Human"` |
| -f |  | The path to a custom file to use as portrait | `kimsay -f ./ascii/deer` |
## Acknowledgements and disclaimers
Two single-header libraries are used by the project:
- [Nlohmann's JSON library](https://github.com/nlohmann/json)
- [Philsquared's Textflow](https://github.com/catchorg/textflowcpp/)

Special thanks to scribegoat for their **amazing** [Disco-Elysium-Scribe](https://disco-elysium-scribe.pages.dev/), it made it possible to get the lines said by Kim, and it is an overall great tool, also very pretty :)

Many thanks to all of them for their work ^^.

_All of Disco Elysium's dialog lines and the original art of the portraits, as well as the characters depicted in said art, are property of their respective owners and authors._