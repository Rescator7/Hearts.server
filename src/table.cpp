#include <cstdarg> // va_start, etc. 
#include "comm.h"
#include "define.h"
#include "global.h"
#include "player.h"
#include "log.h"
#include "datagrams.h"
#include "game.h"
#include "table.h"
#include "sql.h"
#include "config.h"

cTable::cTable(cDescriptor &desc, int f)
{
  muted = false;
  paused = false;
  owner = &desc;
  table_id = ++num_table;
  flags = f;
  expire = time(nullptr);

  Clear();
  game = new cGame(f);
}

cTable::~cTable()
{
  delete game;
}

void cTable::Send(usINT chair, const char *format, ... )
{
  struct cDescriptor *desc = player_desc[chair];

  if (desc == nullptr) return;

  va_list args;
  char buffer [BUF_SIZE];

  va_start(args, format);
  vsnprintf(buffer, BUF_SIZE, format, args);

  desc->Socket_Write((const char *) &buffer);  

  va_end(args);
}

void cTable::Say(cDescriptor &desc, const char *message)
{
 struct cDescriptor *player;

 if (muted) {
   desc.Socket_Write(TABLE_MUTED);
   return;
 }

 player = player_desc[PLAYER_NORTH];
 if ((player != nullptr) && (player != &desc))
   player->Socket_Write("%s %s %s", TABLE_SAY, desc.player->Handle(), message);

 player = player_desc[PLAYER_SOUTH];
 if ((player != nullptr) && (player != &desc))
   player->Socket_Write("%s %s %s", TABLE_SAY, desc.player->Handle(), message);

 player = player_desc[PLAYER_WEST]; 
 if ((player != nullptr) && (player != &desc))
   player->Socket_Write("%s %s %s", TABLE_SAY, desc.player->Handle(), message);

 player = player_desc[PLAYER_EAST]; 
 if ((player != nullptr) && (player != &desc))
   player->Socket_Write("%s %s %s", TABLE_SAY, desc.player->Handle(), message);
}

usINT cTable::PlayerLink(cDescriptor &desc)
{
 cPlayer *player = desc.player;

 if (player == nullptr) return PLAYER_NOWHERE;

 if (player->ID() == player_id[PLAYER_NORTH]) {
   player->table = this;
   player_desc[PLAYER_NORTH] = &desc;
   num_players++;
   return PLAYER_NORTH;
 }

 if (player->ID() == player_id[PLAYER_SOUTH]) {
   player->table = this;
   player_desc[PLAYER_SOUTH] = &desc;
   num_players++;
   return PLAYER_SOUTH;
 }

 if (player->ID() == player_id[PLAYER_WEST]) {
   player->table = this;
   player_desc[PLAYER_WEST] = &desc;
   num_players++;
   return PLAYER_WEST;
 }

 if (player->ID() == player_id[PLAYER_EAST]) {
   player->table = this;
   player_desc[PLAYER_EAST] = &desc;
   num_players++;
   return PLAYER_EAST;
 }

 return PLAYER_NOWHERE;
}

void cTable::Clear()
{
  player_id[PLAYER_NORTH] = NOPLAYER;
  player_id[PLAYER_SOUTH] = NOPLAYER;
  player_id[PLAYER_WEST] = NOPLAYER;
  player_id[PLAYER_EAST] = NOPLAYER;
  player_desc[PLAYER_NORTH] = nullptr;
  player_desc[PLAYER_SOUTH] = nullptr;
  player_desc[PLAYER_WEST] = nullptr;
  player_desc[PLAYER_EAST] = nullptr;
  num_players = 0;
}

