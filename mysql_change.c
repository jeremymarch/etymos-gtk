#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mysql_change.h"

#define MAX_ERROR_MESG_LEN 4096
#define MAX_QUERY_LEN 2048

static char errorMesg[MAX_ERROR_MESG_LEN];
static char query[MAX_QUERY_LEN];

char *int10_to_str(long int val,char *dst,int radix);

char *int10_to_str(long int val,char *dst,int radix)
{
  char buffer[65];
  register char *p;
  long int new_val;
  unsigned long int uval = (unsigned long int) val;

  if (radix < 0)				/* -10 */
  {
    if (val < 0)
    {
      *dst++ = '-';
      /* Avoid integer overflow in (-val) for LONGLONG_MIN (BUG#31799). */
      uval = (unsigned long int)0 - uval;
    }
  }

  p = &buffer[sizeof(buffer)-1];
  *p = '\0';
  new_val= (long) (uval / 10);
  *--p = '0'+ (char) (uval - (unsigned long) new_val * 10);
  val = new_val;

  while (val != 0)
  {
    new_val=val/10;
    *--p = '0' + (char) (val-new_val*10);
    val= new_val;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}


char *
getError ()
{
  return errorMesg;
}

void
print_error (MYSQL *conn, char *message)
{
  fprintf (stderr, "%s\n", message);
  if (conn != NULL)
  {
    fprintf (stderr, "Error %u (%s)\n",
             mysql_errno (conn), mysql_error (conn));
  }
}

char *
strmov (register char *dst, register const char *src)
{
  while ((*dst++ = *src++)) ;
  return dst-1;
}

MYSQL *
do_connect (char *host_name, char *user_name, char *password, char *db_name,
      unsigned int port_num, char *socket_name, unsigned int flags)
{
  MYSQL *conn = NULL; /* pointer to connection handler */

  conn = mysql_init (NULL); /* allocate, initialize connection handler */
  if (conn == NULL)
  {
    print_error (NULL, "mysql_init() failed (probably out of memory)");
    return (NULL);
  }
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 32200 /* 3.22 and up */
  if (mysql_real_connect (conn, host_name, user_name, password,
          db_name, port_num, socket_name, flags) == NULL)
  {
    print_error (conn, "mysql_real_connect() failed");
    return (NULL);
  }
#else              /* pre-3.22 */
  if (mysql_real_connect (conn, host_name, user_name, password,
          port_num, socket_name, flags) == NULL)
  {
    print_error (conn, "mysql_real_connect() failed");
    return (NULL);
  }
  if (db_name != NULL)    /* simulate effect of db_name parameter */
  {
    if (mysql_select_db (conn, db_name) != 0)
    {
      print_error (conn, "mysql_select_db() failed");
      mysql_close (conn);
      return (NULL);
    }
  }
#endif
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 4100 /* 4.1.0 and up */
  if (mysql_query(conn, "SET CHARACTER SET utf8") != 0)
  {
    print_error (conn, "set character set failed");
    mysql_close (conn);
    return (NULL);
  }
#endif
  return (conn);     /* connection is established */
}

void
do_disconnect (MYSQL *conn)
{
  mysql_close (conn);
}

int
addAntonym (MYSQL *conn, unsigned int word_id, unsigned int antonym_id, char *lang)
{
  snprintf(query, MAX_QUERY_LEN, "INSERT INTO %s_antonyms "
                       "VALUES (%i, %i)", lang, word_id, antonym_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }

  return mysql_affected_rows(conn);
}

int
removeAntonym (MYSQL *conn, unsigned int word_id, unsigned int antonym_id, char *lang)
{
  snprintf(query, MAX_QUERY_LEN, "DELETE FROM %s_antonyms "
                       "WHERE word_id=%i AND antonym_id=%i", lang, word_id, antonym_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }
  return mysql_affected_rows(conn);
}

int
addSynonym (MYSQL *conn, unsigned int word_id, unsigned int synonym_id, char *lang)
{
  snprintf(query, MAX_QUERY_LEN, "INSERT INTO %s_synonyms "
                       "VALUES (%i, %i)", lang, word_id, synonym_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }
  return mysql_affected_rows(conn);
}

int
removeSynonym (MYSQL *conn, unsigned int word_id, unsigned int synonym_id, char *lang)
{
  snprintf(query, MAX_QUERY_LEN, "DELETE FROM %s_synonyms "
                       "WHERE word_id=%i AND synonym_id=%i", lang, word_id, synonym_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }
  return mysql_affected_rows(conn);
}

int
addIndexTerm (MYSQL *conn, unsigned int word_id, unsigned int index_id, char *lang)
{
  snprintf(query, MAX_QUERY_LEN, "INSERT INTO %s_index_x_words "
                       "VALUES (%i, %i)", lang, word_id, index_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }

  return mysql_affected_rows(conn);
}

int
removeIndexTerm (MYSQL *conn, unsigned int word_id, unsigned int index_id, char *lang)
{
  snprintf(query, MAX_QUERY_LEN, "DELETE FROM %s_index_x_words "
                       "WHERE word_id=%i AND index_id=%i", lang, word_id, index_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }

  return mysql_affected_rows(conn);
}

int
validateRoot (const char *root, 
              const char *def, 
              const char *ieroot, 
              const char *phonetic, 
              const char *note,
              unsigned int *lenRoot,
              unsigned int *lenDef,
              unsigned int *lenIeroot,
              unsigned int *lenPhonetic,
              unsigned int *lenNote)
{
  if ((*lenRoot = strlen(root)) < 1) /* root can't be blank */
  {
    strncpy(errorMesg, "Query failed: Root can't be blank", MAX_ERROR_MESG_LEN);
    return 0;
  }
  if (*lenRoot > MAX_ROOT_LEN)
  {
    strncpy(errorMesg, "Query failed:  Root too long", MAX_ERROR_MESG_LEN);
    return 0;
  }
  if ((*lenDef = strlen(def)) > MAX_ROOT_DEF_LEN)
  {
    strncpy(errorMesg, "Query failed:  Definition too long", MAX_ERROR_MESG_LEN);
    return 0;
  }
  if ((*lenIeroot = strlen(ieroot)) > MAX_ROOT_IEROOT_LEN)
  {
    strncpy(errorMesg, "Query failed:  IEroot too long", MAX_ERROR_MESG_LEN);
    return 0;
  }
  if ((*lenPhonetic = strlen(phonetic)) > MAX_ROOT_PHONETIC_LEN)
  {
    strncpy(errorMesg, "Query failed:  Note Phonetic long", MAX_ERROR_MESG_LEN);
    return 0;
  }
  if ((*lenNote = strlen(note)) > MAX_ROOT_NOTE_LEN)
  {
    strncpy(errorMesg, "Query failed:  Note too long", MAX_ERROR_MESG_LEN);
    return 0;
  }
  return 1;
}

int
updateRoot (MYSQL *conn, 
            unsigned int root_id, 
            const char *root, 
            const char *def, 
            const char *ieroot, 
            const char *phonetic, 
            const char *note,
            const char *lang)
{
  char *end, *query;
  unsigned int combinedLen;
  unsigned int lenRoot, lenDef, lenIeroot, lenPhonetic, lenNote;

  if (!validateRoot (root, def, ieroot, phonetic, note, &lenRoot, &lenDef, &lenIeroot, &lenPhonetic, &lenNote))
    return -1;

  combinedLen = ((lenRoot + lenDef + lenIeroot + lenPhonetic + lenNote) * 2) + 1 + strlen(lang) + 81 + 11;
  /* 81 for query 11 for maxlen of root_id */

  query = (char *) malloc (sizeof(char) * combinedLen);

  end = strmov (query, "UPDATE ");
  end = strmov (end, lang);
  end = strmov (end, "_roots SET root='");
  end += mysql_real_escape_string (conn, end, root, lenRoot);
  end = strmov (end, "', def='");
  end += mysql_real_escape_string (conn, end, def, lenDef);
  end = strmov (end, "', ieroot='");
  end += mysql_real_escape_string (conn, end, ieroot, lenIeroot);
  end = strmov (end, "', phonetic='");
  end += mysql_real_escape_string (conn, end, phonetic, lenPhonetic);
  end = strmov (end, "', note='");
  end += mysql_real_escape_string (conn, end, note, lenNote);
  end = strmov (end, "' WHERE root_id=");
  end = (char *) int10_to_str (root_id, end, 10);

  if (mysql_real_query(conn, query, end - query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));

    free(query);
    return -1;
  }
/*
  fprintf(stdout, "query %s.  Len %i\n", query, strlen(query));
  fprintf(stdout, "1: %i\n", lenRoot);
  fprintf(stdout, "2: %i\n", lenDef);
  fprintf(stdout, "3: %i\n", lenIeroot);
  fprintf(stdout, "4: %i\n", lenPhonetic);
  fprintf(stdout, "5: %i\n", lenNote);
  free(query);
*/
  return mysql_affected_rows(conn);
}

