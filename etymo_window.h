#define MAX_LANG_LEN 20
#define MAX_WORD_FIELDS 8

enum new_form
{
  EDIT_ROW = 0,
  NEW_ROW
};

enum root_list
{
  ROOT_COL_ID = 0,
  ROOT_COL_ROOT,
  ROOT_COL_DEF,
  ROOT_NUM_COLS
};

enum word_list
{
  WORD_COL_ID = 0,
  WORD_COL_WORD,
  WORD_NUM_COLS
};

enum itree_list
{
  ITREE_COL_ID = 0,
  ITREE_COL_WORD,
  ITREE_NUM_COLS
};

enum atree_list
{
  ATREE_COL_ID = 0,
  ATREE_COL_WORD,
  ATREE_NUM_COLS
};

enum dragTargets
{
  TARGET_INDEX_ID,
  TARGET_WORD_ID,
  TARGET_ROOT_ID,
  TARGET_STRING
};

typedef struct {
  GtkEntry     *wordSearchTextBox;
  GtkTreeView  *view;
  GtkListStore *store;
  guint         num_word_fields;
  gchar        *word_fields[MAX_WORD_FIELDS];
  gchar        *wordquery;
} word_view;

typedef struct {
  GtkWidget    *window;
  gchar         lang[MAX_LANG_LEN];
  GtkTreeView  *wordListView;
  GtkTreeView  *rootListView;
  GtkTreeView  *itreeListView;
  GtkTreeView  *alphaListView;
  GtkEntry     *wordSearchTextBox;
  GtkEntry     *rootSearchTextBox;
  GtkStatusbar *main_status_bar;
  guint         num_word_fields;
  gchar        *word_fields[MAX_WORD_FIELDS];
  word_view    *word;
} etymosf;

etymosf *
createEtymoWindow();

gint
create_options();

void
wordDragDataGet(GtkWidget *wordView, GdkDragContext *context, GtkSelectionData *seldata, 
                guint info, guint time, gpointer data);

gboolean
sigAddNewRoot(GtkWidget *button, gpointer data);

gboolean
sigEditRoot(GtkTreeView *treeview, GtkTreePath *arg1, GtkTreeViewColumn *arg2, gpointer ety);

gboolean
sigEditWord(GtkTreeView *treeview, GtkTreePath *arg1, GtkTreeViewColumn *arg2, gpointer ety);

gchar *
skipIndents(gchar *s);

gboolean
sigAddtoParent(GtkWidget *button, gpointer data);

gboolean
sigRemoveWord (GtkWidget *button, gpointer data);

gboolean
sigAddtoRoot(GtkWidget *button, gpointer data);

gboolean
sigReQueryRootList(GtkWidget *rootSearchTextBox, gpointer data);

gboolean
sigReQueryITreeList(GtkTreeSelection *rootListSelection, gpointer data);

gboolean
sigReQueryWordList(GtkWidget *wordSearchTextBox, gpointer data);

gboolean
open_options (GtkWidget *view, GdkEvent *e, gpointer data);

gboolean
iDragDataMotion (GtkWidget *destTreeView, 
                 GdkDragContext *drag_context,
                 gint x,
                 gint y,
                 guint time,
                 gpointer data);

void
iDragDataReceived (GtkWidget *destTreeView, 
                   GdkDragContext *context, 
                   gint x, 
                   gint y,
                   GtkSelectionData *seldata, 
                   guint info, 
                   guint time,
                   gpointer ety);

void
init_root_list ();

void
init_itree_list ();

void
init_alpha_list ();

void
init_word_list ();

gboolean
sigChangeLang(GtkWidget *comboBox, gpointer data);

