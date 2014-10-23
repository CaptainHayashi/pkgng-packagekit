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

#include "check.h"		/* query_check... */
#include "../testutils.h"	/* gen_pkg */

/* ATF/kyua tests for 'check.c'. */

ATF_TC(package_match_name_only);
ATF_TC_HEAD(package_match_name_only, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure packages identified by a single name match properly.");
}
ATF_TC_BODY(package_match_name_only, tc)
{
	struct pkg     *pkg;
	struct query_id	id;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	id.namever = strdup("pkg");
	id.arch = NULL;
	id.repo = NULL;

	ATF_CHECK(query_check_package(pkg, &id));

	pkg_free(pkg);
	free(id.namever);
}

ATF_TC(package_match_namever_only);
ATF_TC_HEAD(package_match_namever_only, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure packages identified by a namever match properly.");
}
ATF_TC_BODY(package_match_namever_only, tc)
{
	struct pkg     *pkg;
	struct query_id	id;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	id.namever = strdup("pkg-1.1.4");
	id.arch = NULL;
	id.repo = NULL;

	ATF_CHECK(query_check_package(pkg, &id));

	pkg_free(pkg);
	free(id.namever);
}

ATF_TC(package_match_full);
ATF_TC_HEAD(package_match_full, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure packages identified by a full query ID match properly.");
}
ATF_TC_BODY(package_match_full, tc)
{
	struct pkg     *pkg;
	struct query_id	id;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	id.namever = strdup("pkg-1.1.4");
	id.arch = strdup("freebsd:10:x86:32");
	id.repo = strdup("packagesite");

	ATF_CHECK(query_check_package(pkg, &id));

	/*
	 * This is a remote package, so setting the ID repo to 'installed'
	 * should cause the match to fail.
	 */
	free(id.repo);
	id.repo = strdup("installed");
	ATF_CHECK(!query_check_package(pkg, &id));

	pkg_free(pkg);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

ATF_TC(package_match_bad_namever);
ATF_TC_HEAD(package_match_bad_namever, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure packages fail to match an ID with the wrong namever.");
}
ATF_TC_BODY(package_match_bad_namever, tc)
{
	struct pkg     *pkg;
	struct query_id	id;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	id.namever = strdup("wrong-1.1.4");
	id.arch = strdup("freebsd:10:x86:32");
	id.repo = strdup("packagesite");

	ATF_CHECK(!query_check_package(pkg, &id));

	pkg_free(pkg);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

ATF_TC(package_match_bad_arch);
ATF_TC_HEAD(package_match_bad_arch, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure packages fail to match a query ID with the wrong arch.");
}
ATF_TC_BODY(package_match_bad_arch, tc)
{
	struct pkg     *pkg;
	struct query_id	id;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	id.namever = strdup("pkg-1.1.4");
	id.arch = strdup("freebsd:9:x86:64");
	id.repo = strdup("packagesite");

	ATF_CHECK(!query_check_package(pkg, &id));

	pkg_free(pkg);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

ATF_TC(package_match_bad_repo);
ATF_TC_HEAD(package_match_bad_repo, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure packages fail to match a query ID with the wrong repo.");
}
ATF_TC_BODY(package_match_bad_repo, tc)
{
	struct pkg     *pkg;
	struct query_id	id;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	id.namever = strdup("pkg-1.1.4");
	id.arch = strdup("freebsd:10:x86:32");
	id.repo = strdup("wrong");

	ATF_CHECK(!query_check_package(pkg, &id));

	pkg_free(pkg);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

ATF_TC(package_match_full_installed);
ATF_TC_HEAD(package_match_full_installed, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure installed packages only match installed query IDs.");
}
ATF_TC_BODY(package_match_full_installed, tc)
{
	struct pkg     *pkg;
	struct query_id	id;

	pkg = gen_pkg(PKG_INSTALLED);
	ATF_REQUIRE(pkg != NULL);

	id.namever = strdup("pkg-1.1.4");
	id.arch = strdup("freebsd:10:x86:32");
	id.repo = strdup("packagesite");

	ATF_CHECK(!query_check_package(pkg, &id));

	free(id.repo);
	id.repo = strdup("installed");
	ATF_CHECK(query_check_package(pkg, &id));

	pkg_free(pkg);
	free(id.namever);
	free(id.arch);
	free(id.repo);
}

/*
 * This test condition is commented out because it triggers an assertion
 * failure instead of failing normally.
 */
/*
 * ATF_TC(package_match_no_namever); ATF_TC_HEAD(package_match_no_namever,
 * tc) {
 * 
 * atf_tc_set_md_var(tc, "descr", "Ensure packages fail to match IDs with a
 * missing namever."); } ATF_TC_BODY(package_match_no_namever, tc) { struct
 * pkg     *pkg; struct query_id id;
 * 
 * pkg = gen_pkg(PKG_REMOTE); ATF_REQUIRE(pkg != NULL);
 * 
 * id.namever = NULL; id.arch = strdup("freebsd:10:x86:32"); id.repo =
 * strdup("arch");
 * 
 * ATF_CHECK(!query_check_package(pkg, &id));
 * 
 * pkg_free(pkg); free(id.arch); free(id.repo); }
 */


/*
 * TEST PACK
 */

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, package_match_name_only);
	ATF_TP_ADD_TC(tp, package_match_namever_only);
	ATF_TP_ADD_TC(tp, package_match_full);
	ATF_TP_ADD_TC(tp, package_match_bad_namever);
	ATF_TP_ADD_TC(tp, package_match_bad_arch);
	ATF_TP_ADD_TC(tp, package_match_bad_repo);
	ATF_TP_ADD_TC(tp, package_match_full_installed);
	/* ATF_TP_ADD_TC(tp, package_match_no_namever); */

	return atf_no_error();
}
