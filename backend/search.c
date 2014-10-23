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

#include <assert.h>		/* assert */
#include <stdbool.h>		/* bool */
#include "pkg.h"		/* pkg_... */

#include "emit.h"		/* emit_... */
#include "pkgutils.h"		/* pkgutils_... */
#include "search.h"		/* search_... */

static void	search_do_with_it(struct search *search, struct pkgdb_it *it);

/*
 * Performs the search specified by the given "struct search" and emits each
 * result according to the likewise-specified filter set.
 */
bool
search_do(struct search *search)
{
	struct pkgdb_it *it;

	assert(search != NULL);

	it = pkgdb_search(search->db,
	    search->term,
	    search->type,
	    search->in,
	    search->sort_by,
	    NULL);

	if (it != NULL) {
		search_do_with_it(search, it);
		pkgdb_it_free(it);
	}
	return (it != NULL);
}

static void
search_do_with_it(struct search *search, struct pkgdb_it *it)
{
	struct pkg     *pkg;

	assert(search != NULL);
	assert(it != NULL);

	pkg = NULL;
	while (pkgdb_it_next(it, &pkg, PKG_LOAD_BASIC) == EPKG_OK) {
		assert(pkg != NULL);

		pkgutils_add_old_version(search->db, pkg, NULL);
		emit_filtered_package(pkg,
		    search->backend,
		    search->filters,
		    pkgutils_pkg_current_state(pkg));
	}

	pkg_free(pkg);
}
