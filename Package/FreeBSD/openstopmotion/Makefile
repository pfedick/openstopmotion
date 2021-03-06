# $FreeBSD$

PORTNAME=		OpenStopMotion
PORTVERSION=		0.7.0
CATEGORIES=		multimedia
MASTER_SITES=		https://www.pfp.de/download/software/
MASTER_SITE_SUBDIR=	openstopmotion
EXTRACT_SUFX=		-src.tar.bz2

MAINTAINER=		patrick@pfp.de
COMMENT=		Webcam capture program for creating stopmotion video

LICENSE=		GPLv3

LIB_DEPENDS=		libjpeg.so:${PORTSDIR}/graphics/jpeg \
			libtiff.so:${PORTSDIR}/graphics/tiff \
			libpng16.so:${PORTSDIR}/graphics/png \
			libfreetype.so:${PORTSDIR}/print/freetype2 \
			libpcre.so:${PORTSDIR}/devel/pcre \
			libv4l2.so:${PORTSDIR}/multimedia/libv4l \
			libcuse4bsd.so:${PORTSDIR}/multimedia/cuse4bsd-kmod

BUILD_DEPENDS=		v4l_compat>=1.0.20120501:${PORTSDIR}/multimedia/v4l_compat \
			cuse4bsd-kmod>=0.1.27:${PORTSDIR}/multimedia/cuse4bsd-kmod \
			webcamd:${PORTSDIR}/multimedia/webcamd \
			nasm:${PORTSDIR}/devel/nasm \
			clang60:${PORTSDIR}/devel/llvm60

USE_QT5=		core gui opengl qmake

PLIST_FILES=		bin/${PORTNAME} \
			share/pixmaps/${PORTNAME}.png \
			share/applications/${PORTNAME}.desktop

USES=			iconv

.include <bsd.port.pre.mk>

do-configure:
	cd ${WRKSRC}; \
	export CC=clang60; \
	export CXX=clang++60; \
	./configure --prefix=${LOCALBASE} --with-nasm

do-build:
	export CC=clang60; \
	export CXX=clang++60; \
	@(cd ${WRKSRC}; if ! ${SETENV} ${MAKE_ENV} ${MAKE} ${MAKE_FLAGS} ${MAKEFILE} ${_MAKE_JOBS} ${MAKE_ARGS} ; then \
		if [ x != x${BUILD_FAIL_MESSAGE} ] ; then \
			${ECHO_MSG} "===> Compilation failed unexpectedly."; \
			(${ECHO_CMD} ${BUILD_FAIL_MESSAGE}) | ${FMT} 75 79 ; \
		fi; \
		${FALSE}; \
	fi)

do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/bin ${STAGEDIR}${PREFIX}/share/pixmaps ${STAGEDIR}${PREFIX}/share/applications
	${INSTALL_PROGRAM} ${WRKSRC}/osm/release/${PORTNAME} ${STAGEDIR}${PREFIX}/bin/${PORTNAME}
	${INSTALL_DATA} ${WRKSRC}/osm/resources/icon256x256.png ${STAGEDIR}${PREFIX}/share/pixmaps/${PORTNAME}.png
	${INSTALL_DATA} ${WRKSRC}/OpenStopMotion.desktop ${STAGEDIR}${PREFIX}/share/applications/${PORTNAME}.desktop

.include <bsd.port.post.mk>