bool cTable::Stand(cDescriptor &desc, bool leave)
{
 if (player_desc[PLAYER_NORTH] == &desc) {
   player_desc[PLAYER_NORTH] = nullptr;
   if (leave) {
     if (game->Started())
       desc.player->update(CMD_FOURTH);

     player_id[PLAYER_NORTH] = NOPLAYER;
   }
   descriptor_list->Send_To_All("%s %d n", PLAYER_STAND, table_id);
   num_players--;
   expire = time(nullptr);
#ifdef DEBUG
   printf("num_players: %d\r\n", num_players);
#endif
   return true;
 }
 if (player_desc[PLAYER_SOUTH] == &desc) {
   player_desc[PLAYER_SOUTH] = nullptr;
   if (leave) {
     if (game->Started())
       desc.player->update(CMD_FOURTH);
     player_id[PLAYER_SOUTH] = NOPLAYER;
   }
   descriptor_list->Send_To_All("%s %d s", PLAYER_STAND, table_id);
   num_players--;
#ifdef DEBUG
   printf("num_players: %d\r\n", num_players);
#endif
   expire = time(nullptr);
   return true;
 }
 if (player_desc[PLAYER_WEST] == &desc) {
   player_desc[PLAYER_WEST] = nullptr;
   if (leave) {
     if (game->Started())
       desc.player->update(CMD_FOURTH);
     player_id[PLAYER_WEST] = NOPLAYER;
   }
   descriptor_list->Send_To_All("%s %d w", PLAYER_STAND, table_id);
   num_players--;
#ifdef DEBUG
   printf("num_players: %d\r\n", num_players);
#endif
   expire = time(nullptr);
   return true;
 }
 if (player_desc[PLAYER_EAST] == &desc) {
   player_desc[PLAYER_EAST] = nullptr;
   if (leave) {
     if (game->Started())
       desc.player->update(CMD_FOURTH);
     player_id[PLAYER_EAST] = NOPLAYER;
   }
   descriptor_list->Send_To_All("%s %d e", PLAYER_STAND, table_id);
   num_players--;
#ifdef DEBUG
   printf("num_players: %d\r\n", num_players);
#endif
   expire = time(nullptr);
   return true;
 }
#ifdef DEBUG
 printf("(not found) num_players: %d\r\n", num_players);
 Log.Write("SYSERR: Stand: Player not found! (%d)", num_players);
#endif

 return false;
}

void cTable::Sit(cDescriptor &desc, unsigned int chair)
{
  if (game->Started()) {
    desc.Socket_Write(TABLE_STARTED);
    return;
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
   Stand(desc, true);

   char c;
   switch (chair) {
     case PLAYER_NORTH: c = 'n'; break;
     case PLAYER_SOUTH: c = 's'; break;
     case PLAYER_WEST:  c = 'w'; break;
     case PLAYER_EAST:  c = 'e'; break; 
     default: Log.Write("SYSERR: sit_player invalid chair.");
	      return;
   }

   player_desc[chair] = &desc;
   player_id[chair] = desc.player->ID();
   Send(chair, "%s %d", TABLE_WHO_AM_I, chair);
   descriptor_list->Send_To_All("%s %d %c %d %s", PLAYER_SIT_HERE, table_id, c, muted, desc.player->Handle());
   expire = time(nullptr);
   num_players++;

   if (num_players == 4)
     game->Start();
 } else
     if (player_desc[chair] == &desc) Stand(desc, true);
}

void cTable::Sat(cDescriptor &desc)
{
  struct cPlayer *player;

  if ((player = Player(PLAYER_NORTH)) != nullptr) 
    desc.Socket_Write("%s %d n %d %s", PLAYER_SIT_HERE, TableID(), muted, player->Handle());
    
  if ((player = Player(PLAYER_SOUTH)) != nullptr) 
    desc.Socket_Write("%s %d s %d %s", PLAYER_SIT_HERE, TableID(), muted, player->Handle());
	    
  if ((player = Player(PLAYER_WEST)) != nullptr) 
    desc.Socket_Write("%s %d w %d %s", PLAYER_SIT_HERE, TableID(), muted, player->Handle());

  if ((player = Player(PLAYER_EAST)) != nullptr) 
    desc.Socket_Write("%s %d e %d %s", PLAYER_SIT_HERE, TableID(), muted, player->Handle());
}

usINT cTable::Chair(cDescriptor &desc)
{
 if (player_desc[PLAYER_NORTH] == &desc) return PLAYER_NORTH;
 if (player_desc[PLAYER_SOUTH] == &desc) return PLAYER_SOUTH;
 if (player_desc[PLAYER_WEST] == &desc) return PLAYER_WEST;
 if (player_desc[PLAYER_EAST] == &desc) return PLAYER_EAST;

 return PLAYER_NOWHERE;
}

