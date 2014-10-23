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

#include <atf-c.h>
#include "pkg.h"		/* pkg... */

#include "testutils.h"		/* Prototypes */

/*
 * Utility functions for unit tests.  These shouldn't be used for anything
 * else.
 */

struct pkg     *
gen_pkg(pkg_t type)
{
	struct pkg     *pkg;
	int		pkg_new_result;

	pkg = NULL;
	pkg_new_result = pkg_new(&pkg, type);

	ATF_REQUIRE_EQ(pkg_new_result, EPKG_OK);

	pkg_set(pkg,
	    PKG_NAME, "pkg",
	    PKG_VERSION, "1.1.4",
	    PKG_ARCH, "freebsd:10:x86:32",
	    PKG_REPONAME, "packagesite");

	return pkg;
}
