#define WORD_MAX_LEN 50
#define WORD_DEF_MAX_LEN 50
#define WORD_NOTE_MAX_LEN 50

typedef struct {
  GtkWidget *window;
  GtkWidget *statusBar;
  GtkWidget *word;
  GtkWidget *persDef;  /* definition from Perseus */
  GtkWidget *def;      /* my own definition */
  GtkWidget *note;
  GtkWidget *compNo;
  GtkWidget *compYes;
  GtkWidget *updated;
  GtkWidget *created;
  GtkWidget *present;
  GtkWidget *future;
  GtkWidget *aorist;
  GtkWidget *perfect;
  GtkWidget *perfmid;
  GtkWidget *aoristpass;
  GtkWidget *infinitive;       /* for latin */
  GtkWidget *perfectPassPart;  /* for latin */
  GtkComboBox *part;
  GtkComboBox *status;
  GtkComboBox *gender;
  GtkTreeView *authorTreeView;
  GtkTreeView *indexTreeView;
  GtkTreeView *synTreeView;
  GtkTreeView *antTreeView;
  guint        wordId;
  gchar        lang[20];
} wordForm;

GtkWidget *
create_status_combo ();

GtkWidget *
create_part_combo ();

GtkWidget *
create_gender_combo ();

void
synDragDataReceived (GtkWidget *wgt, GdkDragContext *context, int x, int y,
                        GtkSelectionData *seldata, guint info, guint time,
                        gpointer word_form);

void
antDragDataReceived (GtkWidget *wgt, GdkDragContext *context, int x, int y,
                        GtkSelectionData *seldata, guint info, guint time,
                        gpointer word_form);

void
indexDragDataReceived (GtkWidget *wgt, GdkDragContext *context, int x, int y,
                        GtkSelectionData *seldata, guint info, guint time,
                        gpointer word_form);

gboolean
sigUpdateWord (GtkWidget *button, gpointer data);

gboolean
sigResetWord (GtkWidget *button, gpointer data);

void
greekWordPages (wordForm *word_form, GtkWidget *wordNoteBook);

void
latinWordPages (wordForm *word_form, GtkWidget *wordNoteBook);

void
makeIndexPage (wordForm *word_form, GtkWidget *wordNoteBook);

void
makeAuthorPage (wordForm *word_form, GtkWidget *wordNoteBook);

gboolean
word_cleanup (GtkWidget *window, GdkEvent *event, gpointer word_form);

GtkWindow *
createWordWindow (guint new, guint word_id, gchar *language);

gboolean
sigIndex (GtkWidget *button, gpointer word_form);

gboolean
sigAntRemove (GtkWidget *button, gpointer word_form);

gboolean
sigSynRemove (GtkWidget *button, gpointer wf);

gboolean
sigRemoveIndex (GtkWidget *button, gpointer word_form);
