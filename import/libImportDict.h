#define QUERY_BUF_SIZE 2048

MYSQL *conn;

int
createGreekWordsTable();

int
loadGreekWordsTable(char *datafile);

int
createLatinWordsTable();

int
createRootsTable(char *lang);

int
loadGreekRootsTable(char *rootsDataFile);

int
createMiscTables(char *lang);

int
loadGreekSynonyms(char *synDataFile);

int
preGreekWordsTable();

int
postGreekWordsTable();

int
postLatinWordsTable();

int
getNumRowsGreekWordsTable();

int
addAuthorWords(char *author, char *authorDataFile, char *lang);

int
createAuthorTables(char *lang);

int
createOtherTables();

int
makeOtherTables();

void
print_error (MYSQL *conn, char *message);

MYSQL *
do_connect (char *host_name, char *user_name, char *password, char *db_name,
            unsigned int port_num, char *socket_name, unsigned int flags);

void
do_disconnect (MYSQL *conn);
