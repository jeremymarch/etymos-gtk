#include <gtk/gtk.h>
#include <mysql.h>
#include <string.h>
#include "error.h"
#include "etymo_window.h"
#include "mysql_change.h"
#include "word_window.h" /* for createWordWindow() ... */
#include "root_window.h" /* for createRootWindow() ... */
#include "mysql_selects.h"

gchar *
skipIndents(gchar *s)
{
  while (*s == '\t')
    s++;
  return s;
}

gboolean
sigAddNewRoot(GtkWidget *button, gpointer ety)
{
  etymosf *etyform;

  etyform = (etymosf *) ety;

  if (createRootWindow(NEW_ROW, 0, etyform->lang) == NULL)
    g_print("couldn't open root window2\n");

  return FALSE;
}

gboolean
sigEditRoot(GtkTreeView *treeview, GtkTreePath *arg1, GtkTreeViewColumn *arg2, gpointer ety)

{
  GtkTreeSelection *rootSelection;
  GtkTreeModel     *rootModel;
  GtkTreeIter       rootIter;
  gint              root_id;
  GtkWindow        *rootWindow;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  rootSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(etyform->rootListView));
  rootModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->rootListView));

  if (gtk_tree_selection_get_selected(rootSelection, &rootModel, &rootIter))
  {
    gtk_tree_model_get (rootModel, &rootIter, ROOT_COL_ID, &root_id, -1);

    if ((rootWindow = createRootWindow(EDIT_ROW, root_id, etyform->lang)) == NULL)
    {
      g_print("couldn't open root window\n");
    }
  }
  return FALSE;
}

gboolean
sigEditWord(GtkTreeView *treeview, GtkTreePath *arg1, GtkTreeViewColumn *arg2, gpointer ety)
{
  GtkTreeSelection *wordSelection;
  GtkTreeModel     *wordModel;
  GtkTreeIter       wordIter;
  guint             word_id;
  GtkWindow        *wordWindow;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  wordSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(etyform->wordListView));
  wordModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->wordListView));

  if (gtk_tree_selection_get_selected(wordSelection, &wordModel, &wordIter))
  {

    gtk_tree_model_get (wordModel, &wordIter, WORD_COL_ID, &word_id, -1);

    if ((wordWindow = createWordWindow(EDIT_ROW, word_id, etyform->lang)) == NULL)
      g_print("couldn't open word window\n");

    /* gtk_window_set_transient_for(GTK_WINDOW(wordWindow), parent); */
  }
  return FALSE;
}

