#include <gtk/gtk.h>
#include <mysql.h>
#include <stdio.h>
#include "mysql_index.h"

static char queryBuffer[MAX_QUERY_LEN];
static char errorBuffer[MAX_ERROR_LEN];

char *
getIndexError()
{
  return errorBuffer;
}

int
beginTrans()
{
  strcpy(queryBuffer, "START TRANSACTION");
  if (mysql_query(conn, queryBuffer) != 0)
    return 0;
  else
    return 1;
}

int
commitTrans()
{
  strcpy(queryBuffer, "COMMIT");
  if (mysql_query(conn, queryBuffer) != 0)
    return 0;
  else
    return 1;
}

int
rollbackTrans()
{
  strcpy(queryBuffer, "ROLLBACK");
  if (mysql_query(conn, queryBuffer) != 0)
    return 0;
  else
    return 1;
}

static char *
index_strmov(register char *dst, register const char *src)
{
  while ((*dst++ = *src++)) ;
  return dst-1;
}

static void
print_error (MYSQL *conn, char *message)
{
  fprintf (stderr, "%s\n", message);
  if (conn != NULL)
  {
    fprintf (stderr, "Error %u (%s)\n",
             mysql_errno (conn), mysql_error (conn));
  }
}

MYSQL *
index_do_connect (char *host_name, char *user_name, char *password, char *db_name,
      unsigned int port_num, char *socket_name, unsigned int flags)
{
  MYSQL *conn;

  conn = mysql_init (NULL);
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
    return (NULL);
#endif
  return (conn);     /* connection is established */
}

void
index_do_disconnect (MYSQL *conn)
{
  mysql_close (conn);
}

int
removeRelated(int index_id, int related_id)
{
  /* note that index_id is the id of the non-preferred term
     so this will remove a whole group at a time
   */
#ifdef USE_INNODB
  if (!beginTrans())
    return -1;
#endif
  snprintf(queryBuffer, MAX_QUERY_LEN, "DELETE FROM index_related "
          "WHERE index_id = %i AND related_id = %i",
           index_id, related_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
#ifdef USE_INNODB
    if (!rollbackTrans())
      return -1;
#endif
    return -1;
  }
#ifdef USE_INNODB
  if (!commitTrans())
    return -1;
#endif
  return mysql_affected_rows(conn);
}

int
addRelated(int index_id, int related_id)
{
  snprintf(queryBuffer, MAX_QUERY_LEN, "INSERT INTO index_related VALUES ("
          "%i, %i)",
           index_id, related_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  return mysql_affected_rows(conn);
}

int
removeUse(int index_id, int group)
{
  /* note that index_id is the id of the non-preferred term
     so this will remove a whole group at a time
   */

  snprintf(queryBuffer, MAX_QUERY_LEN, "DELETE FROM index_use "
          "WHERE index_id = %i AND use_and_group = %i",
           index_id, group);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  return mysql_affected_rows(conn);
}

int
addUse(int index_id, int preferred_id, int group) /* if group == 0 then add new group */
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;

/** 
 * maybe pass &group as agument and pass back the group that the
 * term was ultimately assigned--then easy to add another to same
 * group
 */

  /* non-preferred term must be non-preferred */
  snprintf(queryBuffer, MAX_QUERY_LEN, 
             "SELECT posted "
             "FROM index_terms "
             "WHERE index_id = %i AND posted = 1",
              index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    g_printf("store result failed\n");
    return -1;
  }
  if (mysql_num_rows(res_set) > 0)
  {
    mysql_free_result(res_set);
    return -1;
  }
  /* preferred term must be preferred */
  snprintf(queryBuffer, MAX_QUERY_LEN, 
             "SELECT posted "
             "FROM index_terms "
             "WHERE index_id = %i AND posted = 0",
              preferred_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    g_printf("store result failed\n");
    return -1;
  }
  if (mysql_num_rows(res_set) > 0)
  {
    mysql_free_result(res_set);
    return -1;
  }

  if (group == 0) /* start a new group */
  {
    snprintf(queryBuffer, MAX_QUERY_LEN, "SELECT MAX(use_and_group) + 1 "
             "FROM index_use WHERE index_id = %i",
              index_id);

    if (mysql_query(conn, queryBuffer) != 0)
    {
      g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
      return -1;
    }
    if ((res_set = mysql_store_result (conn)) == NULL)
    {
      g_printf("store result failed\n");
      return -1;
    }
    if (mysql_num_rows(res_set) == 0)
    {
      mysql_free_result(res_set);
      group = 1; /* this is the first use group for this term */
    }
    else
    {
      if ((row = mysql_fetch_row (res_set)) == NULL)
      {
        mysql_free_result(res_set);
        return -1;
      }
      group = atoi(row[0]);
      mysql_free_result(res_set);
    }
  }
  snprintf(queryBuffer, MAX_QUERY_LEN, "INSERT INTO index_use VALUES ("
          "%i, %i, %i)",
           index_id, preferred_id, group);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  return mysql_affected_rows(conn);
}

