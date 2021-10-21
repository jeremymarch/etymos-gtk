#include <mysql.h>
#include <stdio.h>
#include "libImportDict.h"

#define def_host_name   NULL   /* host to connect to (default = localhost) */
#define def_user_name  "grweb" /* user name (default = your login name) */
#define def_password   "1234"  /* password (default = none) */
#define def_port_num    0      /* use default port */
#define def_socket_name NULL   /* use default socket name */
#define def_db_name    "grtest"    /* database to use (default = none) */

/**
 * TODO:
 * 1. build authors array by scanning directory not manual list
 * 2. build index, gender ... tables
 * 3. get options from command liine
 * 4. rebuild data files using correct format (optionally enclose in ")
 */

int
loadAuthorTables(char *lang, char *authorsDir)
{
/*
  DIR *dir;
  dir = opendir("grtxt");

  while ((file = readdir(dir)) != FALSE)
  {

  }
*/
  char *greek_authors[] = {
                     "aeschines", 
                     "aeschylus",
                     "andocides",
                     "antiphon",
                     "apollonius_rhodius",
                     "aristophanes",
                     "aristotle",
                     "bacchylides",
                     "callimachus",
                     "demades",
                     "demosthenes",
                     "dinarchus",
                     "diodorus",
                     "euclid",
                     "euripides",
                     "flavius_josephus",
                     "herodotus",
                     "hesiod",
                     "homer",
                     "homeric_hymns",
                     "hyperides",
                     "isaeus",
                     "isocrates",
                     "lycurgus",
                     "lysias",
                     "new_testament",
                     "old_oligarch",
                     "pausanias",
                     "pindar",
                     "plutarch",
                     "pseudo_apollonius",
                     "sophocles",
                     "strabo",
                     "theocritus",
                     "theophrastus",
                     "thucydides",
                     "xenophon",
                      NULL };

  char *latin_authors[] = {
                     "boethius",
                     "caesar_civil_war",
                     "caesar_gallic_wars",
                     "catullus",
                     "cicero",
                     "cornelius_tacitus_opera_minora",
                     "horace",
                     "latin_vulgate",
                     "livy",
                     "new_testament",
                     "old_testament",
                     "ovid",
                     "phaedrus",
                     "plautus",
                     "q_tul_cicero_running_for_office",
                     "servius",
                     "sextus_propertius",
                     "suetonius",
                     "sulpicia",
                     "tacitus",
                     "terence",
                     "vergil",
                     "vitruvius",
                      NULL };

  char **authors;

  if (strcmp(lang, "greek") == 0)
    authors = greek_authors;
  else if (strcmp(lang, "latin") == 0)
    authors = latin_authors;
  else
    return 0;

  int numAdded;
  char authorFile[50];
  int i = 0;
  while (authors[i] != NULL)
  {
    fprintf(stdout, "%s:\n", authors[i]);

    snprintf(authorFile, 128, "%s/%s.txt", authorsDir, authors[i]);
    if ((numAdded = addAuthorWords(authors[i], authorFile, lang)) < 0)
      return 0;

    fprintf(stdout, "rows added: %i\n", numAdded);
    i++;
  }
  return 1;
}

int
makeGreek()
{
  char lang[]          = "greek";
  char datafile[]      = "data/palatinoall.txt";
  char rootsDataFile[] = "data/greek_roots.txt";
  char synDataFile[]   = "data/greek_synonyms.txt";
  char authorsDir[]    = "data/greek_authors";

  if (!createGreekWordsTable())
    return 0;

  if (!loadGreekWordsTable(datafile))
    return 0;

  if (!preGreekWordsTable())
    return 0;

  fprintf(stdout, "Greek words rows: %i\n", getNumRowsGreekWordsTable());

  if (!createAuthorTables(lang))
    return 0;

  if (!loadAuthorTables(lang, authorsDir))
    return 0;

  if (!createRootsTable(lang))
    return 0;

  if (!loadGreekRootsTable(rootsDataFile))
    return 0;

  if (!createMiscTables(lang))
    return 0;

  if (!loadGreekSynonyms(synDataFile))
    return 0;

  if (!postGreekWordsTable())
    return 0;

  return 1;
}

int
makeLatin()
{
  char lang[]       = "latin";
  char authorsDir[] = "data/latin_authors";

  if (!createLatinWordsTable())
    return 0;

  if (!createAuthorTables(lang))
    return 0;

  if (!loadAuthorTables(lang, authorsDir))
    return 0;

  if (!createRootsTable(lang))
    return 0;

  if (!createMiscTables(lang))
    return 0;

  if (!postLatinWordsTable())
    return 0;

  return 1;
}

int
makeOther()
{
  if (!createOtherTables())
    return 0;

  if (!makeOtherTables())
    return 0;

  return 1;
}

int
main (int argc, char **argv)
{
  conn = do_connect (def_host_name, def_user_name, def_password, def_db_name,
                     def_port_num, def_socket_name, 0);
  if (conn == NULL)
    exit (1);

  if (!makeGreek() || !makeLatin() || !makeOther())
    exit (1);

  do_disconnect(conn);

  exit (0);
}