/* do I need to free the selections and models here? */
gboolean
sigAddtoParent(GtkWidget *button, gpointer ety)
{
  GtkTreeSelection *itreeSelection, *atreeSelection, *rootSelection;
  GtkTreeModel     *itreeModel,     *atreeModel,     *rootModel;
  GtkTreeIter       itreeIter,       atreeIter,       rootIter;
  gchar            *message;
  gint              affected_rows;

  GtkTreePath *selPath, *rootPath;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  guint parent_id;
  guint word_id;
  guint root_id;

  itreeSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->itreeListView));
  itreeModel     = gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->itreeListView));

  atreeSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->alphaListView));
  atreeModel     = gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->alphaListView));

  rootSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->rootListView));
  rootModel     = gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->rootListView));

  if (!gtk_tree_selection_get_selected (itreeSelection, &itreeModel, &itreeIter) ||
      !gtk_tree_selection_get_selected (atreeSelection, &atreeModel, &atreeIter) ||
      !gtk_tree_selection_get_selected (rootSelection, &rootModel, &rootIter))
  {
    printStatusMesg (etyform->main_status_bar, "You must select a word and a parent!");
    return FALSE;
  }

  gtk_tree_model_get (itreeModel, &itreeIter, ITREE_COL_ID, &parent_id, -1);
  gtk_tree_model_get (atreeModel, &atreeIter, ATREE_COL_ID, &word_id, -1);
  gtk_tree_model_get (rootModel, &rootIter, ROOT_COL_ID, &root_id, -1);

  selPath = gtk_tree_model_get_path (itreeModel, &itreeIter);
  rootPath = gtk_tree_path_new_first();

  if (!gtk_tree_path_compare (selPath, rootPath))
  {
    affected_rows = changeRoot (conn, word_id, parent_id, etyform->lang);
  }
  else
  {
    affected_rows = changeParent (conn, parent_id, word_id, etyform->lang);
  }
  gtk_tree_path_free (selPath);
  gtk_tree_path_free (rootPath);

  if (affected_rows > 0)
  { 
    gchar *word_word;
    gchar *parent_word;

    gtk_tree_model_get (itreeModel, &itreeIter, ITREE_COL_WORD, &parent_word, -1);
    gtk_tree_model_get (atreeModel, &atreeIter, ATREE_COL_WORD, &word_word, -1);

    message = g_strdup_printf ("Updated %i rows.  Moved %s under %s.", affected_rows, word_word, skipIndents (parent_word));
    printStatusMesg (etyform->main_status_bar, message);
    g_free (message);
    g_free (parent_word);
    g_free (word_word);

    reQueryItreeList (etyform, root_id);
    return FALSE;
  }
  else if (affected_rows == 0)
  {
    printStatusMesg (etyform->main_status_bar, "Updated 0 rows");
    return FALSE;
  }
  else if (affected_rows < 0)
  {
    printStatusMesg (etyform->main_status_bar, getError());
  }
  return FALSE;
}

gboolean
sigRemoveWord (GtkWidget *button, gpointer ety)
{
  GtkTreeSelection *itreeSelection, *rootSelection;
  GtkTreeModel     *itreeModel,     *rootModel;
  GtkTreeIter       itreeIter,       rootIter;
  gchar            *message;
  gint              affected_rows;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  itreeSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->itreeListView));
  itreeModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->itreeListView));

  rootSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(etyform->rootListView));
  rootModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->rootListView));

  if (!gtk_tree_selection_get_selected (itreeSelection, &itreeModel, &itreeIter) ||
      !gtk_tree_selection_get_selected (rootSelection, &rootModel, &rootIter))
  {
    printStatusMesg (etyform->main_status_bar, "You must select a word and a root!");
    return FALSE;
  }
  guint  word_id;
  guint  root_id;
  gchar *word_word;
  gchar *root_word;

  gtk_tree_model_get (itreeModel, &itreeIter, WORD_COL_ID, &word_id, -1);
  gtk_tree_model_get (rootModel, &rootIter, ROOT_COL_ID, &root_id, -1);

  if (word_id == 0)
  {
    printStatusMesg (etyform->main_status_bar, "You can't remove the root from itself!");
    return FALSE;
  }

  /* go to dialogue here to comfirm--but should check if delete is permitted first */

  if ((affected_rows = changeRoot (conn, word_id, 0, etyform->lang)) > 0)
  {
    gtk_tree_model_get (rootModel, &rootIter, ROOT_COL_ROOT, &root_word, -1);
    gtk_tree_model_get (itreeModel, &itreeIter, WORD_COL_WORD, &word_word, -1);

    message = g_strdup_printf ("Updated %i rows.  Removed %s from root %s.", affected_rows, skipIndents(word_word), root_word);
    printStatusMesg (etyform->main_status_bar, message);
    g_free (message);
    g_free (root_word);
    g_free (word_word);

    reQueryItreeList (etyform, root_id);
    return FALSE;
  }
  else if (affected_rows == 0)
  {
    printStatusMesg(etyform->main_status_bar, "Updated 0 rows");
    return FALSE;
  }
  else if (affected_rows < 0)
  {
    printStatusMesg(etyform->main_status_bar, getError());
    return FALSE;
  }
  return FALSE;
}

