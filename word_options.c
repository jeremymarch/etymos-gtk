#include <gtk/gtk.h>

typedef struct {
  GtkWidget *window;
  GtkWidget *word;
  GtkWidget *def;
  GtkWidget *pdef;
  GtkWidget *part;
  GtkWidget *root;
  GtkWidget *gender;
  GtkWidget *status;
} opt;

gint
create_options()
{
  GtkWidget *vbox;
  opt *o;

  o = g_malloc0(sizeof(opt) * 1);

  o->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(o->window), GTK_WIN_POS_MOUSE);

  o->word = gtk_check_button_new_with_mnemonic("_Word");
  o->def  = gtk_check_button_new_with_mnemonic("_Definition");
  o->pdef = gtk_check_button_new_with_mnemonic("Perse_us Definition");
  o->part = gtk_check_button_new_with_mnemonic("_Part of Speech");
  o->root = gtk_check_button_new_with_mnemonic("_Root");
  o->gender = gtk_check_button_new_with_mnemonic("_Gender");
  o->status = gtk_check_button_new_with_mnemonic("_Status");

  vbox = gtk_vbox_new(FALSE, 3);

  gtk_box_pack_start (GTK_BOX(vbox), o->word, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), o->def, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), o->pdef, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), o->part, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), o->root, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), o->gender, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), o->status, TRUE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(o->window), vbox);

  gtk_widget_show_all(GTK_WIDGET(o->window));

  return 1;
}
