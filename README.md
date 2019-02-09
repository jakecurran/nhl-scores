# nhl-scores

`nhl` is a command line interface for recent and live NHL game scores.

![nhl-scores](etc/example.gif)

It uses the JSON information hosted on [live.nhle][]. An example of this data
can be found [here](etc/RegularSeasonScoreboardv3.jsonp).

## Run

To compile and run the program use, use `make run`.

## Usage

`nhl [options]`

`nhl` on its own displays scores of recent and live NHL games

|  Option        |  Description                                                |
| -------------- | ----------------------------------------------------------- |
| `-l`, `--live` | Display scores of live NHL games                            |
| `-t`, `--team` | Display scores recent/live games of a team specified by its city or team name, and without spaces |
| `-r`, `--raw`  | Output raw JSON, beautified, from [live.nhle][] source      |
| `-h`, `--help` | Display command info and usage instructions                 |

## Todo

- Use [statsapi.web.nhl.com][] for all data

[live.nhle]: http://live.nhle.com/GameData/RegularSeasonScoreboardv3.jsonp
[statsapi.web.nhl.com]: https://statsapi.web.nhl.com/api/v1/schedule?startDate=2016-01-31&endDate=2016-02-05&expand=schedule.teams,schedule.linescore,schedule.broadcasts,schedule.ticket,schedule.game.content.media.epg&leaderCategories=&site=en_nhl&teamId=
