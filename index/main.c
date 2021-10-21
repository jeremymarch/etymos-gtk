int
main (int argc, char **argv)
{
  gtk_init(&argc, &argv);

  conn = do_connect (def_host_name, def_user_name, def_password, def_db_name,
                     def_port_num, def_socket_name, 0);
  if (conn == NULL)
    exit (1);

  if (create_index_window(1) != NULL)
  {
    gtk_main();
  }

  do_disconnect(conn);

  exit(0);
}
