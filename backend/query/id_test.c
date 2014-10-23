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

#include "id.h"			/* query_id_... */


/* ATF/kyua tests for 'id.c'. */

/*
 * SINGLE PACKAGE IDS
 */

ATF_TC(query_id_from_package_id_single_valid);
ATF_TC_HEAD(query_id_from_package_id_single_valid, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure single PackageIDs properly convert to query IDs.");
}
ATF_TC_BODY(query_id_from_package_id_single_valid, tc)
{
	bool		success;
	struct query_id	result;
	const char     *id = "pkg;1.1.4;freebsd:9:x86:32;packagesite";

	result.namever = NULL;
	result.arch = NULL;
	result.repo = NULL;

	success = query_id_from_package_id(id, &result);
	ATF_CHECK(success);

	ATF_CHECK_STREQ(result.namever, "pkg-1.1.4");
	ATF_CHECK_STREQ(result.arch, "freebsd:9:x86:32");
	ATF_CHECK_STREQ(result.repo, "packagesite");

	free(result.namever);
	free(result.arch);
	free(result.repo);
}

ATF_TC(query_id_from_package_id_single_no_name);
ATF_TC_HEAD(query_id_from_package_id_single_no_name, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure PackageIDs with no name fail to convert to query IDs.");
}
ATF_TC_BODY(query_id_from_package_id_single_no_name, tc)
{
	bool		success;
	struct query_id	result;
	const char     *no_name = ";1.1.4;freebsd:9:x86:32;packagesite";

	result.namever = NULL;
	result.arch = NULL;
	result.repo = NULL;

	success = query_id_from_package_id(no_name, &result);
	ATF_CHECK(!success);

	ATF_CHECK_EQ(result.namever, NULL);
	ATF_CHECK_EQ(result.arch, NULL);
	ATF_CHECK_EQ(result.repo, NULL);
}

ATF_TC(query_id_from_package_id_single_no_version);
ATF_TC_HEAD(query_id_from_package_id_single_no_version, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure PackageIDs with no version fail to convert to query IDs.");
}
ATF_TC_BODY(query_id_from_package_id_single_no_version, tc)
{
	bool		success;
	struct query_id	result;
	const char     *no_version = "pkg;;freebsd:9:x86:32;packagesite";

	result.namever = NULL;
	result.arch = NULL;
	result.repo = NULL;

	success = query_id_from_package_id(no_version, &result);
	ATF_CHECK(!success);

	ATF_CHECK_EQ(result.namever, NULL);
	ATF_CHECK_EQ(result.arch, NULL);
	ATF_CHECK_EQ(result.repo, NULL);
}

ATF_TC(query_id_from_package_id_single_no_repo);
ATF_TC_HEAD(query_id_from_package_id_single_no_repo, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure PackageIDs with no repo properly convert to query IDs.");
}
ATF_TC_BODY(query_id_from_package_id_single_no_repo, tc)
{
	bool		success;
	struct query_id	result;
	const char     *no_version = "pkg;1.1.4;freebsd:9:x86:32;";

	result.namever = NULL;
	result.arch = NULL;
	result.repo = NULL;

	success = query_id_from_package_id(no_version, &result);
	ATF_CHECK(success);

	ATF_CHECK_STREQ(result.namever, "pkg-1.1.4");
	ATF_CHECK_STREQ(result.arch, "freebsd:9:x86:32");
	ATF_CHECK_EQ(result.repo, NULL);

	free(result.namever);
	free(result.arch);
}

ATF_TC(query_id_from_package_id_single_no_arch);
ATF_TC_HEAD(query_id_from_package_id_single_no_arch, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure PackageIDs with no arch properly convert to query IDs.");
}
ATF_TC_BODY(query_id_from_package_id_single_no_arch, tc)
{
	bool		success;
	struct query_id	result;
	const char     *no_version = "pkg;1.1.4;;packagesite";

	result.namever = NULL;
	result.arch = NULL;
	result.repo = NULL;

	success = query_id_from_package_id(no_version, &result);
	ATF_CHECK(success);

	ATF_CHECK_STREQ(result.namever, "pkg-1.1.4");
	ATF_CHECK_EQ(result.arch, NULL);
	ATF_CHECK_STREQ(result.repo, "packagesite");

	free(result.namever);
	free(result.repo);
}

/*
 * SINGLE NAMES
 */

ATF_TC(query_id_from_name_single_valid);
ATF_TC_HEAD(query_id_from_name_single_valid, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure valid package names properly convert to query IDs.");
}
ATF_TC_BODY(query_id_from_name_single_valid, tc)
{
	bool		success;
	struct query_id	result;
	const char     *valid_name = "pkg-1.1.4";

	result.namever = NULL;
	result.arch = NULL;
	result.repo = NULL;

	success = query_id_from_name(valid_name, &result);
	ATF_CHECK(success);

	ATF_CHECK_STREQ(result.namever, "pkg-1.1.4");
	ATF_CHECK_EQ(result.arch, NULL);
	ATF_CHECK_EQ(result.repo, NULL);

	free(result.namever);
}

/*
 * SINGLE NAMES
 */

