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

#ifndef _PKGNG_BACKEND_EMIT_H_
#define _PKGNG_BACKEND_EMIT_H_

#include "pk-backend.h"		/* Pk... */
#include "pkg.h"		/* pkg... */

void		emit_package(struct pkg *pkg, PkBackend *backend, PkInfoEnum info);
void		emit_filtered_package(struct pkg *pkg, PkBackend *backend, PkBitfield filters, PkInfoEnum info);

#endif				/* !_PKGNG_BACKEND_EMIT_H_ */
