#define MAX_QUERY_LEN 1024
#define MAX_ERROR_LEN 2048

extern char *int10_to_str(long val,char *dst,int radix);

typedef struct {
  GtkWidget *window;
  GtkWidget *termText;
  GtkWidget *parentLabel;
  GtkWidget *parentText;
  GtkWidget *scopeLabel;
  GtkWidget *scopeTextView;
  GtkWidget *seeAlsoLabel;
  GtkWidget *seeAlsoText;
  GtkWidget *useTreeView;
  GtkWidget *relatedTreeView;
  GtkWidget *preferred;
  GtkWidget *updated;
  GtkWidget *created;
  GtkWidget *statusBar;
  GtkWidget *useScrolledWindow;
  GtkWidget *relatedScrolledWindow;
  uint       index_id;
} indexForm;

typedef struct {
  int  id;
  char term[50];
  int  group;
} useStruct;

int
beginTrans();

int
commitTrans();

int
rollbackTrans();

char *
getError();

MYSQL *
do_connect (char *host_name, char *user_name, char *password, char *db_name,
      unsigned int port_num, char *socket_name, unsigned int flags);

void
do_disconnect (MYSQL *conn);

int
removeRelated(int index_id, int related_id);

int
addRelated(int index_id, int related_id);

int
removeUse(int index_id, int group);

int
addUse(int index_id, int preferred_id, int group);

int
deleteTerm(int index_id, char *foreignTable, int audoDelNonPref);

int
updateTerm(int index_id, const char *term, const char *scope_note, int parent_id, const char *see_also, int preferred);

int
insertTerm (const char *term, const char *scope_note, int parent_id, const char *see_also);

int
selectRelated(int index_id, indexForm *index);

int
selectUseFor(int index_id, indexForm *index);

int
selectUse(int index_id, indexForm *index);

int
selectTerm(int index_id, indexForm *index);

