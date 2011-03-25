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

#include <string.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>

#include "config.h"
#include "erreurs.h"
#include "texte.h"
#include "struct.h"
#include "fonctions.h"

/* combo_box_trace_children
 * Description : Modification de la propriété trace_children
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_trace_children(GtkComboBox *widget, Projet *projet)
{
	projet->general.trace_children = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_track_fds
 * Description : Modification de la propriété track_fds
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_track_fds(GtkComboBox *widget, Projet *projet)
{
	projet->general.track_fds = gtk_combo_box_get_active(widget);
	return;
}

/* spin_button_num_callers
 * Description : Modification de la propriété num_callers
 * Paramètres : GtkSpinButton *spinbutton : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void spin_button_num_callers(GtkSpinButton *spinbutton, Projet *projet)
{
	projet->general.num_callers = gtk_spin_button_get_value_as_int(spinbutton);
	return;
}

/* combo_box_error_limit
 * Description : Modification de la propriété error_limit
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_error_limit(GtkComboBox *widget, Projet *projet)
{
	projet->general.error_limit = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_show_below_main
 * Description : Modification de la propriété show_below_main
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_show_below_main(GtkComboBox *widget, Projet *projet)
{
	projet->general.show_below_main = gtk_combo_box_get_active(widget);
	return;
}

/* spin_button_max_stackframe
 * Description : Modification de la propriété max_stackframe
 * Paramètres : GtkSpinButton *spinbutton : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void spin_button_max_stackframe(GtkSpinButton *spinbutton, Projet *projet)
{
	projet->general.max_stackframe = gtk_spin_button_get_value_as_int(spinbutton);
	return;
}

/* combo_box_smc_check
 * Description : Modification de la propriété smc_check
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_smc_check(GtkComboBox *widget, Projet *projet)
{
	projet->general.smc_check = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_read_var_info
 * Description : Modification de la propriété read_var_info
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_read_var_info(GtkComboBox *widget, Projet *projet)
{
	projet->general.read_var_info = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_verbose
 * Description : Modification de la propriété verbose
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_verbose(GtkComboBox *widget, Projet *projet)
{
	projet->general.verbose = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_leak_check
 * Description : Modification de la propriété leak_check
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_leak_check(GtkComboBox *widget, Projet *projet)
{
	projet->memcheck.leak_check = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_show_possibly_lost
 * Description : Modification de la propriété show_possibly_lost
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_show_possibly_lost(GtkComboBox *widget, Projet *projet)
{
	projet->memcheck.show_possibly_lost = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_leak_resolution
 * Description : Modification de la propriété leak_resolution
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_leak_resolution(GtkComboBox *widget, Projet *projet)
{
	projet->memcheck.leak_resolution = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_show_reachable
 * Description : Modification de la propriété show_reachable
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_show_reachable(GtkComboBox *widget, Projet *projet)
{
	projet->memcheck.show_reachable = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_undef_value_errors
 * Description : Modification de la propriété undef_value_errors
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_undef_value_errors(GtkComboBox *widget, Projet *projet)
{
	projet->memcheck.undef_value_errors = gtk_combo_box_get_active(widget);
	return;
}

/* combo_box_track_origins
 * Description : Modification de la propriété track_origins
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void combo_box_track_origins(GtkComboBox *widget, Projet *projet)
{
	projet->memcheck.track_origins = gtk_combo_box_get_active(widget);
	return;
}

/* entry_nom_programme
 * Description : Modifie l'emplacement du programme
 * Paramètres : GtkEntry *entry : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void entry_nom_programme(GtkButton *button, Projet *projet)
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
			gtk_button_set_label(button, sChemin);
			projet->programme.nom_fichier = sChemin;
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

/* entry_arguments
 * Description : Modifie les arguments du programme
 * Paramètres : GtkEntry *entry : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void entry_arguments(GtkEntry *entry, Projet *projet)
{
	const gchar *	nom_entry;
	
	if (projet->programme.arguments != NULL)
		free(projet->programme.arguments);
	nom_entry = gtk_entry_get_text(entry);
	projet->programme.arguments = malloc(sizeof(char)*(strlen(nom_entry)+1));
	if (projet->programme.arguments == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	strcpy(projet->programme.arguments, nom_entry);
	
	return;
}

/* main_valgrind
 * Description : Lancement de valgring avec les options demandés
 * Paramètres : GtkMenuItem *menuitem : menu à l'origine de l'évènement
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void main_valgrind(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
{
	char	*commande;
	int	longueur;
	char	*caractere;
	
	longueur = strlen("valgrind ")+1;
	commande = malloc(sizeof(char)*longueur);
	if (commande == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	strcpy(commande, "valgrind ");
	
	switch (projet->general.trace_children)
	{
		case 0 :
		{
			longueur = longueur + strlen("--trace-children=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--trace-children=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--trace-children=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--trace-children=yes ");
			break;
		}
	}
	switch (projet->general.track_fds)
	{
		case 0 :
		{
			longueur = longueur + strlen("--track-fds=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--track-fds=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--track-fds=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--track-fds=yes ");
			break;
		}
	}
	
	caractere = malloc(sizeof(char)*(strlen("--num-callers=10 ")+1));
	if (caractere == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	sprintf(caractere, "--num-callers=%d ", projet->general.num_callers);
	longueur = longueur + strlen(caractere);
	commande = realloc(commande, longueur*sizeof(char));
	if (commande == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	strcat(commande, caractere);
	free(caractere);
	
	switch (projet->general.error_limit)
	{
		case 0 :
		{
			longueur = longueur + strlen("--error-limit=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--error-limit=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--error-limit=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--error-limit=yes ");
			break;
		}
	}
	switch (projet->general.show_below_main)
	{
		case 0 :
		{
			longueur = longueur + strlen("--show-below-main=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--show-below-main=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--show-below-main=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--show-below-main=yes ");
			break;
		}
	}
	
	caractere = malloc(sizeof(char)*(strlen("--max-stackframe=2000000000 ")+1));
	if (caractere == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	sprintf(caractere, "--max-stackframe=%d ", projet->general.max_stackframe);
	longueur = longueur + strlen(caractere);
	commande = realloc(commande, longueur*sizeof(char));
	if (commande == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	strcat(commande, caractere);
	free(caractere);
	
	switch (projet->general.smc_check)
	{
		case 0 :
		{
			longueur = longueur + strlen("--smc-check=none ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--smc-check=none ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--smc-check=stack ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--smc-check=stack ");
			break;
		}
		case 2 :
		{
			longueur = longueur + strlen("--smc-check=all ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--smc-check=all ");
			break;
		}
	}
	switch (projet->general.read_var_info)
	{
		case 0 :
		{
			longueur = longueur + strlen("--read-var-info=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--read-var-info=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--read-var-info=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--read-var-info=yes ");
			break;
		}
	}
	switch (projet->general.verbose)
	{
		case 1 :
		{
			longueur = longueur + strlen("-v ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "-v ");
			break;
		}
	}
	
	// Options memcheck
	switch (projet->memcheck.leak_check)
	{
		case 0 :
		{
			longueur = longueur + strlen("--leak-check=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--leak-check=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--leak-check=summary ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--leak-check=summary ");
			break;
		}
		case 2 :
		{
			longueur = longueur + strlen("--leak-check=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--leak-check=yes ");
			break;
		}
		case 3 :
		{
			longueur = longueur + strlen("--leak-check=full ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--leak-check=full ");
			break;
		}
	}
	switch (projet->memcheck.show_possibly_lost)
	{
		case 0 :
		{
			longueur = longueur + strlen("--show-possibly-lost=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--show-possibly-lost=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--show-possibly-lost=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--show-possibly-lost=yes ");
			break;
		}
	}
	switch (projet->memcheck.leak_resolution)
	{
		case 0 :
		{
			longueur = longueur + strlen("--leak-resolution=low ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--leak-resolution=low ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--leak-resolution=med ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--leak-resolution=med ");
			break;
		}
		case 2 :
		{
			longueur = longueur + strlen("--leak-resolution=high ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--leak-resolution=high ");
			break;
		}
	}
	switch (projet->memcheck.show_reachable)
	{
		case 0 :
		{
			longueur = longueur + strlen("--show-reachable=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--show-reachable=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--show-reachable=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--show-reachable=yes ");
			break;
		}
	}
	switch (projet->memcheck.undef_value_errors)
	{
		case 0 :
		{
			longueur = longueur + strlen("--undef-value-errors=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--undef-value-errors=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--undef-value-errors=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--undef-value-errors=yes ");
			break;
		}
	}
	switch (projet->memcheck.track_origins)
	{
		case 0 :
		{
			longueur = longueur + strlen("--track-origins=no ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--track-origins=no ");
			break;
		}
		case 1 :
		{
			longueur = longueur + strlen("--track-origins=yes ");
			commande = realloc(commande, longueur*sizeof(char));
			if (commande == NULL)
				BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
			strcat(commande, "--track-origins=yes ");
			break;
		}
	}
	
	longueur = longueur + strlen("--xml=yes --xml-file=rapport.xml ");
	commande = realloc(commande, longueur*sizeof(char));
	if (commande == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	strcat(commande, "--xml=yes --xml-file=rapport.xml ");
	
	longueur = longueur + strlen(projet->programme.nom_fichier) + 1;
	commande = realloc(commande, longueur*sizeof(char));
	if (commande == NULL)
		BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
	strcat(commande, projet->programme.nom_fichier);
	strcat(commande, " ");
	
	if (projet->programme.arguments != NULL)
	{
		longueur = longueur + strlen(projet->programme.arguments);
		commande = realloc(commande, longueur*sizeof(char));
		if (commande == NULL)
			BUGTEXTE(, gettext("Erreur d'allocation mémoire.\n"));
		strcat(commande, projet->programme.arguments);
	}
	
	printf("%s\n", commande);
	FILE* tmp = popen(commande, "r");
	char c;
	while(fread(&c, sizeof(char), 1, tmp))
	{
		printf("%c", (char)c);
	};
	fclose(tmp);
	free(commande);
	
	converti_rapport_valgrind("rapport.xml", projet);
	
	// On ajoute à tree_store une représentation graphique du document XML
	if (ajout_erreur(xmlDocGetRootElement(projet->document), projet->tree_store) != 0)
		BUG();
	
	return;
}

/* gtk_window_option_destroy
 * Description : Affiche les options de lancement de valgrind
 * Paramètres : GtkComboBox *widget : composant à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void gtk_window_option_destroy(GtkWidget *object __attribute__((unused)), Projet *projet __attribute__((unused)))
{
	return;
}

/* main_nouveau
 * Description : Crée un nouveau projet valgrind
 * Paramètres : GtkMenuItem *menuitem : composant GtkMenuItem à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void main_nouveau(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
{
	init_projet(projet);
	
	return;
}

/* main_options
 * Description : Affiche les options du projet
 * Paramètres : GtkMenuItem *menuitem : composant GtkMenuItem à l'origine de la demande
 *            : Projet *projet : la variable projet
 * Valeur renvoyée : Aucune
 */
