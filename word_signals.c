#include <gtk/gtk.h>
#include <mysql.h>
#include <string.h>
#include "word_window.h"
#include "error.h"
#include "mysql_change.h"
#include "etymo_window.h"
#include "index/mysql_index.h"
#include "index/generic_index_window.h"
#include "word_selects.h"

extern MYSQL *conn;

gboolean
sigSynRemove(GtkWidget *button, gpointer wf)
{
  GtkTreeModel *synStore;
  GtkTreeIter iter;
  GtkTreeSelection *synSelection;
  guint synId;
  gint affected_rows;
  wordForm *word_form;

  word_form = (wordForm *) wf;

  synStore = gtk_tree_view_get_model(GTK_TREE_VIEW(((wordForm *) word_form)->synTreeView));

  synSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(((wordForm *) word_form)->synTreeView));

  if (gtk_tree_selection_get_selected(synSelection, &synStore, &iter))
  {
     gtk_tree_model_get (synStore, &iter, 0, &synId, -1);

     if ((affected_rows = removeSynonym(conn, ((wordForm *) word_form)->wordId, synId, ((wordForm *) word_form)->lang)) > 0)
       selectSynonyms(((wordForm *) word_form)->wordId, word_form);
     else if (affected_rows == 0)
       printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Removed 0 rows");
     else if (affected_rows < 0)
       printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), getError());
  }
  return FALSE;
}

gboolean
sigAntRemove(GtkWidget *button, gpointer word_form)
{
  GtkTreeModel *antStore;
  GtkTreeIter iter;
  GtkTreeSelection *antSelection;
  guint antId;
  gint affected_rows;

  antStore = gtk_tree_view_get_model(GTK_TREE_VIEW(((wordForm *) word_form)->antTreeView));

  antSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(((wordForm *) word_form)->antTreeView));

  if (gtk_tree_selection_get_selected(antSelection, &antStore, &iter))
  {
     gtk_tree_model_get (antStore, &iter, 0, &antId, -1);

     if ((affected_rows = removeAntonym(conn, ((wordForm *) word_form)->wordId, antId, ((wordForm *) word_form)->lang)) > 0)
       selectAntonyms(((wordForm *) word_form)->wordId, word_form);
     else if (affected_rows == 0)
       printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Removed 0 rows");
     else if (affected_rows < 0)
       printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), getError());
  }
  return FALSE;
}

void
indexGetData(GtkWidget *sourceView, GdkDragContext *context, GtkSelectionData *seldata, 
             guint info, guint time, gpointer data)
{
  GtkTreeModel *indexStore;
  GtkTreeIter iter;
  GtkTreeSelection *indexSelection;

  indexStore = gtk_tree_view_get_model(GTK_TREE_VIEW(sourceView));

  indexSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sourceView));

  if (gtk_tree_selection_get_selected(indexSelection, &indexStore, &iter))
  {
/*
    gchar *index_term;
    gtk_tree_model_get (indexStore, &iter, 1, &index_term, -1);

    gtk_selection_data_set(seldata,
                         GDK_SELECTION_TYPE_STRING,
                         8,
                         index_term, strlen(index_term));
*/

/* this works because data_set copies the data so we're not referring to a local variable */
    guint index_id;

    gtk_tree_model_get (indexStore, &iter, 0, &index_id, -1);
    gtk_selection_data_set(seldata,
                         GDK_SELECTION_TYPE_INTEGER,
                         sizeof (guint32)*8,
                         (guchar *) &index_id,
                         sizeof (guint32));
/*
  An example from:
  http://www.mit.edu/afs/sipb/project/gtk/gtk_v1.0/src/gtk+-1.0.0/gtk/gtkselection.c
  gtk_selection_data_set (data,
				      GDK_SELECTION_TYPE_INTEGER,
				      sizeof (guint32)*8,
				      (guchar *)&selection_info->time,
				      sizeof (guint32));
*/

  }
}

void
antDragDataReceived(GtkWidget *wgt, GdkDragContext *context, int x, int y,
                        GtkSelectionData *seldata, guint info, guint time,
                        gpointer word_form)
{
  gint          affected_rows;
  guint         ant_id;
  guint         *pant_id;

/* is there a prettier way to get an guint out of a pointer to guchar? */
  pant_id = &ant_id;
  memcpy(pant_id, seldata->data, sizeof(guint32));

  if ((affected_rows = addAntonym(conn, ((wordForm *) word_form)->wordId, *pant_id, ((wordForm *) word_form)->lang)) < 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), getError());
  }
  else if (affected_rows == 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Added 0 rows");
  }
  else if (affected_rows > 0)
  {
    selectAntonyms(((wordForm *) word_form)->wordId, (wordForm *) word_form);
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Added Antonym");
  }
}

