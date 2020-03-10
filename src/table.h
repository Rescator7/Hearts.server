#ifndef _TABLE_
#define _TABLE_

class cTable {
public:
  cTable(unsigned int oID);
  ~cTable();

public:
  unsigned int table_id;
  unsigned int owner_id;
  unsigned int table_flags;
  unsigned int player_up;
  unsigned int player_down;
  unsigned int player_left;
  unsigned int player_right;
  unsigned short int turn;
  unsigned short int player_cards[4][13];
  bool muted;

public:
  void generate_cards();
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
  bool Remove(cTable *elem);
};
#endif // _TABLE_
