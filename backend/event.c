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
#include "pk-backend.h"		/* pk_..., Pk... */
#include "pkg.h"		/* pkg... */

#include "emit.h"		/* emit... */
#include "event.h"		/* event_... */
#include "utils.h"		/* ERR, STATUS */

/*
 * Event handler for events emitted by pkg during an installation. TODO: Many
 * of these events are unhandled or deficiently handled.
 */
int
event_cb(void *backend_v, struct pkg_event *event)
{
	PkBackend      *backend;

	assert(backend_v != NULL);
	assert(event != NULL);

	backend = (PkBackend *)backend_v;

	switch (event->type) {
	case PKG_EVENT_INSTALL_BEGIN:
		STATUS(backend, PK_STATUS_ENUM_INSTALL);
		emit_package(event->e_install_begin.pkg,
		    backend,
		    PK_INFO_ENUM_INSTALLING);
		break;
	case PKG_EVENT_INSTALL_FINISHED:
		emit_package(event->e_install_finished.pkg,
		    backend,
		    PK_INFO_ENUM_FINISHED);
		break;
	case PKG_EVENT_DEINSTALL_BEGIN:
		STATUS(backend, PK_STATUS_ENUM_REMOVE);
		emit_package(event->e_deinstall_begin.pkg,
		    backend,
		    PK_INFO_ENUM_REMOVING);
		break;
	case PKG_EVENT_DEINSTALL_FINISHED:
		emit_package(event->e_deinstall_finished.pkg,
		    backend,
		    PK_INFO_ENUM_FINISHED);
		break;
	case PKG_EVENT_UPGRADE_BEGIN:
		STATUS(backend, PK_STATUS_ENUM_UPDATE);
		emit_package(event->e_upgrade_begin.pkg,
		    backend,
		    PK_INFO_ENUM_UPDATING);
		break;
	case PKG_EVENT_UPGRADE_FINISHED:
		emit_package(event->e_upgrade_finished.pkg,
		    backend,
		    PK_INFO_ENUM_FINISHED);
		break;
	case PKG_EVENT_FETCHING:
		STATUS(backend, PK_STATUS_ENUM_DOWNLOAD);
		break;
	case PKG_EVENT_INTEGRITYCHECK_BEGIN:
	case PKG_EVENT_INTEGRITYCHECK_FINISHED:
		/* Unimplemented */
		break;
	case PKG_EVENT_INTEGRITYCHECK_CONFLICT:
		ERR(backend,
		    PK_ERROR_ENUM_PACKAGE_CORRUPT,
		    event->e_integrity_conflict.pkg_name);
		break;
	case PKG_EVENT_NEWPKGVERSION:
		/* TODO: find a better enum for this? */
		ERR(backend,
		    PK_ERROR_ENUM_PACKAGE_INSTALL_BLOCKED,
		    "A new version of pkg is available; install that first.");
		break;
	case PKG_EVENT_NOTICE:
	case PKG_EVENT_INCREMENTAL_UPDATE:
		/* Unimplemented */
		break;
	case PKG_EVENT_ERROR:
		/*
		 * This is sometimes used for nonfatal errors, so we can't
		 * throw an error code here.  What we'll do (mainly for debug
		 * purposes) is post the error into the backend so we can
		 * retrieve it if it was fatal.
		 * 
		 * pk_backend_set_string seems to strdup, so we don't have to.
		 */
		(void)pk_backend_set_string(backend, "job_error",
		    event->e_pkg_error.msg);
		break;
	case PKG_EVENT_ERRNO:
	case PKG_EVENT_ARCHIVE_COMP_UNSUP:
		/* Unimplemented */
		break;
	case PKG_EVENT_ALREADY_INSTALLED:
		ERR(backend,
		    PK_ERROR_ENUM_PACKAGE_ALREADY_INSTALLED,
		    "Already installed.");
		break;
	case PKG_EVENT_FAILED_CKSUM:
	case PKG_EVENT_CREATE_DB_ERROR:
		/* Unimplemented */
		break;
	case PKG_EVENT_LOCKED:
		/* TODO: find a better enum for this? */
		ERR(backend,
		    PK_ERROR_ENUM_PACKAGE_INSTALL_BLOCKED,
		    "Package is locked.");
		break;
	case PKG_EVENT_REQUIRED:
		/* TODO: find a better enum for this? */
		ERR(backend,
		    PK_ERROR_ENUM_CANNOT_REMOVE_SYSTEM_PACKAGE,
		    "Package is required.");
		break;
	case PKG_EVENT_MISSING_DEP:
		ERR(backend,
		    PK_ERROR_ENUM_DEP_RESOLUTION_FAILED,
		    "Package is missing a dependency.");
		break;
	case PKG_EVENT_NOREMOTEDB:
	case PKG_EVENT_NOLOCALDB:
		/* Unimplemented */
		break;
	case PKG_EVENT_FILE_MISMATCH:
		ERR(backend,
		    PK_ERROR_ENUM_PACKAGE_CORRUPT,
		    pkg_file_path(event->e_file_mismatch.file));
		break;
	case PKG_EVENT_DEVELOPER_MODE:
	case PKG_EVENT_PLUGIN_ERRNO:
	case PKG_EVENT_PLUGIN_ERROR:
	case PKG_EVENT_PLUGIN_INFO:
		/* Unimplemented */
		break;
	case PKG_EVENT_NOT_FOUND:
		ERR(backend,
		    PK_ERROR_ENUM_PACKAGE_DOWNLOAD_FAILED,
		    event->e_not_found.pkg_name);
	}

	return EPKG_OK;
}
