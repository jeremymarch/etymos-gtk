#include <gtk/gtk.h>
#include <mysql.h>
#include <string.h>
#include "root_window.h"
#include "error.h"
#include "mysql_change.h"
#include "etymo_window.h"
#include "mysql_selects.h"

static gboolean 
sigUpdateRoot(GtkWidget *button, gpointer data);

static gboolean 
sigDeleteRoot(GtkWidget *button, gpointer data);

static gboolean
sigInsertRoot(GtkWidget *button, gpointer data);

static gboolean
root_cleanup(GtkWidget *window, GdkEvent *event, gpointer root_form);

static gboolean
sigReset(GtkWidget *resetButton, gpointer root_form);

static gboolean
sigRootChanged (GtkWidget *entry, gpointer root_Form);

/*** end of declarations ***/

static gboolean
sigRootChanged (GtkWidget *entry, gpointer root_Form)
{
  /* blockChangeHandlers(root_Form); */
  printStatusMesg(GTK_STATUSBAR( ((rootForm *) root_Form)->statusBar), "dirty");

  return FALSE;
}

int
attachChangeHandlers (rootForm *root_Form)
{
  gulong rid;
  gulong defid;
  gulong ieid;
  gulong phoid;
  gulong noteid;

  rid = g_signal_connect (GTK_WIDGET(root_Form->root), "changed", 
                    G_CALLBACK(sigRootChanged), (gpointer) root_Form);
  defid = g_signal_connect (GTK_WIDGET(root_Form->def), "changed", 
                    G_CALLBACK(sigRootChanged), (gpointer) root_Form);
  ieid = g_signal_connect (GTK_WIDGET(root_Form->ie), "changed", 
                    G_CALLBACK(sigRootChanged), (gpointer) root_Form);
  phoid = g_signal_connect (GTK_WIDGET(root_Form->phonetic), "changed", 
                    G_CALLBACK(sigRootChanged), (gpointer) root_Form);
  noteid = g_signal_connect (GTK_WIDGET(root_Form->note), "changed", 
                    G_CALLBACK(sigRootChanged), (gpointer) root_Form);

  return 1;
}

int
blockChangeHandlers (rootForm *root_form)
{
  return 0;
}

static gboolean
sigUpdateRoot (GtkWidget *button, gpointer root_form)
{
  const gchar *root;
  const gchar *def;
  const gchar *ieroot;
  const gchar *phonetic;
  const gchar *note;
  gint  affected_rows;

  root     = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->root));
  def      = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->def));
  ieroot   = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->ie));
  phonetic = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->phonetic));
  note     = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->note));

  if ((affected_rows = updateRoot(conn, (guint) ((rootForm *) root_form)->rootId, root, def, ieroot, phonetic, note, ((rootForm *) root_form)->lang)) > 0)
  {
    selectRoot((guint) ((rootForm *) root_form)->rootId, root_form);
    printStatusMesg(GTK_STATUSBAR(((rootForm *) root_form)->statusBar), "Updated!");
  }
  else if (affected_rows == 0)
  {
    printStatusMesg(GTK_STATUSBAR(((rootForm *) root_form)->statusBar), "Updated 0 rows");
  }
  else if (affected_rows < 0)
  {
    printStatusMesg(GTK_STATUSBAR(((rootForm *) root_form)->statusBar), getError());
  }
  return FALSE;
}

static gboolean
sigDeleteRoot (GtkWidget *button, gpointer root_form)
{
  if (deleteRoot(conn, (guint) ((rootForm *) root_form)->rootId, ((rootForm *) root_form)->lang) > 0)
  {
/*    requeryRootList(etyform, ""); 
    printStatusMesg(main_status_bar, "Deleted"); */

    GtkWidget *tempWindow;

    tempWindow = ((rootForm *) root_form)->window;

    g_free(root_form);
    gtk_widget_destroy(GTK_WIDGET(tempWindow));
  }
  else
  {
    printStatusMesg(GTK_STATUSBAR(((rootForm *) root_form)->statusBar), getError());
  }
  return FALSE;
}

