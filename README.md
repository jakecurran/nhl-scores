# nhl-scores

`nhl` is a command line interface for recent and live NHL game scores.

![nhl-scores](etc/example.gif)

It uses the JSON information hosted on [live.nhle][]. An example of this data
can be found [here](etc/RegularSeasonScoreboardv3.jsonp).

## Compilation / Installation

To compile and install the program to , use `sudo make`.

## Usage

`nhl [options]`

`nhl` on its own displays scores of recent and live NHL games

|  Option        |  Description                                                |
| -------------- | ----------------------------------------------------------- |
| `-l`, `--live` | Display scores of live NHL games                            |
| `-t`, `--team` | Display scores recent/live games of a team specified by its city or team name, and without spaces |
| `-r`, `--raw`  | Output raw JSON, beautified, from [live.nhle][] source      |
| `-h`, `--help` | Display command info and usage instructions                 |

## Compatibility

Should be compatible with *nix systems, but tested using

- Xubuntu 15.10 x64, GCC 5.2

[live.nhle]: http://live.nhle.com/GameData/RegularSeasonScoreboardv3.jsonp
