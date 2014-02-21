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
	cd ${WRKSRC}; ${MKDIR} bin include lib; cd  ${WRKSRC}/ppl7; \
	./configure --with-pcre=${LOCALBASE} --with-libiconv-prefix=${LOCALBASE} \
		--with-nasm --with-jpeg --with-png --with-libtiff=${LOCALBASE} \
		--prefix=${WRKSRC} \
		--without-libmicrohttpd --without-libmcrypt-prefix --without-libmhash \
		--without-libidn --without-libcurl --without-mpg123 --without-lame \
		--without-ogg --without-sdl-prefix --without-imlib \
		--without-mysql --without-postgresql --without-freetds --without-libldns

do-build:
	@(cd ${WRKSRC}/ppl7; if ! ${SETENV} ${MAKE_ENV} ${MAKE} ${MAKE_FLAGS} ${MAKEFILE} ${_MAKE_JOBS} ${MAKE_ARGS} ${ALL_TARGET}; then \
		if [ x != x${BUILD_FAIL_MESSAGE} ] ; then \
			${ECHO_MSG} "===> Compilation failed unexpectedly."; \
			(${ECHO_CMD} ${BUILD_FAIL_MESSAGE}) | ${FMT} 75 79 ; \
		fi; \
		${FALSE}; \
	fi)
	@(PATH="${WRKSRC}/bin:${PATH}"; export PATH; cd ${WRKSRC}/osm; \
	qmake-qt4; \
	if ! ${SETENV} ${MAKE_ENV} ${MAKE} ${MAKE_FLAGS} ${MAKEFILE} ${_MAKE_JOBS} ${MAKE_ARGS} ${ALL_TARGET}; then \
		if [ x != x${BUILD_FAIL_MESSAGE} ] ; then \
			${ECHO_MSG} "===> Compilation failed unexpectedly."; \
			(${ECHO_CMD} ${BUILD_FAIL_MESSAGE}) | ${FMT} 75 79 ; \
		fi; \
		${FALSE}; \
	fi)

do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/bin ${STAGEDIR}${PREFIX}/share/pixmaps ${STAGEDIR}${PREFIX}/share/applications
	${INSTALL_PROGRAM} ${WRKSRC}/osm/release/${PORTNAME} ${STAGEDIR}${PREFIX}/bin
	${INSTALL_DATA} ${WRKSRC}/osm/resources/icon256x256.png ${STAGEDIR}${PREFIX}/share/pixmaps/${PORTNAME}.png
	${INSTALL_DATA} ${FILESDIR}/OpenStopMotion.desktop ${STAGEDIR}${PREFIX}/share/applications/${PORTNAME}.desktop

.include <bsd.port.post.mk>
