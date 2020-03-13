#ifndef _DEFINE_
#define _DEFINE_

#define socket_t int

#define MAX_SERVER_CMDS      500

//OPT_USEC determines how many commands will be processed by the server per
#define OPT_USEC             100000      /* 10 passes per second */

#ifndef INVALID_SOCKET
#define INVALID_SOCKET       (-1)
#endif // INVALID_SOCKET

#define MAX_IDLE             3600
#define MAX_LOGON_IDLE       60 

#define SIZE_QUERY_BUFFER    1024
#define SOCKET_BUFSIZE       512
#define SOCKET_MAX_READ_BYTES 256
#define SIT_DELAY            2      // in seconds
#define TABLE_EXPIRE         10 
#define MAX_CONNECTION_PER_IP 50

#define MAX_HANDLE_LENGTH    8
#define MIN_HANDLE_LENGTH    3
#define MAX_REALNAME_LENGTH  40
#define MAX_EMAIL_LENGTH     40
#define MAX_PASSWORD_LENGTH  16
#define MIN_PASSWORD_LENGTH  4
#define MAX_PROMPT_LENGTH    50

#define CON_CONNECTED        0
#define CON_CLOSE            1
#define CON_DISCONNECT       2
#define CON_LOGIN            3
#define CON_PASSWORD         4
#define CON_NEW_HANDLE       5
#define CON_NEW_REALNAME     6
#define CON_NEW_EMAIL        7
#define CON_NEW_PASSWORD     8
#define CON_CONFIRM_PASSWORD 9
#define CON_RETURN           10
#define CON_MOTD             11
#define CON_PROMPT           12

#define LVL_GUEST            0
#define LVL_REGISTERED       1
#define LVL_ADMIN            10

#define pLEFT                0
#define pRIGHT               1
#define pUP                  2
#define pDOWN                3

#define NOPLAYER             0
#define DECK_SIZE            52

#define two_clubs            0

/*
const int ace_clubs     = 12;
const int two_spade     = 13;
const int queen_spade   = 23;
const int king_spade    = 24;
const int ace_spade     = 25;
const int two_diamond   = 26;
const int jack_diamond  = 35;
const int queen_diamond = 36;
const int king_diamond  = 37;
const int ace_diamond   = 38;
const int two_heart     = 39;
const int jack_heart    = 48;
const int queen_heart   = 49;
const int king_heart    = 50;
const int ace_heart     = 51;
const int empty         = 127;
const int your_turn     = 255;
*/
#endif // _DEFINE_
