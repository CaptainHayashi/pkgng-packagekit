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
#include "pkg.h"		/* pkg... */
#include "../pk-backend.h"	/* PkBackend */

#include "../emit.h"		/* emit_package */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../utils.h"		/* ERR */
#include "depends.h"		/* query_depends_... */
#include "error.h"		/* query_error_... */
#include "id.h"			/* query_id, query_id_... */
#include "match.h"		/* query_match_... */

static bool	emit_dependency(struct pkg_dep *dep, PkBackend *backend, struct pkgdb *db, unsigned int load_flags);
static bool	emit_found_dependency(PkBackend *backend, struct pkg *package);
static bool	emit_missing_dependency(PkBackend *backend, gchar *package_id);
static gchar   *dep_to_package_id(struct pkg_dep *dep);
static struct pkg *id_to_package(gchar *package_id, struct pkgdb *db, unsigned int load_flags);

bool
query_depends_emit(PkBackend *backend, struct pkg *pkg, depends_get_ptr get,
    struct pkgdb *db, unsigned int load_flags)
{
	bool		success;
	struct pkg_dep *dep;

	assert(backend != NULL);
	assert(pkg != NULL);
	assert(get != NULL);

	success = true;

	dep = NULL;
	while (get(pkg, &dep) == EPKG_OK) {
		success = emit_dependency(dep, backend, db, load_flags);
		if (!success) {
			break;
		}
	}

	return success;
}

static bool
emit_dependency(struct pkg_dep *dep, PkBackend *backend, struct pkgdb *db,
    unsigned int load_flags)
{
	bool		success;
	gchar          *package_id;
	struct pkg     *package;

	package_id = dep_to_package_id(dep);

	/*
	 * Does this dependency correspond to a package in our database?
	 */

	package = id_to_package(package_id, db, load_flags);
	if (package == NULL) {
		success = emit_missing_dependency(backend, package_id);
	} else {
		success = emit_found_dependency(backend, package);
	}

	g_free(package_id);
	return success;
}

/* Emits a dependency that has been found in the database. */
static bool
emit_found_dependency(PkBackend *backend, struct pkg *package)
{

	emit_package(package, backend, pkgutils_pkg_current_state(package));
	return true;
}


/* Emits a dependency that has not been found in the database. */
static bool
emit_missing_dependency(PkBackend *backend, gchar *package_id)
{

	return pk_backend_package(backend,
	    PK_INFO_ENUM_UNKNOWN,
	    package_id,
	    "Could not find this package.");
}

/* Attempts to match a dependency to a package. */
static struct pkg *
id_to_package(gchar *package_id, struct pkgdb *db, unsigned int load_flags)
{
	enum query_error error;
	struct pkg     *package;
	struct query_id	query_id;

	assert(package_id != NULL);
	assert(db != NULL);

	error = QUERY_ERROR_OK;
	package = NULL;

	if (package_id != NULL) {
		bool		success;

		success = query_id_from_package_id(package_id, &query_id);
		if (success) {
			package = query_match_id(&query_id, db, load_flags,
			    &error);
			assert(error == QUERY_ERROR_OK || package != NULL);
			assert(error != QUERY_ERROR_OK || package == NULL);
		}
	}
	return package;
}

/*
 * Converts a pkg_dep into a (partial) package ID.
 * 
 * The result may be NULL and should be freed using g_free.
 */
static gchar   *
dep_to_package_id(struct pkg_dep *dep)
{
	const char     *name;
	const char     *version;

	assert(dep != NULL);

	name = pkg_dep_get(dep, PKG_DEP_NAME);
	version = pkg_dep_get(dep, PKG_DEP_VERSION);

	return pk_package_id_build(name, version, "", "");
}
