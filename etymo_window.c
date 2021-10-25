#include <gtk/gtk.h>
#include <mysql.h>
#include <string.h>
#include "error.h"
#include "etymo_window.h"
#include "mysql_change.h"
#include "word_window.h" /* for createWordWindow() ... */
#include "root_window.h" /* for createRootWindow() ... */
#include "mysql_selects.h"

extern MYSQL *conn;

static GtkTreeView *
create_root_list_view_and_model();

static GtkTreeView *
create_itree_list_view_and_model();

static GtkTreeView *
create_alpha_list_view_and_model();

static GtkTreeView *
create_word_list_view_and_model();

static GtkWidget *
create_lang_combo_and_model();

/***end declarations***/

static GtkTreeView *
create_root_list_view_and_model()
{
  GtkCellRenderer     *renderer;
  GtkWidget           *view;
  GtkListStore        *rootListStore;

  view = gtk_tree_view_new();

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Root",  
                                               renderer,
                                               "text", ROOT_COL_ROOT,
                                               NULL);

  /* --- Column #2 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Def",  
                                               renderer,
                                               "text", ROOT_COL_DEF,
                                               NULL);

  rootListStore = gtk_list_store_new (ROOT_NUM_COLS, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(rootListStore));

  return (GtkTreeView *) view;
}

static GtkTreeView *
create_itree_list_view_and_model()
{
  GtkCellRenderer     *renderer;
  GtkWidget           *view;
  GtkListStore        *itreeListStore;

  view = gtk_tree_view_new ();

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Parent",  
                                               renderer,
                                               "text", ITREE_COL_WORD,
                                               NULL);


  itreeListStore = gtk_list_store_new (ITREE_NUM_COLS, G_TYPE_UINT, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(itreeListStore));

  return (GtkTreeView *) view;
}

static GtkTreeView *
create_alpha_list_view_and_model()
{
  GtkCellRenderer     *renderer;
  GtkWidget           *view;
  GtkListStore        *alphaListStore;

  view = gtk_tree_view_new ();

  /* --- Column #1 --- */
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Word",  
                                               renderer,
                                               "text", ATREE_COL_WORD,
                                               NULL);

  alphaListStore = gtk_list_store_new (ATREE_NUM_COLS, G_TYPE_UINT, G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(alphaListStore));

  return (GtkTreeView *) view;
}
/*
static GtkTreeView *
create_word_list_view_and_model()
{
  GtkCellRenderer     *renderer;
  GtkWidget           *view;
  GtkListStore        *wordListStore;

  view = gtk_tree_view_new ();

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Word",  
                                               renderer,
                                               "text", WORD_COL_WORD,
                                               NULL);

  wordListStore = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);

  gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(wordListStore));

  return (GtkTreeView *) view;
}
*/

static void
set_columns_model (GtkTreeView *view, guint num_cols, GType *col_types)
{
  GtkListStore *store;
  GtkTreeModel *model;

  if ((model = gtk_tree_view_get_model (view)))
    g_object_unref (G_OBJECT(model));

  if (num_cols == 0)
  {
    store = NULL;
  }
  else
  {
    store = gtk_list_store_new (1, G_TYPE_INT); /* at least 1 col seems necessary */
    gtk_list_store_set_column_types (store, num_cols, col_types);
  }

  gtk_tree_view_set_model (view, GTK_TREE_MODEL(store));

  return;
}

/* don't show first column -- see for(i = 1)... */
static void
set_columns_cols (GtkTreeView *view, guint num_cols, gchar *col_name[])
{
  guint            i;
  GtkCellRenderer *renderer;
  GList           *cols, *c;

  cols = gtk_tree_view_get_columns (view);
  for (c = cols ; c; c = c->next)
  {
    gtk_tree_view_remove_column (view, GTK_TREE_VIEW_COLUMN(c->data));
  }
  g_list_free (cols);

  renderer = gtk_cell_renderer_text_new ();
  for (i = 1; i < num_cols; i++)
  {
    gtk_tree_view_insert_column_with_attributes (view,
                                                 -1,      
                                                 col_name[i],  
                                                 renderer,
                                                 "text", 
                                                 i,
                                                 NULL);
  }
  return;
}

