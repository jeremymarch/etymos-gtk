#include <stdio.h>
#include <mysql.h>
#include "libImportDict.h"

#define MAX_WARNINGS 50

static char queryBuf[QUERY_BUF_SIZE];

int
printQueryError(char *queryBuf)
{
  fprintf(stdout, "Query: %s\nError: %s\n", queryBuf, mysql_error(conn));
}

int
printQueryWarnings()
{
  MYSQL_RES *res_set;
  MYSQL_ROW row;

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "SHOW WARNINGS LIMIT %i", MAX_WARNINGS);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }
  res_set = mysql_store_result(conn);
  while ((row = mysql_fetch_row(res_set)) != NULL)
  {
    fprintf(stdout, "warning: %s\n", row[2]);
  }
  mysql_free_result(res_set);
  return 1;
}

int
createGreekWordsTable()
{
/*
DROP TABLE IF EXISTS greek_words 
CREATE greek_words
LOAD DATA INFILE into greek_words
DELETE the duplicate record from greek_words
*/

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "DROP TABLE IF EXISTS greek_words");

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

/**
 * note that word and def are utf8_bin to speed up import of author tables
 * this is changed to utf8_unicode_ci in post-import function
 */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "CREATE TABLE `greek_words` ("
    "`word_id` mediumint(8) unsigned NOT NULL auto_increment, "
    "`word` varchar(255) collate utf8_bin NOT NULL default '', "
    "`def` varchar(255) collate utf8_bin NOT NULL default '', "
    "`spionicword` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`partid` tinyint(3) unsigned NOT NULL default '0', "
    "`index_id` smallint(5) unsigned NOT NULL default '0', "
    "`root` smallint(5) unsigned NOT NULL default '0', "
    "`status` tinyint(4) NOT NULL default '0', "
    "`note` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`root_word` tinyint(4) NOT NULL default '0', "
    "`alpha` varchar(255) collate utf8_unicode_ci default NULL, "
    "`lucidaword` varchar(255) collate utf8_unicode_ci default NULL, "
    "`present` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`future` varchar(255) collate utf8_unicode_ci default NULL, "
    "`aorist` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`perfect` varchar(255) collate utf8_unicode_ci default NULL, "
    "`perfmid` varchar(255) collate utf8_unicode_ci default NULL, "
    "`aoristpass` varchar(255) collate utf8_unicode_ci default NULL, "
    "`compound` tinyint(4) NOT NULL default '0', "
    "`verbclass` tinyint(3) unsigned NOT NULL default '0', "
    "`gender` tinyint(3) unsigned NOT NULL default '0', "
    "`beta` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`spaces` tinyint(3) unsigned NOT NULL default '0', "
    "`level` tinyint(3) unsigned NOT NULL default '0', "
    "`parent` smallint(5) unsigned NOT NULL default '0', "
    "PRIMARY KEY  (`word_id`), "
    "KEY `parent_ind` (`parent`,`root`), "
    "KEY `root_ind` (`root`), "
    "KEY `word` (`word`) "
    ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }
  return 1;
}

int
loadGreekWordsTable(char *datafile)
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "LOAD DATA LOCAL INFILE '%s' " 
    "INTO TABLE greek_words "
    "FIELDS TERMINATED BY '\\t' OPTIONALLY ENCLOSED BY '\"' "
    "LINES TERMINATED BY '\\r\\n' IGNORE 1 LINES",
     datafile);

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  if (mysql_warning_count(conn) > 0)
  {
    printQueryWarnings();
  }
  return 1;
}

int
createLatinWordsTable()
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "DROP TABLE IF EXISTS latin_words");

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
          "CREATE TABLE `latin_words` ("
          "`word_id` smallint(5) unsigned NOT NULL auto_increment, "
          "`word` varchar(255) collate utf8_bin NOT NULL default '', "
          "`def` varchar(255) collate utf8_bin NOT NULL default '', "
          "`parent` smallint(5) unsigned NOT NULL default '0', "
          "`root` smallint(5) unsigned NOT NULL default '0', "
          "`partid` tinyint(3) unsigned NOT NULL default '0', "
          "`index_id` smallint(5) unsigned NOT NULL default '0', "
          "`status` tinyint(4) NOT NULL default '0', "
          "`note` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
          "`compound` tinyint(4) NOT NULL default '0', "
          "`gender` tinyint(3) unsigned NOT NULL default '0', "
          "`updated` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, "
          "`created` timestamp NOT NULL default '0000-00-00 00:00:00', "
          "PRIMARY KEY  (`word_id`), "
          "KEY `word` (`word`) "
          ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }
  return 1;
}

