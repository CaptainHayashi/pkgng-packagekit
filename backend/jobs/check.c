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

/* Functions for checking that a job matches up with a set of PackageIDs. */

#include <assert.h>		/* assert */
#include <glib.h>		/* gchar, g_... */
#include <stdbool.h>		/* bool, true, false */
#include <string.h>		/* strcmp */
#include "../pk-backend.h"	/* pk_..., Pk... */
#include "pkg.h"		/* pkg_... */

#include "../namever.h"		/* namever_... */
#include "../utils.h"		/* ERR */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../query.h"		/* query_... */
#include "check.h"		/* jobs_check_... */

static bool	check_package(struct pkg *pkg, struct query_id *query_ids, guint count, bool reject_non_updates);
static struct pkg *jobs_check_query_ids(struct pkg_jobs *jobs, struct query_id *query_ids, guint count, bool reject_non_updates);

/*
 * Checks a solved job against a string vector of PackageIDs to ensure any
 * packages that match the PackageIDs match them fully.
 * 
 * Returns NULL if the check succeeded, and a struct pkg if that package failed
 * the check.
 */
struct pkg     *
jobs_check_package_ids(struct pkg_jobs *jobs, gchar **package_ids,
    guint count, bool reject_non_updates)
{
	struct pkg     *package;
	struct query_id *query_ids;

	assert(jobs != NULL);
	assert(package_ids != NULL);
	assert(0 < count);

	package = NULL;

	query_ids = query_id_array_from_package_ids(package_ids, count);
	if (query_ids != NULL) {

		/* Now do the actual checking, per package. */
		package = jobs_check_query_ids(jobs, query_ids, count,
		    reject_non_updates);
	}
	return package;
}

/*
 * Checks a package and its pre-computed namever against a Query ID to ensure
 * that if the ID refers to the same package name and version, the
 * architecture and repository also match.
 * 
 * Exposed publicly to allow for unit testing.
 */
bool
jobs_check_id_on_package(struct pkg *pkg, struct query_id *query_id,
    const char *namever)
{
	bool		success;

	assert(pkg != NULL);
	assert(query_id != NULL);
	assert(query_id->namever != NULL);
	assert(namever != NULL);

	success = true;

	/* Does this package's name and version match a PackageID? */
	if (strcmp(namever, query_id->namever) == 0) {
		/* Does the rest of the PackageID match up? */
		success = query_check_package(pkg, query_id);
	}
	return success;
}

static struct pkg *
jobs_check_query_ids(struct pkg_jobs *jobs, struct query_id *query_ids,
    guint count, bool reject_non_updates)
{
	bool		success;
	int		err;
	struct pkg     *package;

	assert(jobs != NULL);
	assert(query_ids != NULL);
	assert(0 < count);

	package = NULL;
	success = true;
	while (success) {
		err = pkg_jobs(jobs, &package);
		if (err == EPKG_OK) {
			success = check_package(package, query_ids,
			    count, reject_non_updates);
		} else if (err == EPKG_END) {
			/*
			 * Only return a package if it failed to match. If
			 * we've reached this far all of them have matched,
			 * so don't return one!
			 */
			package = NULL;
			break;
		} else {
			/* Abnormal termination */
			success = false;
		}
	}

	return package;
}

/*
 * Checks a single job target package against a set of candidate query IDs to
 * make sure that the originally requested packages in the job have been
 * resolved properly.
 */
static bool
check_package(struct pkg *package, struct query_id *query_ids,
    guint count, bool reject_non_updates)
{
	bool		success;
	guint		i;
	char           *namever;

	assert(package != NULL);

	success = true;

	namever = namever_from_package(package);
	if (namever == NULL) {
		success = false;
	}
	for (i = 0; i < count; i++) {
		if (!success) {
			break;
		}
		assert(success);
		success = jobs_check_id_on_package(package,
		    query_ids + i,
		    namever);

		if (success && reject_non_updates &&
		    pkgutils_pkg_install_state(package) !=
		    PK_INFO_ENUM_UPDATING) {
			success = false;
		}
	}

	/* Do not free the struct pkg, we actually don't own it. */

	return success;
}
