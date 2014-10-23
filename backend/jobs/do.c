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

/* Higher level helper code for adapting pkgng jobs to PackageKit actions. */

#include <assert.h>		/* assert */
#include <glib.h>		/* gchar, g_... */
#include <string.h>		/* strcmp */
#include "../pk-backend.h"	/* pk_..., Pk... */
#include "pkg.h"		/* pkg_... */

#include "../db.h"		/* db_... */
#include "../namever.h"		/* namever_array_free */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../utils.h"		/* get_package_ids */
#include "check.h"		/* jobs_check_... */
#include "core.h"		/* jobs_... */
#include "do.h"			/* jobs_do_... */

static bool	jobs_do_apply(const struct jobs_spec *spec, struct pkg_jobs *jobs);
static bool	jobs_do_check(const struct jobs_spec *spec, struct pkg_jobs *jobs, gchar **package_ids, guint count);
static bool	jobs_do_count(const struct jobs_spec *spec, struct pkg_jobs *jobs);
static bool	jobs_do_multiple_repos(struct pkgdb *db, const struct jobs_spec *spec, gchar **package_ids, guint count);
static bool	jobs_do_same_repo(struct pkgdb *db, const struct jobs_spec *spec, gchar **package_ids, guint count, const char *reponame);
static bool	jobs_do_set_repo(const struct jobs_spec *spec, struct pkg_jobs *jobs, const char *reponame);
static bool	jobs_do_solve(const struct jobs_spec *spec, struct pkg_jobs *jobs);
static char   **jobs_do_populate(const struct jobs_spec *spec, struct pkg_jobs *jobs, gchar **package_ids, guint count);
static struct pkg_jobs *jobs_do_allocate(const struct jobs_spec *spec, struct pkgdb *db);
static struct pkg_jobs *jobs_do_initialise(const struct jobs_spec *spec, struct pkgdb *db, const char *reponame);

/* Performs a job from a job specification. */
bool
jobs_do(const struct jobs_spec *spec)
{
	bool		success;
	struct pkgdb   *db;

	assert(spec != NULL);
	assert(spec->backend != NULL);
	assert(spec->info != NULL);

	success = false;

	db = db_open_remote(spec->backend);
	if (db != NULL) {
		if (spec->use_package_ids) {
			guint		count;
			gchar         **package_ids;

			package_ids = get_package_ids(spec->backend, &count);
			assert(package_ids != NULL);

			success = jobs_do_multiple_repos(db, spec, package_ids,
			    count);
		} else {
			success = jobs_do_same_repo(db, spec, NULL, 0, "");
		}

		pkgdb_close(db);
	}
	return success;
}

static bool
jobs_do_apply(const struct jobs_spec *spec, struct pkg_jobs *jobs)
{
	bool		success;

	assert(spec != NULL);
	assert(jobs != NULL);

	STATUS(spec->backend, spec->status);

	if (spec->simulate) {
		success = true;
		jobs_emit_packages(jobs, spec->backend, spec->info);
	} else {
		success = jobs_apply(jobs, spec->backend,
		    spec->jobs_failed_error);
	}

	return success;
}

static bool
jobs_do_check(const struct jobs_spec *spec, struct pkg_jobs *jobs,
    gchar **package_ids, guint count)
{
	struct pkg     *failed_package;

	assert(spec != NULL);
	assert(jobs != NULL);
	assert(package_ids != NULL || 0 == count);
	assert(package_ids == NULL || 0 < count);

	failed_package = NULL;

	if (package_ids != NULL) {
		failed_package = jobs_check_package_ids(jobs, package_ids,
		    count, spec->reject_non_updates);
	}
	if (failed_package != NULL) {
		char           *message;
		char           *package_id;

		package_id = pkgutils_pkg_to_id(failed_package);
		message = NULL;
		(void)asprintf(&message,
		    "(internal error) package %s failed suitability check.",
		    package_id);
		ERR(spec->backend,
		    PK_ERROR_ENUM_DEP_RESOLUTION_FAILED,
		    message);

		g_free(package_id);
		free(message);
	}
	assert(0 < count || failed_package == NULL);

	return (failed_package == NULL);
}

static bool
jobs_do_count(const struct jobs_spec *spec, struct pkg_jobs *jobs)
{
	int		count;

	assert(spec != NULL);
	assert(jobs != NULL);

	count = pkg_jobs_count(jobs);
	if (count == 0) {
		ERR(spec->backend, spec->no_jobs_error, "nothing to do");
	}
	return (0 < count);
}

/*
 * Performs a job on a batch of PackageIDs whose repos may be different.
 */
