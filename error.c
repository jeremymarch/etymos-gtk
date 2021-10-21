#include <gtk/gtk.h>
#include "error.h"

void
printStatusMesg(GtkStatusbar *statusBar, gchar *mesg)
{
  guint context_id;

  context_id = gtk_statusbar_get_context_id( GTK_STATUSBAR(statusBar), "status example" );
  gtk_statusbar_push( GTK_STATUSBAR(statusBar), context_id, mesg );
}
