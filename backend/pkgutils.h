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

#ifndef	_PKGNG_BACKEND_PKGUTILS_H_
#define	_PKGNG_BACKEND_PKGUTILS_H_

#include <glib.h>
#include "pk-backend.h"
#include "pkg.h"

PkInfoEnum	pkgutils_pkg_current_state(struct pkg *pkg);
PkInfoEnum	pkgutils_pkg_install_state(struct pkg *pkg);
PkInfoEnum	pkgutils_pkg_remove_state(struct pkg *pkg);
bool		pkgutils_pkg_matches_filters(struct pkg *pkg, PkBitfield filters);
gchar          *pkgutils_pkg_to_id(struct pkg *pkg);
const char     *pkgutils_pkg_to_pk_repo(struct pkg *pkg);
void		pkgutils_add_old_version(struct pkgdb *db, struct pkg *pkg, struct pkg **old_p);

#endif				/* !_PKGNG_BACKEND_PKGUTILS_H_ */
