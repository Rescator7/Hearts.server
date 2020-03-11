#ifndef _PLAYER_
#define _PLAYER_

#include "table.h"

#define PLAYER_NORTH 0
#define PLAYER_SOUTH 1
#define PLAYER_WEST  2
#define PLAYER_EAST  3

class cPlayer {
public:
  cPlayer();
  ~cPlayer();

  cTable *table;
  unsigned int playerid;
  char *handle;
  char *realname;
  char *email;
  char *password;

  char prompt   [MAX_PROMPT_LENGTH];
  char salt     [3];

public:
  void setPassword( const char * p, bool encrypt );
  bool doesPasswordMatch( const char * p );
  bool isHandle( const char * h );
  bool save();
  bool load();
  unsigned int get_playerid();
};

#endif // _PLAYER_
