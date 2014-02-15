/*
 * gvalgrind : représentation graphique d'un rapport en format XML de valgrind
 * Copyright (C) 2011
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>

#include "config.h"
#include "erreurs.h"
#include "texte.h"
#include "struct.h"
#include "fonctions.h"
#include "options.h"

/* gtk_window_destroy
 * Description : Action effectuée lors de la fermeture de la fenêtre principale
 * Paramètres : Aucun paramètre n'est utilisé
 * Valeur renvoyée : void
 */
void gtk_window_destroy(GtkWidget *pWidget __attribute__((unused)), Projet *projet)
{
	if (projet->programme.enregistrement != NULL)
		free(projet->programme.enregistrement);
	if (projet->programme.environnement != NULL)
		free(projet->programme.environnement);
	if (projet->programme.nom_fichier != NULL)
		free(projet->programme.nom_fichier);
	if (projet->programme.arguments != NULL)
		free(projet->programme.arguments);
	if (projet->programme.dossier_courant != NULL)
		free(projet->programme.dossier_courant);
	xmlFreeDoc(projet->document);
	gtk_main_quit();
	return;
}

/* main_ouvrir
 * Description : Ouvre une fenêtre de sélection et ouvre le log valgrind demandé
 * Paramètres : GtkMenuItem *menuitem : composant GtkMenuItem à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void main_ouvrir(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
{
	GtkWidget	*pFileSelection;
	gchar		*sChemin;
	
	pFileSelection = gtk_file_chooser_dialog_new(gettext("Ouvrir..."), NULL, GTK_FILE_CHOOSER_ACTION_OPEN, gettext("_Annuler"), GTK_RESPONSE_CANCEL, gettext("_Ouvrir"), GTK_RESPONSE_OK, NULL);
	gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);
	
	switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
	{
		case GTK_RESPONSE_OK:
		{
			// Recuperation du chemin
			sChemin = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(pFileSelection));
			ouvrir_projet(sChemin, projet);
			g_free(sChemin);
			break;
		}
		default:
		{
			break;
		}
	}
	gtk_widget_destroy(pFileSelection);
	
	return;
}

/* main_enregistrer
 * Description : Ouvre une fenêtre de sélection et enregistre le log valgrind demandé
 * Paramètres : GtkMenuItem *menuitem : composant GtkMenuItem à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void main_enregistrer(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
{
	if (projet->programme.enregistrement != NULL)
		enregistrer_projet(projet->programme.enregistrement, projet);
	
	return;
}

/* main_enregistrer_sous
 * Description : Ouvre une fenêtre de sélection et enregistre le log valgrind demandé
 * Paramètres : GtkMenuItem *menuitem : composant GtkMenuItem à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void main_enregistrer_sous(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
{
	GtkWidget	*pFileSelection;
	gchar		*sChemin;
	
	pFileSelection = gtk_file_chooser_dialog_new(gettext("Enregistrer..."), NULL, GTK_FILE_CHOOSER_ACTION_SAVE, gettext("_Annuler"), GTK_RESPONSE_CANCEL, gettext("_Enregistrer"), GTK_RESPONSE_OK, NULL);
	gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);
	
	switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
	{
		case GTK_RESPONSE_OK:
		{
			// Recuperation du chemin
			sChemin = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(pFileSelection));
			enregistrer_projet(sChemin, projet);
			g_free(sChemin);
			break;
		}
		default:
		{
			break;
		}
	}
	gtk_widget_destroy(pFileSelection);
	
	return;
}

/* tree_view_row_expanded
 * Description : Evènement lorsqu'on ouvre une ligne contenant un enfant
 *             : Est utilisé pour ouvrir automatique les lignes jusqu'à ce que la ligne en cours
 *               ne possède pas d'enfant ou bien au minimum deux enfants.
 * Paramètres : GtkTreeView *tree_view : composant tree_view à l'origine de l'évènement
 *            : GtkTreeIter *iter : ligne s'"ouvrant"
 *            : GtkTreePath *path : chemin vers la ligne
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void tree_view_row_expanded(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer data __attribute__((unused)))
{
	GtkTreeModel	*model = gtk_tree_view_get_model(tree_view);
	
	if (gtk_tree_model_iter_n_children(model, iter) == 1)
	{
		gtk_tree_path_down(path);
		gtk_tree_view_expand_row(tree_view, path, FALSE);
	}
	
	return;
}

/* gtk_window_demande_confirmation
 * Description : Demande une confirmation pour quitter l'application si des modifications
 *             : non enregistrées ont été effectuées dans le projet en cours
 * Paramètres : GtkWidget *widget : composant Widget
 *            : GdkEvent *event : nature de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée :
 *   Quitter : FALSE
 *   Ne pas quitter : TRUE
 */
