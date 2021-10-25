#include <gtk/gtk.h>
#include <mysql.h>
#include <stdlib.h>
#include "etymo_window.h"
#include "word_window.h"
#include "word_selects.h"
#include "mysql_change.h"
#include "error.h"

char *int10_to_str(long int val,char *dst,int radix);

extern MYSQL *conn;

void
indexPrintRow (unsigned int wordId, char *word, unsigned int parent, 
               GtkTreeIter *parentIter, GtkTreeStore *indexTreeStore, 
               indexTree *row, unsigned int num_rows)
{
  GtkTreeIter iter;
  unsigned int i;

  gtk_tree_store_append (indexTreeStore, &iter, parentIter);
  gtk_tree_store_set (indexTreeStore, &iter,
                      0, wordId,
                      1, word,
                      -1);

  for (i = 0; i < num_rows; i++)
  {
    if (row[i].parent == wordId)
    {
      indexPrintRow(row[i].id, row[i].indexTerm, row[i].parent, &iter, indexTreeStore, row, num_rows);
    }
  }
}

int
requeryIndexTree (GtkTreeView *indexTreeView)
{
  GtkTreeModel  *indexTreeStore;
  MYSQL_RES     *res_set;
  MYSQL_ROW      row;
  unsigned int           num_rows;

  indexTreeStore = gtk_tree_view_get_model (GTK_TREE_VIEW(indexTreeView));

  gchar query[] = "SELECT index_id, term, parent "
                  "FROM index_ "
                  "ORDER BY term";

  if (mysql_query (conn, query) != 0)
  {
    print_error(conn, "Query failed:");
    return 1;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    print_error(conn, "Couldn't create result set");
    return 2;
  }

  num_rows = mysql_num_rows(res_set);
  indexTree indexTerms[num_rows];

  int i = 0;
  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    if (row[2]) /* omit NULL used for root node */
    {
      indexTerms[i].id = atoi(row[0]);
      indexTerms[i].indexTerm = row[1];
      indexTerms[i].parent = atoi(row[2]);
      i++;
    }
  }

  for (i = 0; i < num_rows; i++)
  {
    if (indexTerms[i].parent == 0)
    {
      indexPrintRow (indexTerms[i].id, indexTerms[i].indexTerm, 
                    indexTerms[i].parent, NULL, GTK_TREE_STORE(indexTreeStore), 
                    indexTerms, num_rows);
    }
  }
  mysql_free_result (res_set);
  return 0;
}

