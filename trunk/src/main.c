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

/* gtk_window_destroy
 * Description : Action effectuée lors de la fermeture de la fenêtre principale
 * Paramètres : Aucun paramètre n'est utilisé
 * Valeur renvoyée : void
 */
void gtk_window_destroy(GtkWidget *pWidget __attribute__((unused)), GdkEvent *event __attribute__((unused)), gpointer data __attribute__((unused)))
{
	gtk_main_quit();
	return;
}

/* ajout_erreur_recursif
 * Description : Ajoute dans un Widget de type TreeStore, à la ligne iter_racine, les éléments contenus dans le noeud XML racine
 * Paramètres : xmlNodePtr racine : Noeud XML contenant les informations à ajouter
 *            : GtkTreeStore *tree_store : Emplacement où stocker les données pour un composant de type TreeView
 *            : GtkTreeIter *iter_racine : ligne parent de tree_store où stocker les données
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int ajout_erreur_recursif(xmlNodePtr racine, GtkTreeStore *tree_store, GtkTreeIter *iter_racine)
{
	GtkTreeIter	iter_parent, iter;
	xmlNodePtr	n0;
	
	// Vérification des paramètres
	if ((racine == NULL) || (tree_store == NULL) || (iter_racine == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// On parcours l'ensemble des enfants du noeud racine à la recherche de la propriété fichier
	for (n0 = racine->children; n0 != NULL; n0 = n0->next)
	{
		xmlChar		*contenu_fichier = xmlGetProp(n0, BAD_CAST "nom");
		xmlNodePtr	n1;
		
		// l'attribut fichier ne peut être absent puisque c'est le présent code qui a créer l'aborescence
		// Si l'attribut file est initialement manquant, il est stocké en ""
		if (contenu_fichier == NULL)
			BUGTEXTE(-2, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "nom");
		// On ajoute la ligne
		gtk_tree_store_append(tree_store, &iter_parent, iter_racine);
		gtk_tree_store_set(tree_store, &iter_parent, 0, (char *)contenu_fichier, -1);
		xmlFree(contenu_fichier);
		
		// On parcours l'ensemble des enfants du noeud racine à la recherche de la propriété ligne et fonction 
		for (n1 = n0->children; n1 != NULL; n1 = n1->next)
		{
			xmlChar		*contenu_ligne = xmlGetProp(n1, BAD_CAST "ligne");
			xmlChar		*contenu_fn = xmlGetProp(n1, BAD_CAST "fn");
			char		*contenu_cat;
			
			if (contenu_ligne == NULL)
				BUGTEXTE(-3, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "ligne");
			if (contenu_fn == NULL)
				BUGTEXTE(-4, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "fonction");
			gtk_tree_store_append(tree_store, &iter, &iter_parent);
			contenu_cat = malloc(sizeof(char)*(strlen((char *)contenu_ligne)+strlen((char *)contenu_fn)+strlen(" ()")+2));
			strcpy(contenu_cat, (char *)contenu_fn);
			strcat(contenu_cat, " (");
			strcat(contenu_cat, (char *)contenu_ligne);
			strcat(contenu_cat, ")");
			gtk_tree_store_set(tree_store, &iter, 0, contenu_cat, -1);
			xmlFree(contenu_ligne);
			xmlFree(contenu_fn);
			if (ajout_erreur_recursif(n1, tree_store, &iter) != 0)
				BUG(-5);
		}
	}
	return 0;
}

/* ajout_erreur
 * Description : Ajoute dans un Widget de type TreeStore, dans la racine de l'arborescence, les éléments contenus dans le noeud XML racine
 * Paramètres : xmlNodePtr racine : Noeud XML contenant les informations à ajouter
 *            : GtkTreeStore *tree_store : Emplacement où stocker les données pour un composant de type TreeView
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int ajout_erreur(xmlNodePtr racine, GtkTreeStore *tree_store)
{
	GtkTreeIter	iter;
	xmlNodePtr	n0;
	
	gtk_tree_store_clear(tree_store);
	for (n0 = racine->children; n0 != NULL; n0 = n0->next)
	{
		xmlChar		*contenu = xmlGetProp(n0, BAD_CAST "nom");
		
		// Dans un arbre généré par le présent programme, le contenu doit
		// obligatoirement avoir un nom même si sa valeur est ""
		if (contenu == NULL)
			BUGTEXTE(-1, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "nom");
			
		gtk_tree_store_append(tree_store, &iter, NULL);
		gtk_tree_store_set(tree_store, &iter, 0, (char *)contenu, -1);
		xmlFree(contenu);
		if (ajout_erreur_recursif(n0, tree_store, &iter) != 0)
			BUG(-2);
	}
	return 0;
}

/* charge_document
 * Description : Charge un document et l'affiche dans le tree_view
 * Paramètres : char *nom_fichier : nom du fichier à charger
 *            : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int charge_document(char *nom_fichier, Projet *projet)
{
	// Variables pour l'utilisation de la librarie libxml2
	xmlDocPtr	doc;
	xmlNodePtr	racine, n0, new_node;
	
	projet->nom_fichier = malloc(sizeof(char)*(strlen(nom_fichier)+1));
	strcpy(projet->nom_fichier, nom_fichier);
	projet->document = xmlNewDoc(BAD_CAST "1.0");
	// On charge l'arbre en mémoire
	doc = xmlParseFile(nom_fichier);
	if (doc == NULL)
		BUGTEXTE(-2, gettext("Impossible d'ouvrir le fichier '%s'.\n"), nom_fichier);
	racine = xmlDocGetRootElement(doc);
	if (racine == NULL)
	{
		xmlFreeDoc(doc);
		BUGTEXTE(-3, gettext("Le document est vide.\n"));
	}
	new_node = xmlNewNode(NULL, BAD_CAST "erreur");
	if (new_node == NULL)
		BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
	xmlDocSetRootElement(projet->document, new_node);
	
	// La première profondeur ne doit normalement contenir que "valgrindoutput"
	for (n0 = racine; n0 != NULL; n0 = n0->next)
	{
		if ((n0->type == XML_ELEMENT_NODE) && (strcmp((char*)n0->name, "valgrindoutput") == 0) && (n0->children != NULL))
		{
			xmlNodePtr	n1;
			
			for (n1 = n0->children; n1 != NULL; n1 = n1->next)
			{
				if ((strcmp((char*)n1->name, "error") == 0) && (n1->type == XML_ELEMENT_NODE) && (n1->children != NULL))
				{
					int		kind = 0;
					xmlNodePtr	n2 = xmlGetLastChild(n1), parcours1;
					xmlChar		*contenu, *nom_erreur;
					int		trouve = 0;
					
					// On commence par rechercher la nature de l'erreur qui est contenu dans le noeud "kinc"
					do
					{
						if (n2->type == XML_ELEMENT_NODE)
						{
							if (strcmp((char*)n2->name, "kind") == 0)
							{
								nom_erreur = xmlNodeGetContent(n2);
								kind = 1;
							}
						}
						n2 = n2->prev;
					}
					while ((n2 != NULL) && (kind == 0));
					if (kind == 0)
						BUGTEXTE(-6, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "kind");
					
				 	// On vérifie dans le nouveau document XML si l'erreur est déjà inscrite
					parcours1 = xmlDocGetRootElement(projet->document);
					parcours1 = parcours1->children;
					trouve = 0;
					if (parcours1 != NULL)
					{
						do
						{
							if (parcours1->type == XML_ELEMENT_NODE)
							{
								if (strcmp((char*)parcours1->name, "erreur") == 0)
								{
									contenu = xmlGetProp(parcours1, BAD_CAST "nom");
									if ((contenu != NULL) && (parcours1->name != NULL) && (strcmp((char*)nom_erreur, (char *)contenu) == 0))
										trouve = 1;
									xmlFree(contenu);
								}
							}
							if (trouve == 0)
								parcours1 = parcours1->next;
						} while ((parcours1 != NULL) && (trouve == 0));
					}
					// Si l'erreur n'est pas déjà inscrite, on l'ajoute
					if (trouve == 0)
					{
						new_node = xmlNewNode(NULL, BAD_CAST "erreur");
						xmlSetProp(new_node, BAD_CAST "nom", nom_erreur);
						xmlAddChild(xmlDocGetRootElement(projet->document), new_node);
						parcours1 = new_node;
					}
					
					// On parcourt le noeud pour trouver la pile
					n2 = xmlGetLastChild(n1);
					do
					{
						if (n2->type == XML_ELEMENT_NODE)
						{
							// Pile trouvée
							if (strcmp((char*)n2->name, "stack") == 0)
							{
								xmlNodePtr	n3;
								
								for (n3 = xmlGetLastChild(n2); n3 != NULL; n3 = n3->prev)
								{
									// On parcourt le noeud pour trouver les frames
									if ((n3->type == XML_ELEMENT_NODE) && (strcmp((char*)n3->name, "frame") == 0))
									{
										xmlChar 	*char_file = NULL, *char_fn = NULL, *char_ligne = NULL;
										xmlNodePtr	n4, parcours2;
										
										// On récupère le nom du fichier, le nom de la fonction et la ligne du code
										// Il est possible que certaines informations ne soient pas disponibles
										for (n4 = n3->children; n4 != NULL; n4 = n4->next)
										{
											if (strcmp((char*)n4->name, "file") == 0)
												char_file = xmlNodeGetContent(n4);
											else if (strcmp((char*)n4->name, "fn") == 0)
												char_fn = xmlNodeGetContent(n4);
											else if (strcmp((char*)n4->name, "line") == 0)
												char_ligne = xmlNodeGetContent(n4);
										}
										
										// On commence par vérifier si le fichier est déjà présent dans la nouvelle arborescence
										trouve = 0;
										parcours2 = parcours1->children;
										if (parcours2 != NULL)
										{
											do
											{
												if ((strcmp((char*)parcours2->name, "fichier") == 0) && (parcours2->type == XML_ELEMENT_NODE))
												{
													xmlChar *contenu = xmlGetProp(parcours2, BAD_CAST "nom");
													if ((contenu == NULL) && (char_file == NULL))
														trouve = 1;
													else if ((contenu == NULL) && (char_file != NULL) && (strcmp("", (char *)char_file) == 0))
														trouve = 1;
													else if ((contenu != NULL) && (strcmp("", (char *)contenu) == 0) && (char_file == NULL))
														trouve = 1;
													else if ((contenu != NULL) && (char_file != NULL) && (strcmp((char*)contenu, (char *)char_file) == 0))
														trouve = 1;
													xmlFree(contenu);
												}
												if (trouve == 0)
													parcours2 = parcours2->next;
											}
											while ((parcours2 != NULL) && (trouve == 0));
										}
										// Si on ne le trouve pas, on l'ajoute
										if (trouve == 0)
										{
											new_node = xmlNewNode(NULL, BAD_CAST "fichier");
											xmlSetProp(new_node, BAD_CAST "nom", char_file);
											xmlAddChild(parcours1, new_node);
											parcours1 = new_node;
										}
										// Si on le trouve, on l'utilise
										else
										{
											parcours1 = parcours2;
										}
										parcours2 = parcours1->children;
										
										// On vérifie si parmi les noeuds enfants l'un contient la même ligne et le même nom de fonction
										trouve = 0;
										parcours2 = parcours1->children;
										if (parcours2 != NULL)
										{
											do
											{
												if ((strcmp((char*)parcours2->name, "ligne") == 0) && (parcours2->type == XML_ELEMENT_NODE))
												{
													xmlChar *contenu_ligne = xmlGetProp(parcours2, BAD_CAST "ligne");
													xmlChar *contenu_fn = xmlGetProp(parcours2, BAD_CAST "fn");
													if (
														(
															((contenu_ligne == NULL) && (char_ligne == NULL)) ||
															((contenu_ligne == NULL) && (char_ligne != NULL) && (strcmp("", (char *)char_ligne) == 0)) ||
															((contenu_ligne != NULL) && (strcmp("", (char *)contenu_ligne) == 0) && (char_ligne == NULL)) ||
															((contenu_ligne != NULL) && (char_ligne != NULL) && (strcmp((char*)contenu_ligne, (char *)char_ligne) == 0))
														) && (
															((contenu_fn == NULL) && (char_fn == NULL)) ||
															((contenu_fn == NULL) && (char_fn != NULL) && (strcmp("", (char *)char_fn) == 0)) ||
															((contenu_fn != NULL) && (strcmp("", (char *)contenu_fn) == 0) && (char_fn == NULL)) ||
															((contenu_fn != NULL) && (char_fn != NULL) && (strcmp((char*)contenu_fn, (char *)char_fn) == 0))
														)
													)
														trouve = 1;
													xmlFree(contenu_ligne);
													xmlFree(contenu_fn);
												}
												if (trouve == 0)
													parcours2 = parcours2->next;
											}
											while ((parcours2 != NULL) && (trouve == 0));
										}
										if (trouve == 0)
										{
											new_node = xmlNewNode(NULL, BAD_CAST "ligne");
											xmlSetProp(new_node, BAD_CAST "ligne", char_ligne);
											xmlSetProp(new_node, BAD_CAST "fn", char_fn);
											xmlAddChild(parcours1, new_node);
											parcours1 = new_node;
										}
										else
											parcours1 = parcours2;
										
										// On libère la mémoire
										xmlFree(char_fn);
										xmlFree(char_ligne);
										xmlFree(char_file);
									}
								}
							}
						}
						n2 = n2->prev;
					}
					while (n2 != NULL);
				}
			}
		}
	}
	// On libère la mémoire
	xmlFreeDoc(doc);
	
	// On ajoute à tree_store une représentation graphique du document XML
	if (ajout_erreur(xmlDocGetRootElement(projet->document), projet->tree_store) != 0)
		BUG(-8);
	
	xmlFreeDoc(projet->document);
	
	return 0;
}

/* main_ouvrir
 * Description : Ouvre une fenêtre de sélection de vérifier et ouvre le log valgrind demandé
 * Paramètres : GtkMenuItem *menuitem : composant GtkMenuItem à l'origine de la demande
 *            : gpointer user_data : donnée utilisateur
 * Valeur renvoyée : Aucune
 */