int
createRootsTable(char *lang)
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "DROP TABLE IF EXISTS %s_roots", lang);

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "CREATE TABLE `%s_roots` ( "
           "`root_id` smallint(5) unsigned NOT NULL auto_increment, "
           "`root` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
           "`ieroot` varchar(255) collate utf8_unicode_ci default NULL, "
           "`oldroot_id` smallint(5) unsigned NOT NULL default '0', "
           "`alpha` varchar(255) collate utf8_unicode_ci default NULL, "
           "`def` varchar(255) collate utf8_unicode_ci default NULL, "
           "`note` varchar(255) collate utf8_unicode_ci default NULL, "
           "`phonetic` varchar(255) collate utf8_unicode_ci default NULL, "
           "`updated` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, "
           "`created` timestamp NOT NULL default '0000-00-00 00:00:00', "
           "PRIMARY KEY  (`root_id`), "
           "KEY `root` (`root`) "
           ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci",
            lang);

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }
  return 1;
}

int
loadGreekRootsTable(char *rootsDataFile)
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
          "LOAD DATA LOCAL INFILE '%s' "
          "INTO TABLE greek_roots "
          "FIELDS TERMINATED BY '\\t' OPTIONALLY ENCLOSED BY '\"' " 
          "LINES TERMINATED BY '\\r\\n' "
          "IGNORE 1 LINES "
          "(root_id, root, ieroot, oldroot_id, alpha, def, note, phonetic)",
           rootsDataFile);

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  if (mysql_warning_count(conn) > 0)
  {
    printQueryWarnings();
  }
  return 1;
}

int
createMiscTables(char *lang)
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "DROP TABLE IF EXISTS %s_index_x_words, %s_synonyms, %s_antonyms", 
     lang, lang, lang);

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
          "CREATE TABLE `%s_index_x_words` ("
          "`word_id` mediumint(8) unsigned NOT NULL default '0', "
          "`index_id` smallint(5) unsigned NOT NULL default '0', "
          "PRIMARY KEY  (`word_id`,`index_id`) "
          ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci",
           lang);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
          "CREATE TABLE `%s_synonyms` ("
          "`word_id` smallint(5) unsigned NOT NULL default '0', "
          "`synonym_id` smallint(5) unsigned NOT NULL default '0', "
          "PRIMARY KEY  (`word_id`,`synonym_id`) "
          ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci",
           lang);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
          "CREATE TABLE `%s_antonyms` ("
          "`word_id` smallint(5) unsigned NOT NULL default '0', "
          "`antonym_id` smallint(5) unsigned NOT NULL default '0', "
          "PRIMARY KEY  (`word_id`,`antonym_id`) "
          ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci",
           lang);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  return 1;
}

int
loadGreekSynonyms(char *synDataFile)
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
          "LOAD DATA LOCAL INFILE '%s' "
          "INTO TABLE greek_synonyms "
          "FIELDS TERMINATED BY '\\t' OPTIONALLY ENCLOSED BY '\"' " 
          "LINES TERMINATED BY '\\r\\n' "
          "IGNORE 1 LINES ",
           synDataFile);

  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  if (mysql_warning_count(conn) > 0)
  {
    printQueryWarnings();
  }
  return 1;
}

int
preGreekWordsTable()
{
/* remove duplicate from greek_words--the other is 9963 */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "DELETE FROM greek_words WHERE word_id = 10032");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }
  if (mysql_affected_rows(conn) != 1)
    return 0;

  return 1;
}

