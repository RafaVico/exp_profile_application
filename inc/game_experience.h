#ifndef GAME_EXPERIENCE_H
#define GAME_EXPERIENCE_H

struct game_summary
{
  int won;
  int won_max;
  int total;
  int total_max;
  int played;
};

struct experience
{
  int id;
  int value;
  int secret;
  int won;
};

struct experience_lang
{
  int id;
  char name[21];
  char desc[101];
};

struct game_value
{
  long int value;
  std::string directory;
};

#endif