static gboolean
sigInsertRoot (GtkWidget *button, gpointer root_form)
{
  const gchar *root;
  const gchar *def;
  const gchar *ieroot;
  const gchar *phonetic;
  const gchar *note;

  root     = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->root));
  def      = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->def));
  ieroot   = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->ie));
  phonetic = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->phonetic));
  note     = gtk_entry_get_text(GTK_ENTRY( ((rootForm *) root_form)->note));

  if (insertRoot(conn, root, def, ieroot, phonetic, note, ((rootForm *) root_form)->lang) > 0)
  {
/*    g_signal_emit_by_name(((rootForm *) root_form)->rootS earchTextBox, "changed"); */
/*    printStatusMesg(main_status_bar, "Inserted"); */

    GtkWidget   *tempWindow;

    tempWindow = ((rootForm *) root_form)->window;

    g_free(root_form);
    gtk_widget_destroy(GTK_WIDGET(tempWindow));
  }
  else
  {
    printStatusMesg(GTK_STATUSBAR(((rootForm *) root_form)->statusBar), getError());
  }
  return FALSE; 
}

static gboolean
sigReset (GtkWidget *resetButton, gpointer root_form)
{
  selectRoot((guint) ((rootForm *)root_form)->rootId, (rootForm *) root_form);

  return FALSE;
}

static gboolean
root_cleanup (GtkWidget *window, GdkEvent *event, gpointer root_form)
{
  g_free (root_form);

  return FALSE;
}