void cTable::SendAll(const char *format, ... )
{
  va_list args;
  char buffer [BUF_SIZE];

  va_start(args, format);
  vsnprintf(buffer, BUF_SIZE, format, args);

  if (player_desc[PLAYER_NORTH] != nullptr) player_desc[PLAYER_NORTH]->Socket_Write(buffer);	
  if (player_desc[PLAYER_SOUTH] != nullptr) player_desc[PLAYER_SOUTH]->Socket_Write(buffer);	
  if (player_desc[PLAYER_WEST] != nullptr) player_desc[PLAYER_WEST]->Socket_Write(buffer);	
  if (player_desc[PLAYER_EAST] != nullptr) player_desc[PLAYER_EAST]->Socket_Write(buffer);	

  va_end(args);
}

bool cTable::PlayerSat(cDescriptor &desc)
{
  if (player_desc[PLAYER_NORTH] == &desc) return true;
  if (player_desc[PLAYER_SOUTH] == &desc) return true;
  if (player_desc[PLAYER_WEST] == &desc) return true;
  if (player_desc[PLAYER_EAST] == &desc) return true;

  return false;
}

cPlayer *cTable::Player(unsigned int chair)
{
  if (player_desc[chair] != nullptr)
    return player_desc[chair]->player;
  else
    return nullptr;
}

