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
		if ((n0->type == XML_ELEMENT_NODE) && (strcmp((char*)n0->name, "fichier") == 0) && (n0->children != NULL))
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
				if ((n1->type == XML_ELEMENT_NODE) && (strcmp((char*)n1->name, "ligne") == 0))
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
					free(contenu_cat);
					if (ajout_erreur_recursif(n1, tree_store, &iter) != 0)
						BUG(-5);
				}
			}
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
		if ((n0->type == XML_ELEMENT_NODE) && (strcmp((char*)n0->name, "erreur") == 0) && (n0->children != NULL))
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
	}
	return 0;
}

/* enregistrer_projet
 * Description : Enregistrer un projet
 * Paramètres : char *nom_fichier : nom du fichier
 *            : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int enregistrer_projet(char *nom_fichier, Projet *projet)
{
	xmlNodePtr	racine, n0;
	
	racine = xmlDocGetRootElement(projet->document);
	for (n0 = racine; n0 != NULL; n0 = n0->next)
	{
		if ((n0->type == XML_ELEMENT_NODE) && (strcmp((char*)n0->name, "erreur") == 0))
		{
			char	contenu[1024];
			
			xmlSetProp(n0, BAD_CAST "nom_fichier", BAD_CAST projet->programme.nom_fichier);
			xmlSetProp(n0, BAD_CAST "arguments", BAD_CAST projet->programme.arguments);
			sprintf(contenu, "%d", projet->general.trace_children);
			xmlSetProp(n0, BAD_CAST "trace_children", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.track_fds);
			xmlSetProp(n0, BAD_CAST "track_fds", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.num_callers);
			xmlSetProp(n0, BAD_CAST "num_callers", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.error_limit);
			xmlSetProp(n0, BAD_CAST "error_limit", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.show_below_main);
			xmlSetProp(n0, BAD_CAST "show_below_main", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.max_stackframe);
			xmlSetProp(n0, BAD_CAST "max_stackframe", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.smc_check);
			xmlSetProp(n0, BAD_CAST "smc_check", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.read_var_info);
			xmlSetProp(n0, BAD_CAST "read_var_info", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->general.verbose);
			xmlSetProp(n0, BAD_CAST "verbose", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->memcheck.leak_check);
			xmlSetProp(n0, BAD_CAST "leak_check", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->memcheck.show_possibly_lost);
			xmlSetProp(n0, BAD_CAST "show_possibly_lost", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->memcheck.leak_resolution);
			xmlSetProp(n0, BAD_CAST "leak_resolution", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->memcheck.show_reachable);
			xmlSetProp(n0, BAD_CAST "show_reachable", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->memcheck.undef_value_errors);
			xmlSetProp(n0, BAD_CAST "undef_value_errors", BAD_CAST contenu);
			sprintf(contenu, "%d", projet->memcheck.track_origins);
			xmlSetProp(n0, BAD_CAST "track_origins", BAD_CAST contenu);
		}
	}
	
	xmlSaveFileEnc(nom_fichier, projet->document, "UTF-8");
	
	return 0;
}

/* ouvrir_projet
 * Description : Ouvrir un projet
 * Paramètres : char *nom_fichier : nom du fichier
 *            : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int ouvrir_projet(char *nom_fichier, Projet *projet)
{
	xmlNodePtr	racine, n0;
	
	projet->document = xmlParseFile(nom_fichier);
	
	racine = xmlDocGetRootElement(projet->document);
	for (n0 = racine; n0 != NULL; n0 = n0->next)
	{
		if ((n0->type == XML_ELEMENT_NODE) && (strcmp((char*)n0->name, "erreur") == 0))
		{
			xmlChar	*contenu;
			
			contenu = xmlGetProp(n0, BAD_CAST "nom_fichier");
			projet->programme.nom_fichier = malloc(sizeof(char)*(strlen((char*)contenu)+1));
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "arguments");
			projet->programme.arguments = malloc(sizeof(char)*(strlen((char*)contenu)+1));
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "trace_children");
			projet->general.trace_children = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "track_fds");
			projet->general.track_fds = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "num_callers");
			projet->general.num_callers = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "error_limit");
			projet->general.error_limit = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "show_below_main");
			projet->general.show_below_main = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "max_stackframe");
			projet->general.max_stackframe = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "smc_check");
			projet->general.smc_check = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "read_var_info");
			projet->general.read_var_info = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "verbose");
			projet->general.verbose = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "leak_check");
			projet->memcheck.leak_check = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "show_possibly_lost");
			projet->memcheck.show_possibly_lost = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "leak_resolution");
			projet->memcheck.leak_resolution = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "show_reachable");
			projet->memcheck.show_reachable = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "undef_value_errors");
			projet->memcheck.undef_value_errors = atoi((char*)contenu);
			xmlFree(contenu);
			contenu = xmlGetProp(n0, BAD_CAST "track_origins");
			projet->memcheck.track_origins = atoi((char*)contenu);
			xmlFree(contenu);
			
			ajout_erreur(n0, projet->tree_store);
		}
	}
	
	return 0;
}

/* converti_rapport_valgrind
 * Description : Charge un document et l'affiche dans le tree_view
 * Paramètres : char *nom_fichier : nom du fichier à charger
 *            : Projet *projet : variable projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int converti_rapport_valgrind(char *nom_fichier, Projet *projet)
{
	// Variables pour l'utilisation de la librarie libxml2
	xmlDocPtr	doc;
	xmlNodePtr	racine, n0, new_node;
	
	if (projet->programme.nom_fichier != NULL)
		free(projet->programme.nom_fichier);
	projet->programme.nom_fichier = malloc(sizeof(char)*(strlen(nom_fichier)+1));
	if (projet->programme.nom_fichier == NULL)
		BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));

	strcpy(projet->programme.nom_fichier, nom_fichier);
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
					xmlFree(nom_erreur);
					
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
	
	return 0;
}

/* init_projet
 * Description : Initialisation de la variable projet
 * Paramètres : Projet *projet
 * Valeur renvoyée :
 *   Succès : 0
 *   Échec : valeur négative en cas d'erreur
 */
int init_projet(Projet *projet)
{
	xmlNodePtr	new_node;
	
	projet->document = xmlNewDoc(BAD_CAST "1.0");
	new_node = xmlNewNode(NULL, BAD_CAST "erreur");
	if (new_node == NULL)
		BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
	xmlDocSetRootElement(projet->document, new_node);
	
	projet->programme.nom_fichier = NULL;
	projet->programme.arguments = NULL;
	
	// Initialisation des options générales à valgrind
	projet->general.trace_children = 0;
	projet->general.track_fds = 1;
	projet->general.num_callers = 50;
	projet->general.error_limit = 0;
	projet->general.show_below_main = 1;
	projet->general.max_stackframe = 2000000;
	projet->general.smc_check = 2;
	projet->general.read_var_info = 0;
	projet->general.verbose = 1;
	
	// Initialisation des options de memcheck
	projet->memcheck.leak_check = 3;
	projet->memcheck.show_possibly_lost = 1;
	projet->memcheck.leak_resolution = 2;
	projet->memcheck.show_reachable = 1;
	projet->memcheck.undef_value_errors = 1;
	projet->memcheck.track_origins = 0;
	
	gtk_tree_store_clear(projet->tree_store);
	
	return 0;
}
