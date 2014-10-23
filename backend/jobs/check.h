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

#ifndef _PKGNG_BACKEND_JOBS_CHECK_H_
#define _PKGNG_BACKEND_JOBS_CHECK_H_

#include <glib.h>		/* gchar */
#include <stdbool.h>		/* bool */
#include "../pkg.h"		/* pkg_... */

#include "../query/id.h"	/* struct query_id */

bool		jobs_check_id_on_package(struct pkg *pkg, struct query_id *query_id, const char *namever);
struct pkg     *jobs_check_package_ids(struct pkg_jobs *jobs, gchar **package_ids, guint count, bool reject_non_updates);

#endif				/* !_PKGNG_BACKEND_JOBS_CHECK_H_ */
