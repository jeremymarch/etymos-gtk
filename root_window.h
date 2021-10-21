typedef struct {
  GtkWidget *window;
  GtkWidget *statusBar;
  GtkWidget *root;
  GtkWidget *def;
  GtkWidget *ie;
  GtkWidget *phonetic;
  GtkWidget *note;
  GtkWidget *updated;
  GtkWidget *created;
  guint      rootId;
  gchar      lang[20];
} rootForm;

GtkWindow *createRootWindow (guint new, guint root_id, gchar *language);
