#ifndef _cMYSQL_
#define _cMYSQL_

#include <mysql/mysql.h>

extern class cMYSQL sql;

class cMYSQL {
public:
  cMYSQL();
  ~cMYSQL();

private:
  int          num_affected_row;
  unsigned int num_fields;
  bool         freed;
  char         last_query[SIZE_QUERY_BUFFER];
  char         host[QUERY_SIZE];
  char         db[QUERY_SIZE];
  char         user[QUERY_SIZE];
  char         password[QUERY_SIZE];
  MYSQL mysql;
  MYSQL_RES *result;
  MYSQL_ROW row;

public:
  int query( const char * format, ... );
  const char *get_row( unsigned int index );
  unsigned int Num_Fields();
  void connect();
};

#endif // cMYSQL
