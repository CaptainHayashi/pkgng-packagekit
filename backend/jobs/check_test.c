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
#include <stdlib.h>		/* free */
#include <string.h>		/* strcmp */
#include "pkg.h"		/* pkg... */

#include "check.h"		/* job_check... */
#include "../testutils.h"	/* gen_pkg */

/* ATF/kyua tests for 'check.c'. */

ATF_TC(check_id_on_package_no_match);
ATF_TC_HEAD(check_id_on_package_no_match, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure package/ID check succeeds if package and ID don't match.");
}
ATF_TC_BODY(check_id_on_package_no_match, tc)
{
	struct pkg     *package;
	struct query_id	id;

	package = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(package != NULL);

	id.namever = strdup("nonsuch-1.2.3");
	id.arch = strdup("freebsd:10:x86:32");
	id.repo = strdup("packagesite");

	ATF_CHECK(jobs_check_id_on_package(package, &id, "pkg-1.1.4"));

	pkg_free(package);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

ATF_TC(check_id_on_package_partial_match);
ATF_TC_HEAD(check_id_on_package_partial_match, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure package/ID check fails if only the namever matches.");
}
ATF_TC_BODY(check_id_on_package_partial_match, tc)
{
	struct pkg     *package;
	struct query_id	id;

	package = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(package != NULL);

	id.namever = strdup("pkg-1.1.4");
	id.arch = strdup("freebsd:10:x86:32");
	id.repo = strdup("nope");

	ATF_CHECK(!jobs_check_id_on_package(package, &id, "pkg-1.1.4"));

	free(id.arch);
	free(id.repo);

	id.arch = strdup("nope");
	id.repo = strdup("packagesite");

	ATF_CHECK(!jobs_check_id_on_package(package, &id, "pkg-1.1.4"));

	pkg_free(package);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

ATF_TC(check_id_on_package_full_match);
ATF_TC_HEAD(check_id_on_package_full_match, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure package/ID check succeeds if everything matches.");
}
ATF_TC_BODY(check_id_on_package_full_match, tc)
{
	struct pkg     *package;
	struct query_id	id;

	package = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(package != NULL);

	id.namever = strdup("pkg-1.1.4");
	id.arch = strdup("freebsd:10:x86:32");
	id.repo = strdup("packagesite");

	ATF_CHECK(jobs_check_id_on_package(package, &id, "pkg-1.1.4"));

	pkg_free(package);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

/*
 * TEST PACK
 */

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, check_id_on_package_no_match);
	ATF_TP_ADD_TC(tp, check_id_on_package_partial_match);
	ATF_TP_ADD_TC(tp, check_id_on_package_full_match);

	return atf_no_error();
}