static GtkTreeView *
create_word_list_view_and_model()
{
  GtkWidget *view;
  GType      col_types[] = { G_TYPE_UINT, G_TYPE_STRING };
  gchar     *col_names[] = { "id", "word" };

  view = gtk_tree_view_new ();

  set_columns_model (GTK_TREE_VIEW(view), 2, col_types);

  set_columns_cols (GTK_TREE_VIEW(view), 2, col_names);

  return GTK_TREE_VIEW(view);
}

static GtkWidget *
create_lang_combo_and_model()
{
  GtkWidget       *comboBox;
  GtkListStore    *model;
  GtkTreeIter      iter;
  GtkCellRenderer *renderer;

  model    = gtk_list_store_new (2, G_TYPE_UINT, G_TYPE_STRING);
  comboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (comboBox), renderer, FALSE);
  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (comboBox), renderer, "text", 1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter, 0, 1, 1, "greek", -1);

  gtk_combo_box_set_active_iter (GTK_COMBO_BOX(comboBox), &iter);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter, 0, 2, 1, "latin", -1);

  return comboBox;
}

etymosf *
createEtymoWindow()
{
  GtkWidget *etymoTable;
  GtkWidget *vbox;                   /* contains etymoTable and main_status_bar */
  GtkWidget *wordListScrolledWindow;
  GtkWidget *rootListScrolledWindow;
  GtkWidget *itreeListScrolledWindow;
  GtkWidget *alphaListScrolledWindow;
  GtkWidget *buttonAddtoParent, *buttonAddtoRoot, *buttonAddNewRoot, *buttonRemoveWord;
  GtkWidget *langCombo;
  GtkTreeSelection *rootListSelection;
  etymosf *etyform;

  etyform = (etymosf *) g_malloc0(sizeof(etymosf) * 1);

  strncpy(etyform->lang, "greek", MAX_LANG_LEN);

  etyform->num_word_fields = 2;
  etyform->word_fields[0] = g_strdup ("id");
  etyform->word_fields[1] = g_strdup ("word");

  etymoTable = gtk_table_new(4, 4, FALSE);

  etyform->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW(etyform->window), 900, 700);
  gtk_window_set_title (GTK_WINDOW(etyform->window), "Etymology Form");

/*******ROOT****************/

  etyform->rootSearchTextBox = (GtkEntry *) gtk_entry_new();

  buttonAddNewRoot = gtk_button_new_with_mnemonic("Add _New Root");

  etyform->rootListView = create_root_list_view_and_model ();
  init_root_list(etyform);

  rootListScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (rootListScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(rootListScrolledWindow), GTK_WIDGET(etyform->rootListView));

  gtk_table_attach(GTK_TABLE(etymoTable), buttonAddNewRoot, 0, 1, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 3, 3);
  gtk_table_attach(GTK_TABLE(etymoTable), GTK_WIDGET(etyform->rootSearchTextBox), 0, 1, 2, 3, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 3, 3);
  gtk_table_attach(GTK_TABLE(etymoTable), rootListScrolledWindow, 0, 1, 3, 4, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 3, 3);

  rootListSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(etyform->rootListView));

/*********INDENTED TREE*********/

  buttonRemoveWord = gtk_button_new_with_mnemonic("_Remove from Root");

  etyform->itreeListView = create_itree_list_view_and_model ();
  init_itree_list (etyform);

  itreeListScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (itreeListScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(itreeListScrolledWindow), GTK_WIDGET(etyform->itreeListView));

  gtk_table_attach(GTK_TABLE(etymoTable), buttonRemoveWord, 1, 2, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 3, 3);
  gtk_table_attach(GTK_TABLE(etymoTable), itreeListScrolledWindow, 1, 2, 3, 4, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 3, 3);

  GtkTargetEntry targetentries[] =
  {
    { "word id", GTK_TARGET_SAME_APP, TARGET_WORD_ID }
  };

  g_signal_connect (G_OBJECT(etyform->itreeListView), "drag-data-received", G_CALLBACK(iDragDataReceived), etyform);
  g_signal_connect (G_OBJECT(etyform->itreeListView), "drag-motion", G_CALLBACK(iDragDataMotion), NULL);

  gtk_drag_dest_set (GTK_WIDGET(etyform->itreeListView), 
                     GTK_DEST_DEFAULT_ALL, targetentries, 1, GDK_ACTION_COPY);

