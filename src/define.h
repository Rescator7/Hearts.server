#ifndef _DEFINE_
#define _DEFINE_

//#define DEBUG
//#define USE_ANSI

#define VERSION "1.0.1"

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
#define MAX_REGISTER_PER_IP  25
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
#define CON_MOTD             10
#define CON_PROMPT           11

#define LVL_GUEST            0
#define LVL_REGISTERED       1
#define LVL_ADMIN            10
#define LVL_SUPERUSER        20

#define pLEFT                0
#define pRIGHT               1
#define pACCROSS             2
#define pNOPASS              3

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