int
insertRoot (MYSQL *conn, 
            const char *root, 
            const char *def, 
            const char *ieroot, 
            const char *phonetic, 
            const char *note,
            const char *lang)
{
  char *end, *query;
  unsigned int combinedLen;
  unsigned int lenRoot, lenDef, lenIeroot, lenPhonetic, lenNote;

  if (!validateRoot (root, def, ieroot, phonetic, note, &lenRoot, &lenDef, &lenIeroot, &lenPhonetic, &lenNote))
    return -1;

  combinedLen = ((lenRoot + lenDef + lenIeroot + lenPhonetic + lenNote) * 2) + 1 + strlen(lang) + 128;
  /* 128 for query */

  query = (char *) malloc(sizeof(char) * combinedLen);

  end = strmov(query, "INSERT INTO ");
  end = strmov(end, lang);
  end = strmov(end, "_roots (root_id, root, def, ieroot, phonetic, note, updated, created) VALUES (NULL, '");
  end += mysql_real_escape_string(conn, end, root, lenRoot);
  end = strmov(end, "', '");
  end += mysql_real_escape_string(conn, end, def, lenDef);
  end = strmov(end, "', '");
  end += mysql_real_escape_string(conn, end, ieroot, lenIeroot);
  end = strmov(end, "', '");
  end += mysql_real_escape_string(conn, end, phonetic, lenPhonetic);
  end = strmov(end, "', '");
  end += mysql_real_escape_string(conn, end, note, lenNote);
  end = strmov(end, "', NULL, NOW())");

  if (mysql_real_query(conn, query, end - query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));

    free(query);
    return -1;
  }
  free(query);

  return mysql_affected_rows(conn);
}