GtkWindow *
createRootWindow (guint new, guint root_id, gchar *lang)
{
  GtkWidget *rootLabel, *defLabel, *ieLabel, *phoneticLabel, *noteLabel, *updatedLabel, *createdLabel;
  GtkWidget *buttonHbox;
  GtkWidget *buttonInsert, *buttonReset, *buttonUpdate, *buttonDelete;
  GtkWidget *rootTable;
  rootForm  *root_form;
  GtkWidget *rootWindow;
  GtkWidget *vbox;

  root_form = (rootForm *) g_malloc0(sizeof(rootForm) * 1);
  root_form->rootId = root_id;
  strncpy(root_form->lang, lang, MAX_LANG_LEN);
/*  root_form->parent = etyform; */

  rootWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  root_form->window = rootWindow;
/*  gtk_widget_set_size_request(GTK_WIDGET(rootWindow), 400, 245); */
  gtk_window_set_title (GTK_WINDOW(rootWindow), "Roots");
  gtk_window_set_position(GTK_WINDOW(rootWindow), GTK_WIN_POS_CENTER_ALWAYS);

  g_signal_connect (GTK_WIDGET(rootWindow), "delete_event", G_CALLBACK(root_cleanup), (gpointer) root_form);

  rootTable = gtk_table_new(9, 2, FALSE);

  rootLabel = gtk_label_new("Root:");
  gtk_misc_set_alignment(GTK_MISC(rootLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(rootLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), rootLabel, 0, 1, 0, 1, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  root_form->root = gtk_entry_new();
  gtk_widget_set_size_request (GTK_WIDGET(root_form->root), 250, -1); /* just set size of one, the others will follow */
  gtk_entry_set_max_length (GTK_ENTRY(root_form->root), MAX_ROOT_LEN);
  gtk_table_attach(GTK_TABLE(rootTable), root_form->root, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  defLabel = gtk_label_new("Definition:");
  gtk_misc_set_alignment(GTK_MISC(defLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(defLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), defLabel, 0, 1, 1, 2, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  root_form->def = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(root_form->def), MAX_ROOT_DEF_LEN);
  gtk_table_attach(GTK_TABLE(rootTable), root_form->def, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  ieLabel = gtk_label_new("Indo-European:");
  gtk_misc_set_alignment(GTK_MISC(ieLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(ieLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), ieLabel, 0, 1, 2, 3, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  root_form->ie = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(root_form->ie), MAX_ROOT_IEROOT_LEN);
  gtk_table_attach(GTK_TABLE(rootTable), root_form->ie, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  phoneticLabel = gtk_label_new("Phonetic:");
  gtk_misc_set_alignment(GTK_MISC(phoneticLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(phoneticLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), phoneticLabel, 0, 1, 3, 4, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  root_form->phonetic = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(root_form->phonetic), MAX_ROOT_PHONETIC_LEN);
  gtk_table_attach(GTK_TABLE(rootTable), root_form->phonetic, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  noteLabel = gtk_label_new("Note:");
  gtk_misc_set_alignment(GTK_MISC(noteLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(noteLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), noteLabel, 0, 1, 4, 5, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);
  root_form->note = gtk_entry_new();
  gtk_entry_set_max_length (GTK_ENTRY(root_form->note), MAX_ROOT_NOTE_LEN);
  gtk_table_attach(GTK_TABLE(rootTable), root_form->note, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

/*
  GtkWidget *noteTextView;
  GtkTextBuffer *noteTextBuffer;

  noteTextView = gtk_text_view_new ();
  noteTextBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (noteTextView));
  gtk_text_buffer_set_text (noteTextBuffer, "Hello, this is some text", -1);
  gtk_table_attach_defaults(GTK_TABLE(rootTable), noteTextView, 0, 2, 4, 5);
*/

  updatedLabel = gtk_label_new("Updated:");
  gtk_misc_set_alignment(GTK_MISC(updatedLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(updatedLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), updatedLabel, 0, 1, 5, 6, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  root_form->updated = gtk_label_new(NULL);
  gtk_misc_set_alignment(GTK_MISC(root_form->updated), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(root_form->updated), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), root_form->updated, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  createdLabel = gtk_label_new("Created:");
  gtk_misc_set_alignment(GTK_MISC(createdLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(createdLabel), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), createdLabel, 0, 1, 6, 7, GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 0, 0);

  root_form->created = gtk_label_new(NULL);
  gtk_misc_set_alignment(GTK_MISC(root_form->created), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(root_form->created), 5, 5);
  gtk_table_attach(GTK_TABLE(rootTable), root_form->created, 1, 2, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  buttonHbox = gtk_hbox_new(FALSE, 5);
  if (new)
  {
    buttonInsert = gtk_button_new_with_mnemonic("_Insert");
    gtk_box_pack_start(GTK_BOX(buttonHbox), buttonInsert, TRUE, TRUE, 0);

    g_signal_connect (GTK_WIDGET(buttonInsert), "clicked", G_CALLBACK(sigInsertRoot), root_form);

    /* gtk_window_set_default (GTK_WINDOW(root_form->window), GTK_WIDGET(buttonInsert)); */
  }
  else
  {
    buttonReset  = gtk_button_new_with_mnemonic("_Reset");
    buttonUpdate = gtk_button_new_with_mnemonic("_Update");
    buttonDelete = gtk_button_new_with_mnemonic("_Delete");

    gtk_box_pack_start(GTK_BOX(buttonHbox), buttonReset, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(buttonHbox), buttonUpdate, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(buttonHbox), buttonDelete, TRUE, TRUE, 0);

    g_signal_connect (GTK_WIDGET(buttonReset), "clicked", G_CALLBACK(sigReset), (gpointer) root_form);
    g_signal_connect (GTK_WIDGET(buttonUpdate), "clicked", G_CALLBACK(sigUpdateRoot), (gpointer) root_form);
    g_signal_connect (GTK_WIDGET(buttonDelete), "clicked", G_CALLBACK(sigDeleteRoot), (gpointer) root_form);
  }
  gtk_table_attach_defaults(GTK_TABLE(rootTable), buttonHbox, 0, 2, 7, 8);

  vbox = gtk_vbox_new(FALSE, 5);
  root_form->statusBar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(root_form->statusBar), FALSE);

  gtk_box_pack_start(GTK_BOX(vbox), rootTable, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), root_form->statusBar, FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(rootWindow), vbox);

  if (!new)
  {
    if (!selectRoot(root_id, root_form))
    {
      g_free(root_form);
      gtk_widget_destroy(GTK_WIDGET(rootWindow));
/*      g_signal_emit_by_name (G_OBJECT(rootWindow), "delete_event"); */
      return NULL;
    }
  }
  attachChangeHandlers(root_form);

  gtk_widget_show_all (rootWindow);

  return GTK_WINDOW(rootWindow);
}
