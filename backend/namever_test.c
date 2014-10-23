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

#include "namever.h"		/* pkgutils_... */
#include "testutils.h"		/* gen_pkg */

/* ATF/kyua tests for 'pkgutils.c'. */

ATF_TC(test_namever_from_name_and_version);
ATF_TC_HEAD(test_namever_from_name_and_version, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'namever_from_name_and_version' on a valid pair.");
}
ATF_TC_BODY(test_namever_from_name_and_version, tc)
{
	char           *namever;

	namever = namever_from_name_and_version("foo", "1.2.3");
	ATF_CHECK_STREQ(namever, "foo-1.2.3");
	free(namever);
}

ATF_TC(test_namever_from_package_id);
ATF_TC_HEAD(test_namever_from_package_id, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'namever_from_package_id' on a valid PackageID.");
}
ATF_TC_BODY(test_namever_from_package_id, tc)
{
	char           *namever;

	namever = namever_from_package_id("foo;1.2.3;freebsd:10:x86:32;lohac");
	ATF_CHECK_STREQ(namever, "foo-1.2.3");
	free(namever);
}

ATF_TC(test_namever_array_from_package_ids);
ATF_TC_HEAD(test_namever_array_from_package_ids, tc)
{
	atf_tc_set_md_var(tc, "descr",
	    "Test 'namever_array_from_package_ids' on valid PackageIDs.");
}
ATF_TC_BODY(test_namever_array_from_package_ids, tc)
{
	char           *package_ids[9];
	char          **namevers;
	unsigned int	i;

	/* Full PackageIDs */
	package_ids[0] = strdup("dave;1.2.3;freebsd:10:x86:32;lohac");
	package_ids[1] = strdup("rose;4.5.6;freebsd:10:x86:64;lolar");
	package_ids[2] = strdup("john;7.8;freebsd:9:x86:32;lowas");
	package_ids[3] = strdup("jade;9.0a;freebsd:9:x86:64;lofaf");

	/* Non-canonical/non-remote PackageIDs should still work */
	package_ids[4] = strdup("dirk;1;freebsd:10:x86:32;installed");
	package_ids[5] = strdup("roxy;wizardy-herbert;;");
	package_ids[6] = strdup("jane;rolling;freebsd:8:x86:32;");
	package_ids[7] = strdup("jake;999;;lomax");

	/* And one for luck */
	package_ids[8] = strdup("nepeta;3.3;dragonflybsd:3.4:x86:32;lolcat");

	namevers = namever_array_from_package_ids(package_ids, 9);

	ATF_REQUIRE(namevers != NULL);

	ATF_CHECK_STREQ(namevers[0], "dave-1.2.3");
	ATF_CHECK_STREQ(namevers[1], "rose-4.5.6");
	ATF_CHECK_STREQ(namevers[2], "john-7.8");
	ATF_CHECK_STREQ(namevers[3], "jade-9.0a");
	ATF_CHECK_STREQ(namevers[4], "dirk-1");
	ATF_CHECK_STREQ(namevers[5], "roxy-wizardy-herbert");
	ATF_CHECK_STREQ(namevers[6], "jane-rolling");
	ATF_CHECK_STREQ(namevers[7], "jake-999");
	ATF_CHECK_STREQ(namevers[8], "nepeta-3.3");

	namever_array_free(&namevers, 9);
	ATF_CHECK_EQ(namevers, NULL);

	for (i = 0; i < 4; i++) {
		free(package_ids[i]);
	}
}

ATF_TC(test_namever_from_package);
ATF_TC_HEAD(test_namever_from_package, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Test 'namever_from_package_id' on a valid package.");
}
ATF_TC_BODY(test_namever_from_package, tc)
{
	char           *namever;
	struct pkg     *package;

	package = gen_pkg(PKG_FILE);
	namever = namever_from_package(package);
	ATF_CHECK_STREQ(namever, "pkg-1.1.4");
	pkg_free(package);
	free(namever);
}

/*
 * TEST PACK
 */

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, test_namever_from_name_and_version);
	ATF_TP_ADD_TC(tp, test_namever_from_package_id);
	ATF_TP_ADD_TC(tp, test_namever_array_from_package_ids);
	ATF_TP_ADD_TC(tp, test_namever_from_package);

	return atf_no_error();
}
