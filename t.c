#include <gtk/gtk.h>

typedef struct {
GtkWidget *view;
} blah;

int
main (int argc, char **argv)
{
  GtkWidget *window;
  GtkTreeModel *m;
  GtkTreeSelection *s;
  blah *b;

  gtk_init (&argc, &argv);

  b = (blah *) g_malloc(sizeof(blah) * 1);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  b->view = (GtkTreeView *) gtk_tree_view_new ();

  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(b->view));

  gtk_widget_show_all(window);

  g_print("here\n");

  m = gtk_tree_view_get_model(GTK_TREE_VIEW(b->view));
  s = gtk_tree_view_get_selection(GTK_TREE_VIEW(b->view));

  gtk_main();
}