int
postGreekWordsTable()
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "ALTER TABLE greek_words "
    "CHANGE def perseus_def varchar(255) "
      "COLLATE utf8_unicode_ci NOT NULL default '', "
    "MODIFY word varchar(255) "
      "COLLATE utf8_unicode_ci NOT NULL default ''");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "ALTER TABLE greek_words "
    "ADD COLUMN def varchar(255) "
    "COLLATE utf8_unicode_ci NOT NULL default '' "
    "AFTER perseus_def");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

/* change index_id from one-to-many to many-to-many */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "INSERT INTO greek_index_x_words "
    "SELECT word_id, index_id FROM greek_words");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "ALTER TABLE greek_words "
    "DROP COLUMN index_id");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "UPDATE greek_words SET status = 2 WHERE status = -1");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "UPDATE greek_words SET status = 1 WHERE status = 0");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  return 1;
}

int
postLatinWordsTable()
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "ALTER TABLE latin_words "
    "MODIFY word varchar(255) NOT NULL COLLATE utf8_roman_ci default '', "
    "MODIFY def varchar(255) NOT NULL COLLATE utf8_roman_ci default ''");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  return 1;
}

int
getNumRowsGreekWordsTable()
{
  MYSQL_RES *res_set;
  MYSQL_ROW row;
  int num;

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "SELECT COUNT(*) FROM greek_words");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }
  res_set = mysql_store_result(conn);
  if ((row = mysql_fetch_row(res_set)) == NULL)
    return -1;
 
  num = atoi(row[0]);
  mysql_free_result(res_set);

  return num;
}

/**
 * author         -- author name to be inserted in lang_authors table 
 * authorDataFile -- file to import
 * lang           -- language
 *
 * returns number of rows added to lang_word table or -1 on error
 */
int
addAuthorWords(char *author, char *authorDataFile, char *lang)
{
/*
DROP TABLE IF EXISTS
CREATE temporary authors_word TABLE
LOAD DATA INFILE for data file
SHOW WARNINGS LOAD DATA query
INSERT unique rows into lang_word table
SHOW WARNINGS LOAD DATA query
INSERT author name into lang_author table
INSERT author-word ids into author_word join table
check that unique rows in author table == rows inserted into join table
DROP author_word table
*/

  MYSQL_RES *res_set;
  MYSQL_ROW  row;
  int langWordTableRowsAdded, wordAuthorJoinRowsAdded, authorNumRows;

  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "DROP TABLE IF EXISTS %s",
           author);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }

/* create authors_word table */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "CREATE TABLE %s "
           "(id int UNSIGNED NOT NULL primary key auto_increment, "
           "word VARCHAR(255) NOT NULL, "
           "def VARCHAR(255) NOT NULL, "
           "key (word)) "
           "CHARACTER SET utf8 " 
           "COLLATE utf8_bin",
            author);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }

/* load data into authors_word table */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "LOAD DATA LOCAL INFILE '%s' "
           "INTO TABLE %s "
           "FIELDS "
             "TERMINATED BY '\\t' "
             "OPTIONALLY ENCLOSED BY '\"' "
           "LINES "
             "TERMINATED BY '\\r\\n' "
             "IGNORE 1 LINES "
           "(word, def)",
           authorDataFile, author);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }
  if (mysql_warning_count(conn) > 0)
  {
    printQueryWarnings();
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
          "SELECT COUNT(DISTINCT word,def) FROM %s", author);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }
  res_set = mysql_store_result(conn);
  if ((row = mysql_fetch_row(res_set)) == NULL)
  {
    mysql_free_result(res_set);
    return -1;
  }

  authorNumRows = atoi(row[0]);
  mysql_free_result(res_set);

/** 
 * no need to test for duplicate rows 
 * because we use DISTINCT in SELECT below
 */

/* insert unique author words into lang_words */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "INSERT INTO %s_words (word, def) "
           "SELECT DISTINCT %s.word, %s.def "
           "FROM %s "
           "LEFT JOIN %s_words " 
           "ON %s_words.word = %s.word COLLATE utf8_bin "
           "AND %s_words.def = %s.def COLLATE utf8_bin "
           "WHERE %s_words.word_id IS NULL",
            lang, author, author, author, lang, lang, author, lang, author, lang);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }
  if (mysql_warning_count(conn) > 0)
  {
    printQueryWarnings();
  }
  langWordTableRowsAdded = mysql_affected_rows(conn);


