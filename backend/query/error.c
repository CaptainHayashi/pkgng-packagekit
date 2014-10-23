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

#include <assert.h>
#include <stdbool.h>
#include "../pk-backend.h"

#include "../utils.h"		/* ERR */
#include "error.h"		/* query_error */

/* Reports query errors on the backend. */

static const char *ERROR_STRINGS[QUERY_ERROR_COUNT] = {
	"QUERY_ERROR_OK reported as error, this is an internal bug.",
	"Cannot perform this action on a package from a remote repository.",
	"No results were found.",
	"The given PackageID(s) could not be interpreted."
};

static PkErrorEnum ERROR_ENUMS[QUERY_ERROR_COUNT] = {
	PK_ERROR_ENUM_INTERNAL_ERROR,
	PK_ERROR_ENUM_CANNOT_GET_FILELIST,
	PK_ERROR_ENUM_PACKAGE_NOT_FOUND,
	PK_ERROR_ENUM_PACKAGE_ID_INVALID
};

bool
query_error_handle(PkBackend *backend, enum query_error error)
{

	assert(backend != NULL);
	assert(error < QUERY_ERROR_COUNT);

	if (error != QUERY_ERROR_OK) {
		ERR(backend, ERROR_ENUMS[error], ERROR_STRINGS[error]);
	}
	return (error == QUERY_ERROR_OK);
}
