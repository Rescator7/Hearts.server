#ifndef _DEFINE_
#define _DEFINE_

//#define DEBUG
//#define USE_ANSI

#define VERSION "1.1.1"

#define socket_t int
#define usINT unsigned short int

#define MAX_SERVER_CMDS      500

#ifndef INVALID_SOCKET
#define INVALID_SOCKET       (-1)
#endif // INVALID_SOCKET

#define MAX_LOG_SIZE         5000000    // max log files in bytes (5M)

#define MAX_LOGON_IDLE       60 

#define QUERY_SIZE           80
#define BUF_SIZE             2048
#define SIZE_QUERY_BUFFER    1024
#define SOCKET_BUFSIZE       512
#define SOCKET_MAX_READ_BYTES 256
#define SIT_DELAY            2          // in seconds
#define TABLE_EXPIRE         10 
#define PAUSE_EXPIRE         300
#define MAX_REGISTER_PER_IP  25
#define MAX_CONNECTION_PER_IP 50

#define MAX_HANDLE_LENGTH    8
#define MIN_HANDLE_LENGTH    3
#define MAX_REALNAME_LENGTH  40
#define MAX_EMAIL_LENGTH     40
#define MAX_PASSWORD_LENGTH  16
#define MIN_PASSWORD_LENGTH  4
#define MAX_PROMPT_LENGTH    50
#define UUID_LENGTH          48
#define SHA_PASSWORD_SIZE    40

enum CONNECTION_STATE {
     CON_CONNECTED        = 0,
     CON_CLOSE            = 1,
     CON_DISCONNECT       = 2,
     CON_LOGIN            = 3,
     CON_UUID             = 4,
     CON_PASSWORD         = 5,
     CON_NEW_HANDLE       = 6,
     CON_NEW_REALNAME     = 7,     
     CON_NEW_EMAIL        = 8,
     CON_NEW_PASSWORD     = 9,
     CON_CONFIRM_PASSWORD = 10,
     CON_MOTD             = 11,
     CON_PROMPT           = 12
};

#define FLAG_TABLE_DISCONNECT 0
#define FLAG_TABLE_SWITCH     1
#define FLAG_TABLE_LEAVE      2

enum LEVEL {
     LVL_GUEST      = 0,
     LVL_REGISTERED = 1,
     LVL_ADMIN      = 10,
     LVL_SUPERUSER  = 20
};

enum DIRECTION {
     pLEFT    = 0,
     pRIGHT   = 1,
     pACCROSS = 2,
     pNOPASS  = 3
};

#define NOPLAYER             0
#define DECK_SIZE            52

#define two_clubs            0
#define empty                127

#define CLUB                 0
#define SPADE                1
#define DIAMOND              2
#define HEART                3
#define FREESUIT             4
#define TWO_CLUBS            0
#define ACE_SPADE            25
#define KING_SPADE           24
#define QUEEN_SPADE          23
#define JACK_DIAMOND         35

#endif // _DEFINE_
