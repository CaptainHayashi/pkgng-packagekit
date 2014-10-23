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

#include <assert.h>		/* assert */
#include <stdbool.h>		/* bool, true, false */
#include <stdlib.h>		/* NULL */
#include <string.h>		/* strchr, strcmp, strdup */
#include <sys/sbuf.h>		/* struct sbuf, sbuf_... */

#include "pk-backend.h"		/* PkGroupEnum, PK_... */
#include "pkg.h"		/* pkg_... */

#include "group_map.h"		/* group_mappings */
#include "group.h"		/* group_... */

static const char ORIGIN_SEPARATOR = '/';

static int	map_compare(const void *key, const void *mapping);
static PkGroupEnum group_from_port_dir(const char *port_dir);

/* Reports the PackageKit groups available on this backend as a bitfield. */
PkBitfield
group_bitfield(void)
{
	size_t		i;
	PkBitfield	bits;

	bits = 0;
	for (i = 0; i < num_group_mappings; i++)
		pk_bitfield_add(bits, group_mappings[i].group);

	return bits;
}

/*
 * Maps from package origins to PackageKit groups.
 */
PkGroupEnum
group_from_origin(const char *origin)
{
	size_t		len;
	char           *dir;
	char           *sep;
	PkGroupEnum	group;

	assert(origin != NULL);

	/* Find the separation between dir and port name */
	sep = strchr(origin, ORIGIN_SEPARATOR);

	/*
	 * Is this a valid origin (did it have a separator)? If not, we want
	 * the default group.  If so, then the number of chars between the
	 * origin start and the separator mark the port directory name we
	 * want to use.
	 */
	len = (size_t) (sep == NULL ? 0 : sep - origin);
	dir = strndup(origin, len);
	group = group_from_port_dir(dir);
	free(dir);

	return group;
}

/*
 * Maps from packages to PackageKit groups.  PKG_LOAD_CATEGORIES must have
 * been set whilst loading the package for this to work properly.
 */
PkGroupEnum
group_of_pkg(struct pkg *pkg)
{
	PkGroupEnum	group;
	struct pkg_category *category;

	assert(pkg != NULL);

	/* Default failure state group. */
	group = PK_GROUP_ENUM_UNKNOWN;

	/* Take the first category only. */
	category = NULL;
	if (pkg_categories(pkg, &category) == EPKG_OK) {
		assert(category != NULL);

		group = group_from_port_dir(pkg_category_name(category));
	}
	/* Fall back to checking the origin if category mapping failed. */
	if (group == PK_GROUP_ENUM_UNKNOWN) {
		const char     *origin;

		origin = NULL;
		if (pkg_get(pkg, PKG_ORIGIN, &origin) == EPKG_OK) {
			assert(origin != NULL);

			group = group_from_origin(origin);
		}
	}
	return group;
}

/*
 * Constructs an Extended Regular Expression matching any package origins
 * that lie within the given group.  The regex should be freed using free(3).
 */
char           *
group_list_to_origin_regex(unsigned int groupc, PkGroupEnum *groupv)
{
	char           *regex;
	struct sbuf    *sb;

	assert(groupv != NULL);

	regex = NULL;
	sb = sbuf_new_auto();
	if (sb != NULL) {
		bool		at_least_one;
		unsigned int	i;
		unsigned int	j;

		at_least_one = false;

		/* (group1|group2|group3|...)/.* */
		(void)sbuf_putc(sb, '(');

		/* Quadratic time, any improvements welcomed */
		for (i = 0; i < num_group_mappings; i++) {
			for (j = 0; j < groupc; j++) {
				if (group_mappings[i].group == groupv[j]) {
					if (at_least_one)
						(void)sbuf_putc(sb, '|');

					(void)sbuf_cat(sb,
					    group_mappings[i].key);

					at_least_one = true;
				}
			}
		}

		(void)sbuf_cat(sb, ")/.*");

		/*
		 * Invalidate the regex if there were no matching groups.
		 */
		if (at_least_one == false)
			(void)sbuf_clear(sb);

		if (sbuf_finish(sb) == 0)
			regex = strdup(sbuf_data(sb));

		sbuf_delete(sb);
	}
	return regex;
}

static int
map_compare(const void *key, const void *mapping)
{

	assert(key != NULL);
	assert(mapping != NULL);

	return strcmp((const char *)key,
	    ((const struct group_mapping *)mapping)->key);
}

/*
 * Maps from port/origin directories to PackageKit groups.
 */
static PkGroupEnum
group_from_port_dir(const char *port_dir)
{
	const struct group_mapping *result;

	assert(port_dir != NULL);

	result = bsearch(port_dir,
	    group_mappings,
	    num_group_mappings,
	    sizeof(struct group_mapping),
	    map_compare);
	return (result == NULL ? PK_GROUP_ENUM_UNKNOWN : result->group);
}
