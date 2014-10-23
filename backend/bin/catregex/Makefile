# $FreeBSD$

PROG=		pkgpk-catregex
SRCS=		pkgpk-catregex.c ../../group.c ../../group_map.c

DESTDIR?=	/usr/local
BINDIR?=	/bin
MANDIR?=	/man/man

CFLAGS+=	`pkgconf --cflags pkg glib-2.0`
CFLAGS+=        -I/usr/local/include/PackageKit -I/usr/local/include/PackageKit/backend
CFLAGS+=        -I../../
CFLAGS+=        -DPK_COMPILATION
LDFLAGS+=	`pkgconf --libs pkg glib-2.0` -lpackagekit-glib2

.include <bsd.prog.mk>
