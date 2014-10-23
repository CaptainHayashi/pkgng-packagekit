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

/* Macros for dealing with string-key-to-enum-value mapping lists. */

#include "glib.h"		/* g_strcmp0 */

/*
 * Finds a mapping entry given its string key "sk", and sets the pointer
 * pointed to by "vp" to reference it. Expects the last (fallthrough) entry
 * to have a string value of NULL, will point to it if no other match was
 * found.
 * 
 * Currently a linear search, could be improved.  Any way to make this not a
 * horrible macro would also be welcomed (it's a macro to make it work across
 * different types of mapping, at the moment).
 */
#define MAPPING_FIND(sk, vp, map) do {					\
	for (*(vp) = map;						\
	     (*(vp))->key != NULL && g_strcmp0((*(vp))->key, (sk)) != 0;\
	     (*(vp)) += 1);						\
} while(0)
