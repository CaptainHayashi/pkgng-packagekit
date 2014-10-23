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

/* Functions for dealing with PackageKit repositories. */

#include <assert.h>
#include <string.h>

#include "pkg.h"		/* pkg_..., struct pkg */
#include "repo.h"		/* repo_... */

static const char *repo_of_remote_package(struct pkg *pkg);

/*
 * Finds the type of the given PackageKit repository name.
 * 
 * This function checks remote repositories to ensure they match actually
 * configured repositories, and instead returns 'REPO_INVALID' when they
 * don't. A separate function 'repo_type_no_remote_check' does not do this
 * and can thus be unit tested.
 */
enum repo_type
repo_type(const char *name)
{
	enum repo_type	type;

	/* 'name' may be NULL.  This implies REPO_ANY. */

	type = repo_type_no_remote_check(name);

	if (type == REPO_REMOTE) {
		struct pkg_repo *repo;
		repo = pkg_repo_find_name(name);

		if (repo == NULL) {
			type = REPO_INVALID;
		} else if (!pkg_repo_enabled(repo)) {
			type = REPO_DISABLED;
		}
	}
	return type;
}

/* As repo_type, but without the remote repository check. */
enum repo_type
repo_type_no_remote_check(const char *name)
{
	enum repo_type	type;

	/* Null or empty implies no specific repository */
	if (name == NULL || name[0] == '\0') {
		type = REPO_ANY;
	} else if (strcmp(name, "installed") == 0) {
		type = REPO_LOCAL;
	} else {
		type = REPO_REMOTE;
	}

	return type;
}

/*
 * Gets the PackageKit repository name for the package.
 */
const char     *
repo_of_package(struct pkg *package)
{
	const char     *repo;

	assert(package != NULL);

	/*
	 * clang complains about this redundant assignment, but gcc complains
	 * if it doesn't exist.
	 */
#ifndef	__clang__
	repo = "";
#endif				/* __clang */

	switch (pkg_type(package)) {
	case PKG_OLD_FILE:
	case PKG_FILE:
		repo = "local";
		break;
	case PKG_INSTALLED:
		repo = "installed";
		break;
	case PKG_REMOTE:
		repo = repo_of_remote_package(package);
		break;
	case PKG_NONE:
		repo = "unknown";
		break;
	}

	assert(repo != NULL);
	return repo;
}

/*
 * Gets the PackageKit repository name for the (remote) package.
 * 
 * Currently this is the pkgng repository name (not the ident as was previously
 * the case).
 * 
 * This does not need to be freed (possibly, TODO: check).
 */
static const char *
repo_of_remote_package(struct pkg *pkg)
{
	const char     *repo_name;

	assert(pkg != NULL);
	assert(pkg_type(pkg) == PKG_REMOTE);

	repo_name = NULL;
	pkg_get(pkg, PKG_REPONAME, &repo_name);
	return repo_name;
}
