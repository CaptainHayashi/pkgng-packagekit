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

#ifndef _PKGNG_BACKEND_QUERY_MATCH_H_
#define _PKGNG_BACKEND_QUERY_MATCH_H_

#include <stdbool.h>
#include "pkg.h"

#include "error.h"		/* enum query_error */
#include "id.h"			/* struct query_id */

struct pkg     *query_match_id(struct query_id *query_id, struct pkgdb *db, unsigned int load_flags, enum query_error *error_p);
struct pkg    **query_match_ids(struct query_id *query_ids, unsigned int count, struct pkgdb *db, unsigned int load_flags, enum query_error *error_p);

#endif				/* !_PKGNG_BACKEND_QUERY_MATCH_H_ */