/* insert author name into lang_authors table */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "INSERT INTO %s_authors (author_id, author) "
           "VALUES (NULL, '%s')",
            lang, author);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }

  int last_id;
  last_id = mysql_insert_id(conn);

/* insert values into author word join table */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "INSERT INTO %s_authors_x_words (word_id, author_id) "
           "SELECT DISTINCT %s_words.word_id, %u "
           "FROM %s_words INNER JOIN %s "
           "ON %s_words.word = %s.word COLLATE utf8_bin "
           "AND %s_words.def = %s.def COLLATE utf8_bin", 
            lang, lang, last_id, lang, author, lang, author, lang, author);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }
  if (mysql_warning_count(conn) > 0)
  {
    printQueryWarnings();
  }
  wordAuthorJoinRowsAdded = mysql_affected_rows(conn);

/* DROP author table */
  snprintf(queryBuf, QUERY_BUF_SIZE, "DROP TABLE %s", author);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return -1;
  }

/* make sure rows added to word-author join table equals total rows in author table */
  if (wordAuthorJoinRowsAdded != authorNumRows)
  {
    fprintf(stdout, "ERROR: rows added to %s_authors_x_words (%i) "
                    "does not equal rows in author table (%i)\n",
                    lang, wordAuthorJoinRowsAdded, authorNumRows);
    return -1;
  }

  return langWordTableRowsAdded;
}

int
createAuthorTables(char *lang)
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "DROP TABLE IF EXISTS %s_authors, %s_authors_x_words",
           lang, lang);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "CREATE TABLE %s_authors ("
           "`author_id` smallint(5) unsigned NOT NULL auto_increment, "
           "`author` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
           "PRIMARY KEY  (`author_id`) "
           ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci",
           lang);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "CREATE TABLE %s_authors_x_words ("
           "`word_id` mediumint(8) unsigned NOT NULL default '0', "
           "`author_id` smallint(5) unsigned NOT NULL default '0', "
           "PRIMARY KEY  (`word_id`,`author_id`), "
           "INDEX (`author_id`) "
           ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci",
           lang);
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }
  return 1;
}

int
createOtherTables()
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
           "DROP TABLE IF EXISTS gender, index_terms, part_speech"
           "status, languages, greek_verbclass");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
           " CREATE TABLE `gender` ("
           "`id` tinyint(3) unsigned NOT NULL auto_increment, "
           "`gender` char(15) collate utf8_unicode_ci NOT NULL default '', "
           "PRIMARY KEY  (`id`) "
           ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "CREATE TABLE `index_terms` ("
    "`index_id` smallint(5) unsigned NOT NULL auto_increment, "
    "`term` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`parent` smallint(6) default NULL, "
    "`has_children` tinyint(3) unsigned NOT NULL default '0', "
    "`scope_note` text collate utf8_unicode_ci NOT NULL, "
    "`use_for` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`use_` smallint(5) unsigned NOT NULL default '0', "
    "`see_also` varchar(255) collate utf8_unicode_ci NOT NULL default '', "
    "`display` tinyint(1) NOT NULL default '1', "
    "`lft` smallint(5) unsigned NOT NULL default '0', "
    "`rgt` smallint(5) unsigned NOT NULL default '0', "
    "`updated` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, "
    "`updated_by` varchar(25) collate utf8_unicode_ci NOT NULL default '', "
    "`created` timestamp NOT NULL default '0000-00-00 00:00:00', "
    "`created_by` varchar(25) collate utf8_unicode_ci NOT NULL default '', "
    "PRIMARY KEY  (`index_id`), "
    "KEY `lft_rgt` (`lft`,`rgt`), "
    "KEY `idx_parent` (`parent`) "
    ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "CREATE TABLE `part_speech` ("
    "`pid` tinyint(3) unsigned NOT NULL default '0', "
    "`part` char(20) collate utf8_unicode_ci NOT NULL default '', "
    "PRIMARY KEY  (`pid`) "
    ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "CREATE TABLE `status` ("
    "`status_id` tinyint(3) unsigned NOT NULL auto_increment, "
    "`status` char(25) collate utf8_unicode_ci NOT NULL default '', "
    "PRIMARY KEY  (`status_id`) "
    ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "CREATE TABLE `languages` ("
    "`lang_id` tinyint(3) unsigned NOT NULL auto_increment, "
    "`language` char(20) collate utf8_unicode_ci NOT NULL default '', "
    "PRIMARY KEY  (`lang_id`) "
    ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