int
deleteTerm(int index_id, char *foreignTable, int autoDelNonPref)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;

  /* optionally check that term isn't referred to elsewhere */
  if (foreignTable)
  {
    snprintf(queryBuffer, MAX_QUERY_LEN, 
            "SELECT index_id FROM %s "
            "WHERE index_id = %i",
             foreignTable, index_id);

    if (mysql_query(conn, queryBuffer) != 0)
    {
      g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
      return -1;
    }

    if ((res_set = mysql_store_result (conn)) == NULL)
    {
      g_printf("store result failed\n");
      return -1;
    }
    if (mysql_num_rows(res_set) < 0)
    {
      mysql_free_result(res_set);
      return 0;
    }
  }

/** 
 * optionally return error if this term references 
 * non-preferred terms--must delete them first. 
 * otherwise automatically delete them after 
 * deleting preferred term.
 */
  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "SELECT preferred_id "
          "FROM index_use "
          "WHERE index_id = %i",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  if ((res_set = mysql_store_result(conn)) == NULL)
    return -1;

  if (!autoDelNonPref && mysql_num_rows(res_set) > 0)
  {
    mysql_free_result(res_set);
    return -1;
  }

  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "DELETE FROM index_related "
          "WHERE index_id = %i",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }

  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "DELETE FROM index_use "
          "WHERE index_id = %i",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }

  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "DELETE FROM index_use "
          "WHERE preferred_id = %i",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }

  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "DELETE FROM index_var "
          "WHERE index_id = %i",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }

  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "DELETE FROM index_terms "
          "WHERE index_id = %i",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }

  /*now delete all non-preferred terms if autoDelNonPref is true */
  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    if (deleteTerm(atoi(row[0]), foreignTable, autoDelNonPref) < 1)
      return -1;
  }

  return mysql_affected_rows(conn);
}

int
updateTerm(int index_id, const char *term, const char *scope_note, int parent_id, const char *see_also, int preferred)
{
  char *end;
  int   res;
  MYSQL_RES *res_set;
  MYSQL_ROW row;

  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "SELECT preferred_id "
          "FROM index_use "
          "WHERE index_id = %i LIMIT 1",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    snprintf(errorBuffer, MAX_ERROR_LEN, 
             "query failed: %s\nmysql error: %s\n", 
             queryBuffer, mysql_error(conn));
    return -1;
  }
  if ((res_set = mysql_store_result(conn)) == NULL)
    return -1;

  if (preferred && mysql_num_rows(res_set) > 0)
  {
    snprintf(errorBuffer, MAX_ERROR_LEN, 
             "references preferred terms");
    return -1;
  }

  snprintf(queryBuffer, MAX_QUERY_LEN, 
          "SELECT preferred_id "
          "FROM index_use "
          "WHERE preferred_id = %i LIMIT 1",
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  if ((res_set = mysql_store_result(conn)) == NULL)
    return -1;

  if (!preferred && mysql_num_rows(res_set) > 0)
  {
    snprintf(errorBuffer, MAX_ERROR_LEN, 
             "references non-preferred terms");
    return -1;
  }

  /* should I do multi-table update or two separate update queries? */
  end = strmov(queryBuffer, "UPDATE index_terms SET term='");
  end += mysql_real_escape_string(conn, end, term, strlen(term));
  end = strmov(end, "', parent_id=");
  end = int10_to_str(parent_id, end, 10);
  end = strmov(end, ", posted=");
  end = int10_to_str(preferred, end, 10);
  end = strmov(end, ", updated_by = CURRENT_USER() ");
  end = strmov(end, " WHERE index_id=");
  end = (char *) int10_to_str(index_id, end, 10);

  if (mysql_real_query(conn, queryBuffer, end - queryBuffer) != 0)
  {
    return -1;
  }

  res = mysql_affected_rows(conn);

  end = strmov(queryBuffer, "UPDATE index_var SET scope_note='");
  end += mysql_real_escape_string(conn, end, scope_note, strlen(scope_note));
  end = strmov(end, "', see_also='");
  end += mysql_real_escape_string(conn, end, see_also, strlen(see_also));
  end = strmov(end, "' WHERE index_id=");
  end = (char *) int10_to_str(index_id, end, 10);

  if (mysql_real_query(conn, queryBuffer, end - queryBuffer) != 0)
  {
    return -1;
  }
  return res ? res : mysql_affected_rows(conn);
}

