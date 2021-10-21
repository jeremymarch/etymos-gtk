typedef struct {
  uint  id;
  char *indexTerm;
  uint  parent;
} indexTree;

int
makePartList(GtkComboBox *partCombo, gint selectedPart);

int
selectSynonyms(guint word_id, wordForm *word_form);

int
selectAntonyms(guint word_id, wordForm *word_form);

int
selectIndex(guint word_id, wordForm *word_form);

int
selectAuthors(guint word_id, wordForm *word_form);

int
selectWord(guint word_id, wordForm *word_form);

void
indexPrintRow (uint wordId, char *word, uint parent, GtkTreeIter *parentIter, GtkTreeStore *indexTreeStore, indexTree *row, uint num_rows);

int
requeryIndexTree(GtkTreeView *indexTreeView);
