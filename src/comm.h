#ifndef _COMM_
#define _COMM_

#include <arpa/inet.h> // socklen_t
#include "define.h"

class cDescriptor {
public:
  cDescriptor(socket_t mother_desc);
  ~cDescriptor();

private:
  socket_t desc;
  socklen_t len;
  char buffer [SOCKET_BUFSIZE];
  fd_set input_set, output_set, exc_set, null_set;
  time_t last_sockread;
  time_t sit_time;
  struct timeval null_time;  
  struct sockaddr_in peer;
  struct hostent *from;
  unsigned int bytes_read;
  int    state;
  char   ip [16];       // numeric ip

public:
  class cPlayer *player;

public:
  ssize_t Socket_Read();
  bool Socket_Write( const char *format, ... );
  bool Is_Connected();
  bool process_input();
  bool IsHandleValid( const char *handle, const char *message );
  const char *process_ansi( const char *outbuf );
  void Disconnect();
  void Set_Sit_Time(time_t t);
  time_t Get_Sit_Time();
  char *IP_Adress();
  int State();
};

class cDescList {
public:
  cDescList();
  ~cDescList();

private:
  int num_elem;
  struct sList {
    cDescriptor *elem;
    struct sList *next;
  }; 
  struct sList *head;

public:
  bool Add( cDescriptor *elem );
  bool Remove( cDescriptor *elem );
  bool Empty();
  bool Send_To_All( const char *format, ... );
  bool Check_Conns();
  struct cPlayer *Find_Username( const char *handle );
  unsigned int Connection_Per_Ip(char *ip);
  void DisconnectPlayerID(unsigned int pID);
  void ULink_TableID(unsigned int id);
  void Who( cDescriptor &desc);
  void Table_Kick_Unplaying(struct cTable *t);
};

#endif // _COMM_
