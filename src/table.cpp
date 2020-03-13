#include <stdlib.h> // rand()
#include <stdarg.h> // va_start, etc. 
#include <time.h>   // difftime()
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
  expire = time(nullptr);
  num_players = 0;
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

bool cTable::PlayerLink(cDescriptor &desc)
{
 cPlayer *player = desc.player;

 if (player == nullptr) return false;

 if (player->ID() == player_id[PLAYER_NORTH]) {
   player->table = this;
   player_desc[PLAYER_NORTH] = &desc;
   return true;
 }

 if (player->ID() == player_id[PLAYER_SOUTH]) {
   player->table = this;
   player_desc[PLAYER_SOUTH] = &desc;
   return true;
 }

 if (player->ID() == player_id[PLAYER_WEST]) {
   player->table = this;
   player_desc[PLAYER_WEST] = &desc;
   return true;
 }

 if (player->ID() == player_id[PLAYER_EAST]) {
   player->table = this;
   player_desc[PLAYER_EAST] = &desc;
   return true;
 }

 return false;
}

unsigned int cTable::TableID()
{
 return table_id;
}

bool cTable::Stand(cDescriptor &desc)
{
 if (player_desc[PLAYER_NORTH] == &desc) {
   player_desc[PLAYER_NORTH] = nullptr;
   player_id[PLAYER_NORTH] = NOPLAYER;
   descriptor_list->Send_To_All("%s %d n", PLAYER_STAND, table_id);
   num_players--;
   expire = time(nullptr);
   return true;
 }
 if (player_desc[PLAYER_SOUTH] == &desc) {
   player_desc[PLAYER_SOUTH] = nullptr;
   player_id[PLAYER_SOUTH] = NOPLAYER;
   descriptor_list->Send_To_All("%s %d s", PLAYER_STAND, table_id);
   num_players--;
   expire = time(nullptr);
   return true;
 }
 if (player_desc[PLAYER_WEST] == &desc) {
   player_desc[PLAYER_WEST] = nullptr;
   player_id[PLAYER_WEST] = NOPLAYER;
   descriptor_list->Send_To_All("%s %d w", PLAYER_STAND, table_id);
   num_players--;
   expire = time(nullptr);
   return true;
 }
 if (player_desc[PLAYER_EAST] == &desc) {
   player_desc[PLAYER_EAST] = nullptr;
   player_id[PLAYER_EAST] = NOPLAYER;
   descriptor_list->Send_To_All("%s %d e", PLAYER_STAND, table_id);
   num_players--;
   expire = time(nullptr);
   return true;
 }
 return false;
}

void cTable::Sit(cDescriptor &desc, unsigned int chair)
{
 char c;

 switch (chair) {
   case PLAYER_NORTH: c = 'n'; break;
   case PLAYER_SOUTH: c = 's'; break;
   case PLAYER_WEST:  c = 'w'; break;
   case PLAYER_EAST:  c = 'e'; break; 
   default: Log.Write("SYSERR: sit_player invalid chair.");
 }

 // is the chair free?
 if (player_desc[chair] == nullptr) {
    // do we have a delay before sitting again?
    if (desc.Get_Sit_Time() && (difftime(time(nullptr), desc.Get_Sit_Time()) <= SIT_DELAY)) {
      desc.Socket_Write(PLAYER_SIT_DELAY);
      return;
    } else
        desc.Set_Sit_Time(time(nullptr));

   // Calling Stand() for possible switching chair.
   Stand(desc);

   player_desc[chair] = &desc;
   player_id[chair] = desc.player->ID();
   descriptor_list->Send_To_All("%s %d %c %s", PLAYER_SIT_HERE, table_id, c, desc.player->Handle());
   expire = time(nullptr);
   num_players++;
 }
}

void cTable::Sat(cDescriptor &desc)
{
  struct cPlayer *player;

  if ((player = Player(PLAYER_NORTH)) != nullptr) 
    desc.Socket_Write("%s %d n %s", PLAYER_SIT_HERE, TableID(), player->Handle());
    
  if ((player = Player(PLAYER_SOUTH)) != nullptr) 
    desc.Socket_Write("%s %d s %s", PLAYER_SIT_HERE, TableID(), player->Handle());
	    
  if ((player = Player(PLAYER_WEST)) != nullptr) 
    desc.Socket_Write("%s %d w %s", PLAYER_SIT_HERE, TableID(), player->Handle());

  if ((player = Player(PLAYER_EAST)) != nullptr) 
    desc.Socket_Write("%s %d e %s", PLAYER_SIT_HERE, TableID(), player->Handle());
}

unsigned int cTable::Flags()
{
 return flags;
}

time_t cTable::Expire()
{
 return expire;
}

unsigned int cTable::Num_Players()
{
 return num_players;
}

void cTable::set_flags(unsigned int f)
{
  flags = f;
}

cPlayer *cTable::Player(unsigned int chair)
{
  if (player_desc[chair])
    return player_desc[chair]->player;
  else
    return nullptr;
}

bool cTable::Full()
{
  if (player_cards[PLAYER_NORTH] == nullptr) return false;
  if (player_cards[PLAYER_SOUTH] == nullptr) return false;
  if (player_cards[PLAYER_WEST] == nullptr) return false;
  if (player_cards[PLAYER_EAST] == nullptr) return false;

  return true;
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

  descriptor_list->Send_To_All("%s %d %d", TABLE_CREATED, elem->TableID(), elem->Flags());

  return ( true );
}

cTabList::cTabList()
{
 num_elem = 0;
 head = nullptr;
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

      descriptor_list->ULink_TableID(Q->elem->TableID());
      delete Q->elem;
      delete Q;
      num_elem--;

      descriptor_list->Send_To_All("%s %d", TABLE_DELETED, elem->TableID());
      return ( true );
    }
    prev = Q;
  }
  return false;
}

cTable *cTabList::Search(unsigned int id)
{
 struct sList *Q = head;

 while ( Q ) {
   if (Q->elem->TableID() == id)
     return Q->elem;
   Q = Q->next;
 }
 return nullptr;
}

bool cTabList::Empty()
{
 struct sList *Q = head, *prev;

 while ( Q ) {
   prev = Q;
   Q = Q->next;
   num_elem--;
   delete prev;
 }
 return true;
}

void cTabList::Remove_Expired() 
{
  struct sList *Q = head, *N;

  while ( Q ) {
    N = Q->next;

    if (!Q->elem->Num_Players() && (difftime(time(nullptr), Q->elem->Expire()) >= TABLE_EXPIRE))
      Remove(Q->elem);
    Q = N;
  }
}

void cTabList::List(cDescriptor &desc)
{
  struct sList *Q = head;

  while ( Q ) {
    desc.Socket_Write("%s %d %d", TABLE_CREATED, Q->elem->TableID(), Q->elem->Flags());

    if (Q->elem->PlayerLink(desc))
      desc.Socket_Write("%s %d", PLAYER_CHOOSE_CHAIR, Q->elem->TableID());

    Q->elem->Sat(desc);

    Q = Q->next;
  }
}

