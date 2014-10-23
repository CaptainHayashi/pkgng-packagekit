#-
# Copyright (C) 2013 Matt Windsor <mattbw@FreeBSD.org>
#
# Licensed under the GNU General Public License Version 2
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

# $FreeBSD$

LIB=		pk_backend_pkgng
SHLIB_MAJOR=	1
SRCS=		pk-backend-pkgng.c
SRCS+= 						\
		db.c				\
		emit.c				\
		event.c				\
		group.c				\
		group_map.c			\
		licenses.c			\
		namever.c			\
		pkgutils.c			\
		repo.c				\
		search.c			\
		utils.c
SRCS+=						\
		actions/get_depends.c		\
		actions/get_details.c		\
		actions/get_files.c		\
		actions/get_repo_list.c		\
		actions/get_requires.c		\
		actions/get_update_detail.c	\
		actions/install_files.c		\
		actions/install_packages.c	\
		actions/refresh_cache.c		\
		actions/remove_packages.c	\
		actions/resolve.c		\
		actions/search_files.c		\
		actions/search_groups.c		\
		actions/search_names.c		\
		actions/update_packages.c	\
		actions/update_system.c

SRCS+=						\
		query/check.c			\
		query/depends.c			\
		query/do.c			\
		query/error.c			\
		query/find.c			\
		query/id.c			\
		query/match.c			\
		query/packages.c

SRCS+=						\
		jobs/core.c			\
		jobs/check.c			\
		jobs/do.c

LIBDIR=		/usr/local/lib/packagekit-backend

PKGS= 		pkg glib-2.0

# ATF test flags
TESTPROGS=					\
		namever_test			\
		pkgutils_test			\
		repo_test			\
		jobs/check_test			\
		query/id_test			\
		query/check_test		\

TESTCFLAGS=	`pkgconf --cflags atf-c`
TESTLDFLAGS=	`pkgconf --libs-only-L --libs-only-other atf-c`
TESTLIBS=	`pkgconf --libs-only-l atf-c`

# 0.6.* versions of PackageKit do not export pkgconf information, so here's a
# bodge.
#USE_PK_PKGCONF=	0	# Set to 1 if using recent PackageKit
#.if USE_PK_PKGCONF
#PKGS+=		packagekit-glib2
#.else
CFLAGS+=	-I/usr/local/include/PackageKit -I/usr/local/include/PackageKit/backend
LDFLAGS+=	-L/usr/local/lib -lpackagekit-glib2
#.endif

CFLAGS+=	`pkg-config --cflags ${PKGS}`
CFLAGS+=	-DPK_COMPILATION -g
LDFLAGS+=	`pkg-config --libs ${PKGS}` -g
CSTD?=		gnu99

group_map.c: groups group_map.awk
	sort groups | awk -f group_map.awk > group_map.c

tests: ${TESTPROGS}

# TODO: Find a more BSD way of doing this
jobs/check_test: jobs/check_test.o jobs/check.o namever.o testutils.o query/check.o query/id.o pkgutils.o repo.o
	${CC} ${LDFLAGS} ${TESTLDFLAGS} -o ${.TARGET} ${.ALLSRC} ${LIBS} ${TESTLIBS}

jobs/check_test.o: jobs/check_test.c
	${CC} ${CFLAGS} ${TESTCFLAGS} -o ${.TARGET} -c ${.ALLSRC}

query/id_test: query/id_test.o query/id.o namever.o testutils.o
	${CC} ${LDFLAGS} ${TESTLDFLAGS} -o ${.TARGET} ${.ALLSRC} ${LIBS} ${TESTLIBS}

query/id_test.o: query/id_test.c
	${CC} ${CFLAGS} ${TESTCFLAGS} -o ${.TARGET} -c ${.ALLSRC}

query/check_test: query/check_test.o query/check.o query/id.o query/packages.o query/find.o namever.o testutils.o pkgutils.o repo.o
	${CC} ${LDFLAGS} ${TESTLDFLAGS} -o ${.TARGET} ${.ALLSRC} ${LIBS} ${TESTLIBS}

query/check_test.o: query/check_test.c
	${CC} ${CFLAGS} ${TESTCFLAGS} -o ${.TARGET} -c ${.ALLSRC}

pkgutils_test: pkgutils_test.o pkgutils.o namever.o testutils.o repo.o
	${CC} ${LDFLAGS} ${TESTLDFLAGS} -o ${.TARGET} ${.ALLSRC} ${LIBS} ${TESTLIBS}

pkgutils_test.o: pkgutils_test.c
	${CC} ${CFLAGS} ${TESTCFLAGS} -o ${.TARGET} -c ${.ALLSRC}

namever_test: namever_test.o namever.o testutils.o
	${CC} ${LDFLAGS} ${TESTLDFLAGS} -o ${.TARGET} ${.ALLSRC} ${LIBS} ${TESTLIBS}

namever_test.o: namever_test.c
	${CC} ${CFLAGS} ${TESTCFLAGS} -o ${.TARGET} -c ${.ALLSRC}

repo_test: repo_test.o repo.o testutils.o
	${CC} ${LDFLAGS} ${TESTLDFLAGS} -o ${.TARGET} ${.ALLSRC} ${LIBS} ${TESTLIBS}

repo_test.o: repo_test.c
	${CC} ${CFLAGS} ${TESTCFLAGS} -o ${.TARGET} -c ${.ALLSRC}

testutils.o: testutils.c
	${CC} ${CFLAGS} ${TESTCFLAGS} -o ${.TARGET} -c ${.ALLSRC}

.include <bsd.lib.mk>
