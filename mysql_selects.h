#define TAB_SIZE 1
#define MAX_ROWS 100

typedef struct itree {
  unsigned int  id;
  char *word;
  unsigned int  parent;
} itree;

int
selectRoot(unsigned int root_id, rootForm *root_form);

int
reQueryWordList(etymosf *etyform, const gchar *arg);

char *
indent(char *word, unsigned int level);

void 
itreePrintRow (unsigned int wordId, char *word, unsigned int parent, unsigned int level, GtkListStore *itreeListStore, struct itree *row, unsigned int num_rows);

int
reQueryItreeList(etymosf *etyform, unsigned int root_id);

int
reQueryRootList(etymosf *etyform, const gchar *arg);

char *
strmov (register char *dst, register const char *src);

