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

#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include "struct.h"

void main_nouveau(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet);
void main_options(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet);
void main_valgrind(GtkMenuItem *menuitem __attribute__((unused)), Projet *projet);

#endif