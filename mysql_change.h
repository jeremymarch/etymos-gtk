#define MAX_ROOT_LEN 255
#define MAX_ROOT_DEF_LEN 255
#define MAX_ROOT_IEROOT_LEN 255
#define MAX_ROOT_PHONETIC_LEN 255
#define MAX_ROOT_NOTE_LEN 255

MYSQL *
index_do_connect (char *host_name, char *user_name, char *password, char *db_name,
                  unsigned int port_num, char *socket_name, unsigned int flags);

void
index_do_disconnect (MYSQL *conn);

MYSQL *
do_connect (char *host_name, char *user_name, char *password, char *db_name,
            unsigned int port_num, char *socket_name, unsigned int flags);

void
do_disconnect (MYSQL *conn);

int
addAntonym(MYSQL *conn, unsigned int word_id, unsigned int antonym_id, char *lang);

int
removeAntonym(MYSQL *conn, unsigned int word_id, unsigned int antonym_id, char *lang);

int 
addSynonym(MYSQL *conn, unsigned int word_id, unsigned int synonym_id, char *lang);

int 
removeSynonym(MYSQL *conn, unsigned int word_id, unsigned int synonym_id, char *lang);

int 
addIndexTerm(MYSQL *conn, unsigned int word_id, unsigned int index_id, char *lang);

int 
removeIndexTerm(MYSQL *conn, unsigned int word_id, unsigned int index_id, char *lang);

int 
deleteRoot(MYSQL *conn, unsigned int root_id, const char *lang);

int 
updateRoot(MYSQL *conn, unsigned int root_id, const char *root, const char *def, 
           const char *ieroot, const char *phonetic, const char *note, 
           const char *lang);

int 
insertRoot(MYSQL *conn, const char *root, const char *def, const char *ieroot, 
           const char *phonetic, const char *note, const char *lang);

int 
validateChangeParent(MYSQL *conn, unsigned int ppid, unsigned int word_id, char* lang);

int 
changeParent(MYSQL *conn, unsigned int parent_id, unsigned int word_id, char* lang);

int 
changeRoot(MYSQL *conn, unsigned int word_id, unsigned int root_id, char* lang);

char *
getIndexError();

char *
getError();

void 
print_error (MYSQL *conn, char *message);

char *
index_strmov(register char *dst, register const char *src);

int 
updateWord(MYSQL *conn, unsigned int word_id, const char *def, unsigned int partId, const char *note, 
           char compound, int status, unsigned int genderId, const char *lang);
           