static bool
jobs_do_multiple_repos(struct pkgdb *db, const struct jobs_spec *spec,
    gchar **package_ids, guint count)
{
	bool		success;
	unsigned int	i;
	char           *repo;
	gchar         **splits;

	/*
	 * HACK ALERT
	 * 
	 * We'd ideally like to do one job for all PackageIDs but, because there
	 * could be more than one repo being used (as each PackageID has its
	 * own repo), we need to do one job per PackageID. TODO: consider
	 * bundling PackageIDs up into separate jobs per repo?
	 */
	success = true;
	for (i = 0; i < count; i++) {
		if (!success)
			break;

		/* Nastily inefficient */
		splits = pk_package_id_split(package_ids[i]);
		if (splits == NULL) {
			repo = strdup("");
		} else {
			repo = strdup(splits[PK_PACKAGE_ID_DATA]);
		}
		g_strfreev(splits);

		success = jobs_do_same_repo(db, spec, package_ids + i,
		    1, repo);
		free(repo);
	}

	return success;
}

/*
 * Performs a job on a certain batch of PackageIDs, each with the same repo.
 * count can be 0, in which case no PackageIDs are used.
 */
static bool
jobs_do_same_repo(struct pkgdb *db, const struct jobs_spec *spec,
    gchar **package_ids, guint count, const char *reponame)
{
	bool		success;
	struct pkg_jobs *jobs;

	assert(db != NULL);
	assert(spec != NULL);
	assert(package_ids != NULL || 0 == count);
	assert(package_ids == NULL || 0 < count);

	success = false;

	jobs = jobs_do_initialise(spec, db, reponame);
	if (jobs != NULL) {
		bool		has_actions;
		bool		populated;
		bool		solved;
		bool		valid;
		char          **namevers;

		solved = has_actions = valid = false;

		namevers = jobs_do_populate(spec, jobs, package_ids, count);
		populated = (package_ids == NULL || namevers != NULL);

		if (populated) {
			solved = jobs_do_solve(spec, jobs);
		}
		if (populated && solved) {
			has_actions = jobs_do_count(spec, jobs);
		}
		if (populated && solved && has_actions) {
			valid = jobs_do_check(spec, jobs, package_ids, count);
		}
		if (populated && solved && has_actions && valid) {
			success = jobs_do_apply(spec, jobs);
		}
		pkg_jobs_free(jobs);
		namever_array_free(&namevers, count);
	}
	return success;
}

static bool
jobs_do_set_repo(const struct jobs_spec *spec, struct pkg_jobs *jobs,
    const char *reponame)
{
	bool		success;
	enum repo_set_status status;

	assert(spec != NULL);
	assert(jobs != NULL);
	/* reponame can be NULL */

	status = jobs_set_repo(jobs, reponame);
	success = (status == REPO_SET_OK);
	if (!success) {
		char           *err_message;
		const char     *detail;

		detail = jobs_repo_set_status_to_string(status);

		(void)asprintf(&err_message,
		    "Could not set job repository to '%s': %s",
		    reponame, detail);
		ERR(spec->backend,
		    PK_ERROR_ENUM_REPO_NOT_FOUND,
		    err_message);
		free(err_message);
	}
	return success;
}

static bool
jobs_do_solve(const struct jobs_spec *spec, struct pkg_jobs *jobs)
{
	bool		solved;
	int		err;

	err = pkg_jobs_solve(jobs);
	solved = (err == EPKG_OK);

	if (!solved) {
		ERR(spec->backend,
		    PK_ERROR_ENUM_DEP_RESOLUTION_FAILED,
		    "could not solve the job");
	}
	return solved;
}

static char   **
jobs_do_populate(const struct jobs_spec *spec, struct pkg_jobs *jobs,
    gchar **package_ids, guint count)
{
	char          **namevers;

	assert(spec != NULL);
	assert(jobs != NULL);
	assert(package_ids != NULL || count == 0);
	assert(package_ids == NULL || count >= 1);

	namevers = NULL;
	if (package_ids != NULL) {
		STATUS(spec->backend, PK_STATUS_ENUM_DEP_RESOLVE);

		namevers = jobs_add_package_ids(jobs, package_ids, count);
		if (namevers == NULL) {
			ERR(spec->backend,
			    PK_ERROR_ENUM_DEP_RESOLUTION_FAILED,
			    "could not find all requested packages");
		}
	}
	assert(count >= 1 || namevers == NULL);

	return namevers;
}

static struct pkg_jobs *
jobs_do_allocate(const struct jobs_spec *spec, struct pkgdb *db)
{
	struct pkg_jobs *jobs;

	assert(spec != NULL);
	assert(db != NULL);

	jobs = jobs_allocate(spec->type, db);
	if (jobs == NULL) {
		ERR(spec->backend,
		    PK_ERROR_ENUM_INTERNAL_ERROR,
		    "could not init pkg_jobs");
	}
	return jobs;
}

static struct pkg_jobs *
jobs_do_initialise(const struct jobs_spec *spec, struct pkgdb *db,
    const char *reponame)
{
	struct pkg_jobs *jobs;

	assert(spec != NULL);
	assert(db != NULL);
	/* reponame may be NULL */

	jobs = jobs_do_allocate(spec, db);
	if (jobs != NULL) {
		bool		repo_success;

		repo_success = jobs_do_set_repo(spec, jobs, reponame);
		if (!repo_success) {
			jobs_free(&jobs);
		}
	}
	return jobs;
}
