# $FreeBSD$

PORTNAME=		OpenStopMotion
PORTVERSION=		0.6.6
CATEGORIES=		multimedia
MASTER_SITES=		${MASTER_SITE_SOURCEFORGE}
MASTER_SITE_SUBDIR=	openstopmotion
EXTRACT_SUFX=		-src.tar.bz2

MAINTAINER=		patrick@pfp.de
COMMENT=		Webcam capture program for creating stopmotion video

LICENSE=		GPLv3

LIB_DEPENDS=		libjpeg.so:${PORTSDIR}/graphics/jpeg \
			libtiff.so:${PORTSDIR}/graphics/tiff \
			libpng15.so:${PORTSDIR}/graphics/png \
			libfreetype.so:${PORTSDIR}/print/freetype2 \
			libpcre.so:${PORTSDIR}/devel/pcre \
			libv4l2.so:${PORTSDIR}/multimedia/libv4l \
			libcuse4bsd.so:${PORTSDIR}/multimedia/cuse4bsd-kmod \

BUILD_DEPENDS=		v4l_compat>=1.0.20120501:${PORTSDIR}/multimedia/v4l_compat \
			cuse4bsd-kmod>=0.1.27:${PORTSDIR}/multimedia/cuse4bsd-kmod \
			webcamd:${PORTSDIR}/multimedia/webcamd \
			nasm:${PORTSDIR}/devel/nasm

USE_QT4=		corelib gui opengl qmake

PLIST_FILES=		bin/OpenStopMotion \
			share/pixmaps/OpenStopMotion.png \
			share/applications/OpenStopMotion.desktop

USES=			iconv

.include <bsd.port.pre.mk>

do-configure:
	cd ${WRKSRC}; \
	./configure --prefix=${LOCALBASE} --with-nasm
	
do-build:
	@(cd ${WRKSRC}; if ! ${SETENV} ${MAKE_ENV} ${MAKE} ${MAKE_FLAGS} ${MAKEFILE} ${_MAKE_JOBS} ${MAKE_ARGS} ; then \
		if [ x != x${BUILD_FAIL_MESSAGE} ] ; then \
			${ECHO_MSG} "===> Compilation failed unexpectedly."; \
			(${ECHO_CMD} ${BUILD_FAIL_MESSAGE}) | ${FMT} 75 79 ; \
		fi; \
		${FALSE}; \
	fi)

do-install:
	cd ${WRKSRC}; \
	PREFIX=${STAGEDIR}${LOCALBASE} make install

.include <bsd.port.post.mk>