/********ALPHA LIST************/

  buttonAddtoParent = gtk_button_new_with_mnemonic("Add to _Parent");

  etyform->alphaListView = create_alpha_list_view_and_model ();
  init_alpha_list (etyform);

  alphaListScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (alphaListScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(alphaListScrolledWindow), GTK_WIDGET(etyform->alphaListView));

  gtk_table_attach(GTK_TABLE(etymoTable), buttonAddtoParent, 2, 3, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 3, 3);
  gtk_table_attach(GTK_TABLE(etymoTable), alphaListScrolledWindow, 2, 3, 3, 4, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 3, 3);

  gtk_drag_source_set (GTK_WIDGET(etyform->alphaListView), GDK_BUTTON1_MASK, 
                       targetentries, 1, GDK_ACTION_COPY);

  g_signal_connect (G_OBJECT(etyform->alphaListView), "drag_data_get", 
                                G_CALLBACK(wordDragDataGet), 
                                NULL);
/*********WORD***************/

  langCombo = create_lang_combo_and_model();

  etyform->wordSearchTextBox = (GtkEntry *) gtk_entry_new();

  buttonAddtoRoot = gtk_button_new_with_mnemonic("_Add to Root");

  etyform->wordListView = create_word_list_view_and_model();
/*
  gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW(wordListView), TRUE);
*/
  g_signal_connect(G_OBJECT(etyform->wordListView), "event", G_CALLBACK(open_options), NULL);

  init_word_list (etyform);

  wordListScrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (wordListScrolledWindow),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  gtk_container_add(GTK_CONTAINER(wordListScrolledWindow), GTK_WIDGET(etyform->wordListView));

  gtk_drag_source_set (GTK_WIDGET(etyform->wordListView), GDK_BUTTON1_MASK, 
                       targetentries, 1, GDK_ACTION_COPY);

  gtk_table_attach(GTK_TABLE(etymoTable), langCombo, 3, 4, 0, 1, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 3, 3);
  gtk_table_attach(GTK_TABLE(etymoTable), buttonAddtoRoot, 3, 4, 1, 2, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 3, 3);
  gtk_table_attach(GTK_TABLE(etymoTable), GTK_WIDGET(etyform->wordSearchTextBox), 3, 4 , 2, 3, 
                   GTK_SHRINK | GTK_FILL, GTK_SHRINK | GTK_FILL, 3, 3);
  gtk_table_attach(GTK_TABLE(etymoTable), wordListScrolledWindow, 3, 4, 3, 4, 
                   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 3, 3);

/******SIGNALS**************/

  g_signal_connect (G_OBJECT(etyform->wordListView), "drag_data_get", 
                                G_CALLBACK(wordDragDataGet), 
                                NULL);

  g_signal_connect(G_OBJECT(etyform->rootSearchTextBox), "changed",
                                G_CALLBACK(sigReQueryRootList),
                                etyform);

  g_signal_connect(G_OBJECT(etyform->wordSearchTextBox), "changed",
                                G_CALLBACK(sigReQueryWordList),
                                etyform);

  g_signal_connect(G_OBJECT(etyform->rootListView), "row-activated",
                                G_CALLBACK(sigEditRoot),
                                (gpointer) etyform);

  g_signal_connect(G_OBJECT(etyform->wordListView), "row-activated",
                                G_CALLBACK(sigEditWord),
                                (gpointer) etyform);

  g_signal_connect(G_OBJECT(rootListSelection), "changed",
                                G_CALLBACK(sigReQueryITreeList),
                                etyform);

  g_signal_connect(G_OBJECT(buttonAddtoParent), "clicked",
                                G_CALLBACK(sigAddtoParent),
                                etyform);

  g_signal_connect(G_OBJECT(buttonAddtoRoot), "clicked",
                                G_CALLBACK(sigAddtoRoot),
                                etyform);

  g_signal_connect(G_OBJECT(buttonRemoveWord), "clicked",
                                G_CALLBACK(sigRemoveWord),
                                etyform);

  g_signal_connect(G_OBJECT(buttonAddNewRoot), "clicked",
                                G_CALLBACK(sigAddNewRoot),
                                etyform);

  g_signal_connect(G_OBJECT(GTK_COMBO_BOX(langCombo)), "changed",
                                G_CALLBACK(sigChangeLang),
                                etyform);
/*******ALL***************/

  etyform->main_status_bar = (GtkStatusbar *) gtk_statusbar_new();

  vbox = gtk_vbox_new(FALSE, 3);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(etymoTable), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(etyform->main_status_bar), FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(etyform->window), vbox);
  gtk_widget_show_all (etyform->window);

  return etyform;
}