usINT cTable::PID(usINT chair)
{
  return player_id[chair];
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

unsigned int cTable::TableID()
{
 return table_id;
}

bool cTable::Full()
{
  return num_players == 4;
/*
  if (player_desc[PLAYER_NORTH] == nullptr) return false;
  if (player_desc[PLAYER_SOUTH] == nullptr) return false;
  if (player_desc[PLAYER_WEST] == nullptr) return false;
  if (player_desc[PLAYER_EAST] == nullptr) return false;

  return true;
*/
}

cDescriptor *cTable::desc(usINT chair)
{
  return player_desc[chair];
}

void cTable::Mute()
{
  muted = true;
}

bool cTable::Muted()
{
  return muted;
}

void cTable::Pause(bool pause)
{
  paused = pause;
}

bool cTable::Paused()
{
  return paused;
}

usINT cTable::NumPlayers()
{
  return num_players;
}
// *************************************************************************************************************************************

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

void cTabList::Play()
{
  struct sList *Q = head;
  struct cGame *game;
  struct cTable *table;
  int turn;
  int delay;

  while ( Q ) {
    table = Q->elem;
    game = table->game;

    if (game->Started() && !table->Paused()) {
      turn = game->Turn();

      switch (game->State()) {
	 case STATE_GAME_STARTED: descriptor_list->Send_To_All("%s %d", GAME_STARTED, table->TableID());
				  descriptor_list->Table_Kick_Unplaying(table);
	 case STATE_SEND_CARDS: if (game->PassTo() == pNOPASS) {
				  game->SetState(STATE_WAIT_PLAY);
				  delay = config.Wait_Play();
			          table->Send(turn, "%s %d", TABLE_YOUR_TURN, config.Wait_Play());
				}
				else {
				  game->SetState(STATE_WAIT_PASS);
	                          delay = config.Wait_Select();
				}
		                for (int player = 0; player < 4; player++)
	                           table->Send(player, "%s %d %d %s", TABLE_YOUR_CARDS, game->PassTo(), delay, game->Str_Cards(player));
				game->Wait(delay);
		                break;
	 case STATE_WAIT_PASS: if (game->WaitOver()) {
                                 if (!game->Passed(PLAYER_NORTH)) game->ForcePass(table, PLAYER_NORTH);
                                 if (!game->Passed(PLAYER_SOUTH)) game->ForcePass(table, PLAYER_SOUTH);
                                 if (!game->Passed(PLAYER_WEST)) game->ForcePass(table, PLAYER_WEST);
                                 if (!game->Passed(PLAYER_EAST)) game->ForcePass(table, PLAYER_EAST);
			       } 
			       break; 
	 case STATE_WAIT_PLAY: if ((table->Player(turn) == nullptr) || game->WaitOver())
				 game->ForcePlay(*table);
                               break;
	 case STATE_END_TURN:  if (!game->WaitOver()) break;
			       game->EndTurn(*table);
			       break; 
	 case STATE_END_ROUND: if (!game->WaitOver()) break;
			       game->EndRound(*table);
			       break;
	 case STATE_SHUFFLE:   table->SendAll(TABLE_SHUFFLE);
			       game->SetState(STATE_WAIT_ROUND);
                               game->Wait(config.Wait_End_Round());
			       break;
	 case STATE_WAIT_ROUND:
			       if (!game->WaitOver()) break;
			       game->SetState(STATE_SEND_CARDS);
			       break; 
	 case STATE_GAME_OVER: table->SendAll("%s %d %d %d %d", TABLE_GAMEOVER, game->Score(PLAYER_NORTH), game->Score(PLAYER_SOUTH), 
					                        game->Score(PLAYER_WEST), game->Score(PLAYER_EAST));
			       if (table->Player(PLAYER_NORTH)) table->Player(PLAYER_NORTH)->update(game->CMD_Rank(PLAYER_NORTH));
			       else if (table->PID(PLAYER_NORTH) != NOPLAYER) 
				      sql.query("update account set fourth = fourth + 1 where playerid = %d;", table->PID(PLAYER_NORTH));

			       if (table->Player(PLAYER_SOUTH)) table->Player(PLAYER_SOUTH)->update(game->CMD_Rank(PLAYER_SOUTH));
			       else if (table->PID(PLAYER_SOUTH) != NOPLAYER) 
				      sql.query("update account set fourth = fourth + 1 where playerid = %d;", table->PID(PLAYER_SOUTH));

			       if (table->Player(PLAYER_WEST)) table->Player(PLAYER_WEST)->update(game->CMD_Rank(PLAYER_WEST));
			       else if (table->PID(PLAYER_WEST) != NOPLAYER)
				      sql.query("update account set fourth = fourth + 1 where playerid = %d;", table->PID(PLAYER_WEST));

			       if (table->Player(PLAYER_EAST)) table->Player(PLAYER_EAST)->update(game->CMD_Rank(PLAYER_EAST));
			       else if (table->PID(PLAYER_EAST) != NOPLAYER)
				      sql.query("update account set fourth = fourth + 1 where playerid = %d;", table->PID(PLAYER_EAST));
			       table->Clear();
			       break;
	 case STATE_CORRUPTED: table->SendAll(TABLE_CORRUPTED);
			       table->Clear();
			       break;
      }
    }
    Q = Q->next;
  }
}

void cTabList::List(cDescriptor &desc)
{
  struct sList *Q = head;
  struct cTable *table;
  struct cGame *game;
  usINT chair;

  while ( Q ) {
    table = Q->elem;
    game = table->game;

    if (!game->Started())
      desc.Socket_Write("%s %d %d", TABLE_CREATED, table->TableID(), table->Flags());
    else
      if ((chair = table->PlayerLink(desc)) != PLAYER_NOWHERE) {
        desc.Socket_Write("%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s", 
  	                  DG_RECONNECTED, 
		          table->TableID(), 
		          game->Score(PLAYER_SOUTH), game->Score(PLAYER_WEST), game->Score(PLAYER_NORTH), game->Score(PLAYER_EAST),
			  game->HandScore(PLAYER_SOUTH), game->HandScore(PLAYER_WEST), game->HandScore(PLAYER_NORTH), game->HandScore(PLAYER_EAST),
			  game->Played(PLAYER_SOUTH), game->Played(PLAYER_WEST), game->Played(PLAYER_NORTH), game->Played(PLAYER_EAST),
			  chair, game->Status(chair), game->TimeLeft(chair), game->PassTo(), game->HeartBroken(), game->Str_Cards(chair));
    }

    table->Sat(desc);

    Q = Q->next;
  }
}

void cTabList::Show(cDescriptor &desc)
{
  const char *nobody = "<nobody>";
  const char *left = "<left>";
  struct sList *Q = head;
  struct cTable *table;
  struct cDescriptor *North, *South, *West, *East;
  const char *name_N, *name_S, *name_W, *name_E;
  bool started;

  while ( Q ) {
    table = Q->elem;
    started = table->game->Started();

    North = table->desc(PLAYER_NORTH);
    South = table->desc(PLAYER_SOUTH);
    West = table->desc(PLAYER_WEST);
    East = table->desc(PLAYER_EAST);

    name_N = (North && North->player) ? North->player->Handle() : started ? left : nobody;
    name_S = (South && South->player) ? South->player->Handle() : started ? left : nobody;
    name_W = (West  && West->player)  ? West->player->Handle()  : started ? left : nobody;
    name_E = (East  && East->player)  ? East->player->Handle()  : started ? left : nobody;

    desc.Socket_Write("%s [%5d] %-8s %-8s %-8s %-8s", DG_TEXT, table->TableID(), name_N, name_S, name_W, name_E);
    Q = Q->next;
  }
}
