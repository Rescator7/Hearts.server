#include <cstdio> // sscanf()
#include <ctime>  // required by game.h
#include "../define.h"
#include "../table.h"
#include "../game.h"
#include "../player.h"
#include "../datagrams.h"
#include "pass.h"

// we are passing the position of our cards, not the cards themself.
// it's a lot more efficient for the server this way.
void cPass::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player;
  struct cTable *table;

  if ((player = d.player) == nullptr) return;

  if ((table = player->table) == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }

  int chair = table->Chair(d);
  if (chair == PLAYER_NOWHERE) {
    d.Socket_Write(DGE_PLAYER_NOT_SAT);
    return;
  }

  struct cGame *game = table->game;

  if (!game->Started()) {
    d.Socket_Write(DGE_TABLE_NOT_STARTED);
    return;
  }

  if (table->Paused()) {
    d.Socket_Write(DGI_TABLE_PAUSED);
    return;
  }

  if (!game->Passing()) {
    d.Socket_Write(DGE_TABLE_PASSING_OVER);
    return;
  }

  usINT ret, card1, card2, card3;

  ret = sscanf(param.arguments, "%hd %hd %hd", &card1, &card2, &card3);

  if (ret != 3) {
    d.Socket_Write(DGE_TABLE_NOT_3PASSED);
    return;
  }

  int error;
  if ((error = game->PlayerPass(*table, chair, card1, card2, card3))) {
    switch (error) {
      case ERROR_CARDS_PASSED:   d.Socket_Write(DGE_TABLE_ALREADY_PASSED); break;
      case ERROR_ILLEGAL_CARD:   d.Socket_Write(DGE_TABLE_ILLEGAL_CARD);   break;
      case ERROR_CARD_NOT_FOUND: d.Socket_Write(DGE_TABLE_CARD_NOT_FOUND); break;
      case ERROR_DOUBLE_CARD:    d.Socket_Write(DGE_TABLE_DOUBLE_CARD);    break;
    }
  }
}