int
makeGenderList (GtkComboBox *genderCombo, gint selectedGender)
{
  GtkTreeModel *genderModel;
  MYSQL_RES    *res_set;
  MYSQL_ROW     row;
  GtkTreeIter   iter;
  gint gender;

  genderModel = gtk_combo_box_get_model (GTK_COMBO_BOX(genderCombo));

  gchar query[] = "SELECT id, gender "
                  "FROM gender "
                  "ORDER BY gender";

  if (mysql_query (conn, query) != 0)
  {
    print_error(conn, "Query failed:");
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    print_error(conn, "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gender = atoi(row[0]);

    gtk_list_store_append (GTK_LIST_STORE(genderModel), &iter);
    gtk_list_store_set (GTK_LIST_STORE(genderModel), &iter,
                        0, gender,
                        1, row[1],
                        -1);

    if (gender == selectedGender)
      gtk_combo_box_set_active_iter (genderCombo, &iter);
  }
  mysql_free_result (res_set);
  return 1;
}

int
makeStatusList (GtkComboBox *statusCombo, gint selectedStatus)
{
  GtkTreeModel *statusModel;
  MYSQL_RES    *res_set;
  MYSQL_ROW     row;
  GtkTreeIter   iter;
  gint status;

  statusModel = gtk_combo_box_get_model (GTK_COMBO_BOX(statusCombo));

  gchar query[] = "SELECT status_id, status "
                  "FROM status "
                  "ORDER BY status_id";

  if (mysql_query (conn, query) != 0)
  {
    print_error(conn, "Query failed:");
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    print_error(conn, "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    status = atoi (row[0]);

    gtk_list_store_append (GTK_LIST_STORE(statusModel), &iter);
    gtk_list_store_set (GTK_LIST_STORE(statusModel), &iter,
                        0, status,
                        1, row[1],
                        -1);

    if (status == selectedStatus)
      gtk_combo_box_set_active_iter (statusCombo, &iter);
  }
  mysql_free_result (res_set);
  return 1;
}

int
makePartList (GtkComboBox *partCombo, gint selectedPart)
{
  GtkTreeModel *partModel;
  MYSQL_RES    *res_set;
  MYSQL_ROW     row;
  GtkTreeIter   iter;

  partModel = gtk_combo_box_get_model (GTK_COMBO_BOX(partCombo));

  gchar query[] = "SELECT pid, part "
                  "FROM part_speech "
                  "ORDER BY part";

  if (mysql_query (conn, query) != 0)
  {
    print_error (conn, "Query failed:");
    return 0;
  }

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    print_error (conn, "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gint part;
    part = atoi (row[0]);

    gtk_list_store_append (GTK_LIST_STORE(partModel), &iter);
    gtk_list_store_set (GTK_LIST_STORE(partModel), &iter,
                        0, part,
                        1, row[1],
                        -1);

    if (part == selectedPart)
      gtk_combo_box_set_active_iter (partCombo, &iter);
  }
  mysql_free_result (res_set);
  return 1;
}

int
selectSynonyms (unsigned int word_id, wordForm *word_form)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  gchar     *query;
  gchar     *message;
  GtkTreeIter iter;
  GtkListStore *synListStore;

  synListStore = (GtkListStore *) gtk_tree_view_get_model (GTK_TREE_VIEW(((wordForm *) word_form)->synTreeView));

  gtk_list_store_clear (GTK_LIST_STORE(synListStore));

  query = g_strdup_printf("SELECT b.synonym_id, a.word "
                          "FROM %s_words AS a INNER JOIN %s_synonyms AS b "
                          "ON a.id = b.synonym_id "
                          "WHERE b.word_id = %i "
                          "ORDER BY a.word", word_form->lang, word_form->lang, word_id);

  if (mysql_query (conn, query) != 0)
  {
    message = g_strdup_printf ("Query failed: %s", mysql_error(conn));
    g_print ("query: %s\n", query);
    printStatusMesg (GTK_STATUSBAR(((wordForm *) word_form)->statusBar), message);
    g_free (message);
    g_free (query);
    return 0;
  }
  g_free (query);

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    printStatusMesg (GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(synListStore), &iter);
    gtk_list_store_set (GTK_LIST_STORE(synListStore), &iter,
                        0, atoi(row[0]),
                        1, row[1],
                        -1);
  }
  mysql_free_result (res_set);
  return 1;
}

int
selectAntonyms (unsigned int word_id, wordForm *word_form)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  gchar     *query;
  gchar     *message;
  GtkTreeIter iter;
  GtkListStore *antListStore;

  antListStore = (GtkListStore *) gtk_tree_view_get_model (GTK_TREE_VIEW(((wordForm *) word_form)->antTreeView));

  gtk_list_store_clear (GTK_LIST_STORE(antListStore));

  query = g_strdup_printf("SELECT b.antonym_id, a.word "
                          "FROM %s_words AS a INNER JOIN %s_antonyms AS b "
                          "ON a.id = b.antonym_id "
                          "WHERE b.word_id = %i "
                          "ORDER BY a.word", word_form->lang, word_form->lang, word_id);

  if (mysql_query (conn, query) != 0)
  {
    message = g_strdup_printf ("Query failed: %s", mysql_error(conn));
    g_print ("query: %s\n", query);
    printStatusMesg (GTK_STATUSBAR(((wordForm *) word_form)->statusBar), message);
    g_free (message);
    g_free (query);
    return 0;
  }
  g_free(query);

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(antListStore), &iter);
    gtk_list_store_set (GTK_LIST_STORE(antListStore), &iter,
                        0, atoi (row[0]),
                        1, row[1],
                        -1);
  }
  mysql_free_result (res_set);
  return 1;
}

int
selectIndex(unsigned int word_id, wordForm *word_form)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  gchar     *query;
  gchar     *message;
  GtkTreeIter iter;
  GtkListStore *indexListStore;

  indexListStore = (GtkListStore *) gtk_tree_view_get_model (GTK_TREE_VIEW(((wordForm *) word_form)->indexTreeView));

  gtk_list_store_clear (GTK_LIST_STORE(indexListStore));

  query = g_strdup_printf("SELECT a.index_id, a.term "
                          "FROM index_ AS a INNER JOIN %s_index_x_words AS b "
                          "ON a.index_id = b.index_id "
                          "WHERE b.word_id = %i "
                          "ORDER BY a.term", word_form->lang, word_id);

  if (mysql_query (conn, query) != 0)
  {
    message = g_strdup_printf ("Query failed: %s", mysql_error(conn));
    g_print ("query: %s\nerror: %s", query, mysql_error(conn));
    printStatusMesg (GTK_STATUSBAR(((wordForm *) word_form)->statusBar), message);
    g_free (message);
    g_free (query);
    return 0;
  }
  g_free (query);

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(indexListStore), &iter);
    gtk_list_store_set (GTK_LIST_STORE(indexListStore), &iter,
                        0, atoi (row[0]),
                        1, row[1],
                        -1);
  }
  mysql_free_result (res_set);
  return 1;
}

