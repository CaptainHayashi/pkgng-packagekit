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

#include <assert.h>
#include <string.h>
#include <glib.h>
#include "pk-backend.h"
#include "pkg.h"

#include "namever.h"		/* namever_... */
#include "pkgutils.h"		/* Prototypes */
#include "repo.h"		/* repo_... */
#include "utils.h"		/* INTENTIONALLY_IGNORE */

/*
 * Package utility functions.
 * 
 * This file CANNOT depend on the PackageKit backend.
 */

/*
 * Infers the correct PkInfoEnum to emit for this package for its current
 * state.
 */
PkInfoEnum
pkgutils_pkg_current_state(struct pkg *pkg)
{
	bool		locally_installed;
	bool		remote_but_already_installed;
	PkInfoEnum	info;

	assert(pkg != NULL);

	/*
	 * If the package is local, then it's installed.  If it is remote,
	 * then it is installed if it is the same version as an installed
	 * package, and available if it is not.
	 */
	locally_installed = (pkg_type(pkg) == PKG_INSTALLED);
	remote_but_already_installed = (pkgutils_pkg_install_state(pkg) ==
	    PK_INFO_ENUM_REINSTALLING);

	if (locally_installed || remote_but_already_installed) {
		info = PK_INFO_ENUM_INSTALLED;
	} else {
		info = PK_INFO_ENUM_AVAILABLE;
	}

	return info;
}

/*
 * Infers the correct PkInfoEnum to emit for this package if it is being
 * installed, mainly from its current installation information.
 */
PkInfoEnum
pkgutils_pkg_install_state(struct pkg *pkg)
{
	PkInfoEnum	info;
	const char     *old;

	assert(pkg != NULL);

	/* Sane default */
	info = PK_INFO_ENUM_INSTALLING;

	/* Does this package already have a version installed? */
	old = NULL;
	pkg_get(pkg, PKG_OLD_VERSION, &old);

	if (old != NULL) {
		/* Yes, but how does it compare to the new one? */
		switch (pkg_version_change(pkg)) {
		case PKG_DOWNGRADE:
			info = PK_INFO_ENUM_DOWNGRADING;
			break;
		case PKG_REINSTALL:
			info = PK_INFO_ENUM_REINSTALLING;
			break;
		case PKG_UPGRADE:
			info = PK_INFO_ENUM_UPDATING;
			break;
		}
	}
	return info;
}

/*
 * Infers the correct PkInfoEnum to emit for this package if it is being
 * removed.
 */
PkInfoEnum
pkgutils_pkg_remove_state(struct pkg *pkg)
{

	INTENTIONALLY_IGNORE(pkg);
	return PK_INFO_ENUM_REMOVING;
}

/*
 * Converts a package to a PackageID.
 */
gchar          *
pkgutils_pkg_to_id(struct pkg *pkg)
{
	const char     *name;
	const char     *version;
	const char     *arch;
	const char     *repo;

	assert(pkg != NULL);

	pkg_get(pkg, PKG_NAME, &name, PKG_VERSION, &version, PKG_ARCH, &arch);

	repo = repo_of_package(pkg);

	return pk_package_id_build(name, version, arch, repo);
}

/*
 * Adds any old version of the given package to itself as PKG_OLD_VERSION.
 * 
 * If old_p is given, the package representing the old version is retained and
 * pointed to through old_p; else it is freed.
 */
void
pkgutils_add_old_version(struct pkgdb *db, struct pkg *pkg, struct pkg **old_p)
{
	const char     *origin;
	struct pkg     *pkg2;

	assert(db != NULL);
	assert(pkg != NULL);

	origin = NULL;
	pkg_get(pkg, PKG_ORIGIN, &origin);

	pkg2 = NULL;
	if (pkg_try_installed(db, origin, &pkg2, PKG_LOAD_BASIC) == EPKG_OK) {
		const char     *version;

		assert(pkg2 != NULL);

		pkg_get(pkg2, PKG_VERSION, &version);
		pkg_set(pkg, PKG_OLD_VERSION, version);
	}
	if (old_p != NULL) {
		*old_p = pkg2;
	} else {
		pkg_free(pkg2);
	}
	assert(old_p == NULL || pkg2 == NULL || *old_p != NULL);
}

bool
pkgutils_pkg_matches_filters(struct pkg *pkg, PkBitfield filters)
{
	bool		matches_filters;
	PkFilterEnum	wrong_filter;

	assert(pkg != NULL);

	matches_filters = true;

	if (pkgutils_pkg_current_state(pkg) == PK_INFO_ENUM_INSTALLED) {
		wrong_filter = PK_FILTER_ENUM_NOT_INSTALLED;
	} else {
		wrong_filter = PK_FILTER_ENUM_INSTALLED;
	}

	/* TODO: more filters? */

	if (pk_bitfield_contain(filters, wrong_filter) == TRUE) {
		matches_filters = false;
	}
	return matches_filters;
}
