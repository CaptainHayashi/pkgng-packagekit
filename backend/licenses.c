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

#include <errno.h>		/* errno */
#include <string.h>		/* strdup, strlen, strerror */

#include "pkg.h"		/* struct pkg, etc */
#include "pk-backend.h"		/* PkLicenseEnum, PK_* */

#include "licenses.h"		/* prototypes */

/*
 * Retrieves the name of the license combination attached to the package. The
 * name must be free(3)d when finished with.
 */
char           *
license_name_from_pkg(struct pkg *pkg)
{
	/* TODO: handle conjunctions and disjunctions */
	int		sb_err;
	lic_t		logic;
	char           *license_dup;
	const char     *license;
	const char     *logic_str;
	struct pkg_license *lic;
	struct sbuf    *sb;

	/*
	 * GCC worries about this being NULL, so initialise it here. Clang
	 * worries about the lack of enum saturation, so it doesn't ever get
	 * used as a default though.
	 */
#ifndef __clang__
	logic_str = " ";
#endif				/* !__clang__ */

	pkg_get(pkg, PKG_LICENSE_LOGIC, &logic);
	switch (logic) {
	case LICENSE_OR:
		logic_str = " or ";
		break;
	case LICENSE_AND:
		logic_str = " and ";
		break;
	case LICENSE_SINGLE:
		logic_str = " ";
		break;
	}

	/* Join all the licenses together with the logic word above. */
	lic = NULL;
	sb = sbuf_new_auto();
	while (pkg_licenses(pkg, &lic) == EPKG_OK) {
		(void)sbuf_cat(sb, logic_str);
		(void)sbuf_cat(sb, pkg_license_name(lic));
	}
	/*
	 * Stop the following code from bombing if there was no license
	 * available.
	 */
	if (sbuf_len(sb) == 0)
		(void)sbuf_cat(sb, logic_str);

	sb_err = sbuf_finish(sb);
	/* Make sure that we remove the initial logic string instance! */
	license = (sb_err
	    ? strerror(errno)
	    : (sbuf_data(sb) + strlen(logic_str)));

	license_dup = strdup(license);
	sbuf_delete(sb);
	return license_dup;
}
