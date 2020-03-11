#include <stdlib.h> // rand()
#include <stdarg.h> // va_start, etc. 
#include "comm.h"
#include "table.h"
#include "define.h"
#include "global.h"
#include "player.h"
#include "log.h"
#include "errors.h"

cTable::cTable(cDescriptor &desc)
{
  muted = false;
  owner = &desc;
  table_id = ++num_table;
  flags = 0;
  player_id[PLAYER_NORTH] = NOPLAYER;
  player_id[PLAYER_SOUTH] = NOPLAYER;
  player_id[PLAYER_WEST] = NOPLAYER;
  player_id[PLAYER_EAST] = NOPLAYER;
  player_desc[PLAYER_NORTH] = nullptr;
  player_desc[PLAYER_SOUTH] = nullptr;
  player_desc[PLAYER_WEST] = nullptr;
  player_desc[PLAYER_EAST] = nullptr;
  
  generate_cards();
}

cTable::~cTable()
{
}

void cTable::Send(const char *format, ...)
{
 va_list args;
 char buf [10 * 1024];

 va_start(args, format);
 vsprintf(buf, format, args);

 if (player_desc[PLAYER_NORTH]) player_desc[PLAYER_NORTH]->Socket_Write(buf);
 if (player_desc[PLAYER_SOUTH]) player_desc[PLAYER_SOUTH]->Socket_Write(buf);
 if (player_desc[PLAYER_WEST])  player_desc[PLAYER_WEST]->Socket_Write(buf);
 if (player_desc[PLAYER_EAST])  player_desc[PLAYER_EAST]->Socket_Write(buf);

 va_end(args);
}

void cTable::sit_player(cDescriptor &desc, unsigned int chair)
{
 // is the chair free?
 char c;

 switch (chair) {
   case PLAYER_NORTH: c = 'n'; break;
   case PLAYER_SOUTH: c = 's'; break;
   case PLAYER_WEST:  c = 'w'; break;
   case PLAYER_EAST:  c = 'e'; break; 
   default: Log.Write("SYSERR: sit_player invalid chair.");
 }

 if (player_desc[chair] == nullptr) {
   if ((chair != PLAYER_NORTH) && (player_desc[PLAYER_NORTH] == &desc)) {
     Send("%s n", PLAYER_STAND);
     player_desc[PLAYER_NORTH] = nullptr;
     player_id[PLAYER_NORTH] = NOPLAYER;
   }
   if ((chair != PLAYER_SOUTH) && (player_desc[PLAYER_SOUTH] == &desc)) {
     Send("%s s", PLAYER_STAND);
     player_desc[PLAYER_SOUTH] = nullptr;
     player_id[PLAYER_SOUTH] = NOPLAYER;
   }
   if ((chair != PLAYER_WEST) && (player_desc[PLAYER_WEST] == &desc)) {
     Send("%s w", PLAYER_STAND);
     player_desc[PLAYER_WEST] = nullptr;
     player_id[PLAYER_WEST] = NOPLAYER;
   }
   if ((chair != PLAYER_EAST) && (player_desc[PLAYER_EAST] == &desc)) {
     Send("%s e", PLAYER_STAND);
     player_desc[PLAYER_EAST] = nullptr;
     player_id[PLAYER_EAST] = NOPLAYER;
   }

   player_desc[chair] = &desc;
   player_id[chair] = desc.player->get_playerid();
   Send("%s %c %s", PLAYER_SIT_HERE, c, desc.player->handle);
 }
}

void cTable::set_flags(unsigned int f)
{
  flags = f;
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
  Log.Write("SOCKETS: created table: %d", num_elem);
  return ( true );
}

cTabList::cTabList()
{
 num_elem = 0;
}

cTabList::~cTabList()
{
  Empty();
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

bool cTabList::Empty()
{
 struct sList *Q, *prev;

 Q = head;
 while ( Q ) {
   prev = Q;
   Q = Q->next;
   num_elem--;
   delete prev;
 }
 return ( true );
}
