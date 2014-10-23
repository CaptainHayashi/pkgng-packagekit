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

#ifndef _PKGNG_BACKEND_ACTIONS_H_
#define _PKGNG_BACKEND_ACTIONS_H_

#include <glib.h>		/* gboolean */
#include "pk-backend.h"		/* PkBackend */

/*
 * Each thread is implemented in its namesake C file, except simulations
 * which are stored with their non-simulated counterparts.
 */
gboolean	get_depends_thread(PkBackend *backend);
gboolean	get_details_thread(PkBackend *backend);
gboolean	get_files_thread(PkBackend *backend);
gboolean	get_repo_list_thread(PkBackend *backend);
gboolean	get_requires_thread(PkBackend *backend);
gboolean	get_update_detail_thread(PkBackend *backend);
gboolean	install_files_thread(PkBackend *backend);
gboolean	install_packages_thread(PkBackend *backend);
gboolean	refresh_cache_thread(PkBackend *backend);
gboolean	remove_packages_thread(PkBackend *backend);
gboolean	resolve_thread(PkBackend *backend);
gboolean	search_files_thread(PkBackend *backend);
gboolean	search_groups_thread(PkBackend *backend);
gboolean	search_names_thread(PkBackend *backend);
gboolean	simulate_install_files_thread(PkBackend *backend);
gboolean	simulate_install_packages_thread(PkBackend *backend);
gboolean	simulate_remove_packages_thread(PkBackend *backend);
gboolean	simulate_update_packages_thread(PkBackend *backend);
gboolean	update_packages_thread(PkBackend *backend);
gboolean	update_system_thread(PkBackend *backend);

#endif				/* !_PKGNG_BACKEND_ACTIONS_H_ */
