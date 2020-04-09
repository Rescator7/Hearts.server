#include <stdlib.h>  // rand()
#include <algorithm> // sort()
#include <time.h>
#include <cstdio>
#include "define.h"
#include "datagrams.h"
#include "table.h"
#include "game.h"
#include "player.h"
#include "config.h"

cGame::cGame( int f )
{
  flags = f;

  turn = 0;
  passto = pLEFT;
  game_started = false;
  game_over = false;
  game_draw = false;
  shoot_moon = false;

  score[PLAYER_NORTH] = 0;
  score[PLAYER_SOUTH] = 0;
  score[PLAYER_WEST] = 0;
  score[PLAYER_EAST] = 0;

  ResetRound();
  ResetPlayed();

  Generate_Cards();
  Sort();
}

cGame::~cGame()
{
}

void cGame::Run()
{
}

void cGame::EndTurn(cTable &table)
{
  table.SendAll("%s %d %d", TABLE_HAND_SCORE, won_turn, hand_score[won_turn]);
  table.Send(turn, "%s %d", TABLE_YOUR_TURN, config.Wait_Play());
  Wait(config.Wait_Play());
  state = STATE_WAIT_PLAY;
}

bool cGame::AdvanceTurn(cTable &table)
{
  if (jack_diamond) {
    won_jack_diamond = won_turn;
    jack_diamond = false;
  }

  if (++turn == 4)
    turn = 0;

  if (--left_to_play == 0) {
    turn = won_turn;
    suit = FREESUIT;
    best_card = TWO_CLUBS;
    hand_score[won_turn] += current_score;
    current_score = 0;
    ResetPlayed();
    left_to_play = 4;

    if (num_cards[turn] == 0) {
      table.SendAll("%s %d %d", TABLE_HAND_SCORE, won_turn, hand_score[won_turn]);
      Wait(config.Wait_End_Round());
      state = STATE_END_ROUND;
      return false;
    } else {
	Wait(config.Wait_End_Turn());
	state = STATE_END_TURN;
	return true;
      }
  } 

  Wait(config.Wait_Play());

  table.Send(turn, "%s %d", TABLE_YOUR_TURN, config.Wait_Play());

  return true;
}

void cGame::Play(cTable &table, usINT card)
{
  struct cGame *game = table.game;

  int turn = game->turn;

  cards_played++;

  if (suit == FREESUIT)
    suit = card / 13;

  if (card == QUEEN_SPADE) {
    current_score += 13;
    if (!heart_broken && (flags & QUEEN_SPADE_f))
      heart_broken = true;
  }
  else
    if (card == JACK_DIAMOND)
      jack_diamond = true;
    else
    if (card / 13 == HEART) {
      current_score++;
      printf("score +1\r\n");
      heart_broken = true;
    }

  if ((card / 13 == suit) && (card > best_card)) {
    best_card = card;
    won_turn = turn;
  }

  for (int i=0; i<num_cards[turn]; i++)
//  for (int i=0; i<13; i++)
    if (player_cards[turn][i] == card) {
      player_cards[turn][i] = empty;
    }

  num_cards[turn]--;
  cards_in_suit[turn][card / 13]--;
  has_card[turn][card] = false;
  played[turn] = true;

  std::sort(player_cards[turn], player_cards[turn]+13);

  table.SendAll("%s %d %d", TABLE_PLAY, turn, card);
  AdvanceTurn(table);
}

bool cGame::ValidMove(cDescriptor &d, usINT chair, usINT card)
{
  usINT error = ValidMove(chair, card);

  switch (error) {
    case ERROR_NOT_YOUR_TURN:  d.Socket_Write(TABLE_NOT_YOUR_TURN);
			       return false;
    case ERROR_ILLEGAL_CARD:   d.Socket_Write(TABLE_ILLEGAL_CARD);
			       return false;
    case ERROR_QUEEN_SPADE:    d.Socket_Write(TABLE_QUEEN_SPADE);
			       return false;
    case ERROR_BREAK_HEART:    d.Socket_Write(TABLE_CANT_BREAK_HEART);
			       return false;
    case ERROR_WRONG_SUIT:     d.Socket_Write(TABLE_WRONG_SUIT);
			       return false;
    case ERROR_PLAY_TWO_CLUBS: d.Socket_Write(TABLE_PLAY_TWO_CLUBS);
			       return false;
  }
  return true;
}

