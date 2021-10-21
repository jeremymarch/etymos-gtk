MYSQL *conn;

query = "SELECT id, word "
        "FROM palatinoall "
        "WHERE id = ?";

struct {
MYSQL_STMT *stmt,
MYSQL_BIND *bind
} *queryStruct;

int
prepareSelectQuery(MYSQL *conn, const char *query)
{
  MYSQL_STMT *stmt;
  MYSQL_RES  *res;
  int         column_count;


  if ((stmt = mysql_stmt_init(conn)) == NULL)
  {
    return FALSE; /* out of memory */
  }

  if (mysql_stmt_prepare(stmt, query, strlen(queryLen)) != 0)
  {
    printf("Error: %s\n", mysql_stmt_error(stmt));
    return FALSE;
  }

  if ((res = mysql_stmt_result_metadata(stmt)) == NULL)  /* if return result set */
  {
    printf("Error: %s\n", mysql_stmt_error(stmt));
    return FALSE;
  }

  column_count= mysql_num_fields(prepare_meta_result); /* we should probably check that this count is what is intended */



  mysql_stmt_bind_param()

}
executeSelectQuery(*queryStruct)
{
  mysql_stmt_execute(queryStruct->stmt)

  mysql_stmt_bind_result();

  mysql_stmt_fetch();

}



}

