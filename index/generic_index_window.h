int
create_index_add();

gboolean
sigUpdateTerm(GtkWidget *button, indexForm *index);

gboolean
sigRemoveUseGroup(GtkWidget *button, indexForm *index);

gboolean
sigDeleteTerm(GtkWidget *button, indexForm *index);

gboolean
sigRemoveRelated(GtkWidget *button, indexForm *index);

GtkTreeView *
create_related_list(char *colName);

int
sigInsertTerm(GtkWidget *button, indexForm *index);

GtkTreeView *
create_use_list(char *colName);

indexForm *
create_index_window(int index_id);
