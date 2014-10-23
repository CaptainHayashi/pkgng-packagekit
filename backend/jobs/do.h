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

#ifndef _PKGNG_BACKEND_JOBS_DO_H
#define _PKGNG_BACKEND_JOBS_DO_H

#include <stdbool.h>		/* bool */
#include "../pk-backend.h"	/* PkBackend */

#include "core.h"		/* pkg_info_ptr */

struct jobs_spec {
	pkg_jobs_t	type;
	PkErrorEnum	jobs_failed_error;
	PkErrorEnum	no_jobs_error;
	PkStatusEnum	status;
	PkBackend      *backend;
	pkg_info_ptr	info;
	bool		reject_non_updates;
	bool		simulate;
	bool		use_package_ids;
	bool		ignore;	/* Alignment */
};

bool		jobs_do   (const struct jobs_spec *spec);

#endif				/* !_PKGNG_BACKEND_JOBS_DO_H */