gboolean gtk_window_demande_confirmation(GtkWidget *widget __attribute__((unused)), GdkEvent *event __attribute__((unused)), Projet *projet)
{
	GtkWidget	*dialog = NULL;
	GtkResponseType	response = GTK_RESPONSE_CANCEL;
	if (projet->modifie != 0)
	{
		dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, gettext("Des modifications ont été apportées au projet.\nQue souhaitez-vous faire ?"));
		gtk_dialog_add_buttons(GTK_DIALOG(dialog), gettext("Quitter"), GTK_RESPONSE_CLOSE, gettext("Enregistrer"), GTK_RESPONSE_ACCEPT, gettext("Annuler"), GTK_RESPONSE_CANCEL, NULL);
		gtk_window_set_title(GTK_WINDOW(dialog), gettext("Quitter"));
		response = gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		switch (response)
		{
			// On annule la fermeture de l'application
			case GTK_RESPONSE_CANCEL : 
			{
				return TRUE;
				break;
			}
			// On ferme l'application en enregistrant les modifications
			case GTK_RESPONSE_ACCEPT :
			{
				if (projet->programme.enregistrement != NULL)
					main_enregistrer(NULL, projet);
				else
					main_enregistrer_sous(NULL, projet);
				return FALSE;
				break;
			}
			// On ferme l'application sans enregistrer les modifications
			case GTK_RESPONSE_CLOSE :
			{
				return FALSE;
				break;
			}
			// On annule la fermeture de l'application
			default :
			{
				return TRUE;
				break;
			}
		}
	}
	else
		return FALSE;
}

/* main
 * Description : Fonction principale de l'application
 * Paramètres : int argc : nom d'arguments
 *            : char *argv[] : liste des arguments
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int main(int argc, char *argv[])
{
	// Variables pour l'interface graphique
	GtkWidget	        *mainwindow = NULL;
	GtkCellRenderer	    *pCellRenderer;
	GtkTreeViewColumn   *pColumn;
	GtkWidget	        *pGrid;
	GtkWidget	        *pScrollbar;
	GtkWidget	        *pMenuBar;
	GtkWidget	        *pMenu;
	GtkWidget	        *pMenuItem;
	Projet		        projet;
	
	// On initialise l'interface graphique
	if (gtk_init_check(&argc, &argv) == FALSE)
		BUGTEXTE(-1, gettext("Impossible d'initialiser gtk\n"));
	
	// On crée la fenêtre graphique
	mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(mainwindow), 600, 400);
	g_signal_connect(GTK_WINDOW(mainwindow), "destroy", G_CALLBACK(gtk_window_destroy), &projet);
	g_signal_connect(GTK_WINDOW(mainwindow), "delete-event", G_CALLBACK(gtk_window_demande_confirmation), &projet);
	// Le tree_view
	projet.tree_store = gtk_tree_store_new(1, G_TYPE_STRING);
	projet.tree_view = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(projet.tree_store));
	g_signal_connect(G_OBJECT(projet.tree_view), "row-expanded", G_CALLBACK(tree_view_row_expanded), NULL);
	
	init_projet(&projet);
	
	// On vérifie qu'un nom de fichier est passé en argument
	if ((argc == 2) && (file_exists(argv[1]) == 0))
	{
		if (ouvrir_projet(argv[1], &projet) != 0)
			BUG(-2);
	}
	
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes(gettext("Texte"), pCellRenderer, "markup", 0, NULL);
	gtk_tree_view_append_column(projet.tree_view, pColumn);
    gtk_widget_set_hexpand(GTK_WIDGET(projet.tree_view), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(projet.tree_view), TRUE);
	
	// Le menu
	pMenuBar = gtk_menu_bar_new();
	pMenu = gtk_menu_new();
	pMenuItem = gtk_menu_item_new_with_label(gettext("Nouveau"));
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(main_nouveau), &projet);
	pMenuItem = gtk_menu_item_new_with_label(gettext("Ouvrir projet valgrind..."));
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(main_ouvrir), &projet);
	pMenuItem = gtk_menu_item_new_with_label(gettext("Enregistrer projet valgrind..."));
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(main_enregistrer), &projet);
	pMenuItem = gtk_menu_item_new_with_label(gettext("Enregistrer sous projet valgrind..."));
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(main_enregistrer_sous), &projet);
	pMenuItem = gtk_menu_item_new_with_label(gettext("Quitter"));
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(gtk_main_quit), &projet);
	pMenuItem = gtk_menu_item_new_with_label(gettext("Fichier"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);
	
	pMenu = gtk_menu_new();
	pMenuItem = gtk_menu_item_new_with_label(gettext("Options..."));
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(main_options), &projet);
	pMenuItem = gtk_menu_item_new_with_label(gettext("Exécuter valgrind"));
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(main_valgrind), &projet);
	pMenuItem = gtk_menu_item_new_with_label(gettext("Projet"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);
	
	pGrid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(pGrid), pMenuBar, 0, 0, 1, 1);
	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
    gtk_grid_attach(GTK_GRID(pGrid), pScrollbar, 0, 1, 1, 1);
	gtk_container_add(GTK_CONTAINER(mainwindow), pGrid);
    gtk_widget_set_hexpand(pGrid, TRUE);
	gtk_container_add(GTK_CONTAINER(pScrollbar), GTK_WIDGET(projet.tree_view));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_show_all(mainwindow);
	gtk_main();
	
	return 0;
}
