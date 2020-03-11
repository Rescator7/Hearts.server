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
  char   buffer [SOCKET_BUFSIZE];
  struct timeval null_time;  
  fd_set input_set, output_set, exc_set, null_set;
  int    state;
  time_t last_sockread;
  struct sockaddr_in peer;
  char   ip [16];       // numeric ip
  struct hostent *from;
  unsigned int bytes_read;

public:
  class cPlayer *player;

public:
  bool Socket_Write( const char *format, ... );
  ssize_t Socket_Read();
  bool Is_Connected();
  bool process_input();
  const char *process_ansi( const char *outbuf );
  void Send_Prompt();
  bool IsHandleValid( const char *handle, const char *message );
  void Disconnect();
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
  bool send_to_all( const char *format, ... );
  bool Check_Conns();
  bool Find_Handle( const char *handle );
  void DisconnectPlayerID(unsigned int pID);
};

#endif // _COMM_
