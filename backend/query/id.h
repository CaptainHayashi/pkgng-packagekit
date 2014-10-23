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

#ifndef _PKGNG_BACKEND_QUERY_ID_H_
#define _PKGNG_BACKEND_QUERY_ID_H_

#include <stdbool.h>
#include "../pk-backend.h"
#include "pkg.h"

struct query_id {
	char           *namever;
	char           *arch;
	char           *repo;
};

bool		query_id_from_name(const char *name, struct query_id *query_id);
bool		query_id_from_package_id(const gchar *package_id, struct query_id *query_id);
struct query_id *query_id_array_from_names(gchar **package_names, unsigned int package_count);
struct query_id *query_id_array_from_package_ids(gchar **package_ids, unsigned int package_count);
void		query_id_array_free(struct query_id **query_ids_p, unsigned int count);
void		query_id_free(struct query_id **query_id_p);

#endif				/* !_PKGNG_BACKEND_QUERY_ID_H_ */