int
selectAuthors (unsigned int word_id, wordForm *word_form)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  gchar     *query;
  gchar     *message;
  GtkTreeIter iter;
  GtkListStore *authorListStore;

  authorListStore = (GtkListStore *) gtk_tree_view_get_model (GTK_TREE_VIEW(((wordForm *) word_form)->authorTreeView));

  query = g_strdup_printf("SELECT a.author_id, a.author "
                          "FROM %s_authors AS a "
                          "INNER JOIN %s_authors_x_words AS b "
                          "ON a.author_id = b.author_id "
                          "WHERE b.word_id = %i "
                          "ORDER BY a.author", 
                           word_form->lang, word_form->lang, word_id);

  if (mysql_query (conn, query) != 0)
  {
    message = g_strdup_printf ("Query failed: %s", mysql_error(conn));
    g_print ("query: %s\n", query);
    printStatusMesg (GTK_STATUSBAR(((wordForm *) word_form)->statusBar), message);
    g_free (message);
    g_free (query);
    return 0;
  }
  g_free(query);

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
    printStatusMesg (GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Couldn't create result set");
    return 0;
  }

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    gtk_list_store_append (GTK_LIST_STORE(authorListStore), &iter);
    gtk_list_store_set (GTK_LIST_STORE(authorListStore), &iter,
                        0, atoi (row[0]),
                        1, row[1],
                        -1);
  }
  mysql_free_result (res_set);
  return 1;
}

int
selectWord (unsigned int word_id, wordForm *word_form)
{
  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  gchar     *query;
  gchar     *message;
  gchar     *title;
  GtkTreeModel *partModel;
  GtkTreeModel *statusModel;
  GtkTreeModel *genderModel;

  query = g_strdup_printf("SELECT id, word, def, note, updated, created, "
                          "uncertain, compound, partid, gender, perseus_def "
                          "FROM %s_words "
                          "WHERE id = %i", word_form->lang, word_id);

  if (mysql_query (conn, query) != 0)
  {
    message = g_strdup_printf ("Query failed: %s", mysql_error(conn));
/*    printStatusMesg(etyform->main_status_bar, message); */
    g_free (message);
    g_free (query);
    return 0;
  }
  g_free(query);

  if ((res_set = mysql_store_result (conn)) == NULL)
  {
/*    printStatusMesg(etyform->main_status_bar, "Couldn't create result set"); */
    return 0;
  }

  if (mysql_num_rows (res_set) > 0 && (row = mysql_fetch_row (res_set)) != NULL)
  {
    word_form->wordId = atoi (row[0]);
    gtk_entry_set_text (GTK_ENTRY(word_form->word), row[1]);
    gtk_entry_set_text (GTK_ENTRY(word_form->persDef), row[10]);
    gtk_entry_set_text (GTK_ENTRY(word_form->def), row[2]);
    gtk_entry_set_text (GTK_ENTRY(word_form->note), row[3]);
    gtk_label_set_text (GTK_LABEL(word_form->updated), row[4]);
    gtk_label_set_text (GTK_LABEL(word_form->created), row[5]);

    if (atoi (row[7]) < 0)
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(word_form->compYes), TRUE);
    else
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(word_form->compNo), TRUE);

    partModel = gtk_combo_box_get_model (word_form->part);
    gtk_list_store_clear (GTK_LIST_STORE(partModel));
    if (!makePartList (word_form->part, atoi(row[8])))
      return 0;

    statusModel = gtk_combo_box_get_model (word_form->status);
    gtk_list_store_clear (GTK_LIST_STORE(statusModel));
    if (!makeStatusList (word_form->status, atoi(row[6])))
      return 0;

    genderModel = gtk_combo_box_get_model (word_form->gender);
    gtk_list_store_clear (GTK_LIST_STORE(genderModel));
    if (!makeGenderList (word_form->gender, atoi(row[9])))
      return 0;

    title = g_strdup_printf ("%s word: %s", word_form->lang, row[1]);
    gtk_window_set_title (GTK_WINDOW(word_form->window), title);
    g_free (title);

    mysql_free_result (res_set);
    return 1;
  }
  return 0;
}
