#!/bin/sh
#
#
# This file is part of OpenStopMotion by Patrick Fedick
#
# $Author$
# $Revision$
# $Date$
# $Id$
#
#
# Copyright (c) 2013 Patrick Fedick <patrick@pfp.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
 

PROGNAME="OpenStopMotion"
VERSION="0.6.0"
PPL7SOURCE=../../../ppl7
OSMSOURCE=../
PPL7REPO="http://svn.code.sf.net/p/pplib/code/lib/trunk"
OSMREPO="https://openstopmotion.googlecode.com/svn/OpenStopMotion/trunk"
OSMDIR="$PROGNAME-$VERSION"
WORK=`pwd`/tmp
DISTFILES=`pwd`/distfiles
DESCRIPTION="Webcam capture program for creating stopmotion video"
HOMEPAGE="http://www.pfp.de/"
MAINTAINER="Patrick Fedick <patrick@pfp.de>"
TARGETPATH=/ftp/build

QMAKE=qmake-qt4


create_dir ()
{
	mkdir -p $1
	if [ $? -ne 0 ] ; then
		echo "ERROR: could not create: $1"
		exit 1
	fi
}

check_package()
{
	PKG=$1
	echo -n "$PKG: "
	dpkg -l | grep $PKG >/dev/null 2>&1
	if [ $? -ne 0 ] ; then
		echo "no"
		MISSING="$MISSING $PKG"
	else
		echo "ok"
	fi
}

gather_sources()
{
	CUR=`pwd`

	if [ -d "$PPL7SOURCE" ] ; then
		echo "INFO: Copy PPL7-sources from local directory: $PPL7SOURCE..."
		create_dir "ppl7"
		cd $PPL7SOURCE
		find *.m4 autoconf configure docs Doxyfile HISTORY.TXT include LICENSE.TXT Makefile.in ppl7-config.in README.TXT resource src tests | cpio -pdm "$CUR/ppl7"
		echo "INFO: done"
	else
		echo "INFO: checkout PPL7-sources from svn repository..."
		svn checkout $PPL7REPO ppl7
		if [ $? -ne 0 ] ; then
			echo "ERROR: checkout failed"
			exit 1
		fi
		echo "INFO: done"
	fi
	cd $CUR
	if [ -d "$OSMSOURCE" ] ; then
		echo "INFO: Copy OpenStopMotion-sources from local directory: $OSMSOURCE..."
		create_dir "osm"
		cd $OSMSOURCE
		find *.TXT OpenStopMotion.pro gui resource.rc resources resources.qrc src | cpio -pdm "$CUR/osm"
                echo "INFO: done"
        else
		echo "INFO: checkout OpenStopMotion-sources from svn repository..."
                svn checkout $OSMREPO osm
                if [ $? -ne 0 ] ; then
                        echo "ERROR: checkout failed"
                        exit 1
                fi
                echo "INFO: done"
	fi
	cd $CUR
}

build_ppl7 ()
{
	echo "INFO: building ppl7..."
	create_dir $1/bin
	create_dir $1/lib
	create_dir $1/include
	cd ppl7
	if [ ! -f Makefile ] ; then
		echo "INFO: Configuring ppl7..."
		./configure --prefix=$1 --with-pcre=/usr --with-jpeg --with-png --with-libtiff=/usr --with-nasm
	fi
	if [ $? -ne 0 ] ; then
		echo "ERROR: configure for ppl7 failed"
		exit 1
	fi
	make -j2
	if [ $? -ne 0 ] ; then
		echo "ERROR: make for ppl7 failed"
		exit 1
	fi
	make install
	if [ $? -ne 0 ] ; then
		echo "ERROR: make install for ppl7 failed"
		exit 1
	fi
	echo "INFO: building ppl7 done"
}

build_osm ()
{
	echo "INFO: building $PROGNAME..."
	create_dir $1/bin
	PATH="$1/bin:$PATH"
	export PATH
	echo "PATH=$PATH"
	cd osm
	qmake-qt4
	if [ $? -ne 0 ] ; then
                echo "ERROR: qmake for $PROGNAME failed"
                exit 1
        fi
	make -j2 release
	if [ $? -ne 0 ] ; then
                echo "ERROR: make for $PROGNAME failed"
                exit 1
        fi
	cp release/OpenStopMotion $1/bin
	echo "INFO: building $PROGNAME done"
}

gen_checksum ()
{
	SUM=`sha256sum -b debian/$1 | awk '{print $1}'`
	if [ $? -ne 0 ] ; then
		echo "ERROR: Generating checksum failed: $SUM"
		exit 1
	fi
	SIZE=`stat --format=%s debian/$1`
	if [ $? -ne 0 ] ; then
		echo "ERROR: stat failed: $SIZE"
		exit 1
	fi
	echo " $SUM $SIZE $1"
}

ubuntu_write_control ()
{
	(
		echo "Source: $PROGNAME"
		echo "Section: video"
		echo "Priority: optional"
		echo "Maintainer: $MAINTAINER"
                echo "Package: $PROGNAME"
                echo "Version: $VERSION"
                echo "Homepage: $HOMEPAGE"
                echo "Architecture: $PLATFORM"
                echo "Depends: $DEPENDS"
                echo "Installed-Size: 1000"
                echo "Description: $DESCRIPTION"
                cat osm/README.TXT | while read line
		do
                        if [ -z "$line" ] ; then
                        	echo " ."
                        else
                        	echo "  $line"
                        fi
                done
        ) > debian/control
}

