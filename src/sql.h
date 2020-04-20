#ifndef _cMYSQL_
#define _cMYSQL_

#include <mysql/mysql.h>

extern class cMYSQL sql;

class cMYSQL {
public:
  cMYSQL(const char *, const char *, const char *, const char *);
  ~cMYSQL();

private:
  int          num_affected_row;
  unsigned int num_fields;
  bool         freed;
  char         last_query[SIZE_QUERY_BUFFER];
  MYSQL mysql;
  MYSQL_RES *result;
  MYSQL_ROW row;

public:
  int query( const char * format, ... );
  const char * get_row( unsigned int index );
  bool fetch();
  unsigned int Num_Fields();
};

#endif // cMYSQL
