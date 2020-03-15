#include <stdlib.h>  // rand()
#include <algorithm> // sort()
#include <time.h>
#include <cstdio>
#include "define.h"
#include "game.h"
#include "player.h"

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

  for (int i=0; i<3; i++) {
    passed_cards[PLAYER_NORTH][i] = empty;
    passed_cards[PLAYER_SOUTH][i] = empty;
    passed_cards[PLAYER_WEST][i] = empty;
    passed_cards[PLAYER_EAST][i] = empty;
  }

  num_cards[PLAYER_NORTH] = 13;
  num_cards[PLAYER_SOUTH] = 13;
  num_cards[PLAYER_WEST] = 13;
  num_cards[PLAYER_EAST] = 13;

  generate_cards();
  Sort();
}

cGame::~cGame()
{
}

usINT cGame::Pass(usINT pid, usINT card1, usINT card2, usINT card3)
{
  if (playing) return ERROR_PLAYING;

  if (passed_cards[pid][0] != empty)
    return ERROR_CARDS_PASSED;

  if ((card1 < 0) || (card1 > DECK_SIZE - 1)) return ERROR_ILLEGAL_CARD;
  if ((card2 < 0) || (card2 > DECK_SIZE - 1)) return ERROR_ILLEGAL_CARD;
  if ((card3 < 0) || (card3 > DECK_SIZE - 1)) return ERROR_ILLEGAL_CARD;

  int found = 0;

  for (int i=0; i<num_cards[pid]; i++) {
    if (player_cards[pid][i] == card1) if (++found == 3) break;
    if (player_cards[pid][i] == card2) if (++found == 3) break;
    if (player_cards[pid][i] == card3) if (++found == 3) break;
  }
    
  if (found != 3) return ERROR_CARD_NOT_FOUND;

  if ((card1 == card2) || (card1 == card3) || (card2 == card3)) return ERROR_DOUBLE_CARD;

  passed_cards[pid][0] = card1;
  passed_cards[pid][1] = card2;
  passed_cards[pid][2] = card3;

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

time_t cGame::Wait()
{
  return wait_time;
}

void cGame::Set_Wait(time_t t)
{
  wait_time = time(nullptr);
}

void cGame::Start()
{
  wait_time = 0;
  game_started = true;
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

usINT cGame::PassTo()
{
  return passto;
}