ATF_TC(query_id_from_name_single_empty);
ATF_TC_HEAD(query_id_from_name_single_empty, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure empty package names fail to convert to query IDs.");
}
ATF_TC_BODY(query_id_from_name_single_empty, tc)
{
	bool		success;
	struct query_id	result;
	const char     *empty_name = "";

	result.namever = NULL;
	result.arch = NULL;
	result.repo = NULL;

	success = query_id_from_name(empty_name, &result);
	ATF_CHECK(!success);

	ATF_CHECK_EQ(result.namever, NULL);
	ATF_CHECK_EQ(result.arch, NULL);
	ATF_CHECK_EQ(result.repo, NULL);
}

/*
 * MULTIPLE ID FUNCTIONS
 */

ATF_TC(query_id_from_name_multiple_valid);
ATF_TC_HEAD(query_id_from_name_multiple_valid, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure valid arrays of package names convert to query IDs.");
}
ATF_TC_BODY(query_id_from_name_multiple_valid, tc)
{
	unsigned int	i;
	struct query_id *results;
	char           *valid_names[3];
	const char     *valid_1 = "pkg-1.1.4";
	const char     *valid_2 = "brasero-1.2";
	const char     *valid_3 = "PackageKit";

	valid_names[0] = strdup(valid_1);
	valid_names[1] = strdup(valid_2);
	valid_names[2] = strdup(valid_3);

	results = query_id_array_from_names(valid_names, 3);
	ATF_CHECK(results != NULL);

	if (results != NULL) {
		for (i = 0; i < 3; i++) {
			ATF_CHECK_STREQ(results[i].namever, valid_names[i]);
			ATF_CHECK_EQ(results[i].arch, NULL);
			ATF_CHECK_EQ(results[i].repo, NULL);

			free(results[i].namever);
		}
	}
	free(valid_names[0]);
	free(valid_names[1]);
	free(valid_names[2]);
}

ATF_TC(query_id_from_name_multiple_empty);
ATF_TC_HEAD(query_id_from_name_multiple_empty, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure package name arrays with empty members fail to convert.");
}
ATF_TC_BODY(query_id_from_name_multiple_empty, tc)
{
	struct query_id *results;
	char           *invalid_names[3];
	const char     *invalid_1 = "pkg-1.1.4";
	const char     *invalid_2 = "";
	const char     *invalid_3 = "PackageKit";

	invalid_names[0] = strdup(invalid_1);
	invalid_names[1] = strdup(invalid_2);
	invalid_names[2] = strdup(invalid_3);

	results = query_id_array_from_names(invalid_names, 3);
	ATF_CHECK_EQ(results, NULL);

	free(invalid_names[0]);
	free(invalid_names[1]);
	free(invalid_names[2]);
}

ATF_TC(query_id_from_package_id_multiple_valid);
ATF_TC_HEAD(query_id_from_package_id_multiple_valid, tc)
{

	atf_tc_set_md_var(tc, "descr",
	    "Ensure package ID arrays with all valid members convert.");
}
ATF_TC_BODY(query_id_from_package_id_multiple_valid, tc)
{
	struct query_id *results;
	char           *valid_package_ids[3];
	const char     *valid_1 = "pkg;1.1.4;freebsd:9:x86:32;packagesite";
	const char     *valid_2 = "brasero;1.2;freebsd:9:x86:64;elsewhere";
	const char     *valid_3 = "PackageKit;0.6;freebsd:10:x86:32;installed";

	valid_package_ids[0] = strdup(valid_1);
	valid_package_ids[1] = strdup(valid_2);
	valid_package_ids[2] = strdup(valid_3);

	results = query_id_array_from_package_ids(valid_package_ids, 3);
	ATF_CHECK(results != NULL);

	if (results != NULL) {
		ATF_CHECK_STREQ(results[0].namever, "pkg-1.1.4");
		ATF_CHECK_STREQ(results[0].arch, "freebsd:9:x86:32");
		ATF_CHECK_STREQ(results[0].repo, "packagesite");

		ATF_CHECK_STREQ(results[1].namever, "brasero-1.2");
		ATF_CHECK_STREQ(results[1].arch, "freebsd:9:x86:64");
		ATF_CHECK_STREQ(results[1].repo, "elsewhere");

		ATF_CHECK_STREQ(results[2].namever, "PackageKit-0.6");
		ATF_CHECK_STREQ(results[2].arch, "freebsd:10:x86:32");
		ATF_CHECK_STREQ(results[2].repo, "installed");
	}
	free(valid_package_ids[0]);
	free(valid_package_ids[1]);
	free(valid_package_ids[2]);
}

/*
 * TEST PACK
 */

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_ADD_TC(tp, query_id_from_package_id_single_valid);
	ATF_TP_ADD_TC(tp, query_id_from_package_id_single_no_name);
	ATF_TP_ADD_TC(tp, query_id_from_package_id_single_no_version);
	ATF_TP_ADD_TC(tp, query_id_from_package_id_single_no_repo);
	ATF_TP_ADD_TC(tp, query_id_from_package_id_single_no_arch);

	ATF_TP_ADD_TC(tp, query_id_from_name_single_valid);
	ATF_TP_ADD_TC(tp, query_id_from_name_single_empty);

	ATF_TP_ADD_TC(tp, query_id_from_name_multiple_valid);
	ATF_TP_ADD_TC(tp, query_id_from_name_multiple_empty);

	ATF_TP_ADD_TC(tp, query_id_from_package_id_multiple_valid);

	return atf_no_error();
}
