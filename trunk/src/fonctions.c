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
#include <errno.h>

#include "config.h"
#include "erreurs.h"
#include "texte.h"
#include "struct.h"
#include "fonctions.h"

/* remplace_texte
 * Description : Renvoie une chaine de caractère contenant le texte d'origine avec les éléments recherchés remplacés par le texte 'remplacer par'
 *             : La chaîne de caractère renvoyée doit être libéré par la fonction free.
 * Paramètres : char *texte_origine : texte d'origine
 *            : char *chercher : texte à rechercher
 *            : char *remplacer_par : valeur du texte à remplacer
 * Valeur renvoyée :
 *   Succès : pointeur vers le texte les éléments remplacés.
 *   Échec : NULL
 */
char *remplace_texte(char *texte_origine, char *chercher, char *remplacer_par)
{
	char	*retour = NULL;
	char	*parcours = texte_origine;
	char	*trouver = NULL;
	int	longueur;
	
	trouver = strstr(parcours, chercher);
	if (trouver == NULL)
	{
		retour = malloc(sizeof(char)*(strlen(texte_origine)+1));
		if (retour == NULL)
			BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
		strcpy(retour, texte_origine);
		return retour;
	}
	retour = malloc(sizeof(char)*(trouver-texte_origine)+1+strlen(remplacer_par));
	if (retour == NULL)
		BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
	longueur = trouver-texte_origine;
	strncpy(retour, texte_origine, longueur);
	retour[longueur] = '\0';

	strcat(retour, remplacer_par);
	parcours = trouver + strlen(chercher);
	trouver = strstr(parcours, chercher);
	
	while (trouver != NULL)
	{
		retour = realloc(retour, sizeof(char)*(strlen(retour)+trouver-parcours+1+strlen(remplacer_par)));
		if (retour == NULL)
			BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
		strncat(retour, parcours, trouver-parcours);
		strcat(retour, remplacer_par);
		parcours = trouver + strlen(chercher);
		trouver = strstr(parcours, chercher);
	}
	retour = realloc(retour, sizeof(char)*(strlen(retour)+strlen(parcours)+1));
	if (retour == NULL)
		BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
	strcat(retour, parcours);
	return retour;
}

/* get_lines
 * Description : Renvoie les lignes de debut à fin d'un fichier texte
 * Paramètres : char *nom_fichier : nom du fichier à ouvrir
 *            : int debut : numéro de la première ligne à afficher
 *            : int fin : numéro de la dernière ligne à afficher
 * Valeur renvoyée :
 *   Succès : pointeur vers le texte contenant les différentes lignes. Le retour doit être libéré via la commande free
 *   Échec : NULL
 */
