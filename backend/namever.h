/*-
 * Copyright (C) 2013 Matt Windsor <mattbw@FreeBSD.org>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _PKGNG_BACKEND_NAMEVER_H_
#define _PKGNG_BACKEND_NAMEVER_H_

#include <glib.h>		/* gchar, guint */
#include "pkg.h"		/* struct pkg */

char           *namever_from_name_and_version(const char *name, const char *version);
char           *namever_from_package(struct pkg *package);
char           *namever_from_package_id(const gchar *package_id);
char          **namever_array_from_package_ids(gchar **package_ids, guint count);
void		namever_array_free(char ***namevers_p, guint count);

#endif				/* !_PKGNG_BACKEND_NAMEVER_H_ */
