#include <openssl/sha.h>
#include <cstring>
#include <cstdio>
#include <ctime>

#include "define.h"
#include "player.h"
#include "sql.h"
#include "string.h"

cPlayer::cPlayer()
{
  player_id = 0;
  level = 0;
  handle = nullptr;
  realname = nullptr;
  email = nullptr;
  password = nullptr;
  ip = nullptr;
  table = nullptr;
  first = 0;
  second = 0;
  third = 0;
  fourth = 0;
  login_time = time(nullptr);
}

cPlayer::~cPlayer()
{
  if (ip)
    free(ip);
}

bool cPlayer::doesPasswordMatch( const char *p )
{ 
  unsigned char obuf[SHA_DIGEST_LENGTH] = {0, };
  char pwd[SHA_DIGEST_LENGTH * 2 + 1] = {0, };

  SHA1((const unsigned char *)p, strlen(p), obuf);

  for (int i=0; i<SHA_DIGEST_LENGTH; i++)
    sprintf(pwd + i * 2, "%02x", obuf[i]); 

#ifdef DEBUG
  printf("sha: '%s'\r\n", pwd);
#endif
  return (!strcmp(password, pwd));
}

void cPlayer::setPassword( const char *p )
{
  unsigned char obuf[SHA_DIGEST_LENGTH] = {0, };
  char pwd[SHA_DIGEST_LENGTH * 2 + 1] = {0, };

  SHA1((const unsigned char*)p, strlen(p), obuf);

  for (int i=0; i<20; i++)
    sprintf(pwd + i * 2, "%02x", obuf[i]); 

  password = strdup(pwd);
}

void cPlayer::NewPassword(const char *p)
{
  if (password)
    free(password);

  setPassword(p);

  sql.query("update account set password = \"%s\" where playerid = %d;", password, player_id);
}

bool cPlayer::isHandle( const char *h ) 
{
  if ((h == nullptr) || (handle == nullptr)) return false;

  return (!strcasecmp(handle, h));
}

unsigned int cPlayer::SQL_ID()
{
  int id = 0;

  if (sql.query("select playerid from account where handle = '%s'", handle))
    id = atoi(sql.get_row(0));

  return id;
}

bool cPlayer::save()
{
#ifdef DEBUG
  printf ("%s %s %s %s %s\r\n", handle, password, ip, realname, email);
#endif

  return ( sql.query("insert into account values (0, '%s', '%s', '%s', '%s', '%s', now(), now(), 0, 1, 0, 0, 0, 0 )", handle, password, ip, realname, email ));
}

bool cPlayer::load()
{
  if (sql.query("select playerid, ip, userlevel, first, second, third, fourth from account where handle = '%s'", handle)) {
    player_id = atoi(sql.get_row(0));

    if (sql.get_row(1)) // can't strdup empty string
      ip = strdup(sql.get_row(1));

    level = atoi(sql.get_row(2));
    first = atoi(sql.get_row(3));
    second = atoi(sql.get_row(4));
    third = atoi(sql.get_row(5));
    fourth = atoi(sql.get_row(6));

#ifdef DEBUG
    if (ip)
      printf("Connected from: %s\r\n", ip);
  }
  printf("player id: %d, userlevel: %d\r\n", player_id, level);
#else
}
#endif

  return ( true );
}

void cPlayer::update(usINT cmd)
{
  long int diff;

  switch (cmd) {
    case CMD_LASTLOGIN: sql.query("update account set lastlogin = now() where playerid = %d;", player_id);
			break;
    case CMD_TOTALTIME: diff = difftime(time(nullptr), login_time);
			sql.query("update account set totaltime = totaltime + %d where playerid = %d;", diff, player_id);
			break;
    case CMD_FIRST:     sql.query("update account set first = first + 1 where playerid = %d;", player_id);
			first++;
			break;
    case CMD_SECOND:    sql.query("update account set second = second + 1 where playerid = %d;", player_id);
			second++;
			break;
    case CMD_THIRD:     sql.query("update account set third = third + 1 where playerid = %d;", player_id);
                        third++;
			break;
    case CMD_FOURTH:    sql.query("update account set fourth = fourth + 1 where playerid = %d;", player_id);
			fourth++;
			break;
  }
}

unsigned int cPlayer::Level()
{
  return level;
}

unsigned int cPlayer::ID()
{
  return player_id;
}

char *cPlayer::Handle()
{
  return handle;
}

char *cPlayer::Password()
{
  return password;
}

char *cPlayer::Ip()
{
  return ip;
}

void cPlayer::Set_Handle(char *h)
{
  handle = h;
}

void cPlayer::Set_Password(char *p)
{
  password = p;
}

void cPlayer::Set_Level(usINT l)
{
  level = l;
}

void cPlayer::Set_Ip(char *_ip)
{
  ip = strdup(_ip);
}

void cPlayer::ULink_Table(unsigned int id)
{
  if (table && (table->TableID() == id))
    table = nullptr;
}

long int cPlayer::First()
{
  return first;
}

long int cPlayer::Second()
{
  return second;
}

long int cPlayer::Third()
{
  return third;
}

long int cPlayer::Fourth()
{
  return fourth;
}
