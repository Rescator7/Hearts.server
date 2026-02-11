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

#define CMD_LASTLOGIN  1
#define CMD_TOTALTIME  2
#define CMD_FIRST      3
#define CMD_SECOND     4
#define CMD_THIRD      5
#define CMD_FOURTH     6
#define CMD_UUID       7

class cPlayer {
public:
  cPlayer();
  ~cPlayer();

  cTable *table;

private:
  unsigned int player_id;
  unsigned int level;
  time_t login_time;
  char handle[MAX_HANDLE_LENGTH+1];
  char realname[MAX_REALNAME_LENGTH+1];
  char email[MAX_EMAIL_LENGTH+1];
  char password[SHA_PASSWORD_SIZE+1];
  char uuid[UUID_LENGTH+1];
  char *ip;
  long int first;
  long int second;
  long int third;
  long int fourth;

public:
  void setPassword( const char *p );
  void NewPassword( const char *p );
  void update(usINT cmd);
  void Set_Handle(const char *h);
  void Set_Password(const char *p);
  void Set_UUID(const char *u);
  void Set_Level(usINT l);
  void Set_Ip(char *_ip);
  void ULink_Table(unsigned int id);
  bool doesPasswordMatch( const char *p );
  bool isHandle( const char *h );
  bool save();
  bool load();
  unsigned int SQL_ID();
  unsigned int ID();
  unsigned int Level();
  char *Handle();
  char *Password();
  char *Ip();
  char *UUID() { return uuid; };
  long int First() { return first; };
  long int Second() { return second; };
  long int Third() { return third; };
  long int Fourth() { return fourth; };
};

#endif // _PLAYER_