int
insertTerm (const char *term, const char *scope_note, int parent_id, const char *see_also)
{
  char *end;
  int   last_id;

  end = strmov(queryBuffer, "INSERT INTO index_terms ");
  end = strmov(end, " (index_id, term, parent_id, created, created_by) VALUES (NULL, '");
  end += mysql_real_escape_string(conn, end, term, strlen(term));
  end = strmov(end, "', ");
  end = (char *) int10_to_str(parent_id, end, 10);
  end = strmov(end, ", NOW(), CURRENT_USER())");

  if (mysql_real_query(conn, queryBuffer, end - queryBuffer) != 0)
  {
    g_printf("query: %s\nError: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }

  last_id = mysql_insert_id(conn);

  end = strmov(queryBuffer, "INSERT INTO index_var ");
  end = strmov(end, " (index_id, scope_note, see_also) VALUES (");
  end = (char *) int10_to_str(last_id, end, 10);
  end = strmov(end, ", '");
  end += mysql_real_escape_string(conn, end, scope_note, strlen(scope_note));
  end = strmov(end, "', '");
  end += mysql_real_escape_string(conn, end, see_also, strlen(see_also));
  end = strmov(end, "')");

  if (mysql_real_query(conn, queryBuffer, end - queryBuffer) != 0)
  {
    g_printf("query: %s\nError: %s\n", queryBuffer, mysql_error(conn));
    return -1;
  }
  /* should we automatically try to delete first half if
   * second half fails?
   */

  return mysql_affected_rows(conn);
}

int
selectRelated(int index_id, indexForm *index)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  snprintf(queryBuffer, MAX_QUERY_LEN, "SELECT a.index_id, a.term "
           "FROM index_terms AS a INNER JOIN index_related AS b "
           "ON a.index_id = b.related_id WHERE b.index_id = %i "
           "ORDER BY a.term", 
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    g_printf("store result failed\n");
    return 0;
  }
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(index->relatedTreeView));

  gtk_list_store_clear(GTK_LIST_STORE(model));

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(model), &iter);
    gtk_list_store_set (GTK_LIST_STORE(model), &iter,
                        0, atoi(row[0]),
                        1, row[1],
                        -1);
  }
  mysql_free_result(res_set);
  return 1;
}

int
selectUseFor(int index_id, indexForm *index)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  snprintf(queryBuffer, MAX_QUERY_LEN, "SELECT b.index_id, b.term "
           "FROM index_use AS a INNER JOIN index_terms AS b "
           "ON a.index_id = b.index_id WHERE a.preferred_id = %i "
           "ORDER BY b.term", 
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    g_printf("store result failed\n");
    return 0;
  }
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(index->useTreeView));
  gtk_list_store_clear(GTK_LIST_STORE(model));

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(model), &iter);
    gtk_list_store_set (GTK_LIST_STORE(model), &iter,
                        0, atoi(row[0]),
                        1, row[1],
                        -1);
  }
  return 1;
}