void main_options(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet)
{
	GtkWidget	*pwindow;
	GtkWidget	*ptable;
	
	GtkWidget	*pnotebook;
	GtkWidget	*ptablabel;
	GtkWidget	*plabel;
	GtkWidget	*pcombobox;
	GtkWidget	*pspinbutton;
	GtkWidget	*pentry;
	GtkWidget	*pbutton;
	
	// Création de la fenêtre graphique
	pwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(pwindow), "Options du projet");
	gtk_window_set_default_size(GTK_WINDOW(pwindow), 320, 200);
	g_signal_connect(GTK_WINDOW(pwindow), "destroy", G_CALLBACK(gtk_window_option_destroy), projet);
	
	// Création du notebook contenant les options
	pnotebook = gtk_notebook_new();
	gtk_container_add(GTK_CONTAINER(pwindow), pnotebook);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(pnotebook), GTK_POS_TOP);
	
	// Création de l'onglet programme
	ptable = gtk_table_new(6, 2, FALSE);
	ptablabel = gtk_label_new("Programme");
	gtk_notebook_append_page_menu(GTK_NOTEBOOK(pnotebook), ptable, ptablabel, NULL);
	// Emplacement du programme
	plabel = gtk_label_new("Emplacement");
	if (projet->programme.nom_fichier == NULL)
		pbutton = gtk_button_new_with_label("...");
	else
		pbutton = gtk_button_new_with_label(projet->programme.nom_fichier);
	g_signal_connect(G_OBJECT(pbutton), "clicked", G_CALLBACK(entry_nom_programme), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pbutton, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// Arguments du programme
	plabel = gtk_label_new("Arguments");
	pentry = gtk_entry_new();
	if (projet->programme.arguments != NULL)
		gtk_entry_set_text(GTK_ENTRY(pentry), projet->programme.arguments);
	g_signal_connect(G_OBJECT(pentry), "changed", G_CALLBACK(entry_arguments), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pentry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	
	// Création de l'onglet Général
	ptable = gtk_table_new(9, 2, FALSE);
	ptablabel = gtk_label_new("Général");
	gtk_notebook_append_page_menu(GTK_NOTEBOOK(pnotebook), ptable, ptablabel, NULL);
	// --trace-children
	plabel = gtk_label_new("--trace-children");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->general.trace_children);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_trace_children), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --track-fds
	plabel = gtk_label_new("--track-fds");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->general.track_fds);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_track_fds), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --num-callers
	plabel = gtk_label_new("--num-callers");
	pspinbutton = gtk_spin_button_new_with_range(2, 999, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(pspinbutton), projet->general.num_callers);
	g_signal_connect(G_OBJECT(pspinbutton), "value-changed", G_CALLBACK(spin_button_num_callers), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pspinbutton, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --error-limit
	plabel = gtk_label_new("--error-limit");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->general.error_limit);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_error_limit), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --show-below-main
	plabel = gtk_label_new("--show-below-main");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->general.show_below_main);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_show_below_main), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --max-stackframe
	plabel = gtk_label_new("--max-stackframe");
	pspinbutton = gtk_spin_button_new_with_range(2, 2000000000, 1000000);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(pspinbutton), projet->general.max_stackframe);
	g_signal_connect(G_OBJECT(pspinbutton), "value-changed", G_CALLBACK(spin_button_max_stackframe), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pspinbutton, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --smc-check
	plabel = gtk_label_new("--smc-check");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "aucun");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "tas");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "tout");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->general.smc_check);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_smc_check), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 6, 7, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --read-var-info
	plabel = gtk_label_new("--read-var-info");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->general.read_var_info);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_read_var_info), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 7, 8, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 7, 8, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --verbose
	plabel = gtk_label_new("--verbose");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->general.verbose);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_verbose), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 8, 9, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 8, 9, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	
	// Création de l'onglet Memcheck
	ptable = gtk_table_new(6, 2, FALSE);
	ptablabel = gtk_label_new("Memcheck");
	gtk_notebook_append_page_menu(GTK_NOTEBOOK(pnotebook), ptable, ptablabel, NULL);
	// --leak-check
	plabel = gtk_label_new("--leak-check");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "sommaire");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "complet");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->memcheck.leak_check);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_leak_check), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --show-possibly-lost
	plabel = gtk_label_new("--show-possibly-lost");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->memcheck.show_possibly_lost);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_show_possibly_lost), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --leak-resolution
	plabel = gtk_label_new("--leak-resolution");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "bas");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "moyen");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "haut");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->memcheck.leak_resolution);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_leak_resolution), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 2, 3, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --show-reachable
	plabel = gtk_label_new("--show-reachable");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->memcheck.show_reachable);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_show_reachable), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 3, 4, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --undef-value-errors
	plabel = gtk_label_new("--undef-value-errors");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->memcheck.undef_value_errors);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_undef_value_errors), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 4, 5, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	// --track-origins
	plabel = gtk_label_new("--track-origins");
	pcombobox = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "non");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pcombobox), "oui");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pcombobox), projet->memcheck.track_origins);
	g_signal_connect(G_OBJECT(pcombobox), "changed", G_CALLBACK(combo_box_track_origins), projet);
	gtk_table_attach (GTK_TABLE (ptable), plabel, 0, 1, 5, 6, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_table_attach (GTK_TABLE (ptable), pcombobox, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_SHRINK, 0, 0);
	
	gtk_window_set_modal(GTK_WINDOW (pwindow), TRUE);
	gtk_widget_show_all(pwindow);
	
	return;
}
