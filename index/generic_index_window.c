#include <gtk/gtk.h>
#include <mysql.h>
#include "mysql_index.h"
#include <stdio.h>
#include "generic_index_window.h"

#define def_host_name   NULL   /* host to connect to (default = localhost) */
#define def_user_name  "grweb" /* user name (default = your login name) */
#define def_password   "1234"  /* password (default = none) */
#define def_port_num    0      /* use default port */
#define def_socket_name NULL   /* use default socket name */
#define def_db_name    "indexing"    /* database to use (default = none) */

int
create_index_add()
{

}

gboolean
sigUpdateTerm(GtkWidget *button, indexForm *index)
{
  const char *term;
  const char *scope_note;
  const char *see_also;
  const int  *parent_id;
  int         preferred;
  GtkTextBuffer *scopeNoteBuffer;
  GtkTextIter startIter, endIter;

  scopeNoteBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(index->scopeTextView));

  gtk_text_buffer_get_start_iter(scopeNoteBuffer, &startIter);
  gtk_text_buffer_get_end_iter(scopeNoteBuffer, &endIter);

  term = gtk_entry_get_text(GTK_ENTRY(index->termText));
  scope_note = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(scopeNoteBuffer),
                                        &startIter, &endIter, FALSE);
  see_also = gtk_entry_get_text(GTK_ENTRY(index->seeAlsoText));

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(index->preferred)))
    preferred = 1;
  else
    preferred = 0;

  if (updateTerm(index->index_id, term, scope_note, 0, see_also, preferred) < 1)
  {
    g_printf("%s\n", getError());
    g_printf("update failed\n");
  }
  else
  {
    g_printf("updated!\n");
    selectTerm(index->index_id, index);
  }
  return FALSE;
}

gboolean
sigRemoveUseGroup(GtkWidget *button, indexForm *index)
{
  int index_id, group;
  GtkTreeModel *useModel;
  GtkTreeIter   iter;
  GtkTreeSelection *selection;

  useModel = gtk_tree_view_get_model(GTK_TREE_VIEW(index->useTreeView));
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(index->useTreeView));

  if (!gtk_tree_selection_get_selected(selection, &useModel, &iter))
    return FALSE;

  gtk_tree_model_get (useModel, &iter, 0, &index_id, 2, &group, -1);

  if (!removeUse(index_id, group))
    g_printf("remove failed\n");
  else
  {
    selectUse(index_id, index);
    g_printf("removed!\n");
  }
  return FALSE;
}

gboolean
sigDeleteTerm(GtkWidget *button, indexForm *index)
{
  if (deleteTerm(index->index_id, NULL, 1))
    g_printf("deleted!\n");
  else
    g_printf("delete failed\n");

  return FALSE;
}

gboolean
sigRemoveRelated(GtkWidget *button, indexForm *index)
{
  int related_id;
  GtkTreeModel *relModel;
  GtkTreeIter   iter;
  GtkTreeSelection *selection;

  relModel = gtk_tree_view_get_model(GTK_TREE_VIEW(index->relatedTreeView));
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(index->relatedTreeView));

  if (!gtk_tree_selection_get_selected(selection, &relModel, &iter))
    return FALSE;

  gtk_tree_model_get (relModel, &iter, 0, &related_id, -1);

  if (!removeRelated(index->index_id, related_id))
    g_printf("remove failed\n");
  else
  {
    selectRelated(index->index_id, index);
    g_printf("removed!\n");
  }
  return FALSE;
}

GtkTreeView *
create_related_list(char *colName)
{
  GtkListStore    *model;
  GtkCellRenderer *renderer;
  GtkTreeView     *treeView;

  model = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  treeView = (GtkTreeView *) gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,
                                               colName,  
                                               renderer,
                                               "text", 1,
                                               NULL);
  return treeView;
}

int
sigInsertTerm(GtkWidget *button, indexForm *index)
{
  const char *term;
  const char *scope_note;
  const char *see_also;
  const int  *parent_id;
  GtkTextBuffer *scopeNoteBuffer;
  GtkTextIter startIter, endIter;

  scopeNoteBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(index->scopeTextView));

  gtk_text_buffer_get_start_iter(scopeNoteBuffer, &startIter);
  gtk_text_buffer_get_end_iter(scopeNoteBuffer, &endIter);

  term = gtk_entry_get_text(GTK_ENTRY(index->termText));
  scope_note = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(scopeNoteBuffer), 
                                        &startIter, &endIter, FALSE);
  see_also = gtk_entry_get_text(GTK_ENTRY(index->seeAlsoText));

  if (insertTerm(term, scope_note, 0, see_also) < 1)
  {
    g_printf("insert failed\n");
  }
  else
    g_printf("inserted!\n");
  return FALSE;
}

