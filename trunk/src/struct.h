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

#ifndef __STRUCT_H
#define __STRUCT_H

#include <gtk/gtk.h>
#include <libxml/tree.h>

typedef struct __Opts_Gen
{
	int	trace_children;		// 0 = no*
					// 1 = yes
	int	track_fds;		// 0 = no
					// 1 = yes*
	int	num_callers;		// 2 à 50*
	int	error_limit;		// 0 = no*
					// 1 = yes
	int	show_below_main;	// 0 = no
					// 1 = yes*
	int	max_stackframe;		// 2000000*
	int	smc_check;		// 0 = none
					// 1 = stack
					// 2 = all*
	int	read_var_info;		// 0 = no*
					// 1 = yes
	int	verbose;		// 0 = no
					// 1 = yes*
} Opts_Gen;

#define GVAL_DEFAUT_TRACE_CHILDREN		0
#define GVAL_DEFAUT_TRACK_FDS			1
#define GVAL_DEFAUT_NUM_CALLERS			50
#define GVAL_DEFAUT_ERROR_LIMIT			0
#define GVAL_DEFAUT_SHOW_BELOW_MAIN		1
#define GVAL_DEFAUT_MAX_STACKFRAME		2000000
#define GVAL_DEFAUT_SMC_CHECK			2
#define GVAL_DEFAUT_READ_VAR_INFO		0
#define GVAL_DEFAUT_VERBOSE			1

typedef struct __Opts_Memcheck
{
	int	leak_check;		// 0 = no
					// 1 = summary
					// 2 = yes
					// 3 = full*
	int	show_possibly_lost;	// 0 = no
					// 1 = yes*
	int	leak_resolution;	// 0 = low
					// 1 = med
					// 2 = high*
	int	show_reachable;		// 0 = no
					// 1 = yes*
	int	undef_value_errors;	// 0 = no
					// 1 = yes*
	int	track_origins;		// 0 = no*
					// 1 = yes
} Opts_Memcheck;

#define GVAL_DEFAUT_LEAK_CHECK		3
#define GVAL_DEFAUT_SHOW_POSSIBLY_LOST	1
#define GVAL_DEFAUT_LEAK_RESOLUTION	2
#define GVAL_DEFAUT_SHOW_REACHABLE	1
#define GVAL_DEFAUT_UNDEF_VALUE_ERRORS	1
#define GVAL_DEFAUT_TRACK_ORIGINS	0

typedef struct __Opts_Prog
{
	char		*environnement;
	char		*nom_fichier;
	char		*arguments;
	char		*dossier_courant;
	char		*enregistrement;
} Opts_Prog;

typedef struct __Projet
{
	GtkTreeStore	*tree_store;
	GtkTreeView	*tree_view;
	xmlDocPtr	document;
	Opts_Prog	programme;
	Opts_Gen	general;
	Opts_Memcheck	memcheck;
	int		modifie;
} Projet;

#endif