/* should I move this to greek specific section ? */
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "CREATE TABLE `greek_verbclass` ("
    "`id` tinyint(3) unsigned NOT NULL auto_increment, "
    "`verbclass` varchar(255) default NULL, "
    "`letters` varchar(255) default NULL, "
    "`note` varchar(255) default NULL, "
    "PRIMARY KEY  (`id`) "
    ") ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }
  return 1;
}

int
makeOtherTables()
{
  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "INSERT INTO gender VALUES "
    "(1,'Feminine'),(2,'Masculine'),(3,'Neuter'),(4,'Both'),(5,'None')");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "INSERT INTO part_speech VALUES "
    "(1,'Noun'),(2,'Verb'),(3,'Adjective'),(4,'Adverb'),(5,'Particle')"
    "(6,'Proper Noun'),(7,'Pronoun'),(8,'Conjunction'),(9,'Interjection'),"
    "(10,'Preposition'),(0,'None')");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "INSERT INTO languages VALUES "
    "(1,'greek'),(2,'latin')");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "INSERT INTO status VALUES "
    "(1,'valid'),(2,'invalid'),(3,'questionable'),"
    "(4,'declined form'),(5,'conjugated form')");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  snprintf(queryBuf, QUERY_BUF_SIZE, 
    "INSERT INTO greek_verbclass VALUES "
    "(1,'First--Simple Class'),(2,'Second--T Class'),(3,'Third-Iota Class'),"
    "(4,'Fourth--N Class'),(5,'Fifth--Inceptive Class'),(6,'Sixth--Mixed Class')");
  if (mysql_query(conn, queryBuf) != 0)
  {
    printQueryError(queryBuf);
    return 0;
  }

  return 1;
}

/**************************************/

void
print_error (MYSQL *conn, char *message)
{
  fprintf (stderr, "%s\n", message);
  if (conn != NULL)
  {
    fprintf (stderr, "Error %u (%s)\n",
             mysql_errno (conn), mysql_error (conn));
  }
}

MYSQL *
do_connect (char *host_name, char *user_name, char *password, char *db_name,
      unsigned int port_num, char *socket_name, unsigned int flags)
{
  MYSQL *conn;

  conn = mysql_init (NULL);
  if (conn == NULL)
  {
    print_error (NULL, "mysql_init() failed (probably out of memory)");
    return (NULL);
  }
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 32200 /* 3.22 and up */
  if (mysql_real_connect (conn, host_name, user_name, password,
          db_name, port_num, socket_name, flags) == NULL)
  {
    print_error (conn, "mysql_real_connect() failed");
    return (NULL);
  }
#else              /* pre-3.22 */
  if (mysql_real_connect (conn, host_name, user_name, password,
          port_num, socket_name, flags) == NULL)
  {
    print_error (conn, "mysql_real_connect() failed");
    return (NULL);
  }
  if (db_name != NULL)    /* simulate effect of db_name parameter */
  {
    if (mysql_select_db (conn, db_name) != 0)
    {
      print_error (conn, "mysql_select_db() failed");
      mysql_close (conn);
      return (NULL);
    }
  }
#endif
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 4100 /* 4.1.0 and up */
if (mysql_query(conn, "SET CHARACTER SET utf8") != 0)
  return (NULL);
#endif
  return (conn);     /* connection is established */
}

void
do_disconnect (MYSQL *conn)
{
  mysql_close (conn);
}
