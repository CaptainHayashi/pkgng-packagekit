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
#include <string.h>		/* ATF_CHECK_STREQ */
#include <glib.h>		/* gchar, g_free */
#include "pkg.h"		/* pkg... */

#include "repo.h"		/* repo_... */
#include "testutils.h"		/* gen_pkg */

/* ATF/kyua tests for 'repo.c'. */

ATF_TC(repo_of_package_valid_local);
ATF_TC_HEAD(repo_of_package_valid_local, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'repo_of_package' on a local (file) package.");
}
ATF_TC_BODY(repo_of_package_valid_local, tc)
{
	struct pkg     *pkg;

	pkg = gen_pkg(PKG_FILE);
	ATF_REQUIRE(pkg != NULL);

	ATF_CHECK_STREQ(repo_of_package(pkg), "local");

	pkg_free(pkg);
}

ATF_TC(repo_of_package_valid_installed);
ATF_TC_HEAD(repo_of_package_valid_installed, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'repo_of_package' on an installed package.");
}
ATF_TC_BODY(repo_of_package_valid_installed, tc)
{
	struct pkg     *pkg;

	pkg = gen_pkg(PKG_INSTALLED);
	ATF_REQUIRE(pkg != NULL);

	ATF_CHECK_STREQ(repo_of_package(pkg), "installed");

	pkg_free(pkg);
}

ATF_TC(repo_of_package_valid_remote);
ATF_TC_HEAD(repo_of_package_valid_remote, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'repo_of_package' on a remote package.");
}
ATF_TC_BODY(repo_of_package_valid_remote, tc)
{
	struct pkg     *pkg;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	ATF_CHECK_STREQ(repo_of_package(pkg), "packagesite");

	pkg_free(pkg);
}

ATF_TC(repo_type_test);
ATF_TC_HEAD(repo_type_test, tc)
{

	atf_tc_set_md_var(tc, "descr", "Test repo_type_no_remote_check.");
}
ATF_TC_BODY(repo_type_test, tc)
{

	ATF_CHECK_EQ(repo_type_no_remote_check("installed"), REPO_LOCAL);
	ATF_CHECK_EQ(repo_type_no_remote_check("derp"), REPO_REMOTE);
	ATF_CHECK_EQ(repo_type_no_remote_check(""), REPO_ANY);
	ATF_CHECK_EQ(repo_type_no_remote_check(NULL), REPO_ANY);
}


/*
 * TEST PACK
 */

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, repo_type_test);
	ATF_TP_ADD_TC(tp, repo_of_package_valid_local);
	ATF_TP_ADD_TC(tp, repo_of_package_valid_installed);
	ATF_TP_ADD_TC(tp, repo_of_package_valid_remote);

	return atf_no_error();
}
