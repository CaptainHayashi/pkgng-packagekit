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

#ifndef _PKGNG_BACKEND_QUERY_FIND_H_
#define _PKGNG_BACKEND_QUERY_FIND_H_

#include "pkg.h"		/* struct pkgdb */
#include "id.h"			/* struct query_id */
#include "error.h"		/* enum query_error */

struct query_find_spec {
	unsigned int	load_flags;

	enum query_error *error_p;
	struct pkgdb   *db;
	struct query_id *query_id;
};

typedef struct pkgdb_it *(*query_find_function) (struct query_find_spec *spec);

struct pkgdb_it *query_find_local(struct query_find_spec *spec);
struct pkgdb_it *query_find_remote(struct query_find_spec *spec);

#endif				/* !_PKGNG_BACKEND_QUERY_FIND_H_ */