gboolean
sigAddtoRoot(GtkWidget *button, gpointer ety)
{
  GtkTreeSelection *wordSelection, *rootSelection;
  GtkTreeModel     *wordModel,     *rootModel;
  GtkTreeIter       wordIter,       rootIter;
  gchar            *message;
  gint              affected_rows;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  wordSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->wordListView));
  wordModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->wordListView));

  rootSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(etyform->rootListView));
  rootModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->rootListView));

  if (!gtk_tree_selection_get_selected(wordSelection, &wordModel, &wordIter) ||
      !gtk_tree_selection_get_selected(rootSelection, &rootModel, &rootIter))
  {
    printStatusMesg(etyform->main_status_bar, "You must select a word and a root!");
    return FALSE;
  }
  guint  word_id;
  guint  root_id;
  gchar *word_word;
  gchar *root_word;

  gtk_tree_model_get (wordModel, &wordIter, WORD_COL_ID, &word_id, -1);
  gtk_tree_model_get (rootModel, &rootIter, ROOT_COL_ID, &root_id, -1);

  if ((affected_rows = changeRoot(conn, word_id, root_id, etyform->lang)) > 0)
  {
    gtk_tree_model_get (rootModel, &rootIter, ROOT_COL_ROOT, &root_word, -1);
    gtk_tree_model_get (wordModel, &wordIter, WORD_COL_WORD, &word_word, -1);

    message = g_strdup_printf("Updated %i rows.  Moved %s under root %s.", affected_rows, word_word, root_word);
    printStatusMesg(etyform->main_status_bar, message);
    g_free(message);
    g_free(root_word);
    g_free(word_word);

    reQueryItreeList(etyform, root_id);
    return FALSE;
  }
  else if (affected_rows == 0)
  {
    printStatusMesg(etyform->main_status_bar, "Updated 0 rows");
    return FALSE;
  }
  else if (affected_rows < 0)
  {
    printStatusMesg(etyform->main_status_bar, getError());
    return FALSE;
  }
  return FALSE;
}

gboolean
sigReQueryRootList(GtkWidget *rootSearchTextBox, gpointer ety)
{
  const gchar *rootLike;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  rootLike = gtk_entry_get_text (GTK_ENTRY (rootSearchTextBox));

  reQueryRootList(etyform, rootLike);

  return FALSE;
}

gboolean
sigReQueryITreeList(GtkTreeSelection *rootListSelection, gpointer ety)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  /* This will only work in single or browse selection mode! */
  if (gtk_tree_selection_get_selected(rootListSelection, &model, &iter))
  {
    int root_id;
    gtk_tree_model_get (model, &iter, ROOT_COL_ID, &root_id, -1);
    reQueryItreeList(etyform, root_id);
  }
  return FALSE;
}

gboolean
sigReQueryWordList(GtkWidget *wordSearchTextBox, gpointer ety)
{
  const gchar *wordLike;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  wordLike = gtk_entry_get_text (GTK_ENTRY (wordSearchTextBox));

  reQueryWordList(etyform, wordLike);

  return FALSE;
}

gboolean
sigChangeLang(GtkWidget *comboBox, gpointer ety)
{
  gchar        *newLang;
  GtkTreeIter   iter;
  GtkTreeModel *model;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX(comboBox), &iter))
  {
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(comboBox));
    gtk_tree_model_get(model, &iter, 1, &newLang, -1);

    strncpy(etyform->lang, newLang, MAX_LANG_LEN);

    init_root_list(etyform);
    init_word_list(etyform);
    init_itree_list(etyform);
    init_alpha_list(etyform);
  }
  return FALSE;
}