char *get_lines(char *nom_fichier, int debut, int fin)
{
	char	buffer[256];
	int	ligne_en_cours = 1;
	char	*retour = NULL;
	int	longueur = 0;
	FILE	*fichier = fopen(nom_fichier, "r");
	
	if (fichier == NULL)
		BUGTEXTE(NULL, gettext("Impossible d'ouvrir le fichier '%s' en lecture.\n"), nom_fichier);
	
	fseek(fichier, 0, SEEK_SET);
	while ((ligne_en_cours <= fin) && (fgets(buffer,256,fichier) != NULL))
	{
		if ((ligne_en_cours == debut) && (retour == NULL))
		{
			longueur = strlen(buffer)+1;
			retour = malloc(sizeof(char)*longueur);
			if (retour == NULL)
				BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
			strcpy(retour, buffer);
		}
		else if (debut <= ligne_en_cours)
		{
			longueur = longueur + strlen(buffer);
			retour = realloc(retour, longueur);
			if (retour == NULL)
				BUGTEXTE(NULL, gettext("Erreur d'allocation mémoire.\n"));
			strcat(retour, buffer);
		}
		if ((strlen(buffer) < 256-1) || (buffer[256-2] == '\n'))
			ligne_en_cours++;
	}
	// On enlève le retour à la ligne finale
	retour[longueur-2] = '\0';
	
	fclose(fichier);
	
	return retour;
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
		if ((n0->type == XML_ELEMENT_NODE) && (strcmp((char*)n0->name, "fichier") == 0) && (n0->children != NULL))
		{
			xmlChar		*contenu_fichier = xmlGetProp(n0, BAD_CAST "nom");
			xmlChar		*contenu_dossier = xmlGetProp(n0, BAD_CAST "dossier");
			char		*contenu_cat;
			xmlNodePtr	n1;
			
			// l'attribut fichier ne peut être absent puisque c'est le présent code qui a créer l'aborescence
			// Si l'attribut file est initialement manquant, il est stocké en ""
			if (contenu_fichier == NULL)
				BUGTEXTE(-2, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "nom");
			if (contenu_dossier == NULL)
				BUGTEXTE(-2, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "dossier");
			// On ajoute la ligne
			gtk_tree_store_append(tree_store, &iter_parent, iter_racine);
			contenu_cat = malloc(sizeof(char)*(strlen((char *)contenu_fichier)+strlen((char *)contenu_dossier)+strlen("<span bgcolor=\"#000000\"> ()</span>")+1));
			if (contenu_cat == NULL)
				BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
            if (strncmp((char*)contenu_dossier, "/var/tmp/portage/", 17) == 0)
    			strcpy(contenu_cat, "<span bgcolor=\"#E0E0E0\">");
            else
    			strcpy(contenu_cat, "<span bgcolor=\"#FFFF00\">");
			strcat(contenu_cat, (char *)contenu_fichier);
			strcat(contenu_cat, " (");
			strcat(contenu_cat, (char *)contenu_dossier);
			strcat(contenu_cat, ")</span>");
			gtk_tree_store_set(tree_store, &iter_parent, 0, contenu_cat, -1);
			
			// On réutilise contenu_cat pour stocker l'emplacement du fichier.
			strcpy(contenu_cat, (char *)contenu_dossier);
			strcat(contenu_cat, "/");
			strcat(contenu_cat, (char *)contenu_fichier);
			
			// On parcours l'ensemble des enfants du noeud racine à la recherche de la propriété ligne et fonction 
			for (n1 = n0->children; n1 != NULL; n1 = n1->next)
			{
				if ((n1->type == XML_ELEMENT_NODE) && (strcmp((char*)n1->name, "ligne") == 0))
				{
					xmlChar		*contenu_ligne = xmlGetProp(n1, BAD_CAST "ligne");
					long		ligne;
					xmlChar		*contenu_fn = xmlGetProp(n1, BAD_CAST "fn");
					char		*contenu_cat2, *endptr, *texte_compatible_markup;
					int		get_ligne = 1;
					
					if (contenu_ligne == NULL)
						BUGTEXTE(-3, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "ligne");
					// On converti contenu_ligne en nombre avec gestion des erreurs
					errno = 0;
					ligne = strtol((char *)contenu_ligne, &endptr, 10);
					if ((errno == ERANGE && (ligne == LONG_MAX || ligne == LONG_MIN)) || (errno != 0 && ligne == 0))
						get_ligne = 0;
					if (endptr == (char *)contenu_ligne)
						get_ligne = 0;
					
					if (contenu_fn == NULL)
						BUGTEXTE(-4, gettext("Propriété '%s' inconnue dans le noeud XML.\n"), "fonction");
					gtk_tree_store_append(tree_store, &iter, &iter_parent);
					
					// On défini le contenu de la ligne : fonction (ligne)
					texte_compatible_markup = remplace_texte((char *)contenu_fn, "<", "&lt;");
					free(contenu_fn);
					contenu_fn = (xmlChar*)remplace_texte(texte_compatible_markup, ">", "&gt;");
					free(texte_compatible_markup);
					contenu_cat2 = malloc(sizeof(char)*(strlen((char *)contenu_ligne)+strlen((char *)contenu_fn)+strlen("<span bgcolor=\"#000000\"> ()</span>")+1));
					strcpy(contenu_cat2, "<span bgcolor=\"#B0B0B0\">");
					strcat(contenu_cat2, (char *)contenu_fn);
					strcat(contenu_cat2, " (");
					strcat(contenu_cat2, (char *)contenu_ligne);
					strcat(contenu_cat2, ")</span>");
					// Et on ajoute le contenu de la source si cette dernière existe
					if ((get_ligne == 1) && (file_exists(contenu_cat) == 0))
					{
						char	*source = get_lines(contenu_cat, ligne, ligne);
						
						contenu_cat2 = realloc(contenu_cat2, strlen(contenu_cat2)+1+strlen("\n<span face=\"Courier New\"><b>"));
						strcat(contenu_cat2, "\n<span face=\"Courier New\"><b>");
						
						texte_compatible_markup = remplace_texte(source, "&", "&amp;");
						free(source);
						source = remplace_texte(texte_compatible_markup, "<", "&lt;");
						free(texte_compatible_markup);
						texte_compatible_markup = remplace_texte(source, ">", "&gt;");
					
						contenu_cat2 = realloc(contenu_cat2, strlen(contenu_cat2)+strlen(texte_compatible_markup)+1+11);
						if (contenu_cat2 == NULL)
							BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
						strcat(contenu_cat2, texte_compatible_markup);
						strcat(contenu_cat2, "</b></span>");
						
						free(source);
						free(texte_compatible_markup);
					}
					
					gtk_tree_store_set(tree_store, &iter, 0, contenu_cat2, -1);
					
					xmlFree(contenu_ligne);
					xmlFree(contenu_fn);
					free(contenu_cat2);
					if (ajout_erreur_recursif(n1, tree_store, &iter) != 0)
						BUG(-5);
				}
			}
			free(contenu_cat);
			xmlFree(contenu_fichier);
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
			
			xmlSetProp(n0, BAD_CAST "dossier_courant", BAD_CAST projet->programme.dossier_courant);
			xmlSetProp(n0, BAD_CAST "environnement", BAD_CAST projet->programme.environnement);
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
	
	if (projet->programme.enregistrement != NULL)
		free(projet->programme.enregistrement);
	projet->programme.enregistrement = malloc(sizeof(char)*strlen((nom_fichier)+1));
	if (projet->programme.enregistrement == NULL)
		BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
	strcpy(projet->programme.enregistrement, nom_fichier);
	
	projet->modifie = 0;
	
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
	xmlDocPtr	nouveau_projet;
	int		vide = 1;
	
	// On passe par une variable temporaire afin de ne pas corrompre le projet en cours si l'ouverture échoue.
	nouveau_projet = xmlParseFile(nom_fichier);
	if (nouveau_projet == NULL)
	{
		printf(gettext("Le fichier spécifié n'est pas dans un format XML.\n"));
		return -1;
	}
	
	racine = xmlDocGetRootElement(nouveau_projet);
	for (n0 = racine; n0 != NULL; n0 = n0->next)
	{
		if ((n0->type == XML_ELEMENT_NODE) && (strcmp((char*)n0->name, "erreur") == 0))
		{
			xmlChar	*contenu;
			
			projet->document = nouveau_projet;
			vide = 0;
			
			contenu = xmlGetProp(n0, BAD_CAST "dossier_courant");
			if (contenu != NULL)
			{
				projet->programme.dossier_courant = malloc(sizeof(char)*(strlen((char*)contenu)+1));
				strcpy(projet->programme.dossier_courant, (char *)contenu);
				xmlFree(contenu);
			}
			else
			{
				projet->programme.dossier_courant = malloc(sizeof(char));
				strcpy(projet->programme.dossier_courant, "");
			}
			
			contenu = xmlGetProp(n0, BAD_CAST "environnement");
			if (contenu != NULL)
			{
				projet->programme.environnement = malloc(sizeof(char)*(strlen((char*)contenu)+1));
				strcpy(projet->programme.environnement, (char *)contenu);
				xmlFree(contenu);
			}
			else
			{
				projet->programme.environnement = malloc(sizeof(char));
				strcpy(projet->programme.environnement, "");
			}
			
			contenu = xmlGetProp(n0, BAD_CAST "nom_fichier");
			if (contenu != NULL)
			{
				projet->programme.nom_fichier = malloc(sizeof(char)*(strlen((char*)contenu)+1));
				strcpy(projet->programme.nom_fichier, (char *)contenu);
				xmlFree(contenu);
			}
			else
			{
				projet->programme.nom_fichier = malloc(sizeof(char));
				strcpy(projet->programme.nom_fichier, "");
			}
			
			contenu = xmlGetProp(n0, BAD_CAST "arguments");
			if (contenu != NULL)
			{
				projet->programme.arguments = malloc(sizeof(char)*(strlen((char*)contenu)+1));
				strcpy(projet->programme.arguments, (char *)contenu);
				xmlFree(contenu);
			}
			else
			{
				projet->programme.arguments = malloc(sizeof(char));
				strcpy(projet->programme.arguments, "");
			}
			
			contenu = xmlGetProp(n0, BAD_CAST "trace_children");
			if (contenu != NULL)
			{
				projet->general.trace_children = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.trace_children = GVAL_DEFAUT_TRACE_CHILDREN;
			
			contenu = xmlGetProp(n0, BAD_CAST "track_fds");
			if (contenu != NULL)
			{
				projet->general.track_fds = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.track_fds = GVAL_DEFAUT_TRACK_FDS;
			
			contenu = xmlGetProp(n0, BAD_CAST "num_callers");
			if (contenu != NULL)
			{
				projet->general.num_callers = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.num_callers = GVAL_DEFAUT_NUM_CALLERS;
			
			contenu = xmlGetProp(n0, BAD_CAST "error_limit");
			if (contenu != NULL)
			{
				projet->general.error_limit = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.error_limit = GVAL_DEFAUT_ERROR_LIMIT;
			
			contenu = xmlGetProp(n0, BAD_CAST "show_below_main");
			if (contenu != NULL)
			{
				projet->general.show_below_main = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.show_below_main = GVAL_DEFAUT_SHOW_BELOW_MAIN;
			
			contenu = xmlGetProp(n0, BAD_CAST "max_stackframe");
			if (contenu != NULL)
			{
				projet->general.max_stackframe = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.max_stackframe = GVAL_DEFAUT_MAX_STACKFRAME;
			
			contenu = xmlGetProp(n0, BAD_CAST "smc_check");
			if (contenu != NULL)
			{
				projet->general.smc_check = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.smc_check = GVAL_DEFAUT_SMC_CHECK;
			
			contenu = xmlGetProp(n0, BAD_CAST "read_var_info");
			if (contenu != NULL)
			{
				projet->general.read_var_info = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.read_var_info = GVAL_DEFAUT_READ_VAR_INFO;
			
			contenu = xmlGetProp(n0, BAD_CAST "verbose");
			if (contenu != NULL)
			{
				projet->general.verbose = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->general.verbose = GVAL_DEFAUT_VERBOSE;
			
			contenu = xmlGetProp(n0, BAD_CAST "leak_check");
			if (contenu != NULL)
			{
				projet->memcheck.leak_check = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->memcheck.leak_check = GVAL_DEFAUT_LEAK_CHECK;
			
			contenu = xmlGetProp(n0, BAD_CAST "show_possibly_lost");
			if (contenu != NULL)
			{
				projet->memcheck.show_possibly_lost = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->memcheck.show_possibly_lost = GVAL_DEFAUT_SHOW_POSSIBLY_LOST;
			
			contenu = xmlGetProp(n0, BAD_CAST "leak_resolution");
			if (contenu != NULL)
			{
				projet->memcheck.leak_resolution = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->memcheck.leak_resolution = GVAL_DEFAUT_LEAK_RESOLUTION;
			
			contenu = xmlGetProp(n0, BAD_CAST "show_reachable");
			if (contenu != NULL)
			{
				projet->memcheck.show_reachable = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->memcheck.show_reachable = GVAL_DEFAUT_SHOW_REACHABLE;
			
			contenu = xmlGetProp(n0, BAD_CAST "undef_value_errors");
			if (contenu != NULL)
			{
				projet->memcheck.undef_value_errors = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->memcheck.undef_value_errors = GVAL_DEFAUT_UNDEF_VALUE_ERRORS;
			
			contenu = xmlGetProp(n0, BAD_CAST "track_origins");
			if (contenu != NULL)
			{
				projet->memcheck.track_origins = atoi((char*)contenu);
				xmlFree(contenu);
			}
			else
				projet->memcheck.track_origins = GVAL_DEFAUT_TRACK_ORIGINS;
			
			ajout_erreur(n0, projet->tree_store);
		}
	}
	
	if (vide == 1)
	{
		printf(gettext("Le document sélectionné ne semble pas contenir d'information compatible avec gvalgrind.\n"));
		return -1;
	}
	else
		projet->modifie = 0;
	
	if (projet->programme.enregistrement != NULL)
		free(projet->programme.enregistrement);
	projet->programme.enregistrement = malloc(sizeof(char)*strlen((nom_fichier)+1));
	if (projet->programme.enregistrement == NULL)
		BUGTEXTE(-1, gettext("Erreur d'allocation mémoire.\n"));
	strcpy(projet->programme.enregistrement, nom_fichier);
	
	return 0;
}

/* file_exists
 * Description : Détermine si un fichier est existant
 * Paramètres : const char *filename : nom du fichier à vérifier
 * Valeur renvoyée :
 *   Existe : 0
 *   Absent : 1
 */
int file_exists(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file != NULL)
	{
		fclose(file);
		return 0;
	}
	return 1;
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
					
					// On commence par rechercher la nature de l'erreur qui est contenu dans le noeud "kind"
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
										xmlChar 	*char_file = NULL, *char_fn = NULL, *char_ligne = NULL, *char_dir = NULL;
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
											else if (strcmp((char*)n4->name, "dir") == 0)
												char_dir = xmlNodeGetContent(n4);
										}
										
										// Dans le cas où certains éléments ne sont pas disponibles, on leur attribue un texte vide
										if (char_file == NULL)
										{
											char_file = malloc(sizeof(char)*1);
											if (char_file == NULL)
												BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
											strcpy((char *)char_file, "");
										}
										if (char_fn == NULL)
										{
											char_fn = malloc(sizeof(char)*1);
											if (char_fn == NULL)
												BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
											strcpy((char *)char_fn, "");
										}
										if (char_ligne == NULL)
										{
											char_ligne = malloc(sizeof(char)*1);
											if (char_ligne == NULL)
												BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
											strcpy((char *)char_ligne, "");
										}
										if (char_dir == NULL)
										{
											char_dir = malloc(sizeof(char)*1);
											if (char_dir == NULL)
												BUGTEXTE(-5, gettext("Erreur d'allocation mémoire.\n"));
											strcpy((char *)char_dir, "");
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
													xmlChar *contenu2 = xmlGetProp(parcours2, BAD_CAST "dossier");
													if ((strcmp((char *)char_file, (char *)contenu) == 0) && (strcmp((char *)char_dir, (char *)contenu2) == 0))
														trouve = 1;
													xmlFree(contenu);
													xmlFree(contenu2);
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
											xmlSetProp(new_node, BAD_CAST "dossier", char_dir);
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
										xmlFree(char_dir);
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
	
	projet->programme.dossier_courant = NULL;
	projet->programme.environnement = NULL;
	projet->programme.enregistrement = NULL;
	projet->programme.nom_fichier = NULL;
	projet->programme.arguments = NULL;
	
	// Initialisation des options générales à valgrind
	projet->general.trace_children = GVAL_DEFAUT_TRACE_CHILDREN;
	projet->general.track_fds = GVAL_DEFAUT_TRACK_FDS;
	projet->general.num_callers = GVAL_DEFAUT_NUM_CALLERS;
	projet->general.error_limit = GVAL_DEFAUT_ERROR_LIMIT;
	projet->general.show_below_main = GVAL_DEFAUT_SHOW_BELOW_MAIN;
	projet->general.max_stackframe = GVAL_DEFAUT_MAX_STACKFRAME;
	projet->general.smc_check = GVAL_DEFAUT_SMC_CHECK;
	projet->general.read_var_info = GVAL_DEFAUT_READ_VAR_INFO;
	projet->general.verbose = GVAL_DEFAUT_VERBOSE;
	
	// Initialisation des options de memcheck
	projet->memcheck.leak_check = GVAL_DEFAUT_LEAK_CHECK;
	projet->memcheck.show_possibly_lost = GVAL_DEFAUT_SHOW_POSSIBLY_LOST;
	projet->memcheck.leak_resolution = GVAL_DEFAUT_LEAK_RESOLUTION;
	projet->memcheck.show_reachable = GVAL_DEFAUT_SHOW_REACHABLE;
	projet->memcheck.undef_value_errors = GVAL_DEFAUT_UNDEF_VALUE_ERRORS;
	projet->memcheck.track_origins = GVAL_DEFAUT_TRACK_ORIGINS;
	
	projet->modifie = 0;
	
	gtk_tree_store_clear(projet->tree_store);
	
	return 0;
}
