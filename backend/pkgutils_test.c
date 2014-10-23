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

#include "pkgutils.h"		/* pkgutils_... */
#include "testutils.h"		/* gen_pkg */

/* ATF/kyua tests for 'pkgutils.c'. */

ATF_TC(pkg_to_id_valid_local);
ATF_TC_HEAD(pkg_to_id_valid_local, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_to_id' on a local (file) package.");
}
ATF_TC_BODY(pkg_to_id_valid_local, tc)
{
	struct pkg     *pkg;
	gchar          *package_id;

	pkg = gen_pkg(PKG_FILE);
	ATF_REQUIRE(pkg != NULL);

	package_id = pkgutils_pkg_to_id(pkg);
	ATF_CHECK_STREQ(package_id, "pkg;1.1.4;freebsd:10:x86:32;local");

	pkg_free(pkg);
	g_free(package_id);
}

ATF_TC(pkg_to_id_valid_installed);
ATF_TC_HEAD(pkg_to_id_valid_installed, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_to_id' on an installed package.");
}
ATF_TC_BODY(pkg_to_id_valid_installed, tc)
{
	struct pkg     *pkg;
	gchar          *package_id;

	pkg = gen_pkg(PKG_INSTALLED);
	ATF_REQUIRE(pkg != NULL);

	package_id = pkgutils_pkg_to_id(pkg);
	ATF_CHECK_STREQ(package_id, "pkg;1.1.4;freebsd:10:x86:32;installed");

	pkg_free(pkg);
	g_free(package_id);
}

ATF_TC(pkg_to_id_valid_remote);
ATF_TC_HEAD(pkg_to_id_valid_remote, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_to_id' on a remote package.");
}
ATF_TC_BODY(pkg_to_id_valid_remote, tc)
{
	struct pkg     *pkg;
	gchar          *package_id;

	pkg = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(pkg != NULL);

	package_id = pkgutils_pkg_to_id(pkg);
	ATF_CHECK_STREQ(package_id, "pkg;1.1.4;freebsd:10:x86:32;packagesite");

	pkg_free(pkg);
	g_free(package_id);
}

ATF_TC(pkg_install_state);
ATF_TC_HEAD(pkg_install_state, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_install_state'.");
}
ATF_TC_BODY(pkg_install_state, tc)
{
	struct pkg     *package;

	package = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(package != NULL);

	/* Old version newer -> downgrading */
	pkg_set(package, PKG_OLD_VERSION, "1.2.3");
	ATF_CHECK_EQ(pkgutils_pkg_install_state(package),
	    PK_INFO_ENUM_DOWNGRADING);

	/* Old version the same -> reinstalling */
	pkg_set(package, PKG_OLD_VERSION, "1.1.4");
	ATF_CHECK_EQ(pkgutils_pkg_install_state(package),
	    PK_INFO_ENUM_REINSTALLING);

	/* Old version older -> updating */
	pkg_set(package, PKG_OLD_VERSION, "1.0.0");
	ATF_CHECK_EQ(pkgutils_pkg_install_state(package),
	    PK_INFO_ENUM_UPDATING);

	pkg_free(package);
}

ATF_TC(pkg_current_state_local);
ATF_TC_HEAD(pkg_current_state_local, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_current_state' for installed packages.");
}
ATF_TC_BODY(pkg_current_state_local, tc)
{
	struct pkg     *package;

	package = gen_pkg(PKG_INSTALLED);
	ATF_REQUIRE(package != NULL);

	pkg_set(package, PKG_OLD_VERSION, "1.2.3");
	ATF_CHECK_EQ(pkgutils_pkg_current_state(package),
	    PK_INFO_ENUM_INSTALLED);

	pkg_free(package);
}

