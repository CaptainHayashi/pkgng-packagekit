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

#include <stdbool.h>		/* bool */
#include "pk-backend.h"		/* Pk... */
#include "pkg.h"		/* match_t, pkgdb... */

#ifndef _PKGNG_BACKEND_SEARCH_H_
#define _PKGNG_BACKEND_SEARCH_H_

/*
 * Structure collating the various inputs to a search.
 */
struct search {
	match_t		type;
	pkgdb_field	in;
	pkgdb_field	sort_by;
	PkBitfield	filters;
	char           *term;
	struct pkgdb   *db;
	PkBackend      *backend;
};

bool		search_do (struct search *search);

#endif				/* _PKGNG_BACKEND_SEARCH_H_ */
