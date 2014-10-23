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

#ifndef _PKGNG_BACKEND_UTILS_H_
#define _PKGNG_BACKEND_UTILS_H_

#include <stdbool.h>		/* bool */
#include <glib.h>		/* gchar, guint */
#include "pk-backend.h"		/* PkBackend */

#define	INTENTIONALLY_IGNORE(x)	(void)(x)
#define STATUS(backend, status) \
	(void)pk_backend_set_status((backend), (status))
#define	ERR(backend, type, msg) \
	(void)pk_backend_error_code((backend), (type), (msg))

gchar         **get_package_ids(PkBackend *backend, guint * count_p);
gchar         **get_strv_and_length(PkBackend *backend, const char *name, guint * count_p);


#endif				/* !_PKGNG_BACKEND_DETAILS_H_ */
