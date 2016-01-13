/*
 * nhl is a CLI for the score of live and recent NHL games.
 *
 * Dependencies: jansson, libcurl
 *
 * request and write_response functions adapted from Petri Lehtinen's
 * github_commits.c tutorial from the Jansson documentation.
 *
 *   https://jansson.readthedocs.org/en/2.7/tutorial.html
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <curl/curl.h>
#include <jansson.h>

#define HELP \
"nhl is a command line interface for live and recent NHL game scores.\n\
\n\
Usage:\n\
\n\
  nhl [options]\n\
\n\
nhl command on its own displays scores of recent and live NHL games\n\
\n\
Options:\n\
\n\
  -l, --live\n\
  Display scores of live NHL games\n\
\n\
  -t, --team <team>\n\
  Display scores recent/live games of team <team>\n\
  <team> can be either the city or team name, but must not include any spaces\n\
\n\
  -r, --raw\n\
  Output raw JSON, beautified, from live.nhle source\n\
\n\
  -h, --help\n\
  Display command info and usage instructions\n\
\n"

// JSON info
#define JSON_URL \
  "http://live.nhle.com/GameData/RegularSeasonScoreboardv3.jsonp"

#define JSON_BUFFER (16 * 1024)   // 16KB
#define JSON_PREFIX 15
#define JSON_SUFFIX 1

// JSON keys
#define GAMES_ARRAY "games"
#define AWAY_NAME   "atv"
#define AWAY_CITY   "atn"
#define AWAY_SCORE  "ats"
#define HOME_NAME   "htv"
#define HOME_CITY   "htn"
#define HOME_SCORE  "hts"
#define PREGAME     "ts"
#define LIVE        "bs"
#define FINAL       "tsc"

// Output colours
#define KGRN  "\x1B[32m"
#define KYEL  "\x1b[33m"
#define RESET "\033[0m"

struct write_result {
  char *data;
  int pos;
};

static size_t write_response(
  void *ptr, size_t size, size_t nmemb, void *stream) {

  struct write_result *result = (struct write_result *)stream;

  if (result->pos + size * nmemb >= JSON_BUFFER - 1) {
    fprintf(stderr, "error: too small buffer\n");
    return 0;
  }

  memcpy(result->data + result->pos, ptr, size * nmemb);
  result->pos += size * nmemb;

  return size * nmemb;
}

static char *request(const char *url) {
  CURL *curl = NULL;
  CURLcode status;
  char *data = NULL;
  long code;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if(!curl) {
    goto error;
  }

  data = malloc(JSON_BUFFER);
  if(!data) {
    goto error;
  }

  struct write_result write_result = {
    .data = data,
    .pos = 0
  };

  curl_easy_setopt(curl, CURLOPT_URL, url);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_result);

  status = curl_easy_perform(curl);
  if (status != 0) {
    fprintf(stderr, "error: unable to request data from %s:\n", url);
    fprintf(stderr, "%s\n", curl_easy_strerror(status));
    goto error;
  }

  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
  if (code != 200) {
    fprintf(stderr, "error: server responded with code %ld\n", code);
    goto error;
  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  data[write_result.pos] = '\0';

  return data;

  error:
    if(data) {
      free(data);
    }
    if(curl) {
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return NULL;
}

// Returns true iff str1 and str2 are equal
bool streq(const char *str1, const char *str2) {
  return (strcmp(str1, str2) == 0);
}

// If 'c' is an uppercase character, returns the
//   corresponding lowercase character
char to_lowercase(char c) {
  if ((c >= 'A') && (c <= 'Z')) {
    return c - 'A' + 'a';
  } else {
    return c;
  }
}

// Returns a string that is str with all uppercase
//   characters converted to lowercase
char *lowercase(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = to_lowercase(str[i]);
  }
  return str;
}

// Returns new allocated string that is str in lowercase
//   and without any space characters
char *new_lowercase_nospace(const char *str) {
  int len = strlen(str);
  char *lowercase = malloc(sizeof(char) * (len + 1));

  int j = 0;
  for (int i = 0; i < len; i++) {
    if (str[i] == ' ') {
      j++;
    }

    lowercase[i] = to_lowercase(str[j]);
    j++;
  }
  lowercase[len] = '\0';

  return lowercase;
}

// Returns true iff the provided game is live / in progress
bool is_live(json_t *game) {
  return streq(json_string_value(json_object_get(game, "bs")), "LIVE")
    && !streq(json_string_value(json_object_get(game, "ts")), "PRE GAME");
}

// Returns true iff the provided game has completed
bool is_over(json_t *game) {
  return streq(json_string_value(json_object_get(game, "tsc")), "final");
}

// Prints select game information
void print_game(json_t *game) {
  const char *away_name = json_string_value(
    json_object_get(game, AWAY_NAME)
  );

  int away_score = atoi(json_string_value(
    json_object_get(game, AWAY_SCORE))
  );

  const char *home_name = json_string_value(
    json_object_get(game, HOME_NAME)
  );

  int home_score = atoi(json_string_value(
    json_object_get(game, HOME_SCORE))
  );

  printf(KYEL "%s %s @ %s %s\n" RESET,
    json_string_value(json_object_get(game, AWAY_CITY)), away_name,
    json_string_value(json_object_get(game, HOME_CITY)), home_name
  );

  printf("%s %s\n",
    json_string_value(json_object_get(game, "ts")),
    json_string_value(json_object_get(game, "bs"))
  );

  if (is_over(game) || is_live(game)) {
    if (away_score > home_score) {
      printf(KGRN "%s: %d\n" RESET, away_name, away_score);
      printf("%s: %d\n\n", home_name, home_score);

    } else if (home_score > away_score) {
      printf("%s: %d\n", away_name, away_score);
      printf(KGRN "%s: %d\n\n" RESET, home_name, home_score);

    } else {
      printf("%s: %d\n%s: %d\n\n",
        away_name, away_score, home_name, home_score
      );
    }
  } else {
    printf("\n");
  }
}

// Returns true iff 'team' is participating in 'game'
bool is_team_in_game(json_t *game, char *team) {
  char *away_city = new_lowercase_nospace(
    json_string_value(json_object_get(game, AWAY_CITY))
  );
  const char *away_name = json_string_value(
    json_object_get(game, AWAY_NAME)
  );

  char *home_city = new_lowercase_nospace(
    json_string_value(json_object_get(game, HOME_CITY))
  );
  const char *home_name = json_string_value(
    json_object_get(game, HOME_NAME)
  );

  bool res = streq(away_city, team) || streq(away_name, team)
          || streq(home_city, team) || streq(home_name, team);

  free(away_city);
  free(home_city);

  return res;
}

// Returns new allocated string that is the substring of str that
//   results from removing 'front' characters from the beginning
//   of the string and 'back' characters from the end
char *new_substring(char *str, int front, int back) {
  int str_len = strlen(str);
  int substring_len = str_len - front - back;

  char *substring = malloc(sizeof(char) * (substring_len + 1));
  if (!substring) {
    return NULL;
  }

  int i = front;
  for (int j = 0; j <= substring_len; j++) {
    substring[j] = str[i];
    i++;
  }
  substring[substring_len] = '\0';

  return substring;
}

int main(int argc, char *argv[]) {

  bool live = false, raw = false, team = false;
  if (argc == 2) {
    if (streq(argv[1], "-h") || streq(argv[1], "--help")) {
      printf("%s", HELP);
      return 0;
    }

    if (streq(argv[1], "-l") || streq(argv[1], "--live")) live = true;
    if (streq(argv[1], "-r") || streq(argv[1], "--raw"))  raw  = true;

  } else if (argc == 3) {
    if (streq(argv[1], "-t") || streq(argv[1], "--team")) team = true;

  } else if (argc > 3) {
    fprintf(stderr, "Incorrect arguments, see '%s -h' for usage\n", argv[0]);
    return 2;
  }

  if ((argc == 1) || live || raw || team) {

    char *jsonp = request(JSON_URL);
    if (!jsonp) {
      return -1;
    }

    int jsonp_len = strlen(jsonp);
    char *json_raw = new_substring(jsonp, JSON_PREFIX, JSON_SUFFIX);
    free(jsonp);

    json_t *json;
    json_error_t error;
    json = json_loads(json_raw, 0, &error);
    free(json_raw);

    if (!json) {
      fprintf(stderr,
        "Error processing JSON\n line %d: %s\n", error.line, error.text
      );

      return -1;
    }

    if (raw) {
      printf("%s\n", json_dumps(json,
        JSON_INDENT(2)  | JSON_ENSURE_ASCII | JSON_PRESERVE_ORDER)
      );

      json_decref(json);
      return 0;
    }

    json_t *games = json_object_get(json, "games");
    if (!(games && json_is_array(games))) {
      json_decref(json);
      fprintf(stderr, "%s\n", "error");
      return -1;
    }

    int games_count = json_array_size(games);
    for (int i = 0; i < games_count; i++) {
      json_t *game = json_array_get(games, i);
      if ((live && !is_live(game))
        || (team && !is_team_in_game(game, lowercase(argv[2])))) {
        continue;
      }
      print_game(game);
    }

    json_decref(json);

  } else {
    fprintf(stderr, "Incorrect arguments, see '%s -h' for usage\n", argv[0]);
    return 2;
  }
}
