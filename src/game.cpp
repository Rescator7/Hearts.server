#include <stdlib.h>  // rand()
#include <algorithm> // sort()
#include <time.h>
#include <cstdio>
#include "define.h"
#include "errors.h"
#include "table.h"
#include "game.h"
#include "player.h"

/*
#define STATE_SEND_CARDS       1
#define STATE_WAIT_PASS        2
#define STATE_FORCE_PASS       3
#define STATE_WAIT_TWO_CLUBS   4
#define STATE_FORCE_TWO_CLUBS  5
#define STATE_WAIT_PLAY        6
#define STATE_FORCE_PLAY       7
*/

cGame::cGame( int f )
{
  flags = f;

  turn = 0;
  suit = CLUB;
  passto = pLEFT;
  game_started = false;
  game_over = false;
  game_draw = false;
  playing = false;

  num_cards[PLAYER_NORTH] = 13;
  num_cards[PLAYER_SOUTH] = 13;
  num_cards[PLAYER_WEST] = 13;
  num_cards[PLAYER_EAST] = 13;

  num_passed = 0;

  ResetPassed();
  generate_cards();
  Sort();
}

cGame::~cGame()
{
}

void cGame::Run()
{
}

usINT cGame::State()
{
  return state;
}

void cGame::ResetPassed()
{
  for (int i=0; i<3; i++) {
    passed_cards[PLAYER_NORTH][i] = empty;
    passed_cards[PLAYER_SOUTH][i] = empty;
    passed_cards[PLAYER_WEST][i] = empty;
    passed_cards[PLAYER_EAST][i] = empty;
  }
}

bool cGame::Passed(usINT pid)
{
  return passed_cards[pid][0] != empty;
}

// This is a simple choose 3 cards to pass.
// Step 1. It choose available cards from those:
//         QUEEN_SPADE
//         KIND_SPADE
//         ACE_SPADE
//         lowest HEART card.
// Step 2. Fill missing by random cards.
void cGame::ForcePass(cTable *table, usINT chair)
{
  int card, selected = 0;

  for (int i=0; i<num_cards[chair]; i++) {
    card = player_cards[chair][i];
    if (card == QUEEN_SPADE) {
      passed_cards[chair][selected] = i;
      if (++selected == 3) break;
    } else
    if (card == KING_SPADE) {
      passed_cards[chair][selected] = i;
      if (++selected == 3) break;
    } else
    if (card == ACE_SPADE) {
      passed_cards[chair][selected] = i;
      if (++selected == 3) break;
    } else
    if (card / 13 == HEART) {
      passed_cards[chair][selected] = i;
      break;
    }
  }
 
  if (selected == 0)
     passed_cards[chair][0] = rand() % num_cards[chair];

  if (selected <= 1) while ((passed_cards[chair][1] == empty) || (passed_cards[chair][1] == passed_cards[chair][0])) 
                       passed_cards[chair][1] = rand() % num_cards[chair];

  if (selected <= 2) while ((passed_cards[chair][2] == empty) || (passed_cards[chair][2] == passed_cards[chair][0]) || 
                            (passed_cards[chair][2] == passed_cards[chair][1]))
                       passed_cards[chair][2] = rand() % num_cards[chair]; 

  cDescriptor *desc = table->desc(chair);

  if (desc != nullptr)
    desc->Socket_Write("%s %d %d %d", TABLE_FORCE_PASS,       passed_cards[chair][0], 
                                      passed_cards[chair][1], passed_cards[chair][2]);  

  if (++num_passed == 4)
    Pass(*table); 
}

