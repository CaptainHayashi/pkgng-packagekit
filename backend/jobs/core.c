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

/* Helpers for jobs that don't rely on the query system. */

#include <assert.h>		/* assert */
#include <glib.h>		/* gchar, g_... */
#include <stdbool.h>		/* bool, true, false */
#include <string.h>		/* strcmp */
#include "../pk-backend.h"	/* pk_..., Pk... */
#include "pkg.h"		/* pkg_... */

#include "../db.h"		/* db_... */
#include "../emit.h"		/* emit_package */
#include "../event.h"		/* event_cb */
#include "../namever.h"		/* namever_* */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../repo.h"		/* repo_... */
#include "../utils.h"		/* ERR */
#include "check.h"		/* jobs_check_... */
#include "core.h"		/* jobs_... */

static const char *APPLY_ERRORS[] = {
	"This shouldn't occur and is a bug.",	/* EPKG_OK */
	"This shouldn't occur and is a bug.",	/* EPKG_END */
	"This shouldn't occur and is a bug.",	/* EPKG_WARN */
	"Fatal error.",		/* EPKG_FATAL */
	"Package required by another package.",	/* EPKG_REQUIRED */
	"Already installed.",	/* EPKG_INSTALLED */
	"Unresolved dependencies.",	/* EPKG_DEPENDENCY */
	"A package is currently locked.",	/* EPKG_LOCKED */
	"No usable database available.",	/* EPKG_ENODB */
	"Local file newer than remote file.",	/* EPKG_UPTODATE */
	"Unknown keyword.",	/* EPKG_UNKNOWN */
	"Incompatible repo schema version",	/* EPKG_REPOSCHEMA */
	"Insufficient privilege for action.",	/* EPKG_ENOACCESS */
	"Insecure privileges on DB file."	/* EPKG_INSECURE */
};

/* Applies a job with the given error enums and the standard event callback. */
bool
jobs_apply(struct pkg_jobs *jobs, PkBackend *backend, PkErrorEnum job_failed)
{
	int		err;

	assert(backend != NULL);

	pkg_event_register(event_cb, backend);
	{
		err = pkg_jobs_apply(jobs);
	}
	pkg_event_register(NULL, NULL);

	assert(EPKG_OK <= err);
	assert(err <= EPKG_INSECURE);
	if (err != EPKG_OK) {
		const gchar    *error_string;

		/* Did we trap an error in the events handler? */
		error_string = pk_backend_get_string(backend, "job_error");
		if (error_string == NULL) {
			error_string = APPLY_ERRORS[err];
		}
		assert(error_string != NULL);

		ERR(backend, job_failed, error_string);
	}
	return (err == EPKG_OK);
}

enum repo_set_status
jobs_set_repo(struct pkg_jobs *jobs, const char *reponame)
{
	enum repo_set_status status;
	enum repo_type	type;

	assert(jobs != NULL);

	type = repo_type(reponame);
	if (type == REPO_INVALID) {
		status = REPO_SET_INVALID_REPO;
	} else if (type == REPO_INVALID) {
		status = REPO_SET_DISABLED_REPO;
	} else if (type == REPO_REMOTE) {
		int		err;

		err = pkg_jobs_set_repository(jobs,
		    pkg_repo_ident_from_name(reponame));
		status = (err == EPKG_OK) ? REPO_SET_OK : REPO_SET_FAILURE;
	} else {
		/* Ignore any other type of repository. */
		status = REPO_SET_OK;
	}

	return status;
}

/*
 * Adds each PackageID into an already created job.  Returns NULL on failure
 * and a vector of added package name-versions to be freed after solution on
 * success.
 */
char          **
jobs_add_package_ids(struct pkg_jobs *jobs, gchar **package_ids, guint count)
{
	char          **namevers;

	assert(jobs != NULL);
	assert(package_ids != NULL);

	namevers = NULL;

	if (count > 0) {
		int		err;

		namevers = namever_array_from_package_ids(package_ids, count);

		err = pkg_jobs_add(jobs, MATCH_EXACT, namevers, (int)count);
		if (err != EPKG_OK) {
			namever_array_free(&namevers, count);
		}
	}
	return namevers;
}

/* Converts a repo-set result to a string constant. */
const char     *
jobs_repo_set_status_to_string(enum repo_set_status status)
{
	const char     *detail;

#ifndef __clang__
	detail = "Detail message not set.";
#endif

	switch (status) {
	case REPO_SET_OK:
		detail = "This shouldn't happen.";
		break;
	case REPO_SET_INVALID_REPO:
		detail = "This repo doesn't seem to exist.";
		break;
	case REPO_SET_DISABLED_REPO:
		detail = "This repo is disabled.";
		break;
	case REPO_SET_FAILURE:
		detail = "Error while setting repo.";
		break;
	}

	return detail;
}

/* Emits each package queued up in a (solved) job. */
void
jobs_emit_packages(struct pkg_jobs *jobs, PkBackend *backend,
    pkg_info_ptr info)
{
	struct pkg     *pkg;

	assert(jobs != NULL);
	assert(backend != NULL);
	assert(info != NULL);

	pkg = NULL;
	while (pkg_jobs(jobs, &pkg) == EPKG_OK) {
		assert(pkg != NULL);
		emit_package(pkg, backend, info(pkg));
	}
}

/* Overly safe wrapper around pkg_jobs_new. */
struct pkg_jobs *
jobs_allocate(pkg_jobs_t type, struct pkgdb *db)
{
	int		err;
	struct pkg_jobs *jobs;

	assert(db != NULL);

	jobs = NULL;
	err = pkg_jobs_new(&jobs, type, db);
	if (err != EPKG_OK && jobs != NULL) {
		jobs_free(&jobs);
	}
	assert(err == EPKG_OK || jobs == NULL);
	assert(err != EPKG_OK || jobs != NULL);

	return jobs;
}

/* Overly safe wrapper around pkg_jobs_free. */
void
jobs_free(struct pkg_jobs **jobs_p)
{

	assert(jobs_p != NULL);
	pkg_jobs_free(*jobs_p);
	*jobs_p = NULL;
}
