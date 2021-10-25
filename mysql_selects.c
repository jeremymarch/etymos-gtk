#include <gtk/gtk.h>
#include <mysql.h>
#include "etymo_window.h"          /* for column enums */
#include "mysql_change.h"
#include "root_window.h"   /* must come before mysql_selects.h */
#include "word_window.h"
#include "mysql_selects.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>

extern MYSQL *conn;

int
selectRoot(guint root_id, rootForm *root_form)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  gchar     *query;
  gchar     *message;
  gchar     *title;

  query = g_strdup_printf("SELECT root_id, root, def, ieroot, "
                          "phonetic, note, updated, created "
                          "FROM %s_roots "
                          "WHERE root_id = %i", 
                           root_form->lang, root_id);

  if (mysql_query(conn, query) != 0)
  {
    message = g_strdup_printf("Query failed: %s", mysql_error(conn));
/*    printStatusMesg(etyform->main_status_bar, message); */
    g_free(message);
    g_free(query);
    return 0;
  }
  g_free(query);

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
 /*    printStatusMesg(etyform->main_status_bar, "Couldn't create result set"); */
    return 0;
  }

  if (mysql_num_rows(res_set) > 0 && (row = mysql_fetch_row (res_set)) != NULL)
  {
    root_form->rootId = atoi(row[0]);
    gtk_entry_set_text(GTK_ENTRY(root_form->root), row[1]);
    gtk_entry_set_text(GTK_ENTRY(root_form->def), row[2]);
    gtk_entry_set_text(GTK_ENTRY(root_form->ie), row[3]);
    gtk_entry_set_text(GTK_ENTRY(root_form->phonetic), row[4]);
    gtk_entry_set_text(GTK_ENTRY(root_form->note), row[5]);
    gtk_label_set_text(GTK_LABEL(root_form->updated), row[6]);
    gtk_label_set_text(GTK_LABEL(root_form->created), row[7]);

    title = g_strdup_printf("%s root: %s", root_form->lang, row[1]);
    gtk_window_set_title(GTK_WINDOW(root_form->window), title);
    g_free(title);

    mysql_free_result(res_set);
    return 1;
  }
  return 0;
}

int
reQueryWordList(etymosf *etyform, const gchar *likeArg)
{
  GtkListStore *wordListStore;
  GtkTreeIter    iter;
  MYSQL_RES     *res_set;
  MYSQL_ROW      row;
  char           query[350]; /* should test likeArg length and return if too long--else pos. buff-over-run */
  char          *end;
  guint          i;

  wordListStore = (GtkListStore *) gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->wordListView));
  gtk_list_store_clear (GTK_LIST_STORE(wordListStore));

  end = strmov(query, "SELECT ");

  for (i = 0; i < etyform->num_word_fields; i++)
  {
    end = strmov(end, etyform->word_fields[i]);
    if (i < etyform->num_word_fields - 1)
      end = strmov(end, ", ");
  }

  end = strmov(end, " FROM ");
  end = strmov(end, etyform->lang);
  end = strmov(end, "_words WHERE word LIKE '");
  end += mysql_real_escape_string(conn, end, likeArg, strlen(likeArg));
  end = strmov(end, "%' ORDER BY word LIMIT ");
  end = (char *) int10_to_str(MAX_ROWS, end, 10);

  if (mysql_real_query(conn, query, end - query) != 0)
  {
    print_error(conn, "Query failed--reQueryWordList:");
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    print_error(conn, "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(wordListStore), &iter);
    gtk_list_store_set (GTK_LIST_STORE(wordListStore), &iter,
                        WORD_COL_ID, atoi(row[0]),
                        WORD_COL_WORD, row[1],
                        -1);
  }  
  mysql_free_result (res_set);
  return 1;
}

gchar *
indent (gchar *word, guint level)
{
  gchar *indentedWord;
  gsize  indentLen = level * TAB_SIZE;
  gchar *indent;

  if (level > 0)
  {
    indent = g_strnfill (indentLen, '\t');

    indentedWord = g_strjoin (NULL, indent, word, NULL);
    g_free(indent);
  }
  else
  {
    indentedWord = g_strdup(word);
  }
  return indentedWord;
}

void
itreePrintRow (uint wordId, char *word, uint parent, uint level, 
               GtkListStore *itreeListStore, struct itree *row, uint num_rows)
{
  static GtkTreeIter iter;
  uint i;
  char *indentedWord;

  indentedWord = indent(word, level);

  gtk_list_store_append (itreeListStore, &iter);
  gtk_list_store_set (itreeListStore, &iter,
                      ITREE_COL_ID, wordId,
                      ITREE_COL_WORD, indentedWord,
                      -1);

  g_free(indentedWord);

  /* I couldn't free indentedWord before because I didn't 
   * allocate it if I didn't need to indent.
   * Now indent() always allocates it for each word 
   */ 

  for (i = 0; i < num_rows; i++)
  {
    if (row[i].parent == wordId)
    {
      itreePrintRow(row[i].id, row[i].word, row[i].parent, 
                    level + 1, itreeListStore, row, num_rows);
    }
  }
}