int
deleteRoot (MYSQL *conn, unsigned int root_id, const char *lang)
{
  MYSQL_RES *res_set;
  MYSQL_ROW row;

  snprintf(query, MAX_QUERY_LEN, "SELECT COUNT(*) "
                       "FROM %s_words "
                       "WHERE root = %i", lang, root_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Result set failed: %s", mysql_error(conn));
    return -1;
  }

  if ((row = mysql_fetch_row (res_set)) == NULL)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Other error");
    return -1;
  }

  if (atoi(row[0]) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Cannot delete root because it has children");
    mysql_free_result(res_set);
    return -1;
  }
  mysql_free_result(res_set);

  snprintf(query, MAX_QUERY_LEN, "DELETE FROM %s_roots "
                       "WHERE root_id = %i", lang, root_id);

  if (mysql_query(conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", query);
    return 0;
  }
  return mysql_affected_rows(conn);
}

/***************etymo*********************/

/* select parent's parent_id until we find 0 or the word_id of the word we're moving 
 * return 0 for OK to move -1 for not OK
 */
int
validateChangeParent (MYSQL *conn, unsigned int ppid, unsigned int word_id, char *lang)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;

  if (ppid == word_id)
  {
    strncpy(errorMesg, "Parent cannot equal child!", MAX_ERROR_MESG_LEN);
    return -1;
  }

  while (1)
  {
    if (ppid == 0)
    {
      return 0; /* ok--word not in parent's path to root */
    }
    if (ppid == word_id)
    {
    strncpy(errorMesg, "Recursion Error!", MAX_ERROR_MESG_LEN);
      return -1; /* invalid--found word in parent's path to root */
    }

    snprintf(query, MAX_QUERY_LEN, "SELECT parent "
                         "FROM %s_words "
                         "WHERE id = %i", lang, ppid);

    if (mysql_query (conn, query) != 0)
    {
      snprintf(errorMesg, MAX_ERROR_MESG_LEN, 
               "Query failed. Mysql Error: %s", mysql_error(conn));
      return -1;
    }

    res_set = mysql_store_result (conn);
    if (res_set == NULL)
    {
    strncpy(errorMesg, "Couldn't create result set", MAX_ERROR_MESG_LEN);
      return -1;
    }

    row = mysql_fetch_row(res_set);
    ppid = atoi(row[0]);

    mysql_free_result(res_set);
  }
  return -1; /* we never get here */
}

