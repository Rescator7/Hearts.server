#include <stdlib.h>  // rand()
#include <algorithm> // sort()
#include <time.h>
#include <cstdio>
#include "define.h"
#include "game.h"

cGame::cGame( int f )
{
  flags = f;

  turn = 0;
  suit = CLUB;
  passto = pRIGHT;
  game_started = false;
  game_over = false;
  game_draw = false;

  num_cards[0] = 13;
  num_cards[1] = 13;
  num_cards[2] = 13;
  num_cards[3] = 13;

  generate_cards();
  Sort();
}

cGame::~cGame()
{
}

void cGame::Sort()
{
  std::sort(player_cards[0], player_cards[0]+13);
  std::sort(player_cards[1], player_cards[1]+13);
  std::sort(player_cards[2], player_cards[2]+13);
  std::sort(player_cards[3], player_cards[3]+13);

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