int
selectUse(int index_id, indexForm *index)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  snprintf(queryBuffer, MAX_QUERY_LEN, "SELECT a.index_id, b.term, a.use_and_group "
           "FROM index_use AS a INNER JOIN index_terms AS b "
           "ON a.preferred_id = b.index_id WHERE a.index_id = %i "
           "ORDER BY a.use_and_group", 
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    g_printf("store result failed\n");
    return 0;
  }
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(index->useTreeView));
  gtk_list_store_clear(GTK_LIST_STORE(model));

  int num_rows;
  num_rows = mysql_num_rows(res_set);

  useStruct i[num_rows];
  int n = 0;
  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    i[n].id = atoi(row[0]);    /* index_id not preferred_id */
    i[n].group = atoi(row[2]);
    strcpy(i[n].term, row[1]);
    n++;
  }
  mysql_free_result(res_set);

  char use_row[200];
  char *end;
  end = &use_row[0];

  for (n = 0; n < num_rows; n++)
  {
    end = strmov(end, i[n].term);
    for ( ; i[n].group == i[n + 1].group; n++)
    {
      end = strmov(end, " AND ");
      end = strmov(end, i[n + 1].term);
    }

    gtk_list_store_append (GTK_LIST_STORE(model), &iter);
    gtk_list_store_set (GTK_LIST_STORE(model), &iter,
                        0, i[n].id,
                        1, use_row,
                        2, i[n].group,
                        -1);
    end = &use_row[0];
  }
}

int
selectTerm(int index_id, indexForm *index)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  gboolean preferred;
  GtkTextBuffer *textBuf;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *col;
/* should this be a left join */
  snprintf(queryBuffer, MAX_QUERY_LEN, 
           "SELECT a.index_id, a.term, b.see_also, b.scope_note, a.updated, a.created, a.posted "
           "FROM index_terms AS a INNER JOIN index_var AS b "
           "ON a.index_id = b.index_id WHERE a.index_id = %i", 
           index_id);

  if (mysql_query(conn, queryBuffer) != 0)
  {
    g_printf("query failed: %s\nmysql error: %s\n", queryBuffer, mysql_error(conn));
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    g_printf("store result failed\n");
    return 0;
  }
  if ((row = mysql_fetch_row (res_set)) == NULL)
  {
    g_printf("fetch row failed\n");
    mysql_free_result(res_set);
    return 0;
  }

    col = gtk_tree_view_get_column (GTK_TREE_VIEW(index->useTreeView), 0);
    renderer = gtk_cell_renderer_text_new ();
  if (atoi(row[6]) == 0)
  {
    preferred = FALSE;
    gtk_tree_view_column_set_title (col, "Use... or");

    gtk_widget_hide(GTK_WIDGET(index->scopeTextView));
    gtk_widget_hide(GTK_WIDGET(index->scopeLabel));
    gtk_widget_hide(GTK_WIDGET(index->parentText));
    gtk_widget_hide(GTK_WIDGET(index->parentLabel));
    gtk_widget_hide(GTK_WIDGET(index->seeAlsoText));
    gtk_widget_hide(GTK_WIDGET(index->seeAlsoLabel));
    gtk_widget_hide(GTK_WIDGET(index->relatedScrolledWindow));
  }
  else
  {
    preferred = TRUE;
    gtk_tree_view_column_set_title (col, "Use For");

    gtk_widget_show(GTK_WIDGET(index->scopeTextView));
    gtk_widget_show(GTK_WIDGET(index->scopeLabel));
    gtk_widget_show(GTK_WIDGET(index->parentText));
    gtk_widget_show(GTK_WIDGET(index->parentLabel));
    gtk_widget_show(GTK_WIDGET(index->seeAlsoText));
    gtk_widget_show(GTK_WIDGET(index->seeAlsoLabel));
    gtk_widget_show(GTK_WIDGET(index->relatedScrolledWindow));
  }

  textBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(index->scopeTextView));

  index->index_id = atoi(row[0]);
  gtk_entry_set_text(GTK_ENTRY(index->termText), row[1]);
  gtk_text_buffer_set_text(textBuf, row[3], strlen(row[3]));
  gtk_entry_set_text(GTK_ENTRY(index->seeAlsoText), row[2]);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(index->preferred), preferred);
  gtk_label_set_text(GTK_LABEL(index->updated), row[4]);
  gtk_label_set_text(GTK_LABEL(index->created), row[5]);

  mysql_free_result(res_set);
  if (preferred)
      selectUseFor(index_id, index);
  else
    selectUse(index_id, index);
  selectRelated(index_id, index);

  return 1;
}
