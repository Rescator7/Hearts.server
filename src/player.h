#ifndef _PLAYER_
#define _PLAYER_

#include "table.h"

#define PLAYER_NOWHERE  4
#define PLAYER_NORTH    2
#define PLAYER_SOUTH    0
#define PLAYER_WEST     1
#define PLAYER_EAST     3

#define Direction(a, b) if((a) == 'n') (b) = PLAYER_NORTH; \
                                else if ((a) == 's') (b) = PLAYER_SOUTH; \
                                else if ((a) == 'w') (b) = PLAYER_WEST; \
                                else if ((a) == 'e') (b) = PLAYER_EAST; \
                                else (b) = PLAYER_NOWHERE;

class cPlayer {
public:
  cPlayer();
  ~cPlayer();

  cTable *table;

private:
  unsigned int player_id;
  unsigned int level;
  char *handle;
  char *realname;
  char *email;
  char *password;
  char *ip;

  char prompt   [MAX_PROMPT_LENGTH];
  char salt     [3];

public:
  void setPassword( const char * p, bool encrypt );
  bool doesPasswordMatch( const char * p );
  bool isHandle( const char * h );
  bool save();
  bool load();
  unsigned int SQL_ID();
  unsigned int ID();
  unsigned int Level();
  char *Handle();
  char *Password();
  char *Ip();
  void Set_Handle(char *h);
  void Set_Password(char *p);
  void Set_Ip(char *_ip);
  void ULink_Table(unsigned int id);
};

#endif // _PLAYER_
