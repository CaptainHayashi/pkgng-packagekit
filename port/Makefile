# $FreeBSD$

PORTNAME=	pkgpackagekit
PORTVERSION=	0.1.1
CATEGORIES=	ports-mgmt
MASTER_SITES=	http://ftp1.fr.freebsd.org/mirrors/ftp.freebsd.org/ports/local-distfiles/jlaffaye/ \
		http://ftp2.fr.freebsd.org/mirrors/ftp.freebsd.org/ports/local-distfiles/jlaffaye/

MAINTAINER=	mattbw@FreeBSD.org
COMMENT=	PackageKit backend for pkgng

LICENSE=	GPLv2

LIB_DEPENDS=	libpackagekit-glib2.so:${PORTSDIR}/ports-mgmt/packagekit \
		libpkg.so:${PORTSDIR}/ports-mgmt/pkg

USE_LDCONFIG=	yes

WRKSRC=		${WRKDIR}/backend
PLIST_FILES=	lib/packagekit-backend/libpk_backend_pkgng.so

post-install:
	@${CAT} pkg-message | ${SED} 's,%%PREFIX%%,${PREFIX},g'

.include <bsd.port.mk>
