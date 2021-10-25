#define TAB_SIZE 1
#define MAX_ROWS 100

typedef struct itree {
  uint  id;
  char *word;
  uint  parent;
} itree;

int
selectRoot(guint root_id, rootForm *root_form);

int
reQueryWordList(etymosf *etyform, const gchar *arg);

char *
indent(char *word, uint level);

void 
itreePrintRow (uint wordId, char *word, uint parent, uint level, GtkListStore *itreeListStore, struct itree *row, uint num_rows);

int
reQueryItreeList(etymosf *etyform, guint root_id);

int
reQueryRootList(etymosf *etyform, const gchar *arg);

char *
strmov (register char *dst, register const char *src);

