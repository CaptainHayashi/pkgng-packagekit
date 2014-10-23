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

/* String/ID utilities. */

#include <assert.h>		/* assert */
#include <string.h>

#include <glib.h>
#include "pkg.h"
#include "pk-backend.h"		/* pk_backend_get_strv */

#include "utils.h"		/* prototypes */

/*
 * Retrieves PackageIDs from the backend, as well as the total number of
 * them.
 */
gchar         **
get_package_ids(PkBackend *backend, guint * count_p)
{

	assert(backend != NULL);
	assert(count_p != NULL);
	return get_strv_and_length(backend, "package_ids", count_p);
}

/*
 * Retrieves a string vector from the backend, as well as the number of
 * strings pointed to by it.
 */
gchar         **
get_strv_and_length(PkBackend *backend, const char *name, guint * count_p)
{
	gchar         **strings;

	assert(backend != NULL);
	assert(count_p != NULL);

	strings = pk_backend_get_strv(backend, name);
	assert(strings != NULL);

	*count_p = g_strv_length(strings);

	return strings;
}
