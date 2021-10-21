#include <gtk/gtk.h>
#include <mysql.h>
#include <stdlib.h>       /* for exit () */
#include "etymo_window.h"
#include "mysql_change.h" /* for conn I assume */

#define def_host_name   NULL   /* host to connect to (default = localhost) */
#define def_user_name  "grweb" /* user name (default = your login name) */
#define def_password   "1234"  /* password (default = none) */
#define def_port_num    0      /* use default port */
#define def_socket_name NULL   /* use default socket name */
#define def_db_name    "gr"    /* database to use (default = none) */

/*
TODO:
0. allow drag drop of roots/parents/words
1. allow to add/change columns in etymo lists
2. maybe use different store/model for each language so that when I switch back I have same values in list as before change.
   --then also save values in root and word entries and selected root so complete state save.  Allow flip back and forth!
*/

gint
loginWindow (gpointer main_window)
{
  GtkWidget *loginWindow;
  GtkWidget *userLabel, *passwordLabel;
  GtkWidget *userText, *passwordText;
  GtkWidget *buttonOK, *buttonCancel;
  GtkWidget *loginTable;
  GtkWidget *buttonHbox;

  loginTable = gtk_table_new(3, 2, FALSE);

  loginWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_modal(GTK_WINDOW(loginWindow), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(loginWindow), GTK_WINDOW(main_window));
  gtk_window_set_position(GTK_WINDOW(loginWindow), GTK_WIN_POS_CENTER_ALWAYS);

  userLabel = gtk_label_new("Username:");
  gtk_misc_set_alignment(GTK_MISC(userLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(userLabel), 5, 5);
  gtk_table_attach_defaults(GTK_TABLE(loginTable), userLabel, 0, 1, 0, 1);
  userText = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(loginTable), userText, 1, 2, 0, 1);

  passwordLabel = gtk_label_new("Password:");
  gtk_misc_set_alignment(GTK_MISC(passwordLabel), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC(passwordLabel), 5, 5);
  gtk_table_attach_defaults(GTK_TABLE(loginTable), passwordLabel, 0, 1, 1, 2);
  passwordText = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(loginTable), passwordText, 1, 2, 1, 2);

  buttonHbox = gtk_hbox_new(TRUE, 5);
  buttonOK = gtk_button_new_with_label("Login");
  buttonCancel = gtk_button_new_with_label("Cancel");
  gtk_box_pack_start(GTK_BOX(buttonHbox), buttonOK, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(buttonHbox), buttonCancel, TRUE, TRUE, 0);
  gtk_table_attach_defaults(GTK_TABLE(loginTable), buttonHbox, 0, 2, 2, 3);

  g_signal_connect (buttonCancel, "clicked", gtk_main_quit, NULL);
/*  g_signal_connect (buttonOK, "clicked", G_CALLBACK(login), NULL); */

  gtk_container_add(GTK_CONTAINER(loginWindow), loginTable);
  gtk_widget_show_all(loginWindow);

  gtk_window_set_focus (GTK_WINDOW(loginWindow), GTK_WIDGET(userText));

  return 1;
}

gboolean
cleanup (GtkWidget *window, gpointer data)
{
  do_disconnect (conn);
  return FALSE;
}

int
main (int argc, char **argv)
{
  etymosf *etyform;

  gtk_init (&argc, &argv);

  conn = do_connect (def_host_name, def_user_name, def_password, def_db_name,
                     def_port_num, def_socket_name, 0);
  if (conn == NULL)
    exit (1);

 /* language = loginWindow (window); */

  etyform = createEtymoWindow ();

  g_signal_connect (etyform->window, "delete_event", G_CALLBACK(cleanup), NULL);
  g_signal_connect (etyform->window, "destroy", gtk_main_quit, NULL);

  gtk_main ();

  exit (0);
}
