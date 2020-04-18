#include <cstring>
#include <cstdlib> // exit()
#include <cstdarg>
#include "define.h"
#include "sql.h"
#include "log.h"

int cMYSQL::query( const char * format, ... )
{
 int errno;
 va_list args;
 char buffer[SIZE_QUERY_BUFFER];

 if (!freed) {
   mysql_free_result(result);                    // free any previous query storage
   freed = true;
 }

 va_start(args, format);

 vsnprintf(buffer, SIZE_QUERY_BUFFER, format, args);
 strncpy(last_query, buffer, SIZE_QUERY_BUFFER);  // keep a backup of the last query
 errno = mysql_query(&mysql, buffer);

#ifdef DEBUG
 printf("query: %s, errno: %d\n", buffer, errno);
#endif

 va_end(args);

 if (errno) return false;

 if (!(result = mysql_store_result(&mysql))) {
   freed = true; // nothings has been saved, so nothing will need to be freed.
   if (mysql_field_count(&mysql) != 0) { // data was expected
     Log.Write("SYSERR: cMYSQL::query() error: %s\n", mysql_error(&mysql));
     return false; // error
   }
   return true; // not error, was a "insert", "delete"... not data to retreive.
 } 

 freed = false; // data has been saved, will need to release it later.
 num_affected_row = mysql_affected_rows(&mysql);
 num_fields = mysql_num_fields(result);
 if (!(row = mysql_fetch_row(result))) {
   mysql_free_result(result);
   freed = true;
   return num_affected_row;
 }
 return num_affected_row;
}

const char *cMYSQL::get_row(unsigned int index)
{
 if (index + 1 > num_fields) {
   Log.Write("SYSERR: cMYSQL::get_row() invalid index: %d > num_fields: %d\n"
             "SYSERR: last query was '%s'" , index, num_fields, last_query);
   return "";
 }
 return row[index];
}

bool cMYSQL::fetch()
{
 if (!(row = mysql_fetch_row(result))) {
   if (!freed) {
     mysql_free_result(result);
     freed = true;
   }
   return false;
 }
 return true;
}

cMYSQL::cMYSQL(const char *host, const char *user, const char *password, const char *db)
{
 bool option = true;
 mysql_init(&mysql);
 mysql_options(&mysql, MYSQL_OPT_RECONNECT, &option);
 mysql_options(&mysql, MYSQL_OPT_COMPRESS, 0);
 mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "odbc");

 if (!mysql_real_connect(&mysql, host, user, password, NULL, 0, NULL, 0)) {
// A MYSQL* connection handle if the connection was successful, NULL if the connection was unsuccessful. For a successful connection, the return value is the same as the value of the first parameter.
   Log.Write("SYSERR: can't connect to the mysql database");
   exit(1);
 }

 if (mysql_select_db(&mysql, db)) { 
// Zero for success. Non-zero if an error occurred.
   Log.Write("SYSERR: can't select the database: '%s'", db);
   exit(1);
 }

 freed = true;
 num_affected_row = num_fields = 0;
}

cMYSQL::~cMYSQL()
{
 if (!freed)
   mysql_free_result(result);
}

// TODO: this function is not good, because it return a Int, so can't catch an error.
int cMYSQL::singleIntQuery(const char * query)
{
 int errno, value;

 errno = mysql_query(&mysql, query);
 switch ( errno ) {
   case CR_COMMANDS_OUT_OF_SYNC : 
        break;
   case CR_SERVER_GONE_ERROR : 
        break;
   case CR_SERVER_LOST : 
        break;
   case CR_UNKNOWN_ERROR :
        break;
 }

// TODO: those CR_ seem unknown from <mysql/mysql.h> to be checked
 result = mysql_store_result(&mysql);
 num_affected_row = mysql_affected_rows(&mysql);
 row = mysql_fetch_row(result);

 value = atoi(row[0]);
 mysql_free_result(result);

 return value;
}

unsigned int cMYSQL::Num_Fields()
{
 return num_fields;
}
