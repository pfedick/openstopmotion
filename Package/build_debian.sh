#!/bin/sh
#
#
# This file is part of OpenStopMotion by Patrick Fedick
#
# $Author: pafe1234@gmail.com $
# $Revision: 495 $
# $Date: 2014-02-23 17:06:29 +0100 (So, 23 Feb 2014) $
# $Id: build_package.sh 495 2014-02-23 16:06:29Z pafe1234@gmail.com $
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
 
CUR=`pwd`

if [ -f "OpenStopMotion.pro" ] ; then
    CUR=`pwd`
elif [ -f "build_source.sh" ] ; then
    cd ..
    CUR=`pwd`
else
    echo "ERROR: Wrong directory, did not found sources"
    exit 1
fi
. $CUR/Package/config.sh


check_debian_package()
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

       
prepare_debian ()
{
	#
	# Check Dependencies
	#
	MISSING=""
	echo "INFO: Checking dependency packages..."
	check_debian_package "libpcre3-dev"
	check_debian_package "libjpeg8-dev"
	check_debian_package "libpng12-dev"
	check_debian_package "libtiff4-dev"
	check_debian_package "libfreetype6-dev"
	check_debian_package "libbz2-dev"
	check_debian_package "zlib1g-dev"
	check_debian_package "libqt4-dev"
	check_debian_package "qt4-dev-tools"
	check_debian_package "qt4-qmake"
	check_debian_package "libqt4-opengl-dev"
	check_debian_package "nasm"
	check_debian_package "subversion"
	check_debian_package "dpkg-dev"
	
	if [ "$MISSING" ] ; then
		echo "ERROR: Missing packages, please run the following command:"
		echo ""
		echo "   sudo apt-get install $MISSING"
		echo ""
		exit 1
	fi
	echo "INFO: all required packages are installed"
}

prepare_ubuntu ()
{
    #
    # Check Dependencies
    #
    MISSING=""
    echo "INFO: Checking dependency packages..."
    check_debian_package "libpcre3-dev"
    check_debian_package "libjpeg-dev"
    check_debian_package "libpng12-dev"
    check_debian_package "libtiff4-dev"
    check_debian_package "libfreetype6-dev"
    check_debian_package "libbz2-dev"
    check_debian_package "zlib1g-dev"
    check_debian_package "libqt4-dev"
    check_debian_package "qt4-dev-tools"
    check_debian_package "qt4-qmake"
    check_debian_package "libqt4-opengl-dev"
    check_debian_package "nasm"
    check_debian_package "subversion"
    check_debian_package "dpkg-dev"
    
    if [ "$MISSING" ] ; then
        echo "ERROR: Missing packages, please run the following command:"
        echo ""
        echo "   sudo apt-get install $MISSING"
        echo ""
        exit 1
    fi
    echo "INFO: all required packages are installed"
}


build_debian ()
{
    rm -rf $WORK/build_debian
	create_dir $WORK/build_debian
	cd $WORK/build_debian
	DISTNAME="$PROGNAME-$VERSION"
	
	tar -xjf $DISTFILES/$DISTNAME-src.tar.bz2
	if [ $? -ne 0 ] ; then
	    exit 1
    fi
    cd $DISTNAME
    ./configure --prefix=/usr --with-pcre=/usr --with-jpeg --with-png --with-libtiff=/usr --with-nasm --without-libmcrypt-prefix
    if [ $? -ne 0 ] ; then
        exit 1
    fi
	make	
    
	echo "INFO: Build Debian-Packet for $DISTRIB_ID $DISTRIB_RELEASE: $DISTRIB_CODENAME"
	
	rm -rf debian
	create_dir debian
	create_dir debian/DEBIAN
	create_dir debian/usr/bin
	create_dir debian/usr/share/applications
	create_dir debian/usr/share/pixmaps
	cp osm/release/OpenStopMotion debian/usr/bin/$PROGNAME
	cp osm/resources/icon256x256.png debian/usr/share/pixmaps/$PROGNAME.png
    cp OpenStopMotion.desktop debian/usr/share/applications/$PROGNAME.desktop
	DEPENDS=""
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



create_dir $DISTFILES

echo "Build $PROGNAME $VERSION for: $DISTRIB_ID $DISTRIB_RELEASE..."
echo ""


if [ "$DISTRIB_ID" = "Ubuntu" ] ; then
	prepare_ubuntu
	build_debian
elif [ "$DISTRIB_ID" = "Debian" ] ; then
    prepare_debian
	build_debian
else
	echo "ERROR: no automated build for this system"
	echo "INFO: DISTRIB_ID=$DISTRIB_ID"
	exit 1
fi


exit 0





