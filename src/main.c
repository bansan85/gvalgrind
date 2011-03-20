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

#include "config.h"
#include "erreurs.h"
#include "texte.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>

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
 *            : GtkTreeStore *tree_store_etat : Emplacement où stocker les données pour un composant de type TreeView
 *            : GtkTreeIter *iter_racine : ligne parent de tree_store_etat où stocker les données
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int ajout_erreur_recursif(xmlNodePtr racine, GtkTreeStore *tree_store_etat, GtkTreeIter *iter_racine)
{
	GtkTreeIter	iter_parent, iter;
	xmlNodePtr	n0;
	
	// Vérification des paramètres
	if ((racine == NULL) || (tree_store_etat == NULL) || (iter_racine == NULL))
		BUGTEXTE(-1, gettext("Paramètres invalides.\n"));
	
	// On parcours l'ensemble des enfants du noeud racine à la recherche de la propriété fichier
	for (n0 = racine->children; n0 != NULL; n0 = n0->next)
	{
		xmlChar		* contenu_fichier = xmlGetProp(n0, BAD_CAST "nom");
		xmlNodePtr	n1;
		
		// l'attribut fichier ne peut être absent puisque c'est le présent code qui a créer l'aborescence
		// Si l'attribut file est initialement manquant, il est stocké en ""
		if (contenu_fichier == NULL)
			BUGTEXTE(-2, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "nom");
		// On ajoute la ligne
		gtk_tree_store_append(tree_store_etat, &iter_parent, iter_racine);
		gtk_tree_store_set(tree_store_etat, &iter_parent, 0, "", 1, (char *)contenu_fichier, 2, "", 3, "", -1);
		xmlFree(contenu_fichier);
		
		// On parcours l'ensemble des enfants du noeud racine à la recherche de la propriété ligne et fonction 
		for (n1 = n0->children; n1 != NULL; n1 = n1->next)
		{
			xmlChar		* contenu_ligne = xmlGetProp(n1, BAD_CAST "ligne");
			xmlChar		* contenu_fn = xmlGetProp(n1, BAD_CAST "fn");
			
			if (contenu_ligne == NULL)
				BUGTEXTE(-3, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "ligne");
			if (contenu_fn == NULL)
				BUGTEXTE(-4, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "fonction");
			gtk_tree_store_append(tree_store_etat, &iter, &iter_parent);
			gtk_tree_store_set(tree_store_etat, &iter, 0, "", 1, "", 2, (char *)contenu_fn, 3, (char *)contenu_ligne, -1);
			xmlFree(contenu_ligne);
			xmlFree(contenu_fn);
			if (ajout_erreur_recursif(n1, tree_store_etat, &iter) != 0)
				BUG(-5);
		}
	}
	return 0;
}

/* ajout_erreur
 * Description : Ajoute dans un Widget de type TreeStore, dans la racine de l'arborescence, les éléments contenus dans le noeud XML racine
 * Paramètres : xmlNodePtr racine : Noeud XML contenant les informations à ajouter
 *            : GtkTreeStore *tree_store_etat : Emplacement où stocker les données pour un composant de type TreeView
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int ajout_erreur(xmlNodePtr racine, GtkTreeStore *tree_store_etat)
{
	GtkTreeIter	iter;
	xmlNodePtr	n0;
	for (n0 = racine->children; n0 != NULL; n0 = n0->next)
	{
		xmlChar		*contenu = xmlGetProp(n0, BAD_CAST "nom");
		
		// Dans un arbre généré par le présent programme, le contenu doit
		// obligatoirement avoir un nom même si sa valeur est ""
		if (contenu == NULL)
			BUGTEXTE(-1, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "nom");
			
		gtk_tree_store_append(tree_store_etat, &iter, NULL);
		gtk_tree_store_set(tree_store_etat, &iter, 0, (char *)contenu, 1, "", 2, "", 3, "", -1);
		xmlFree(contenu);
		if (ajout_erreur_recursif(n0, tree_store_etat, &iter) != 0)
			BUG(-2);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	// Variables pour l'interface graphique
	GtkWidget	*MainWindow = NULL;
	GtkTreeStore	*tree_store_etat;
	GtkTreeView	*tree_view_etat;
	GtkCellRenderer	*pCellRenderer;
	GtkTreeViewColumn *pColumn;
	GtkWidget	*pScrollbar;
	
	// Variables pour l'utilisation de la librarie libxml2
	xmlDocPtr	doc, doc_revu;
	xmlNodePtr	racine, n0, new_node;
	
	char		*destination;
	
	// On vérifie qu'un nom de fichier est passé en argument
	if (argc != 2)
	{
		show_help();
		return -1;
	}
	
	// On charge l'arbre en mémoire
	doc = xmlParseFile(argv[1]);
	if (doc == NULL)
		BUGTEXTE(-2, gettext("Impossible d'ouvrir le fichier '%s'.\n"), argv[1]);
	racine = xmlDocGetRootElement(doc);
	if (racine == NULL)
	{
		xmlFreeDoc(doc);
		BUGTEXTE(-3, gettext("Le document est vide.\n"));
	}
	doc_revu = xmlNewDoc(BAD_CAST "1.0");
	if (doc_revu == NULL)
	{
		xmlFreeDoc(doc);
		BUGTEXTE(-4, gettext("Erreur d'allocation mémoire.\n"));
	}
	new_node = xmlNewNode(NULL, BAD_CAST "erreur");
	if (new_node == NULL)
		BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
	xmlDocSetRootElement(doc_revu, new_node);
	
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
					parcours1 = xmlDocGetRootElement(doc_revu);
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
						xmlAddChild(xmlDocGetRootElement(doc_revu), new_node);
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
	
	// On enregistre la nouvelle arborescence dans un fichier .xml
	destination = malloc(sizeof(char)*(strlen(argv[1])+strlen(".xml")+2));
	strcpy(destination, argv[1]);
	strcat(destination, ".xml");
	xmlSaveFormatFile(destination, doc_revu, 1);
	
	// On initialise l'interface graphique
	if (gtk_init_check(&argc, &argv) == FALSE)
		BUGTEXTE(-1, gettext("Impossible d'initialiser gtk\n"));
	
	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(MainWindow), "delete-event", G_CALLBACK(gtk_window_destroy), NULL);
	g_signal_connect(GTK_WINDOW(MainWindow), "destroy", G_CALLBACK(gtk_window_destroy), NULL);
	tree_store_etat = gtk_tree_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	tree_view_etat = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(tree_store_etat));
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Erreur", pCellRenderer, "text", 0, NULL);
	gtk_tree_view_append_column(tree_view_etat, pColumn);
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Fichier", pCellRenderer, "text", 1, NULL);
	gtk_tree_view_append_column(tree_view_etat, pColumn);
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Fonction", pCellRenderer, "text", 2, NULL);
	gtk_tree_view_append_column(tree_view_etat, pColumn);
	pCellRenderer = gtk_cell_renderer_text_new();
	pColumn = gtk_tree_view_column_new_with_attributes("Ligne", pCellRenderer, "text", 3, NULL);
	gtk_tree_view_append_column(tree_view_etat, pColumn);
	
	// On ajoute à tree_store une représentation graphique du document XML
	ajout_erreur(xmlDocGetRootElement(doc_revu), tree_store_etat);
	
	pScrollbar = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(MainWindow), pScrollbar);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(pScrollbar), GTK_WIDGET(tree_view_etat));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(pScrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_show_all(MainWindow);
	gtk_main();
	
	xmlFreeDoc(doc_revu);
	
	return 0;
}
