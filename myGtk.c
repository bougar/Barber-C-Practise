#include <gtk/gtk.h>
/*Esquema para interfaz gráfica problema
del barbero. Bougar's license*/
void my_fun(GtkWidget * button, gpointer data){
	gtk_main_quit();
}
GtkWidget * crear_menubar(){
	GtkWidget * menu;
	GtkWidget * menubar;
	GtkWidget * file;
	GtkWidget * quit;
	GtkWidget * box;
	
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	menubar = gtk_menu_bar_new();
	menu = gtk_menu_new();
	
	file = gtk_menu_item_new_with_label("File");
	quit = gtk_menu_item_new_with_label("Quit");
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file),menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),quit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),file);
	gtk_box_pack_start(GTK_BOX(box),menubar,FALSE,FALSE,3);
	
	return box;
}
int main (int argc , char ** argv){
	/*Se inicializan todas la interfaces graficas
	sin interesarse por las funciones que van a 
	realizar*/
	GtkWidget * window;
	GtkWidget * text[3];
	GtkWidget * grid;
	GtkWidget * label;
	GtkWidget * button;
	GtkWidget * box;
	GtkWidget * vbox;
	
	gtk_init(&argc,&argv);
	
	/*Creación de la ventana*/
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	gtk_widget_set_size_request(window,200,100);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	
	/*Se ira creando un label con su correspondiente texto,
	Una caja de texto para introducir un parametro y finalmente
	se introducirá en en una fila de una cuadrícula.
	Fijarse en que sigue un esquema*/
	grid = gtk_grid_new();
	label = gtk_label_new("Barberos: ");
	text[0] = gtk_entry_new();
	gtk_entry_set_width_chars (GTK_ENTRY(text[0]),2);
	gtk_grid_attach(GTK_GRID(grid),label,0,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),text[0],2,0,1,1);
	
	label = gtk_label_new("Personas: ");
	text[1] = gtk_entry_new();
	gtk_entry_set_width_chars (GTK_ENTRY(text[1]),2);
	gtk_grid_attach(GTK_GRID(grid),label,0,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),text[1],2,1,1,1);
	
	label = gtk_label_new("Sillas: ");
	text[2] = gtk_entry_new();
	gtk_entry_set_width_chars (GTK_ENTRY(text[2]),2);
	gtk_grid_attach(GTK_GRID(grid),label,0,2,1,1);
	gtk_grid_attach(GTK_GRID(grid),text[2],2,2,1,1);
	
	/*Creación de botón y lo metemos en la cuadrícula al final*/
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
	button = gtk_button_new_with_label("¡Ejecutar!");
	gtk_grid_attach(GTK_GRID(grid),button,0,3,2,2);
	gtk_grid_remove_column(GTK_GRID(grid),3);
	gtk_box_pack_start(GTK_BOX(box),grid,TRUE,FALSE,1);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_pack_start(GTK_BOX(vbox),crear_menubar(),FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),box,TRUE,FALSE,1);
	
	/*Últimos pasos*/
	gtk_container_add(GTK_CONTAINER(window),vbox);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(my_fun),text);
	gtk_widget_show_all(window);
	gtk_main();
}