usINT cGame::ValidMove(usINT chair, usINT card)
{
  if (chair != turn)
    return ERROR_NOT_YOUR_TURN;

  if ((card < 0) || (card >= DECK_SIZE))
    return ERROR_ILLEGAL_CARD;

  if (!has_card[chair][card])
    return ERROR_ILLEGAL_CARD;

  int card_suit = card / 13;

  if ((card == QUEEN_SPADE) && (cards_played <= 4) && (player_cards[chair][1]/13 != HEART)) 
    return ERROR_QUEEN_SPADE;

  if (suit == FREESUIT) {
     if ((card_suit == HEART) && !heart_broken && (player_cards[chair][0]/13 != HEART))
       return ERROR_BREAK_HEART;
  } else
      if ((card_suit != suit) && cards_in_suit[chair][suit])
        return ERROR_WRONG_SUIT;

  if (!cards_played && (card != TWO_CLUBS))
    return ERROR_PLAY_TWO_CLUBS;

  return NO_ERROR;
}

// This is very simple version.
// It will force to play the first legal move found.
void cGame::ForcePlay(cTable &table)
{
  struct cGame *game = table.game;

  int turn = game->turn;

  struct cDescriptor *d = table.desc(turn);

  printf("force play, turn: %d, num_cards: %d\r\n", turn, num_cards[turn]);
  for (int i=0; i<num_cards[turn]; i++) {
//  for (int i=0; i<13; i++) {
    usINT card = player_cards[turn][i];
    if (ValidMove(turn, card) == NO_ERROR) {
      Play(table, card);

      if (d != nullptr)
	d->Socket_Write("%s %d", TABLE_FORCE_PLAY, card);
      return;
    }
  }
  printf("CARD VALID NOT FOUND\r\n");
  printf("Turn: %d, Suit: %d, num_card: %d\r\n", turn, suit, num_cards[turn]);
  for (int i=0; i<13; i++)
    printf("%d (%d) ", player_cards[turn][i], player_cards[turn][i]/13);
  printf("\r\n");
  printf("cards in suit: \r\n");
  for (int i=0; i<4; i++)
    printf("%d ", cards_in_suit[turn][i]);
  exit(1);
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

  for (int i=0; i<13; i++) {
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

    cards_in_suit[PLAYER_NORTH][n/13]--; 
    cards_in_suit[PLAYER_SOUTH][s/13]--; 
    cards_in_suit[PLAYER_WEST][w/13]--;
    cards_in_suit[PLAYER_EAST][e/13]--;

    has_card[PLAYER_NORTH][n] = false;
    has_card[PLAYER_SOUTH][s] = false;
    has_card[PLAYER_WEST][w] = false;
    has_card[PLAYER_EAST][e] = false;

    switch (passto) {
      case pLEFT: player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][i]] = w;
  	          player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][i]] = e;
	          player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][i]] = s;
	          player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][i]] = n;

                  cards_in_suit[PLAYER_NORTH][w/13]++;
                  cards_in_suit[PLAYER_SOUTH][e/13]++;
                  cards_in_suit[PLAYER_WEST][s/13]++;
                  cards_in_suit[PLAYER_EAST][n/13]++;

                  has_card[PLAYER_NORTH][w] = true;
                  has_card[PLAYER_SOUTH][e] = true;
                  has_card[PLAYER_WEST][s] = true;
                  has_card[PLAYER_EAST][n] = true;

	          break;
      case pRIGHT: player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][i]] = e;
	           player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][i]] = w;
	           player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][i]] = n;
	           player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][i]] = s;

                   cards_in_suit[PLAYER_NORTH][e/13]++;
                   cards_in_suit[PLAYER_SOUTH][w/13]++;
                   cards_in_suit[PLAYER_WEST][n/13]++;
                   cards_in_suit[PLAYER_EAST][s/13]++;

                   has_card[PLAYER_NORTH][e] = true;
                   has_card[PLAYER_SOUTH][w] = true;
                   has_card[PLAYER_WEST][n] = true;
                   has_card[PLAYER_EAST][s] = true;
		   break;
      case pACCROSS: player_cards[PLAYER_NORTH][passed_cards[PLAYER_NORTH][i]] = s;
	             player_cards[PLAYER_SOUTH][passed_cards[PLAYER_SOUTH][i]] = n;
	             player_cards[PLAYER_WEST][passed_cards[PLAYER_WEST][i]] = e;
	             player_cards[PLAYER_EAST][passed_cards[PLAYER_EAST][i]] = w;

                     cards_in_suit[PLAYER_NORTH][s/13]++;
                     cards_in_suit[PLAYER_SOUTH][n/13]++;
                     cards_in_suit[PLAYER_WEST][e/13]++;
                     cards_in_suit[PLAYER_EAST][w/13]++;

                     has_card[PLAYER_NORTH][s] = true;
                     has_card[PLAYER_SOUTH][n] = true;
                     has_card[PLAYER_WEST][e] = true;
                     has_card[PLAYER_EAST][w] = true;
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
 
  Sort();

  if (player_cards[PLAYER_NORTH][0] == TWO_CLUBS) turn = PLAYER_NORTH;
  if (player_cards[PLAYER_SOUTH][0] == TWO_CLUBS) turn = PLAYER_SOUTH;
  if (player_cards[PLAYER_WEST][0] == TWO_CLUBS) turn = PLAYER_WEST;
  if (player_cards[PLAYER_EAST][0] == TWO_CLUBS) turn = PLAYER_EAST;

  passing_over = true;

  Wait(config.Wait_Play());
  state = STATE_WAIT_PLAY;
  table.Send(turn, "%s %d", TABLE_YOUR_TURN, config.Wait_Play());  
}