void
synDragDataReceived(GtkWidget *wgt, GdkDragContext *context, int x, int y,
                        GtkSelectionData *seldata, guint info, guint time,
                        gpointer word_form)
{
  gint          affected_rows;
  guint         syn_id;
  guint        *psyn_id;

/* is there a prettier way to get an guint out of a pointer to guchar? */
  psyn_id = &syn_id;
  memcpy(psyn_id, seldata->data, sizeof(guint32));

  if ((affected_rows = addSynonym(conn, ((wordForm *) word_form)->wordId, *psyn_id, ((wordForm *) word_form)->lang)) < 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), getError());
  }
  else if (affected_rows == 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Added 0 rows");
  }
  else if (affected_rows > 0)
  {
    selectSynonyms(((wordForm *) word_form)->wordId, (wordForm *) word_form);
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Added Synonym");
  }
}

void
indexDragDataReceived(GtkWidget *wgt, GdkDragContext *context, int x, int y,
                        GtkSelectionData *seldata, guint info, guint time,
                        gpointer word_form)
{
  gint          affected_rows;
  guint         index_id;
  guint        *pindex_id;

/* is there a prettier way to get an guint out of a pointer to guchar? */
  pindex_id = &index_id;
  memcpy(pindex_id, seldata->data, sizeof(guint32));

  if ((affected_rows = addIndexTerm(conn, ((wordForm *) word_form)->wordId, *pindex_id, ((wordForm *) word_form)->lang)) < 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), getError());
  }
  else if (affected_rows == 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Added 0 rows");
  }
  else if (affected_rows > 0)
  {
    selectIndex(((wordForm *) word_form)->wordId, (wordForm *) word_form);
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Added Index Term");
  }
}

gboolean
sigExpandIndex(GtkWidget *button, gpointer indexView)
{
  /* doesn't show up till scroll over tree? */
  gtk_tree_view_expand_all (GTK_TREE_VIEW(indexView));

  return FALSE;
}

gboolean
sigRemoveIndex(GtkWidget *button, gpointer word_form)
{
  GtkTreeModel *indexStore;
  GtkTreeIter   iter;
  GtkTreeSelection *indexSelection;
  guint index_id;
  gint affected_rows;

  indexStore = gtk_tree_view_get_model(GTK_TREE_VIEW(((wordForm *) word_form)->indexTreeView));
  indexSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(((wordForm *) word_form)->indexTreeView));

  if (gtk_tree_selection_get_selected(indexSelection, &indexStore, &iter))
  {
    gtk_tree_model_get (indexStore, &iter, 0, &index_id, -1);

    if ((affected_rows = removeIndexTerm(conn, ((wordForm *) word_form)->wordId, index_id, ((wordForm *) word_form)->lang)) < 0)
    {
      printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), getError());
    }
    else if (affected_rows == 0)
    {
      printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Deleted 0 rows");
    }
    else if (affected_rows > 0)
    {
      selectIndex(((wordForm *) word_form)->wordId, (wordForm *) word_form);
      printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Removed Index Term");
    }
  }
  else
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Select an Index Term");
  }
  return FALSE;
}

/* not tested yet */
gboolean
sigCreateIndexWindow(GtkWidget *indexView, gpointer data)
{
  GtkTreeSelection *selection;
  GtkTreeModel     *model;
  GtkTreeIter       iter;
  guint             index_id;
  indexForm        *res;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(indexView));
  model =     gtk_tree_view_get_model (GTK_TREE_VIEW(indexView));

  if (gtk_tree_selection_get_selected(selection, &model, &iter))
  {
    gtk_tree_model_get (model, &iter, 0 &index_id, -1);

    if ((res = create_index_window(index_id)) == NULL)
      g_print("couldn't open index window\n");
  }
  return FALSE;
}

gboolean
sigIndex(GtkWidget *button, gpointer word_form)
{
  GtkWidget *indexWindow;
  GtkTreeStore *indexTreeStore;
  GtkWidget *indexScrolledWindow;
  GtkWidget *indexView;
  GtkCellRenderer *renderer;
  GtkWidget *buttonExpand, *buttonAdd, *buttonHbox, *vbox;

  indexWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW(indexWindow), "Index");
  gtk_window_set_position(GTK_WINDOW(indexWindow), GTK_WIN_POS_CENTER_ALWAYS);

  g_signal_connect (GTK_WIDGET(indexWindow), "delete_event", 
                    G_CALLBACK(gtk_widget_destroy), NULL);

  indexView = gtk_tree_view_new ();
  gtk_widget_set_size_request (GTK_WIDGET(indexView), 300, 500);

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (indexView),
                                               -1,
                                               "Index Term",  
                                               renderer,
                                               "text", 1,
                                               NULL);

  indexTreeStore = gtk_tree_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (indexView), GTK_TREE_MODEL(indexTreeStore));
/* gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (indexView), TRUE); */

/*******drag********/

  static GtkTargetEntry targetentries[] =
  {
    { "index id", GTK_TARGET_SAME_APP, TARGET_INDEX_ID }
  };

  gtk_drag_source_set (GTK_WIDGET(indexView), GDK_BUTTON1_MASK, targetentries, 1, GDK_ACTION_COPY);

  g_signal_connect (GTK_WIDGET(indexView), "drag_data_get", G_CALLBACK(indexGetData), NULL);

/*******drag********/

  indexScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (indexScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(indexScrolledWindow), GTK_WIDGET(indexView));

  buttonAdd = gtk_button_new_with_mnemonic("_Add New Term");
  buttonExpand = gtk_button_new_with_mnemonic("_Expand All");

  buttonHbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(buttonHbox), buttonAdd, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(buttonHbox), buttonExpand, TRUE, TRUE, 0);

  vbox = gtk_vbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), indexScrolledWindow, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), buttonHbox, FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(indexWindow), GTK_WIDGET(vbox));

  requeryIndexTree(GTK_TREE_VIEW(indexView));

  g_signal_connect (GTK_OBJECT(buttonExpand), "clicked", G_CALLBACK(sigExpandIndex), (gpointer) indexView);

  g_signal_connect (GTK_OBJECT(indexView), "row-activated", G_CALLBACK(sigCreateIndexWindow), (gpointer) NULL);

  gtk_widget_show_all(indexWindow);

  return FALSE;
}

gboolean
sigUpdateWord(GtkWidget *button, gpointer wf)
{
  const gchar *def;
  const gchar *note;
  gboolean     compound;
  gint         affected_rows;
  GtkTreeModel *partModel;
  GtkTreeModel *statusModel;
  GtkTreeModel *genderModel;
  GtkTreeIter   iter;
  guint         partId;
  gint          statusId; /* signed for now */
  guint         genderId;

  wordForm *word_form;
  word_form = (wordForm *) wf;

  def  = gtk_entry_get_text(GTK_ENTRY( ((wordForm *) word_form)->def));
  note = gtk_entry_get_text(GTK_ENTRY( ((wordForm *) word_form)->note));
  compound = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(((wordForm *)word_form)->compYes));

  /* should test return val of next func */
  if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(((wordForm *)word_form)->part), &iter))
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "part not active");
    return FALSE;
  }
  partModel = gtk_combo_box_get_model(GTK_COMBO_BOX(((wordForm *)word_form)->part));
  gtk_tree_model_get(partModel, &iter, 0, &partId, -1);

  /* should test return val of next func */
  if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(((wordForm *)word_form)->status), &iter))
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "status not active");
    return FALSE;
  }
  statusModel = gtk_combo_box_get_model(GTK_COMBO_BOX(((wordForm *)word_form)->status));
  gtk_tree_model_get(statusModel, &iter, 0, &statusId, -1);

  /* should test return val of next func */
  if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(((wordForm *)word_form)->gender), &iter))
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "gender not active");
    return FALSE;
  }
  genderModel = gtk_combo_box_get_model(GTK_COMBO_BOX(((wordForm *)word_form)->gender));
  gtk_tree_model_get(genderModel, &iter, 0, &genderId, -1);


  if ((affected_rows = updateWord(conn, (guint) ((wordForm *) word_form)->wordId, def, partId, note, compound, statusId, genderId, ((wordForm *) word_form)->lang)) > 0)
  {
    selectWord((guint) ((wordForm *) word_form)->wordId, word_form);
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Updated!");
  }
  else if (affected_rows == 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), "Updated 0 rows");
  }
  else if (affected_rows < 0)
  {
    printStatusMesg(GTK_STATUSBAR(((wordForm *) word_form)->statusBar), getError());
  }

  return FALSE;
}

gboolean
sigResetWord(GtkWidget *resetButton, gpointer word_form)
{
  selectWord((guint) ((wordForm *)word_form)->wordId, (wordForm *) word_form);

  return FALSE;
}
