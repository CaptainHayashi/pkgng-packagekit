/*-
 * Copyright (C) 2007-2010 Richard Hughes <richard@hughsie.com>
 *                    2013 Matt Windsor <mattbw@FreeBSD.org>
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
#include <glib.h>
#include <string.h>
#include <stdlib.h>

#include "pk-backend.h"
#include "pkg.h"

#include "utils.h"		/* INTENTIONALLY_IGNORE, ERR */
#include "group.h"		/* group_bitfield */
#include "actions.h"		/* Actions threads */

#define	THREAD(bend, thread)	(void)pk_backend_thread_create((bend), (thread))

void
pk_backend_initialize(PkBackend *backend)
{
	int		err;

	assert(backend != NULL);
	err = EPKG_OK;

	if (!pkg_initialized())
		err = pkg_init(NULL);
	if (err != EPKG_OK)
		ERR(backend, PK_ERROR_ENUM_INTERNAL_ERROR, "couldn't init pkg");
}

void
pk_backend_destroy(PkBackend *backend)
{

	INTENTIONALLY_IGNORE(backend);
	pkg_shutdown();
}

gchar          *
pk_backend_get_description(PkBackend *backend)
{

	INTENTIONALLY_IGNORE(backend);
	return g_strdup("pkgng");
}

gchar          *
pk_backend_get_author(PkBackend *backend)
{

	INTENTIONALLY_IGNORE(backend);
	return g_strdup("Matt Windsor <mattbw@FreeBSD.org>");
}

/**
 * ACTION STUBS
 *
 * In roughly alphabetical order; usually the thread code corresponding to these
 * can be found in actions/name_of_action.c.
 *
 * If an action is missing here and not otherwise mentioned, it is probably
 * awaiting implementation.
 *
 * Not supported due to unavailable pkgng features:
 * - pk_backend_get_files on a non-installed package
 * - pk_backend_repo_enable
 * - pk_backend_repo_set_data
 */

void
pk_backend_get_depends(PkBackend *backend, PkBitfield filters,
    gchar **package_ids, gboolean recursive)
{

	INTENTIONALLY_IGNORE(filters);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(recursive);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, get_depends_thread);
}

void
pk_backend_get_details(PkBackend *backend, gchar **package_ids)
{

	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, get_details_thread);
}

void
pk_backend_get_files(PkBackend *backend, gchar **package_ids)
{

	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, get_files_thread);
}

void
pk_backend_get_requires(PkBackend *backend, PkBitfield filters,
    gchar **package_ids, gboolean recursive)
{

	INTENTIONALLY_IGNORE(filters);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(recursive);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, get_requires_thread);
}

PkBitfield
pk_backend_get_filters(PkBackend *backend)
{

	INTENTIONALLY_IGNORE(backend);

	/* TODO: support more filters */
	return pk_bitfield_from_enums(PK_FILTER_ENUM_INSTALLED,
	    PK_FILTER_ENUM_NOT_INSTALLED,
	    -1);
}

PkBitfield
pk_backend_get_groups(PkBackend *backend)
{

	INTENTIONALLY_IGNORE(backend);
	return group_bitfield();
}

gchar          *
pk_backend_get_mime_types(PkBackend *backend)
{

	INTENTIONALLY_IGNORE(backend);

	/*
	 * (Backformed from pkg.h: pkg_formats) and using `gvfs-info filename
	 * -a "standard::content-type"
	 */
	return g_strdup("application/x-tar;"
	    "application/x-compressed-tar;"
	    "application/x-bzip-compressed-tar;"
	    "application/x-xz-compressed-tar;");
}

void
pk_backend_get_repo_list(PkBackend *backend, PkBitfield filters)
{

	INTENTIONALLY_IGNORE(filters);	/* not yet supported */
	assert(backend != NULL);
	THREAD(backend, get_repo_list_thread);
}

void
pk_backend_get_update_detail(PkBackend *backend, gchar **package_ids)
{

	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, get_update_detail_thread);
}

void
pk_backend_install_files(PkBackend *backend, gboolean only_trusted,
    gchar **full_paths)
{

	INTENTIONALLY_IGNORE(only_trusted);	/* not yet supported */
	INTENTIONALLY_IGNORE(full_paths);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, install_files_thread);
}

void
pk_backend_install_packages(PkBackend *backend, gboolean only_trusted,
    gchar **package_ids)
{

	INTENTIONALLY_IGNORE(only_trusted);	/* not yet supported */
	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, install_packages_thread);
}

void
pk_backend_refresh_cache(PkBackend *backend, gboolean force)
{

	INTENTIONALLY_IGNORE(force);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, refresh_cache_thread);
}

void
pk_backend_remove_packages(PkBackend *backend, gchar **package_ids,
    gboolean allow_deps, gboolean autoremove)
{

	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(allow_deps);	/* not yet supported */
	INTENTIONALLY_IGNORE(autoremove);	/* not yet supported */
	assert(backend != NULL);
	THREAD(backend, remove_packages_thread);
}

void
pk_backend_resolve(PkBackend *backend, PkBitfield filters, gchar **package_ids)
{

	INTENTIONALLY_IGNORE(filters);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, resolve_thread);
}

void
pk_backend_search_files(PkBackend *backend, PkBitfield filters, gchar **search)
{

	INTENTIONALLY_IGNORE(filters);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(search);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, search_files_thread);
}

void
pk_backend_search_groups(PkBackend *backend, PkBitfield filters, gchar **search)
{

	INTENTIONALLY_IGNORE(filters);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(search);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, search_groups_thread);
}

void
pk_backend_search_names(PkBackend *backend, PkBitfield filters, gchar **search)
{

	INTENTIONALLY_IGNORE(filters);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(search);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, search_names_thread);
}

void
pk_backend_simulate_install_files(PkBackend *backend, gchar **full_paths)
{

	INTENTIONALLY_IGNORE(full_paths);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, simulate_install_files_thread);
}

void
pk_backend_simulate_install_packages(PkBackend *backend, gchar **package_ids)
{

	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, simulate_install_packages_thread);
}

void
pk_backend_simulate_remove_packages(PkBackend *backend, gchar **package_ids,
    gboolean autoremove)
{

	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	INTENTIONALLY_IGNORE(autoremove);	/* not yet supported */
	assert(backend != NULL);
	THREAD(backend, simulate_remove_packages_thread);
}

void
pk_backend_simulate_update_packages(PkBackend *backend, gchar **package_ids)
{

	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, simulate_update_packages_thread);
}

void
pk_backend_update_packages(PkBackend *backend, gboolean only_trusted,
    gchar **package_ids)
{

	INTENTIONALLY_IGNORE(only_trusted);	/* not yet supported */
	INTENTIONALLY_IGNORE(package_ids);	/* retrieved from backend */
	assert(backend != NULL);
	THREAD(backend, update_packages_thread);
}

void
pk_backend_update_system(PkBackend *backend, gboolean only_trusted)
{

	INTENTIONALLY_IGNORE(only_trusted);	/* not yet supported */
	assert(backend != NULL);
	THREAD(backend, update_system_thread);
}
