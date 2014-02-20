# $FreeBSD$

PORTNAME=		OpenStopMotion
PORTVERSION=		0.6.4
CATEGORIES=		multimedia
MASTER_SITES=		${MASTER_SITE_SOURCEFORGE}
MASTER_SITE_SUBDIR=	openstopmotion
EXTRACT_SUFX=		-src.tar.bz2

MAINTAINER=		patrick@pfp.de
COMMENT=		Webcam capture program for creating stopmotion video

LICENSE=		GPLv3

LIB_DEPENDS=		jpeg:${PORTSDIR}/graphics/jpeg \
			tiff:${PORTSDIR}/graphics/tiff \
			png15:${PORTSDIR}/graphics/png \
			freetype:${PORTSDIR}/print/freetype2 \
			iconv:${PORTSDIR}/converters/libiconv \
			pcre:${PORTSDIR}/devel/pcre \

BUILD_DEPENDS=		v4l_compat>=1.0.20100321:${PORTSDIR}/multimedia/v4l_compat \
			cuse4bsd-kmod>=0.1.24:${PORTSDIR}/multimedia/cuse4bsd-kmod \
			webcamd:${PORTSDIR}/multimedia/webcamd \
			nasm:${PORTSDIR}/devel/nasm
LIB_DEPENDS+=		v4l1:${PORTSDIR}/multimedia/libv4l \
			cuse4bsd:${PORTSDIR}/multimedia/cuse4bsd-kmod

USE_QT4=		corelib gui opengl qmake

PLIST_FILES=		bin/OpenStopMotion \
			share/pixmaps/OpenStopMotion.png \
			share/applications/OpenStopMotion.desktop

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
	${MKDIR} ${PREFIX}/bin ${PREFIX}/share/pixmaps ${PREFIX}/share/applications
	${INSTALL_PROGRAM} ${WRKSRC}/osm/release/${PORTNAME} ${PREFIX}/bin
	${INSTALL_DATA} ${WRKSRC}/osm/resources/icon256x256.png ${PREFIX}/share/pixmaps/${PORTNAME}.png
	${INSTALL_DATA} ${FILESDIR}/OpenStopMotion.desktop ${PREFIX}/share/applications/${PORTNAME}.desktop

.include <bsd.port.post.mk>