void cGame::Pass(cTable &table)
{
  struct cDescriptor *d;

  if (passto == pNOPASS) return;

  usINT n, s, w, e;

  for (int i=0; i<3; i++) {
    n = player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][i]];
    s = player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][i]];
    w = player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][i]];
    e = player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][i]];

    switch (passto) {
      case pLEFT: player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][i]] = w;
  	          player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][i]] = e;
	          player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][i]] = s;
	          player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][i]] = n;
	          break;
      case pRIGHT: player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][i]] = e;
	           player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][i]] = w;
	           player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][i]] = n;
	           player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][i]] = s;
		   break;
      case pACCROSS: player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][i]] = s;
	             player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][i]] = n;
	             player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][i]] = e;
	             player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][i]] = w;
		     break;
    }
  }

  if ((d = table.desc(PLAYER_NORTH)) != nullptr) 
    d->Socket_Write("%s %d %d %d", TABLE_CARDS_RECEIVED, player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][0]],
                                                         player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][1]],
		                                         player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][2]]);
  if ((d = table.desc(PLAYER_SOUTH)) != nullptr)
    d->Socket_Write("%s %d %d %d", TABLE_CARDS_RECEIVED, player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][0]],
                                                         player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][1]],
                                                         player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][2]]);
  if ((d = table.desc(PLAYER_WEST)) != nullptr)
    d->Socket_Write("%s %d %d %d", TABLE_CARDS_RECEIVED, player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][0]],
                                                         player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][1]],
                                                         player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][2]]);
  if ((d = table.desc(PLAYER_EAST)) != nullptr)
    d->Socket_Write("%s %d %d %d", TABLE_CARDS_RECEIVED, player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][0]],
                                                         player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][1]],
                                                         player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][2]]);
 
  state = STATE_WAIT_TWO_CLUBS;

 // ResetPassed();
 // ResetPassed() must be done later, or ForcePass() could lead to Pass() while
 // table is still forcing Pass() and card_passed is reset which lead to ForcePass()
 // on player who already passed their cards.
  Sort();
}

usINT cGame::PlayerPass(cTable &table, usINT chair, usINT card1, usINT card2, usINT card3)
{
  if (playing) return ERROR_PLAYING;

  if (passed_cards[chair][0] != empty)
    return ERROR_CARDS_PASSED;

  if ((card1 < 0) || (card1 > num_cards[chair] - 1)) return ERROR_ILLEGAL_CARD;
  if ((card2 < 0) || (card2 > num_cards[chair] - 1)) return ERROR_ILLEGAL_CARD;
  if ((card3 < 0) || (card3 > num_cards[chair] - 1)) return ERROR_ILLEGAL_CARD;

  if (player_cards[chair][card1] == empty) return ERROR_CARD_NOT_FOUND;
  if (player_cards[chair][card2] == empty) return ERROR_CARD_NOT_FOUND;
  if (player_cards[chair][card3] == empty) return ERROR_CARD_NOT_FOUND;

  if ((card1 == card2) || (card1 == card3) || (card2 == card3)) return ERROR_DOUBLE_CARD;

  passed_cards[chair][0] = card1;
  passed_cards[chair][1] = card2;
  passed_cards[chair][2] = card3;

  if (++num_passed == 4)
    Pass(table);

  return 0;
}

void cGame::Sort()
{
  std::sort(player_cards[PLAYER_NORTH], player_cards[PLAYER_NORTH]+13);
  std::sort(player_cards[PLAYER_SOUTH], player_cards[PLAYER_SOUTH]+13);
  std::sort(player_cards[PLAYER_WEST], player_cards[PLAYER_WEST]+13);
  std::sort(player_cards[PLAYER_EAST], player_cards[PLAYER_EAST]+13);

  for (int i=0; i<4; i++)
    sprintf(str_cards[i], "%d %d %d %d %d %d %d %d %d %d %d %d %d", 
                          player_cards[i][0], player_cards[i][1], player_cards[i][2], player_cards[i][3],
                          player_cards[i][4], player_cards[i][5], player_cards[i][6], player_cards[i][7],
                          player_cards[i][8], player_cards[i][9], player_cards[i][10], player_cards[i][11],
		          player_cards[i][12]);
}

void cGame::generate_cards()
{
  bool card_free[DECK_SIZE];

  for (int i=0; i < DECK_SIZE; i++)
    card_free[i] = true;

  int player = 0;
  for (int i=0; i<DECK_SIZE; i++) {
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

bool cGame::Started()
{
  return game_started;
}

usINT cGame::Turn()
{
  return turn;
}

void cGame::Wait(usINT d)
{
  delay = d;
  wait_time = time(nullptr);
}

bool cGame::WaitOver()
{
  if (difftime(time(nullptr), wait_time) >= delay)
    return true;
  else
    return false;
}

void cGame::Start()
{
  wait_time = 0;
  game_started = true;

  state = STATE_SEND_CARDS;
}

usINT cGame::Cards(usINT player, usINT card)
{
  return player_cards[player][card];
}

usINT cGame::Num_Cards(usINT player)
{
  return num_cards[player];
}

char *cGame::Str_Cards(usINT player)
{
  return str_cards[player];
}

void cGame::SetState(usINT s)
{
  state = s;
}

usINT cGame::PassTo()
{
  return passto;
}

bool cGame::MyTurn(usINT chair)
{
  return turn == chair;
}