/**
 * currently not transaction-safe
 * sets root to that of new parent
 */
int
changeParent (MYSQL *conn, unsigned int parent_id, unsigned int word_id, char *lang)
{
  MYSQL_RES *res;
  MYSQL_ROW  row;
  int root;

  if (validateChangeParent(conn, parent_id, word_id, lang) < 0)
  {
    return -1;
  }

  snprintf(query, MAX_QUERY_LEN, "SELECT root FROM %s_words WHERE id = %i", lang, parent_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed. MySQL Error: %s", mysql_error(conn));
    return -1;
  }

  if ((res = mysql_store_result(conn)) == NULL)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Store result failed");
    return -1;
  }

  if ((row = mysql_fetch_row(res)) == NULL)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Fetch row failed");
    mysql_free_result(res);
    return -1;
  }

  root = atoi(row[0]);

  mysql_free_result(res);

  snprintf(query, MAX_QUERY_LEN, "UPDATE %s_words "
                       "SET parent = %i , root = %i "
                       "WHERE id = %i", lang, parent_id, root, word_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed. MySQL Error: %s", mysql_error(conn));
    return -1;
  }

  return mysql_affected_rows(conn);
}

/**
 * 1. changes root.
 * 2. resets parent to 0.  
 * 3. fails if word has children.
 */
int
changeRoot (MYSQL *conn, unsigned int word_id, unsigned int root_id, char *lang)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;

  snprintf(query, MAX_QUERY_LEN, "SELECT COUNT(*) "
                       "FROM %s_words "
                       "WHERE parent = %i", lang, word_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Store Result failed: %s", mysql_error(conn));
    return -1;
  }

  if ((row = mysql_fetch_row (res_set)) == NULL)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Fetch row failed");
    return -1;
  }

  if (atoi(row[0]) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Cannot change root because word has children");
    mysql_free_result(res_set);
    return -1;
  }

  mysql_free_result(res_set);

  snprintf(query, MAX_QUERY_LEN, "UPDATE %s_words "
                       "SET root = %i, parent = 0 "
                       "WHERE id = %i", lang, root_id, word_id);

  if (mysql_query (conn, query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }

  return mysql_affected_rows(conn);
}

int
validateWord (char *def, 
              unsigned int  partId, 
              char *note, 
              char  compound, 
              int   statusId, 
              unsigned int  genderId,
              unsigned int *lenDef,
              unsigned int *lenNote)
{
  if ((*lenNote = strlen(note)) > 255)
  {
    strncpy(errorMesg, "Query failed:  Note too long", MAX_ERROR_MESG_LEN);
    return 0;
  }

  if ((*lenDef = strlen(def)) > 255)
  {
    strncpy(errorMesg, "Query failed:  Definition too long", MAX_ERROR_MESG_LEN);
    return 0;
  }

  return 1;
}

int
updateWord (MYSQL *conn, 
            unsigned int word_id, 
            const char *def, 
            unsigned int partId, 
            const char *note, 
            char compound, 
            int statusId, 
            unsigned int genderId, 
            const char *lang)
{
  char *end;
  unsigned int  lenDef, lenNote;

  if (!validateWord ((char *) def, partId, (char *) note, compound, statusId, genderId, &lenDef, &lenNote))
    return -1;

  if (compound != 0)
    compound = -1;

  end = strmov(query, "UPDATE ");
  end = strmov(end, lang);
  end = strmov(end, "_words SET def ='");
  end += mysql_real_escape_string(conn, end, def, strlen(def));
  end = strmov(end, "', partid=");
  end = (char *) int10_to_str(partId, end, 10);
  end = strmov(end, ", note='");
  end += mysql_real_escape_string(conn, end, note, strlen(note));
  end = strmov(end, "', compound=");
  end = (char *) int10_to_str(compound, end, -10);
  end = strmov(end, ", uncertain=");
  end = (char *) int10_to_str(statusId, end, -10);
  end = strmov(end, ", gender=");
  end = (char *) int10_to_str(genderId, end, 10);
  end = strmov(end, " WHERE id=");
  end = (char *) int10_to_str(word_id, end, 10); /* int10_to_str() is from libmysql */

  if (mysql_real_query(conn, query, end - query) != 0)
  {
    snprintf(errorMesg, MAX_ERROR_MESG_LEN, "Query failed: %s", mysql_error(conn));
    return -1;
  }
  return mysql_affected_rows(conn);
}
