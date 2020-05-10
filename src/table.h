#ifndef _TABLE_
#define _TABLE_

#include "comm.h"

class cTable {
public:
  cTable(cDescriptor &desc, int f);
  ~cTable();

private:
  cDescriptor *player_desc[4];
  cDescriptor *owner;
  unsigned int table_id;
  unsigned int flags;
  unsigned int player_id[4];
  unsigned int num_players;
  char player_name[4][20] = {0};
  bool muted;
  bool paused;
  time_t expire;

public:
  class cGame *game;

  cPlayer *Player(unsigned int chair);
  unsigned int TableID();
  unsigned int Flags();
  unsigned int Num_Players();
  time_t Expire();
  void Sit(cDescriptor &desc, unsigned int chair);
  void Say(cDescriptor &desc, const char *message);
  void Send(usINT chair, const char *format, ... );
  void SendAll(const char *format, ... );
  void Sat(cDescriptor &desc);
  void Mute();
  void Clear();
  void Pause(bool pause);
  void Bot();
  bool Stand(cDescriptor &desc, bool leave);
  bool Full();
  bool Muted();
  bool Paused();
  bool PlayerSat(cDescriptor &desc);
  bool Owner(cDescriptor &d);
  usINT Chair(cDescriptor &desc);
  usINT PlayerLink(cDescriptor &desc);
  usINT TimeLeft(usINT chair);
  usINT NumPlayers();
  usINT PID(usINT chair);
  cDescriptor *desc(usINT chair);
  char *Name(usINT chair);
};

class cTabList {
public:
  cTabList();
  ~cTabList();

private:
  int num_elem;
  struct sList {
     cTable *elem;
     struct sList *next;
  };
  struct sList *head;

public:
  bool Add(cTable *elem);
  bool Empty(); 
  bool Remove(cTable *elem);
  void Remove_Expired();
  void Play();
  void List(cDescriptor &desc);
  void Show(cDescriptor &desc);
  cTable *Search(unsigned int id);
};
#endif // _TABLE_
