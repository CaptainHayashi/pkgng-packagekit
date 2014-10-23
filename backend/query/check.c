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
#include <stdbool.h>
#include <string.h>		/* strcmp */
#include "../pk-backend.h"
#include "pkg.h"		/* struct pkg... */

#include "../namever.h"		/* namever_... */
#include "../pkgutils.h"	/* pkgutils_... */
#include "../repo.h"		/* repo_... */
#include "../utils.h"		/* type_of_repo_name, enum repo_type */
#include "check.h"		/* query_check... */
#include "find.h"		/* query_find_... */
#include "match.h"		/* query_match_... */
#include "packages.h"		/* query_packages_... */

/* Attempts to match a set of QueryIDs into packages. */

static bool	string_match(const char *left, const char *right);
static bool	check_metadata(const char *name, const char *namever, const char *repo, const char *arch, struct query_id *query_id);

/* Returns true if the given package matches the given query ID. */
bool
query_check_package(struct pkg *package, struct query_id *query_id)
{
	bool		matches;
	char           *namever;
	const char     *arch;
	const char     *name;
	const char     *repo;

	assert(package != NULL);
	assert(query_id != NULL);
	assert(query_id->namever != NULL);

	namever = namever_from_package(package);
	repo = repo_of_package(package);
	(void)pkg_get(package, PKG_ARCH, &arch, PKG_NAME, &name);

	/* Be cautious and reject matches if the package fields aren't here. */
	if (namever == NULL || arch == NULL || repo == NULL) {
		matches = false;
	} else {
		matches = check_metadata(name, namever, repo, arch, query_id);
	}

	free(namever);
	return matches;
}

static bool
check_metadata(const char *name, const char *namever, const char *repo,
    const char *arch, struct query_id *query_id)
{
	bool		namever_matches;
	bool		namever_matches_name;
	bool		namever_matches_namever;
	bool		arch_matches;
	bool		repo_matches;

	assert(name != NULL);
	assert(namever != NULL);
	assert(arch != NULL);
	assert(repo != NULL);
	assert(query_id != NULL);
	assert(query_id->namever != NULL);

	/*
	 * Allow raw names to match the Query ID namever, as well as the full
	 * package namevers. This allows Resolve to work properly.
	 */
	namever_matches_name = string_match(query_id->namever, name);
	namever_matches_namever = string_match(query_id->namever, namever);
	namever_matches = (namever_matches_name || namever_matches_namever);

	arch_matches = string_match(query_id->arch, arch);
	repo_matches = string_match(query_id->repo, repo);

	return (namever_matches && arch_matches && repo_matches);
}

/*
 * Checks two strings with strcmp and emits true if they match. If either
 * string is NULL or empty, emit true as well (this is so that missing
 * PackageID elements trigger matches).
 */
static bool
string_match(const char *left, const char *right)
{
	bool		result;

	if (left == NULL || right == NULL)
		result = true;
	else if (*left == '\0' || *right == '\0')
		result = true;
	else
		result = (strcmp(left, right) == 0);

	return result;
}