int
reQueryItreeList(etymosf *etyform, guint root_id)
{
  GtkTreeModel *atreeListStore, *itreeListStore;
  GtkTreeIter iter;
  GtkTreeIter rootIter;
  MYSQL_RES     *res_set;
  MYSQL_RES     *rootRes_set;
  MYSQL_ROW      row;
  MYSQL_ROW      rootRow;
  gchar         *query;
  uint           num_rows;

  itreeListStore = gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->itreeListView));
  atreeListStore = gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->alphaListView));

  gtk_list_store_clear (GTK_LIST_STORE(itreeListStore));
  gtk_list_store_clear (GTK_LIST_STORE(atreeListStore));

/* get root */
  query = g_strdup_printf("SELECT root_id, root "
                          "FROM %s_roots "
			  "WHERE root_id = %i", etyform->lang, root_id);

  if (mysql_query (conn, query) != 0)
  {
    print_error(conn, "Query failed:");
    return 1;
  }
  g_free(query);

  if ((rootRes_set = mysql_store_result (conn)) == NULL)
  {
    print_error(conn, "Couldn't create result set");
    return 2;
  }

  if (mysql_num_rows(rootRes_set) < 1)
    return 3;

  rootRow = mysql_fetch_row(rootRes_set);

/* get words */
  query = g_strdup_printf("SELECT id, word, parent "
                          "FROM %s_words "
			  "WHERE root = %i "
                          "ORDER BY word", etyform->lang, root_id);

  if (mysql_query (conn, query) != 0)
  {
    print_error(conn, "Query failed:");
    return 1;
  }
  g_free(query);
  
  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    print_error(conn, "Couldn't create result set");
    return 2;
  }

  num_rows = mysql_num_rows(res_set);

  struct itree itreeWords[num_rows];

  int i = 0;
  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    itreeWords[i].id = atoi(row[0]);
    itreeWords[i].word = row[1];
    itreeWords[i].parent = atoi(row[2]);
    i++;
  }

  gtk_list_store_append (GTK_LIST_STORE(itreeListStore), &rootIter);
  gtk_list_store_set (GTK_LIST_STORE(itreeListStore), &rootIter,
                      ITREE_COL_ID, atoi(rootRow[0]),
                      ITREE_COL_WORD, rootRow[1],
                      -1);

  for (i = 0; i < num_rows; i++)
  {
    gtk_list_store_append (GTK_LIST_STORE(atreeListStore), &iter);
    gtk_list_store_set (GTK_LIST_STORE(atreeListStore), &iter,
                        ATREE_COL_ID, itreeWords[i].id,
                        ATREE_COL_WORD, itreeWords[i].word,
                        -1);

    if (itreeWords[i].parent == 0)
    {
      itreePrintRow(itreeWords[i].id, itreeWords[i].word, 
                    itreeWords[i].parent, 1, GTK_LIST_STORE(itreeListStore), 
                    itreeWords, num_rows);
    }
  }
  mysql_free_result (rootRes_set);
  mysql_free_result (res_set);
  return 0;
}

int
reQueryRootList(etymosf *etyform, const gchar *likeArg)
{
  GtkListStore *rootListStore;
  GtkTreeIter   iter;
  MYSQL_RES    *res_set;
  MYSQL_ROW     row;
  char          query[250];
  char         *end;

  rootListStore = (GtkListStore *) gtk_tree_view_get_model(GTK_TREE_VIEW(etyform->rootListView));

  gtk_list_store_clear (GTK_LIST_STORE(rootListStore));

  end = strmov(query, "SELECT root_id, root, def FROM ");
  end = strmov(end, etyform->lang);
  end = strmov(end, "_roots WHERE root LIKE '");
  end += mysql_real_escape_string(conn, end, likeArg, strlen(likeArg));
  end = strmov(end, "%' ORDER BY root LIMIT ");
  end = (char *) int10_to_str(MAX_ROWS, end, 10);

  if (mysql_real_query(conn, query, end - query) != 0)
  {
    print_error(conn, "Query failed--reQueryRootList:");
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    print_error(conn, "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(rootListStore), &iter);
    gtk_list_store_set (GTK_LIST_STORE(rootListStore), &iter,
                        ROOT_COL_ID, atoi(row[0]),
                        ROOT_COL_ROOT, row[1],
                        ROOT_COL_DEF, row[2],
                        -1);
  }  
  mysql_free_result (res_set);
  return 1;
}