usINT cGame::PlayerPass(cTable &table, usINT chair, usINT card1, usINT card2, usINT card3)
{
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

void cGame::Generate_Cards()
{
  bool card_free[DECK_SIZE];

  for (int i=0; i < DECK_SIZE; i++) {
    card_free[i] = true;
    has_card[PLAYER_NORTH][i] = false;
    has_card[PLAYER_SOUTH][i] = false;
    has_card[PLAYER_WEST][i] = false;
    has_card[PLAYER_EAST][i] = false;
  }

  int player = 0;
  for (int i=0; i<DECK_SIZE; i++) {
    int card = rand() % DECK_SIZE;

    while (!card_free[card])
      card = rand() % DECK_SIZE;

    card_free[card] = false;
    player_cards[player][i / 4] = card;
    has_card[player][card] = true;
    cards_in_suit[player][card / 13]++;

    if (card == two_clubs)
      turn = player;

    if (player++ == 3)
      player = 0;
  }
}

void cGame::EndRound(cTable &table)
{
  int bonus = 0;
  bool omnibus_set;
  bool new_moon;

  // This function is reentrant. To support new moon.
  if (!shoot_moon) {
    // checking for moon
    for (int i=0; i<4; i++)
      if (hand_score[i] == 26) {
        omnibus_set = flags & OMNIBUS_f; 
        if (!omnibus_set || (omnibus_set && (won_jack_diamond == i))) {
           shoot_moon = true;
	   who_moon = i;
	   new_moon = (flags & NEW_MOON_f) && (score[i] >= 26);
	   table.SendAll("%s %d %d", TABLE_SHOOT_MOON, i, new_moon ? config.Wait_Moon() : 0);
	   if (new_moon) {
	     Wait(config.Wait_Moon());
	     return; 
	   }
	   else
	     break;
	}
      }
  } 
   
  if (shoot_moon) {
    shoot_moon = false;

    if (moon_add) {
      if (who_moon != PLAYER_NORTH) score[PLAYER_NORTH] += 26;
      if (who_moon != PLAYER_SOUTH) score[PLAYER_SOUTH] += 26;
      if (who_moon != PLAYER_WEST) score[PLAYER_WEST] += 26;
      if (who_moon != PLAYER_EAST) score[PLAYER_EAST] += 26;
    } else
        score[who_moon] -= 26;
  } else {
      // Adjust the new scores, 
      score[PLAYER_NORTH] += hand_score[PLAYER_NORTH];
      score[PLAYER_SOUTH] += hand_score[PLAYER_SOUTH];
      score[PLAYER_WEST] += hand_score[PLAYER_WEST];
      score[PLAYER_EAST] += hand_score[PLAYER_EAST];

      // Bonus doesn't apply, if someone shoot the moon.
      if (flags & OMNIBUS_f) {
        bonus = score[won_jack_diamond] < omnibus_bonus ? score[won_jack_diamond] : omnibus_bonus;
        if (bonus) {
          table.SendAll("%s %d %d", TABLE_OMNIBUS, won_jack_diamond, bonus); 
          score[won_jack_diamond] -= bonus;
        }
      }

      if (flags & NO_TRICK_BONUS_f) {
        for (int i=0; i<4; i++) {
          if (!hand_score[i]) {
            bonus = score[i] < no_trick_bonus ? score[i] : no_trick_bonus;
            if (bonus) {
              table.SendAll("%s %d %d", TABLE_NO_TRICK_BONUS, i, bonus);
              score[i] -= bonus;
            }
	  }
        }
      }
    
      if (flags & PERFECT_100_f) {
        for (int i=0; i<4; i++) {
           if (score[i] == GAME_OVER_SCORE) {
             score[i] = 50;
             table.SendAll("%s %d %d", TABLE_PERFECT_100, i, 50);
           }
        } 
      }
  }

  table.SendAll("%s %d %d %d %d", TABLE_SCORE, score[PLAYER_NORTH], score[PLAYER_SOUTH], score[PLAYER_WEST], score[PLAYER_EAST]);
  
  if (!(flags & NO_DRAW_f)) {
    for (int i=0; i<4; i++)
       if (score[i] >= GAME_OVER_SCORE) {
	 state = STATE_GAME_OVER;
	 return;
       }
  } else {
      int lowest = 65535, c = 0;
      bool over = false;

      for (int i=0; i<4; i++) {
        if (score[i] < lowest) {
          c = 1;
          lowest = score[i];
        } else if (score[i] == lowest) c++;

        if (score[i] >= GAME_OVER_SCORE) 
          over = true;
      }

      if (over && (c == 1)) {
        state = STATE_GAME_OVER;
        return;
      }
    }

  ResetRound();
  ResetPlayed();

  if (++passto == 4)
    passto = 0;

  Generate_Cards();
  Sort();

  state = STATE_SEND_CARDS; 
}

void cGame::ResetRound()
{
  heart_broken = false;
  jack_diamond = false;
  passing_over = false;

  suit = CLUB;
  num_passed = 0;
  current_score = 0;
  won_turn = 0;
  left_to_play = 4;
  best_card = 0;
  cards_played = 0;

  moon_add = true;
  who_moon = PLAYER_NOWHERE;

  hand_score[PLAYER_NORTH] = 0;
  hand_score[PLAYER_SOUTH] = 0;
  hand_score[PLAYER_WEST] = 0;
  hand_score[PLAYER_EAST] = 0;

  num_cards[PLAYER_NORTH] = 13;
  num_cards[PLAYER_SOUTH] = 13;
  num_cards[PLAYER_WEST] = 13;
  num_cards[PLAYER_EAST] = 13;

  for (int i=0; i<4; i++) {
    cards_in_suit[PLAYER_NORTH][i] = 0;
    cards_in_suit[PLAYER_SOUTH][i] = 0;
    cards_in_suit[PLAYER_WEST][i] = 0;
    cards_in_suit[PLAYER_EAST][i] = 0;
  }

  for (int i=0; i<3; i++) {
    passed_cards[PLAYER_NORTH][i] = empty;
    passed_cards[PLAYER_SOUTH][i] = empty;
    passed_cards[PLAYER_WEST][i] = empty;
    passed_cards[PLAYER_EAST][i] = empty;
  }
}

void cGame::Show(usINT chair)
{
  printf("cards: ");
  for (int i=0; i<13; i++)
    printf("%d (%d) ", player_cards[chair][i], player_cards[chair][i]/13);

  printf("\r\nhas cards: ");
  for (int i=0; i<DECK_SIZE; i++)
    printf("%d (%d) ", i, has_card[chair][i]);
  printf("\r\n");
}

bool cGame::Ready()
{
  if (passed_cards[PLAYER_NORTH][0] == empty) return false;
  if (passed_cards[PLAYER_SOUTH][0] == empty) return false;
  if (passed_cards[PLAYER_WEST][0] == empty) return false;
  if (passed_cards[PLAYER_EAST][0] == empty) return false;

  return true;
}

void cGame::ResetPlayed()
{
  played[PLAYER_NORTH] = false;
  played[PLAYER_SOUTH] = false;
  played[PLAYER_WEST] = false;
  played[PLAYER_EAST] = false;
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

bool cGame::Passing()
{
  if (passing_over)
    return false;
  else
    return true;
}

void cGame::Wait(usINT d)
{
  delay = d;
  wait_time = time(nullptr);
}

bool cGame::Passed(usINT pid)
{
  return passed_cards[pid][0] != empty;
}

bool cGame::Started()
{
  return game_started;
}

usINT cGame::Turn()
{
  return turn;
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

usINT cGame::State()
{
  return state;
}

bool cGame::Played(usINT chair)
{
  return played[chair];
}

usINT cGame::WhoMoon()
{
  return who_moon;
}

void cGame::SetMoon(bool add)
{
  moon_add = add;
}

usINT cGame::Score(usINT chair)
{
  return score[chair];
}