build_debian ()
{
	echo "INFO: Build Debian-Packet for $DISTRIB_ID $DISTRIB_RELEASE: $DISTRIB_CODENAME"
	DISTNAME="$PROGNAME-$VERSION"
	rm -rf debian
	create_dir debian
	create_dir debian/DEBIAN
	create_dir debian/usr/bin
	create_dir debian/usr/share/applications
	create_dir debian/usr/share/pixmaps
	cp bin/OpenStopMotion debian/usr/bin/$PROGNAME
	cp osm/resources/icon256x256.png debian/usr/share/pixmaps/$PROGNAME.png

	DEPENDS=""

        (
                echo "[Desktop Entry]"
                echo "Encoding=UTF-8"
                echo "Name=$PROGNAME"
                echo "Comment=$DESCRIPTION"
                echo "Exec=/usr/bin/$PROGNAME"
                echo "Terminal=false"
                echo "Type=Application"
                echo "Categories=GTK;GNOME;AudioVideo;"
                echo "Icon=/usr/share/pixmaps/$PROGNAME.png"
        ) > debian/usr/share/applications/$PROGNAME.desktop
        (
                echo "$PROGNAME ($VERSION) unstable; urgency=low"
                echo ""
                echo " * No upstream changes."
                echo ""
                echo -n "-- $USER   "
                date 
                echo ""
                echo "Local variables:"
        	echo "mode: debian-changelog"
        	echo "End:"
        ) > debian/changelog

	ubuntu_write_control
	dpkg-shlibdeps debian/usr/bin/$PROGNAME
        DEPENDS=`grep "shlibs:Depends" debian/substvars | sed -e "s/shlibs:Depends=//"`
        ubuntu_write_control
	cp debian/control debian/DEBIAN
        rm debian/control
        rm debian/substvars
        rm debian/changelog

	dpkg -b debian $DISTFILES/$DISTNAME-$DISTRIB_ID-$DISTRIB_RELEASE-$PLATFORM.deb
        if [ $? -ne 0 ] ; then
                echo "ERROR: Fehler beim Erstellen des Pakets"
                exit
        fi
	if [ -n "$TARGETPATH" ] ; then
		if [ -d "$TARGETPATH" ] ; then
			cp $DISTFILES/$DISTNAME-$DISTRIB_ID-$DISTRIB_RELEASE-$PLATFORM.deb $TARGETPATH
		fi
	fi

}

#
# Identify System
#
if [ -f /etc/lsb-release ] ; then
	. /etc/lsb-release
elif [ -f /etc/system-release ] ; then
	DISTRIB_ID=`cat /etc/system-release | awk '{print $1}'`
	DISTRIB_RELEASE=`cat /etc/system-release | awk '{print $3}'`
else
	DISTRIB_ID=`uname -s`
	DISTRIB_RELEASE=`uname -r`

	case "$DISTRIB_ID:$DISTRIB_RELEASE" in
		MINGW32*:1.0.11*)
			DISTRIB_ID="MINGW32";
			DISTRIB_RELEASE="1.0.11";
			;;
		MINGW32*:1.0.17*)
			DISTRIB_ID="MINGW32";
			DISTRIB_RELEASE="1.0.17";
			QTDIR=/c/Qt/$QTVERSION
			QMAKESPEC=win32-g++
			PATH=$PATH:/c/Qt/$QTVERSION/bin
			echo "PATH=$PATH"
			;;
	esac
fi

# Wir haben nun unter anderem:
#    DISTRIB_ID=Ubuntu
#    DISTRIB_RELEASE=9.10

ARCH=`uname -m`;
PLATFORM="i386";
if [ "$ARCH" = "x86_64" ] ; then
        PLATFORM="amd64"
fi


create_dir $DISTFILES

if [ -f "OpenStopMotion.pro" ] ; then
	#rm -rf tmp
	create_dir "tmp"
	cd $WORK
	gather_sources
	rm -rf $PROGNAME-$VERSION
	create_dir "$PROGNAME-$VERSION"
	cd ppl7
	find *.m4 autoconf configure docs Doxyfile HISTORY.TXT include LICENSE.TXT Makefile.in ppl7-config.in README.TXT resource src tests | cpio -pdm "$WORK/$PROGNAME-$VERSION/ppl7"
	cd $WORK
	cd osm
	find *.TXT OpenStopMotion.pro gui resource.rc resources resources.qrc src | cpio -pdm "$WORK/$PROGNAME-$VERSION/osm"
	cd $WORK
	cp ../build_debian.sh "$PROGNAME-$VERSION"
	cp ../*.TXT "$PROGNAME-$VERSION"
	tar -cjf $DISTFILES/$PROGNAME-$VERSION-src.tar.bz2 "$PROGNAME-$VERSION"
fi


if [ "$DISTRIB_ID" != "Ubuntu" ] ; then
	if [ "$DISTRIB_ID" != "Debian" ] ; then
		echo "ERROR: This build-script is for Ubuntu or Debian based systems only"
		exit 1
	fi
fi

echo "Build $PROGNAME $VERSION for: $DISTRIB_ID $DISTRIB_RELEASE..."
echo ""




#
# Check Dependencies
#
MISSING=""
echo "INFO: Checking dependency packages..."
check_package "libpcre3-dev"
check_package "libjpeg-dev"
check_package "libpng12-dev"
check_package "libtiff4-dev"
check_package "libfreetype6-dev"
check_package "libbz2-dev"
check_package "zlib1g-dev"
check_package "libqt4-dev"
check_package "qt4-dev-tools"
check_package "qt4-qmake"
check_package "libqt4-opengl-dev"
check_package "nasm"
check_package "subversion"
check_package "dpkg-dev"

if [ "$MISSING" ] ; then
	echo "ERROR: Missing packages, please run the following command:"
	echo ""
	echo "   sudo apt-get install $MISSING"
	echo ""
	exit 1
fi
echo "INFO: all required packages are installed"



CUR=`pwd`
build_ppl7 $CUR
cd $CUR
build_osm $CUR

cd $CUR
build_debian 