#include <stdlib.h> // rand()
#include "table.h"
#include "define.h"
#include "global.h"
#include "log.h"

cTable::cTable(unsigned int oID)
{
 muted = false;
 owner_id = oID;
 table_id = ++num_table;
 table_flags = 0;
 player_up = NOPLAYER;
 player_down = NOPLAYER;
 player_left = NOPLAYER;
 player_right = NOPLAYER;

 generate_cards();
}

cTable::~cTable()
{
}

void cTable::generate_cards()
{
 bool card_free[DECK_SIZE];

 for (int i=0; i < DECK_SIZE; i++)
   card_free[i] = true;

 int player = 0;
 for (int i=0; i< DECK_SIZE; i++) {
   int card = rand() % DECK_SIZE;
   while (!card_free[card])
     card = rand() % DECK_SIZE;
   if (card == two_clubs)
     turn = player;
   card_free[card] = false;
   player_cards[player][i / 4] = card;
   if (player++ == 3)
     player = 0;
 }
}

bool cTabList::Add(cTable *elem)
{
 struct sList *Q;

 Q = new (struct sList);
 Q->elem = elem;

 if (head == NULL) {
   Q->next = NULL;
   head = Q;
 } else {
     Q->next = head;
     head = Q;
   }

 num_elem++;
 log.Write("SOCKETS: created table: %d", num_elem);
 return ( true );
}

bool cTabList::Remove(cTable *elem)
{
 struct sList *Q, *prev = NULL;

 for (Q = head; Q; Q = Q->next) {
   if (Q->elem == elem) {
     if ( prev )
       prev->next = Q->next;
     else
       head = Q->next;
     delete Q->elem;
     delete Q;
     num_elem--;
     return ( true );
   }
   prev = Q;
 }
 return ( false );
}

cTabList::cTabList()
{
}

cTabList::~cTabList()
{
}

