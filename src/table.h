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
  unsigned int time_bank[4];
  char player_name[4][20] = {0};
  bool muted;
  bool paused;
  bool bot[4];
  time_t expire;
  time_t time_paused;

public:
  class cGame *game;

  cPlayer *Player(unsigned int chair);
  unsigned int TableID();
  unsigned int Flags();
  unsigned int Num_Players();
  unsigned int Time_Bank(usINT chair);
  time_t Expire();
  time_t Time_Paused();
  void Sit(cDescriptor &desc, unsigned int chair);
  void AutoSit(cDescriptor &desc);
  void Say(cDescriptor &desc, const char *message);
  void Send(usINT chair, const char *format, ... );
  void SendAll(const char *format, ... );
  void Sat(cDescriptor &desc);
  void Reset_Time_Bank();
  void Mute();
  void Clear();
  void Pause(bool pause);
  void Fill_Bot();
  void Adjust_Time_Bank(usINT chair);
  void Purge_Time_Bank(usINT chair);
  void Default();
  bool Stand(cDescriptor &desc, int flag);
  bool Full();
  bool Muted();
  bool Paused();
  bool PlayerSat(cDescriptor &desc);
  bool Owner(cDescriptor &d);
  bool isEmpty();
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
  bool Purge(usINT tableID);
  bool Remove(cTable *elem);
  bool Available();
  void Remove_Expired();
  void Play();
  void List(cDescriptor &desc);
  void Show(cDescriptor &desc);
  void Clean();
  void Create_Default_Table();
  cTable *Search(unsigned int id);
};
#endif // _TABLE_
