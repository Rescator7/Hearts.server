#include <stdio.h> // sscanf()
#include <time.h>  // required by game.h
#include "../define.h"
#include "../table.h"
#include "../game.h"
#include "../player.h"
#include "../errors.h"
#include "pass.h"

// we are passing the position of our cards, not the cards themself.
// it's a lot more efficient for the server this way.
void cPass::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player;
  struct cTable *table;
  struct cGame *game;

  if ((player = d.player) == nullptr) return;

  if ((table = player->table) == nullptr) {
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }

  int chair = table->Chair(d);
  if (chair == PLAYER_NOWHERE) {
    d.Socket_Write(PLAYER_NOT_SAT);
    return;
  }

  game = table->game;

  if (!game->Started()) {
    d.Socket_Write(TABLE_NOT_STARTED);
    return;
  }

  usINT ret, card1, card2, card3;

  ret = sscanf(param.arguments, "%hd %hd %hd", &card1, &card2, &card3);

  if (ret != 3) {
    d.Socket_Write(TABLE_NOT_3PASSED);
    return;
  }

  int error;
  if ((error = game->PlayerPass(*table, chair, card1, card2, card3))) {
    switch (error) {
      case ERROR_CARDS_PASSED:   d.Socket_Write(TABLE_ALREADY_PASSED); break;
      case ERROR_ILLEGAL_CARD:   d.Socket_Write(TABLE_ILLEGAL_CARD);   break;
      case ERROR_CARD_NOT_FOUND: d.Socket_Write(TABLE_CARD_NOT_FOUND); break;
      case ERROR_DOUBLE_CARD:    d.Socket_Write(TABLE_DOUBLE_CARD);    break;
      case ERROR_PLAYING:        d.Socket_Write(TABLE_PASSING_OVER);   break;
    }
  }
}
