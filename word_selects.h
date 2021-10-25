typedef struct {
  unsigned int  id;
  char *indexTerm;
  unsigned int  parent;
} indexTree;

int
makePartList(GtkComboBox *partCombo, gint selectedPart);

int
selectSynonyms(unsigned int word_id, wordForm *word_form);

int
selectAntonyms(unsigned int word_id, wordForm *word_form);

int
selectIndex(unsigned int word_id, wordForm *word_form);

int
selectAuthors(unsigned int word_id, wordForm *word_form);

int
selectWord(unsigned int word_id, wordForm *word_form);

void
indexPrintRow (unsigned int wordId, char *word, unsigned int parent, GtkTreeIter *parentIter, GtkTreeStore *indexTreeStore, indexTree *row, unsigned int num_rows);

int
requeryIndexTree(GtkTreeView *indexTreeView);
