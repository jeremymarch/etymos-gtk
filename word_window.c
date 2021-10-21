#include <gtk/gtk.h>
#include <mysql.h>
#include <string.h>
#include "word_window.h"
#include "error.h"
#include "mysql_change.h"
#include "etymo_window.h"
#include "word_selects.h"

void
greekWordPages (wordForm *word_form, GtkWidget *wordNoteBook)
{
  GtkWidget *verbPageLabel, *wordVerbTable, *presentVerbLabel;
  GtkWidget *futureVerbLabel, *aoristVerbLabel, *perfectVerbLabel, *perfmidVerbLabel, *aoristpassVerbLabel;
  GtkWidget *greekButtonHbox, *buttonReset, *buttonUpdate;

  verbPageLabel = gtk_label_new("Verb");
  wordVerbTable = gtk_table_new(7, 2, FALSE);
  gtk_notebook_append_page( GTK_NOTEBOOK(wordNoteBook), wordVerbTable, verbPageLabel );

  presentVerbLabel = gtk_label_new("Present:");
  gtk_misc_set_alignment(GTK_MISC(presentVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(presentVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), presentVerbLabel, 0, 1, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->present = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->present), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->present, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  futureVerbLabel = gtk_label_new("Future:");
  gtk_misc_set_alignment(GTK_MISC(futureVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(futureVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), futureVerbLabel, 0, 1, 1, 2, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->future = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->future), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->future, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  aoristVerbLabel = gtk_label_new("Aorist:");
  gtk_misc_set_alignment(GTK_MISC(aoristVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(aoristVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), aoristVerbLabel, 0, 1, 2, 3, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->aorist = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->aorist), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->aorist, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  perfectVerbLabel = gtk_label_new("Perfect:");
  gtk_misc_set_alignment(GTK_MISC(perfectVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(perfectVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), perfectVerbLabel, 0, 1, 3, 4, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->perfect = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->perfect), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->perfect, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  perfmidVerbLabel = gtk_label_new("Perfect Middle:");
  gtk_misc_set_alignment(GTK_MISC(perfmidVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(perfmidVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), perfmidVerbLabel, 0, 1, 4, 5, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->perfmid = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->perfmid), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->perfmid, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  aoristpassVerbLabel = gtk_label_new("Aorist Passive:");
  gtk_misc_set_alignment(GTK_MISC(aoristpassVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(aoristpassVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), aoristpassVerbLabel, 0, 1, 5, 6, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->aoristpass = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->aoristpass), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->aoristpass, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  buttonReset = gtk_button_new_with_mnemonic("_Reset");
  buttonUpdate = gtk_button_new_with_mnemonic("_Update");

  greekButtonHbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(greekButtonHbox), buttonReset, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(greekButtonHbox), buttonUpdate, TRUE, TRUE, 0);
  gtk_table_attach(GTK_TABLE(wordVerbTable), greekButtonHbox, 0, 2, 6, 7, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
}

void
latinWordPages (wordForm *word_form, GtkWidget *wordNoteBook)
{
  GtkWidget *verbPageLabel, *wordVerbTable;
  GtkWidget  *presentVerbLabel, *infinitiveVerbLabel, *perfectVerbLabel, *perfectPassPartLabel;
  GtkWidget *latinButtonHbox, *buttonReset, *buttonUpdate;

  verbPageLabel = gtk_label_new("Verb");
  wordVerbTable = gtk_table_new(4, 2, FALSE);
  gtk_notebook_append_page( GTK_NOTEBOOK(wordNoteBook), wordVerbTable, verbPageLabel );

  presentVerbLabel = gtk_label_new("Present:");
  gtk_misc_set_alignment(GTK_MISC(presentVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(presentVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), presentVerbLabel, 0, 1, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->present = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->present), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->present, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  infinitiveVerbLabel = gtk_label_new("Infinitive:");
  gtk_misc_set_alignment(GTK_MISC(infinitiveVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(infinitiveVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), infinitiveVerbLabel, 0, 1, 1, 2, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->infinitive = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->infinitive), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->infinitive, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  perfectVerbLabel = gtk_label_new("Perfect:");
  gtk_misc_set_alignment(GTK_MISC(perfectVerbLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(perfectVerbLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), perfectVerbLabel, 0, 1, 2, 3, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->perfect = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->perfect), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->perfect, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  perfectPassPartLabel = gtk_label_new("Perfect Passive Participle:");
  gtk_misc_set_alignment(GTK_MISC(perfectPassPartLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(perfectPassPartLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordVerbTable), perfectPassPartLabel, 0, 1, 3, 4, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->perfectPassPart = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->perfectPassPart), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordVerbTable), word_form->perfectPassPart, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  buttonReset = gtk_button_new_with_mnemonic("_Reset");
  buttonUpdate = gtk_button_new_with_mnemonic("_Update");

  latinButtonHbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(latinButtonHbox), buttonReset, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(latinButtonHbox), buttonUpdate, TRUE, TRUE, 0);
  gtk_table_attach(GTK_TABLE(wordVerbTable), latinButtonHbox, 0, 2, 4, 5, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
}

void
makeIndexPage (wordForm *word_form, GtkWidget *wordNoteBook)
{
  GtkWidget *indexPageLabel, *indexPageTable;
  GtkWidget *indexButtonHbox, *indexButtonAdd, *indexButtonRemove;
  GtkWidget *indexScrolledWindow;
  GtkListStore *indexListStore;
  GtkCellRenderer *renderer;

  indexPageLabel = gtk_label_new("Index");
  indexPageTable = gtk_table_new(3, 2, FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(wordNoteBook), indexPageTable, indexPageLabel);

  word_form->indexTreeView = (GtkTreeView *) gtk_tree_view_new ();

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (word_form->indexTreeView),
                                               -1,
                                               "Index Term",  
                                               renderer,
                                               "text", 1,
                                               NULL);

  indexListStore = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);

  static GtkTargetEntry targetentries[] =
  {
    { "index id", GTK_TARGET_SAME_APP, TARGET_INDEX_ID }
  };

  gtk_drag_dest_set (GTK_WIDGET(word_form->indexTreeView), GTK_DEST_DEFAULT_ALL, targetentries, 1, GDK_ACTION_COPY);

  gtk_tree_view_set_model (GTK_TREE_VIEW (word_form->indexTreeView), GTK_TREE_MODEL(indexListStore));

  indexScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (indexScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(indexScrolledWindow), GTK_WIDGET(word_form->indexTreeView));

  gtk_table_attach(GTK_TABLE(indexPageTable), indexScrolledWindow, 0, 2, 0, 1, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  indexButtonRemove = gtk_button_new_with_mnemonic("_Remove");
  indexButtonAdd = gtk_button_new_with_mnemonic("_Add");

  indexButtonHbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(indexButtonHbox), indexButtonRemove, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(indexButtonHbox), indexButtonAdd, TRUE, TRUE, 0);
  gtk_table_attach(GTK_TABLE(indexPageTable), indexButtonHbox, 0, 2, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  g_signal_connect(G_OBJECT(indexButtonAdd), "clicked",
                                G_CALLBACK(sigIndex),
                                NULL);

  g_signal_connect(G_OBJECT(indexButtonRemove), "clicked",
                                G_CALLBACK(sigRemoveIndex),
                                word_form);

  g_signal_connect(G_OBJECT(word_form->indexTreeView), "drag_data_received",
                   G_CALLBACK(indexDragDataReceived), word_form);
}

void
makeAuthorPage (wordForm *word_form, GtkWidget *wordNoteBook)
{
  GtkWidget *authorPageLabel, *authorPageTable;
  GtkWidget *authorButtonHbox, *authorButtonAdd, *authorButtonRemove;
  GtkWidget *authorScrolledWindow;
  GtkCellRenderer *renderer;
  GtkListStore *authorListStore;

  authorPageLabel = gtk_label_new("Authors");
  authorPageTable = gtk_table_new(3, 2, FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(wordNoteBook), authorPageTable, authorPageLabel);

  word_form->authorTreeView = (GtkTreeView *) gtk_tree_view_new ();

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(word_form->authorTreeView),
                                               -1,
                                               "Author",  
                                               renderer,
                                               "text", 1,
                                               NULL);

  authorListStore = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW(word_form->authorTreeView), GTK_TREE_MODEL(authorListStore));

  authorScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(authorScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(authorScrolledWindow), GTK_WIDGET(word_form->authorTreeView));

  gtk_table_attach(GTK_TABLE(authorPageTable), authorScrolledWindow, 0, 2, 0, 1, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  authorButtonRemove = gtk_button_new_with_mnemonic("_Remove");
  authorButtonAdd = gtk_button_new_with_mnemonic("_Add");

  authorButtonHbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(authorButtonHbox), authorButtonRemove, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(authorButtonHbox), authorButtonAdd, TRUE, TRUE, 0);
  gtk_table_attach(GTK_TABLE(authorPageTable), authorButtonHbox, 0, 2, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
}

void
makeSynAntPage (wordForm *word_form, GtkWidget *wordNoteBook)
{
  GtkWidget *synAntPageLabel, *synAntPageTable;
  GtkWidget *synButtonHbox, *synButtonRemove;
  GtkWidget *synScrolledWindow;
  GtkWidget *antButtonHbox, *antButtonRemove;
  GtkWidget *antScrolledWindow;
  GtkCellRenderer *renderer;
  GtkListStore *synListStore, *antListStore;

  synAntPageLabel = gtk_label_new("Syn/Antonyms");
  synAntPageTable = gtk_table_new(3, 2, FALSE);
  gtk_notebook_append_page (GTK_NOTEBOOK(wordNoteBook), synAntPageTable, synAntPageLabel);

  word_form->synTreeView = (GtkTreeView *) gtk_tree_view_new ();

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(word_form->synTreeView),
                                               -1,
                                               "Synonyms",  
                                               renderer,
                                               "text", 1,
                                               NULL);

  synListStore = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW(word_form->synTreeView), GTK_TREE_MODEL(synListStore));

  synScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(synScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(synScrolledWindow), GTK_WIDGET(word_form->synTreeView));

/********Antonyms************/
  word_form->antTreeView = (GtkTreeView *) gtk_tree_view_new ();

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (word_form->antTreeView),
                                               -1,
                                               "Antonyms",  
                                               renderer,
                                               "text", 1,
                                               NULL);

  antListStore = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW(word_form->antTreeView), 
                           GTK_TREE_MODEL(antListStore));

  antScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(antScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(antScrolledWindow), 
                    GTK_WIDGET(word_form->antTreeView));

  gtk_table_attach(GTK_TABLE(synAntPageTable), antScrolledWindow, 1, 2, 0, 1, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  antButtonRemove = gtk_button_new_with_mnemonic("Remove _Ant");

  static GtkTargetEntry targetentries[] =
  {
    /* need to make the id language specific 
     * so that can't put one lang in another 
     */
    { "word id", GTK_TARGET_SAME_APP, TARGET_WORD_ID }
  };

  gtk_drag_dest_set (GTK_WIDGET(word_form->antTreeView), 
                     GTK_DEST_DEFAULT_ALL, targetentries, 1, GDK_ACTION_COPY);
  gtk_drag_dest_set (GTK_WIDGET(word_form->synTreeView), 
                     GTK_DEST_DEFAULT_ALL, targetentries, 1, GDK_ACTION_COPY);

  g_signal_connect (G_OBJECT(word_form->antTreeView), "drag_data_received",
                     G_CALLBACK(antDragDataReceived), word_form);

  g_signal_connect(G_OBJECT(word_form->synTreeView), "drag_data_received",
                     G_CALLBACK(synDragDataReceived), word_form);

  antButtonHbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(antButtonHbox), antButtonRemove, TRUE, TRUE, 0);
  gtk_table_attach(GTK_TABLE(synAntPageTable), antButtonHbox, 1, 2, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

/**************/

  gtk_table_attach(GTK_TABLE(synAntPageTable), synScrolledWindow, 0, 1, 0, 1, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  synButtonRemove = gtk_button_new_with_mnemonic("Remove _Syn");

  synButtonHbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(synButtonHbox), synButtonRemove, TRUE, TRUE, 0);
  gtk_table_attach(GTK_TABLE(synAntPageTable), synButtonHbox, 0, 1, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  g_signal_connect(G_OBJECT(antButtonRemove), "clicked",
                     G_CALLBACK(sigAntRemove), word_form);

  g_signal_connect(G_OBJECT(synButtonRemove), "clicked",
                     G_CALLBACK(sigSynRemove), word_form);
}

GtkWidget *
create_part_combo ()
{
  GtkWidget *partCombo;
  GtkListStore    *model;
  GtkCellRenderer *renderer;

  model = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  partCombo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(partCombo), renderer, FALSE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(partCombo), renderer, "text", 1);

/*  makePartList(partCombo, -1); */

  return partCombo;
}

GtkWidget *
create_status_combo ()
{
  GtkWidget *statusCombo;
  GtkListStore    *model;
  GtkCellRenderer *renderer;

  model = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  statusCombo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(statusCombo), renderer, FALSE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(statusCombo), renderer, "text", 1);

/*  makePartList(statusCombo, -1); */

  return statusCombo;
}

GtkWidget *
create_gender_combo ()
{
  GtkWidget *genderCombo;
  GtkListStore    *model;
  GtkCellRenderer *renderer;

  model = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  genderCombo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(genderCombo), renderer, FALSE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(genderCombo), renderer, "text", 1);

/*  makePartList(genderCombo, -1); */

  return genderCombo;
}

gboolean
word_cleanup (GtkWidget *window, GdkEvent *event, gpointer word_form)
{
  g_free(word_form);
  return FALSE;
}

GtkWindow *
createWordWindow (guint new, guint word_id, gchar *lang)
{
  GtkWidget *wordLabel, *defLabel, *noteLabel, *updatedLabel, *createdLabel;
  GtkWidget *statusLabel;
  GtkWidget *compLabel;
  GtkWidget *persDefLabel;
  GtkWidget *partLabel, *genderLabel;
  GtkWidget *buttonHbox;
  GtkWidget *buttonInsert, *buttonReset, *buttonUpdate;
  GtkWidget *wordTable;
  GtkWidget *wordWindow;
  GtkWidget *vbox, *compHBox;
  GtkWidget *wordNoteBook, *generalPageLabel;
  GSList    *compGroup;
  wordForm  *word_form;

  word_form = (wordForm *) g_malloc(sizeof(wordForm) * 1);
  word_form->wordId = word_id;
  strncpy(word_form->lang, lang, 20);

  wordWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  word_form->window = wordWindow;

/*  gtk_widget_set_size_request(GTK_WIDGET(wordWindow), 400, 245); */
  gtk_window_set_title (GTK_WINDOW(wordWindow), "Insert Word");
  gtk_window_set_position(GTK_WINDOW(wordWindow), GTK_WIN_POS_CENTER_ALWAYS);

  g_signal_connect (GTK_WIDGET(wordWindow), "delete_event", 
                    G_CALLBACK(word_cleanup),
                    (gpointer) word_form);
/*  g_signal_connect (GTK_WIDGET(wordWindow), "delete_event", 
                    G_CALLBACK(gtk_widget_destroy),
                    (gpointer) word_form); */
/*  g_signal_connect (wordWindow, "destroy", G_CALLBACK(gtk_widget_destroy), NULL); */

  wordNoteBook = gtk_notebook_new();
  generalPageLabel = gtk_label_new("General");
  wordTable = gtk_table_new(12, 2, FALSE);
  gtk_notebook_append_page(GTK_NOTEBOOK(wordNoteBook), wordTable, generalPageLabel);

  wordLabel = gtk_label_new("Word:");
  gtk_misc_set_alignment(GTK_MISC(wordLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(wordLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), wordLabel, 0, 1, 0, 1, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->word = gtk_entry_new();
  gtk_editable_set_editable (GTK_EDITABLE(word_form->word), FALSE);
  gtk_entry_set_max_length (GTK_ENTRY(word_form->word), WORD_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordTable), word_form->word, 1, 2, 0, 1, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  persDefLabel = gtk_label_new("Perseus Def.:");
  gtk_misc_set_alignment(GTK_MISC(persDefLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(persDefLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), persDefLabel, 0, 1, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->persDef = gtk_entry_new();
  gtk_editable_set_editable (GTK_EDITABLE(word_form->persDef), FALSE);
  gtk_entry_set_max_length (GTK_ENTRY(word_form->persDef), WORD_DEF_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordTable), word_form->persDef, 1, 2, 1, 2, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  defLabel = gtk_label_new("Definition:");
  gtk_misc_set_alignment(GTK_MISC(defLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(defLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), defLabel, 0, 1, 2, 3, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  word_form->def = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->def), WORD_DEF_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordTable), word_form->def, 1, 2, 2, 3, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  partLabel = gtk_label_new("Part of Speech:");
  gtk_misc_set_alignment(GTK_MISC(partLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(partLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), partLabel, 0, 1, 3, 4, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  word_form->part = (GtkComboBox *) create_part_combo();
  gtk_table_attach(GTK_TABLE(wordTable), GTK_WIDGET(word_form->part), 
                   1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  genderLabel = gtk_label_new("Gender:");
  gtk_misc_set_alignment(GTK_MISC(genderLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(genderLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), genderLabel, 0, 1, 4, 5, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  word_form->gender = (GtkComboBox *) create_gender_combo();
  gtk_table_attach(GTK_TABLE(wordTable), GTK_WIDGET(word_form->gender), 
                   1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  compLabel = gtk_label_new("Compound:");
  gtk_misc_set_alignment(GTK_MISC(compLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(compLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), compLabel, 0, 1, 5, 6, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  compHBox = gtk_hbox_new(FALSE, 5);
  word_form->compNo = gtk_radio_button_new_with_label (NULL, "No");
  compGroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (word_form->compNo));
  word_form->compYes = gtk_radio_button_new_with_label (compGroup, "Yes");
  gtk_box_pack_start(GTK_BOX(compHBox), word_form->compNo, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(compHBox), word_form->compYes, TRUE, TRUE, 0);
  gtk_table_attach(GTK_TABLE(wordTable), compHBox, 1, 2, 5, 6, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  statusLabel = gtk_label_new("Status:");
  gtk_misc_set_alignment(GTK_MISC(statusLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(statusLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), statusLabel, 0, 1, 6, 7, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  word_form->status = (GtkComboBox *) create_status_combo();
  gtk_table_attach(GTK_TABLE(wordTable), GTK_WIDGET(word_form->status), 1, 2, 6, 7, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  noteLabel = gtk_label_new("Note:");
  gtk_misc_set_alignment(GTK_MISC(noteLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(noteLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), noteLabel, 0, 1, 7, 8, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  word_form->note = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(word_form->note), WORD_NOTE_MAX_LEN);
  gtk_table_attach(GTK_TABLE(wordTable), word_form->note, 1, 2, 7, 8, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  updatedLabel = gtk_label_new("Updated:");
  gtk_misc_set_alignment(GTK_MISC(updatedLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(updatedLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), updatedLabel, 0, 1, 8, 9, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  word_form->updated = gtk_label_new(NULL);
  gtk_misc_set_alignment(GTK_MISC(word_form->updated), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(word_form->updated), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), word_form->updated, 1, 2, 8, 9, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  createdLabel = gtk_label_new("Created:");
  gtk_misc_set_alignment(GTK_MISC(createdLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(createdLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), createdLabel, 0, 1, 9, 10, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  word_form->created = gtk_label_new(NULL);
  gtk_misc_set_alignment(GTK_MISC(word_form->created), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(word_form->created), 5, 5);
  gtk_table_attach(GTK_TABLE(wordTable), word_form->created, 1, 2, 9, 10, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  buttonHbox = gtk_hbox_new(FALSE, 5);
  if (new)
  {
    buttonInsert = gtk_button_new_with_mnemonic("_Insert");
    gtk_box_pack_start(GTK_BOX(buttonHbox), buttonInsert, TRUE, TRUE, 0);

    /* gtk_window_set_default (GTK_WINDOW(word_form->window), GTK_WIDGET(buttonInsert)); */
  }
  else
  {
    buttonReset  = gtk_button_new_with_mnemonic("_Reset");
    buttonUpdate = gtk_button_new_with_mnemonic("_Update");

    gtk_box_pack_start(GTK_BOX(buttonHbox), buttonReset, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(buttonHbox), buttonUpdate, TRUE, TRUE, 0);

    g_signal_connect (GTK_WIDGET(buttonReset), "clicked", 
                      G_CALLBACK(sigResetWord), 
                      (gpointer) word_form);
    g_signal_connect (GTK_WIDGET(buttonUpdate), "clicked", 
                      G_CALLBACK(sigUpdateWord),
                      (gpointer) word_form);
  }
  gtk_table_attach(GTK_TABLE(wordTable), buttonHbox, 0, 2, 10, 11, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

/********INDEX PAGE********************/

  makeIndexPage(word_form, wordNoteBook);

/********AUTHORS PAGE******************/

  makeAuthorPage(word_form, wordNoteBook);

/********SYN/ANTONYMS PAGE******************/

  makeSynAntPage(word_form, wordNoteBook);

/********LANG SPECIFIC PAGES****************/

  if (strcmp(lang, "greek") == 0)
  {
    greekWordPages(word_form, wordNoteBook);
  }

  if (strcmp(lang, "latin") == 0)
  {
    latinWordPages(word_form, wordNoteBook);
  }

/********************************/

  vbox = gtk_vbox_new(FALSE, 5);
  word_form->statusBar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(word_form->statusBar), FALSE);

  gtk_box_pack_start(GTK_BOX(vbox), wordNoteBook, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), word_form->statusBar, FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(wordWindow), vbox);

  if (!new)
  {
    if (!selectWord(word_id, word_form) ||
      !selectAuthors(word_id, word_form) ||
      !selectIndex(word_id, word_form) ||
      !selectSynonyms(word_id, word_form) ||
      !selectAntonyms(word_id, word_form))
    {
      g_free(word_form);
      gtk_widget_destroy(GTK_WIDGET(wordWindow));
/*      g_signal_emit_by_name (G_OBJECT(wordWindow), "delete_event"); */
      return NULL;
    }
  }
  gtk_widget_show_all (wordWindow);

  return GTK_WINDOW(wordWindow);
}
