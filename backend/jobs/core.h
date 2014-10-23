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

#ifndef _PKGNG_BACKEND_JOBS_CORE_H_
#define _PKGNG_BACKEND_JOBS_CORE_H_

#include <stdbool.h>		/* bool */
#include "../pk-backend.h"	/* PkBackend */

enum repo_set_status {
	REPO_SET_OK,
	REPO_SET_INVALID_REPO,
	REPO_SET_DISABLED_REPO,
	REPO_SET_FAILURE
};

typedef		PkInfoEnum(*pkg_info_ptr) (struct pkg *pkg);

bool		jobs_apply(struct pkg_jobs *jobs, PkBackend *backend, PkErrorEnum jobs_failed);
enum repo_set_status jobs_set_repo(struct pkg_jobs *jobs, const char *reponame);
char          **jobs_add_package_ids(struct pkg_jobs *jobs, gchar **package_ids, guint count);
const char     *jobs_repo_set_status_to_string(enum repo_set_status status);
struct pkg_jobs *jobs_allocate(pkg_jobs_t type, struct pkgdb *db);
void		jobs_emit_packages(struct pkg_jobs *jobs, PkBackend *backend, pkg_info_ptr info);
void		jobs_free (struct pkg_jobs **jobs_p);

#endif				/* !_PKGNG_BACKEND_JOBS_CORE_H_ */
