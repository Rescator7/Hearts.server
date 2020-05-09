#include <cstdio>
#include <cstring>
#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "password.h"

void cPassword::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) return;

  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  // defined +2 = 1 character is used to catch string too long, and 1 character for the NULL terminator
  // after that sscanf %17s = MAX_PASSWORD_LENGTH + 1, will allow me to check string too long.
  // Otherwise if sscan(%16s) is used, it would be possible to do: password aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa allo, and 
  // old_password = aaaaaaaaaaaaaaaa and new_password = aaaaaaaaaaaaaaaa instead of allo 
  char old_password[MAX_PASSWORD_LENGTH+2],
       new_password[MAX_PASSWORD_LENGTH+2];

  // MAX_PASSWORD_LENGTH = 16. If changed, those %17s should be change to MAX_PASSWORD_LENGTH + 1
  int ret = sscanf(param.arguments, "%17s %17s", (char *)&old_password, (char *)&new_password);

  if (ret != 2) return;

  if (!player->doesPasswordMatch(old_password)) {
    d.Socket_Write(DGE_WRONG_PASSWORD);
    return;
  }

  int len = strlen(new_password);

  if (len < MIN_PASSWORD_LENGTH) {
    d.Socket_Write(DGE_PASSWORD_TOO_SHORT);
    return;
  }

  if ((len > MAX_PASSWORD_LENGTH) || (strlen(old_password) > MAX_PASSWORD_LENGTH)) {
    d.Socket_Write(DGE_PASSWORD_TOO_LONG);
    return;
  }

  player->NewPassword(new_password);

  d.Socket_Write(DGI_PLAYER_NEW_PASSWORD);
}