void
iDragDataReceived(GtkWidget *destTreeView, GdkDragContext *context, int x, int y,
                        GtkSelectionData *seldata, guint info, guint time,
                        gpointer ety)
{
  guint word_id;
  guint parent_id;
  guint root_id;
  etymosf *etyform;

  etyform = (etymosf *) ety;

  memcpy(&word_id, seldata->data, sizeof(guint32));

  GtkTreeSelection *itreeSelection, *rootSelection;
  GtkTreeModel     *itreeModel,     *rootModel;
  GtkTreeIter       itreeIter,       rootIter;
  gchar            *message;
  gint              affected_rows;

  itreeSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->itreeListView));
  itreeModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->itreeListView));

  rootSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->rootListView));
  rootModel =     gtk_tree_view_get_model (GTK_TREE_VIEW(etyform->rootListView));

  if (!gtk_tree_selection_get_selected (itreeSelection, &itreeModel, &itreeIter) ||
      !gtk_tree_selection_get_selected (rootSelection, &rootModel, &rootIter))
  {
    printStatusMesg (etyform->main_status_bar, "You must select a word and a parent!");
    return;
  }

  gtk_tree_model_get (itreeModel, &itreeIter, ITREE_COL_ID, &parent_id, -1);
  gtk_tree_model_get (rootModel, &rootIter, ROOT_COL_ID, &root_id, -1);

  if ((affected_rows = changeParent (conn, parent_id, word_id, etyform->lang)) > 0)
  {
    message = g_strdup_printf ("Updated %i rows.", affected_rows);
    printStatusMesg (etyform->main_status_bar, message);
    g_free (message);

    reQueryItreeList (etyform, root_id);
    return;
  }
  else if (affected_rows == 0)
  {
    printStatusMesg (etyform->main_status_bar, "Updated 0 rows");
    return;
  }
  else if (affected_rows < 0)
  {
    printStatusMesg (etyform->main_status_bar, getError());
  }

  return;
}

gboolean
iDragDataMotion(GtkWidget *destTreeView, GdkDragContext *drag_context,
                                            gint x,
                                            gint y,
                                            guint time,
                                            gpointer data)
{
  GtkTreePath *path;

  if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(destTreeView), x, y, &path, NULL, NULL, NULL))
  {
    gtk_tree_view_set_cursor (GTK_TREE_VIEW(destTreeView), path, NULL, FALSE);
    gtk_tree_path_free(path);
  }

  return FALSE;
}

void
wordDragDataGet(GtkWidget *wordView, GdkDragContext *context, GtkSelectionData *seldata, 
                guint info, guint time, gpointer data)
{
  GtkTreeModel *wordStore;
  GtkTreeIter iter;
  GtkTreeSelection *wordSelection;
  guint word_id;

  wordStore = gtk_tree_view_get_model(GTK_TREE_VIEW(wordView));
  wordSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(wordView));

  if (gtk_tree_selection_get_selected(wordSelection, &wordStore, &iter));
  {
    gtk_tree_model_get (wordStore, &iter, 0, &word_id, -1);
    gtk_selection_data_set(seldata,
                         GDK_SELECTION_TYPE_INTEGER,
                         sizeof (guint32)*8,
                         (const guchar *) &word_id,
                         sizeof (guint32));
  }
}

gboolean
open_options (GtkWidget *view, GdkEvent *e, gpointer data)
{
  if (e->type != GDK_BUTTON_PRESS)
    return FALSE;

  if (((GdkEventButton*)e)->button != 3)
    return FALSE;

  create_options();

  return FALSE;
} 

void
init_root_list (etymosf *etyform)
{
  reQueryRootList(etyform, "");
}

void
init_itree_list (etymosf *etyform)
{
  GtkTreeModel * itreeListStore;
  static GtkTreeIter iter;

  itreeListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(etyform->itreeListView));

  gtk_list_store_clear (GTK_LIST_STORE(itreeListStore));
  gtk_list_store_append (GTK_LIST_STORE(itreeListStore), &iter);
}
void
init_alpha_list (etymosf *etyform)
{
  GtkTreeModel *alphaListStore;
  static GtkTreeIter iter;

  alphaListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(etyform->alphaListView));

  gtk_list_store_clear (GTK_LIST_STORE(alphaListStore));
  gtk_list_store_append (GTK_LIST_STORE(alphaListStore), &iter);
}
void
init_word_list (etymosf *etyform)
{
  reQueryWordList(etyform, "");
}
