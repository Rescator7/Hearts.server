#ifndef _TABLE_
#define _TABLE_

#include "comm.h"

class cTable {
public:
  cTable(cDescriptor &desc);
  ~cTable();

private:
  cDescriptor *player_desc[4];
  cDescriptor *owner;
  unsigned int table_id;
  unsigned int flags;
  unsigned int player_id[4];
  unsigned short int turn;
  unsigned short int player_cards[4][13];
  bool muted;

public:
  unsigned int TableID();
  void generate_cards();
  void set_flags(unsigned int flags);
  void sit_player(cDescriptor &desc, unsigned int chair);
  void Send(const char *format, ...);
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
  cTable *Search(unsigned int id);
};
#endif // _TABLE_
