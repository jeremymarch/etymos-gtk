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
addAntonym(MYSQL *conn, uint word_id, uint antonym_id, char *lang);

int
removeAntonym(MYSQL *conn, uint word_id, uint antonym_id, char *lang);

int 
addSynonym(MYSQL *conn, uint word_id, uint synonym_id, char *lang);

int 
removeSynonym(MYSQL *conn, uint word_id, uint synonym_id, char *lang);

int 
addIndexTerm(MYSQL *conn, uint word_id, uint index_id, char *lang);

int 
removeIndexTerm(MYSQL *conn, uint word_id, uint index_id, char *lang);

int 
deleteRoot(MYSQL *conn, uint root_id, const char *lang);

int 
updateRoot(MYSQL *conn, uint root_id, const char *root, const char *def, 
           const char *ieroot, const char *phonetic, const char *note, 
           const char *lang);

int 
insertRoot(MYSQL *conn, const char *root, const char *def, const char *ieroot, 
           const char *phonetic, const char *note, const char *lang);

int 
validateChangeParent(MYSQL *conn, uint ppid, uint word_id, char* lang);

int 
changeParent(MYSQL *conn, uint parent_id, uint word_id, char* lang);

int 
changeRoot(MYSQL *conn, uint word_id, uint root_id, char* lang);

char *
getIndexError();

char *
getError();

void 
print_error (MYSQL *conn, char *message);

char *
index_strmov(register char *dst, register const char *src);

int 
updateWord(MYSQL *conn, uint word_id, const char *def, uint partId, const char *note, 
           char compound, int status, uint genderId, const char *lang);
           