void main_ouvrir(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
{
	GtkWidget	*pFileSelection;
	gchar		*sChemin;
	
	pFileSelection = gtk_file_chooser_dialog_new("Ouvrir...", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);
	gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);
	
	switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
	{
		case GTK_RESPONSE_OK:
		{
			// Recuperation du chemin
			sChemin = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(pFileSelection));
			charge_document(sChemin, projet);
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
	GtkWidget	*mainwindow = NULL;
	GtkCellRenderer	*pCellRenderer;
	GtkTreeViewColumn *pColumn;
	GtkWidget	*pVBox;
	GtkWidget	*pScrollbar;
	GtkWidget	*pMenuBar;
	GtkWidget	*pMenu;
	GtkWidget	*pMenuItem;
	Projet		projet;
	
	// On initialise l'interface graphique
	if (gtk_init_check(&argc, &argv) == FALSE)
		BUGTEXTE(-1, gettext("Impossible d'initialiser gtk\n"));
	
	// On crée la fenêtre graphique
	mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(mainwindow), 600, 400);
	g_signal_connect(G_OBJECT(mainwindow), "delete-event", G_CALLBACK(gtk_window_destroy), NULL);
	g_signal_connect(GTK_WINDOW(mainwindow), "destroy", G_CALLBACK(gtk_window_destroy), NULL);
	// Le tree_view
	projet.tree_store = gtk_tree_store_new(1, G_TYPE_STRING);
	projet.tree_view = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(projet.tree_store));
	g_signal_connect(G_OBJECT(projet.tree_view), "row-expanded", G_CALLBACK(tree_view_row_expanded), NULL);
	
	// On vérifie qu'un nom de fichier est passé en argument
	if (argc == 2)
	{
		if (charge_document(argv[1], &projet) != 0)
			BUG(-2);
	}
	
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Texte", pCellRenderer, "text", 0, NULL);
	gtk_tree_view_append_column(projet.tree_view, pColumn);
	
	// Le menu
	pMenuBar = gtk_menu_bar_new();
	pMenu = gtk_menu_new();
	pMenuItem = gtk_menu_item_new_with_label("Ouvrir log valgrind...");
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenu), pMenuItem);
	g_signal_connect(G_OBJECT(pMenuItem), "activate", G_CALLBACK(main_ouvrir), &projet);
	pMenuItem = gtk_menu_item_new_with_label("Fichier");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(pMenuItem), pMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(pMenuBar), pMenuItem);
	
	pVBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pVBox), pMenuBar, FALSE, FALSE, 0);
	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(pVBox), pScrollbar, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(mainwindow), pVBox);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(pScrollbar), GTK_WIDGET(projet.tree_view));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_show_all(mainwindow);
	gtk_main();
	
	return 0;
}
