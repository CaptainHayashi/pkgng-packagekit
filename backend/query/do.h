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

#ifndef _PKGNG_BACKEND_QUERY_DO_H_
#define _PKGNG_BACKEND_QUERY_DO_H_

#include <stdbool.h>
#include "../pk-backend.h"

typedef bool    (*emit_ptr) (PkBackend *backend, struct pkgdb *db, struct pkg *pkg);

bool		query_do_from_backend_ids(PkBackend *backend, unsigned int load_flags, emit_ptr emitter);
bool		query_do_from_backend_names(PkBackend *backend, unsigned int load_flags, emit_ptr emitter);

#endif				/* !_PKGNG_BACKEND_QUERY_DO_H_ */