ATF_TC(pkg_current_state_remote);
ATF_TC_HEAD(pkg_current_state_remote, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_current_state' for remote packages.");
}
ATF_TC_BODY(pkg_current_state_remote, tc)
{
	struct pkg     *package;

	package = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(package != NULL);

	/* Old version newer -> available */
	pkg_set(package, PKG_OLD_VERSION, "1.2.3");
	ATF_CHECK_EQ(pkgutils_pkg_current_state(package),
	    PK_INFO_ENUM_AVAILABLE);

	/* Old version the same -> installed */
	pkg_set(package, PKG_OLD_VERSION, "1.1.4");
	ATF_CHECK_EQ(pkgutils_pkg_current_state(package),
	    PK_INFO_ENUM_INSTALLED);

	/* Old version older -> available */
	pkg_set(package, PKG_OLD_VERSION, "1.0.0");
	ATF_CHECK_EQ(pkgutils_pkg_current_state(package),
	    PK_INFO_ENUM_AVAILABLE);

	pkg_free(package);
}

ATF_TC(pkg_matches_filters_installed);
ATF_TC_HEAD(pkg_matches_filters_installed, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_matches_filters' for the installed filter.");
}
ATF_TC_BODY(pkg_matches_filters_installed, tc)
{
	struct pkg     *local;
	struct pkg     *remote;
	PkBitfield	filter;

	filter = pk_bitfield_from_enums(PK_FILTER_ENUM_INSTALLED, -1);

	/* Locally installed -> accept */
	local = gen_pkg(PKG_INSTALLED);
	ATF_REQUIRE(local != NULL);
	ATF_CHECK(pkgutils_pkg_matches_filters(local, filter));
	pkg_free(local);

	remote = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(remote != NULL);

	/* Old version newer -> reject */
	pkg_set(remote, PKG_OLD_VERSION, "1.2.3");
	ATF_CHECK(!pkgutils_pkg_matches_filters(remote, filter));

	/* Old version the same -> accept */
	pkg_set(remote, PKG_OLD_VERSION, "1.1.4");
	ATF_CHECK(pkgutils_pkg_matches_filters(remote, filter));

	/* Old version older -> reject */
	pkg_set(remote, PKG_OLD_VERSION, "1.0.0");
	ATF_CHECK(!pkgutils_pkg_matches_filters(remote, filter));

	pkg_free(remote);
}

ATF_TC(pkg_matches_filters_not_installed);
ATF_TC_HEAD(pkg_matches_filters_not_installed, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'pkgutils_pkg_matches_filters' for the ~installed filter.");
}
ATF_TC_BODY(pkg_matches_filters_not_installed, tc)
{
	struct pkg     *local;
	struct pkg     *remote;
	PkBitfield	filter;

	filter = pk_bitfield_from_enums(PK_FILTER_ENUM_NOT_INSTALLED, -1);

	/* Locally installed -> reject */
	local = gen_pkg(PKG_INSTALLED);
	ATF_REQUIRE(local != NULL);
	ATF_CHECK(!pkgutils_pkg_matches_filters(local, filter));
	pkg_free(local);

	remote = gen_pkg(PKG_REMOTE);
	ATF_REQUIRE(remote != NULL);

	/* Old version newer -> reject */
	pkg_set(remote, PKG_OLD_VERSION, "1.2.3");
	ATF_CHECK(pkgutils_pkg_matches_filters(remote, filter));

	/* Old version the same -> accept */
	pkg_set(remote, PKG_OLD_VERSION, "1.1.4");
	ATF_CHECK(!pkgutils_pkg_matches_filters(remote, filter));

	/* Old version older -> reject */
	pkg_set(remote, PKG_OLD_VERSION, "1.0.0");
	ATF_CHECK(pkgutils_pkg_matches_filters(remote, filter));

	pkg_free(remote);
}

/*
 * TEST PACK
 */

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, pkg_to_id_valid_local);
	ATF_TP_ADD_TC(tp, pkg_to_id_valid_installed);
	ATF_TP_ADD_TC(tp, pkg_to_id_valid_remote);
	ATF_TP_ADD_TC(tp, pkg_install_state);
	ATF_TP_ADD_TC(tp, pkg_current_state_local);
	ATF_TP_ADD_TC(tp, pkg_current_state_remote);
	ATF_TP_ADD_TC(tp, pkg_matches_filters_installed);
	ATF_TP_ADD_TC(tp, pkg_matches_filters_not_installed);

	return atf_no_error();
}