GtkTreeView *
create_use_list(char *colName)
{
  GtkListStore    *model;
  GtkCellRenderer *renderer;
  GtkTreeView     *treeView;

  model = gtk_list_store_new (3, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_UINT);
  treeView = (GtkTreeView *) gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,
                                               colName,  
                                               renderer,
                                               "text", 1,
                                               NULL);
/*
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,
                                               "id",  
                                               renderer,
                                               "text", 0,
                                               NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeView),
                                               -1,
                                               "group",  
                                               renderer,
                                               "text", 2,
                                               NULL);
*/
  return treeView;
}

indexForm *
create_index_window(int index_id)
{
  GtkWidget *indexWindow;
  GtkWidget *indexTable;
  GtkWidget *termLabel;
  GtkWidget *buttonHBox, *updateButton, *deleteButton, *insertButton;
  GtkWidget *relbuttonHBox, *relAddButton, *relDeleteButton;
  GtkWidget *usebuttonHBox, *useAddButton, *useDeleteButton;
  GtkWidget *updatedLabel, *createdLabel;
  GtkWidget *preferredLabel;
  GtkWidget *vbox;
  GtkListStore *useModel;
  GtkListStore *relatedModel;
  indexForm *index;
  GtkTextBuffer *scopeBuffer;

  index = (indexForm *) g_malloc(sizeof(indexForm) * 1);

  index->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (index->window, "delete_event", gtk_main_quit, NULL);
  indexTable = gtk_table_new(10, 2, FALSE);

  termLabel = gtk_label_new("Term:");
  gtk_misc_set_alignment(GTK_MISC(termLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(termLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), termLabel, 0, 1, 0, 1, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  index->termText = gtk_entry_new();
  gtk_table_attach(GTK_TABLE(indexTable), index->termText, 1, 2, 0, 1, 
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  index->scopeLabel = gtk_label_new("Scope Note:");
  gtk_misc_set_alignment(GTK_MISC(index->scopeLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(index->scopeLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), index->scopeLabel, 0, 1, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  scopeBuffer = gtk_text_buffer_new(NULL);
  index->scopeTextView = gtk_text_view_new_with_buffer (scopeBuffer);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(index->scopeTextView), GTK_WRAP_WORD_CHAR);
  gtk_table_attach(GTK_TABLE(indexTable), index->scopeTextView, 0, 2, 2, 3, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  index->parentLabel = gtk_label_new("Broader Term:");
  gtk_misc_set_alignment(GTK_MISC(index->parentLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(index->parentLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), index->parentLabel, 0, 1, 3, 4, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  index->parentText = gtk_entry_new();
  gtk_table_attach(GTK_TABLE(indexTable), index->parentText, 1, 2, 3, 4,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  index->seeAlsoLabel = gtk_label_new("See also:");
  gtk_misc_set_alignment(GTK_MISC(index->seeAlsoLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(index->seeAlsoLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), index->seeAlsoLabel, 0, 1, 4, 5,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  index->seeAlsoText = gtk_entry_new();
  gtk_table_attach(GTK_TABLE(indexTable), index->seeAlsoText, 1, 2, 4, 5, 
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  preferredLabel = gtk_label_new("Preferred:");
  gtk_misc_set_alignment(GTK_MISC(preferredLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(preferredLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), preferredLabel, 0, 1, 5, 6,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  index->preferred = gtk_check_button_new();
  gtk_table_attach(GTK_TABLE(indexTable), index->preferred, 1, 2, 5, 6,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);


  buttonHBox = gtk_hbox_new(TRUE, 3);

  if (index_id < 1)
  {
    insertButton = gtk_button_new_with_mnemonic("_Insert");
    gtk_box_pack_start(GTK_BOX(buttonHBox), GTK_WIDGET(insertButton), FALSE, TRUE, 0);
  }
  else
  {
    updateButton = gtk_button_new_with_mnemonic("_Update");
    deleteButton = gtk_button_new_with_mnemonic("_Delete");
    gtk_box_pack_start(GTK_BOX(buttonHBox), GTK_WIDGET(updateButton), FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(buttonHBox), GTK_WIDGET(deleteButton), FALSE, TRUE, 0);
  }
  gtk_table_attach(GTK_TABLE(indexTable), buttonHBox, 0, 2, 6, 7, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
/*
  useLabel = gtk_label_new("Use:");
  gtk_misc_set_alignment(GTK_MISC(useLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(useLabel), 5, 5);
  gtk_table_attach_defaults(GTK_TABLE(indexTable), useLabel, 0, 1, 5, 6);
*/

  index->useTreeView = (GtkWidget *) create_use_list("Use... or");

  index->useScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (index->useScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(index->useScrolledWindow), 
                    GTK_WIDGET(index->useTreeView));


  gtk_table_attach(GTK_TABLE(indexTable), index->useScrolledWindow, 0, 2, 7, 8, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  usebuttonHBox = gtk_hbox_new(TRUE, 3);
  useAddButton = gtk_button_new_with_mnemonic("_Add");
  useDeleteButton = gtk_button_new_with_mnemonic("_Remove");
  gtk_box_pack_start(GTK_BOX(usebuttonHBox), GTK_WIDGET(useAddButton), FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(usebuttonHBox), GTK_WIDGET(useDeleteButton), FALSE, TRUE, 0);

  gtk_table_attach(GTK_TABLE(indexTable), usebuttonHBox, 0, 2, 8, 9, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);


/*
  relatedLabel = gtk_label_new("Related terms:");
  gtk_misc_set_alignment(GTK_MISC(relatedLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(relatedLabel), 5, 5);
  gtk_table_attach_defaults(GTK_TABLE(indexTable), relatedLabel, 0, 1, 7, 8);
*/
  index->relatedTreeView = (GtkWidget *) create_related_list("Related Terms");

  index->relatedScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (index->relatedScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(index->relatedScrolledWindow), 
                    GTK_WIDGET(index->relatedTreeView));

  relatedModel = (GtkListStore *) gtk_tree_view_get_model(GTK_TREE_VIEW(index->relatedTreeView));

  gtk_table_attach(GTK_TABLE(indexTable), index->relatedScrolledWindow, 0, 2, 9, 10, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  relbuttonHBox = gtk_hbox_new(TRUE, 3);
  relAddButton = gtk_button_new_with_mnemonic("_Add");
  relDeleteButton = gtk_button_new_with_mnemonic("_Remove");
  gtk_box_pack_start(GTK_BOX(relbuttonHBox), GTK_WIDGET(relAddButton), FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(relbuttonHBox), GTK_WIDGET(relDeleteButton), FALSE, TRUE, 0);

  gtk_table_attach(GTK_TABLE(indexTable), relbuttonHBox, 0, 2, 10, 11, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  updatedLabel = gtk_label_new("Updated:");
  gtk_misc_set_alignment(GTK_MISC(updatedLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(updatedLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), updatedLabel, 0, 1, 11, 12, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  index->updated = gtk_label_new(NULL);
  gtk_misc_set_alignment(GTK_MISC(index->updated), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(index->updated), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), index->updated, 1, 2, 11, 12, 
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  createdLabel = gtk_label_new("Created:");
  gtk_misc_set_alignment(GTK_MISC(createdLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(createdLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), createdLabel, 0, 1, 12, 13,
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  index->created = gtk_label_new(NULL);
  gtk_misc_set_alignment(GTK_MISC(index->created), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(index->created), 5, 5);
  gtk_table_attach(GTK_TABLE(indexTable), index->created, 1, 2, 12, 13,
                   GTK_EXPAND | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  vbox = gtk_vbox_new(FALSE, 5);
  index->statusBar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(index->statusBar), FALSE);

  gtk_box_pack_start(GTK_BOX(vbox), indexTable, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), index->statusBar, FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(index->window), vbox);
  gtk_widget_show_all(index->window);

  if (index_id > 0)
  {
    if (!selectTerm(index_id, index))
    {
      gtk_widget_destroy(index->window);
      g_free(index);
      return NULL;
    }

    g_signal_connect (G_OBJECT(updateButton), "clicked", G_CALLBACK(sigUpdateTerm), index);
    g_signal_connect (G_OBJECT(deleteButton), "clicked", G_CALLBACK(sigDeleteTerm), index);
    g_signal_connect (G_OBJECT(useDeleteButton), "clicked", G_CALLBACK(sigRemoveUseGroup), index);
    g_signal_connect (G_OBJECT(relDeleteButton), "clicked", G_CALLBACK(sigRemoveRelated), index);
  }
  else
  {
    g_signal_connect (G_OBJECT(insertButton), "clicked", G_CALLBACK(sigInsertTerm), index);

  }

  return index;
}
/*
int
main (int argc, char **argv)
{
  gtk_init(&argc, &argv);

  conn = do_connect (def_host_name, def_user_name, def_password, def_db_name,
                     def_port_num, def_socket_name, 0);
  if (conn == NULL)
    exit (1);

  if (create_index_window(1) != NULL)
  {
    gtk_main();
  }

  do_disconnect(conn);

  exit(0);
}
*/
