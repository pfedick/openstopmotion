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
VERSION=${VERSION:=trunk}
REVISION="1"
PPL7SOURCE=${PPL7SOURCE:="../../ppl7"}
OSMSOURCE=./
PPL7REPO="http://svn.code.sf.net/p/pplib/code/lib/trunk"
OSMREPO="https://openstopmotion.googlecode.com/svn/OpenStopMotion/trunk"
OSMDIR="$PROGNAME-$VERSION"
WORK=`pwd`/tmp
CUR=`pwd`
DISTFILES=`pwd`/distfiles
DESCRIPTION="Webcam capture program for creating stopmotion video"
HOMEPAGE="http://www.pfp.de/osm"
MAINTAINER="Patrick Fedick <patrick@pfp.de>"
TARGETPATH=/ftp/build
QMAKE=qmake-qt4
MAKE=make


create_dir ()
{
	mkdir -p $1
	if [ $? -ne 0 ] ; then
		echo "ERROR: could not create: $1"
		exit 1
	fi
}

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

gather_sources()
{
	
	TARGET=$1
	cd $CUR
	PPL7DIR=$TARGET/ppl7
	if [ -d "$PPL7SOURCE" ] ; then
		echo "INFO: Copy PPL7-sources from local directory: $PPL7SOURCE..."
		echo "INFO: Ziel: $TARGET/ppl7"
		create_dir "$TARGET/ppl7"
		cd $PPL7SOURCE
		find *.m4 autoconf configure docs Doxyfile HISTORY.TXT include LICENSE.TXT Makefile.in ppl7-config.in README.TXT resource src tests | cpio -pdm "$TARGET/ppl7" > /dev/null 2>&1
		echo "INFO: done"
	else
		echo "INFO: checkout PPL7-sources from svn repository..."
		svn checkout $PPL7REPO $TARGET/ppl7
		if [ $? -ne 0 ] ; then
			echo "ERROR: checkout failed"
			exit 1
		fi
		echo "INFO: done"
	fi
	cd $CUR
	OSMDIR=$TARGET/osm
	if [ -d "$OSMSOURCE" ] ; then
		echo "INFO: Copy OpenStopMotion-sources from local directory: $OSMSOURCE..."
		echo "INFO: Ziel: $TARGET/osm"
		create_dir "$TARGET/osm"
		cd $OSMSOURCE
		find *.TXT arduino* OpenStopMotion.pro include gui resource.rc resources resources.qrc src | cpio -pdm "$TARGET/osm" > /dev/null 2>&1
        echo "INFO: done"
    else
		echo "INFO: checkout OpenStopMotion-sources from svn repository..."
                svn checkout $OSMREPO $TARGET/osm
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
	cd $PPL7DIR
	if [ ! -f Makefile ] ; then
		echo "INFO: Configuring ppl7..."
		./configure --prefix=$1 $CONFIGURE
	fi
	if [ $? -ne 0 ] ; then
		echo "ERROR: configure for ppl7 failed"
		exit 1
	fi
	$MAKE -j2
	if [ $? -ne 0 ] ; then
		echo "ERROR: make for ppl7 failed"
		exit 1
	fi
	$MAKE install
	if [ $? -ne 0 ] ; then
		echo "ERROR: make install for ppl7 failed"
		exit 1
	fi
	cd ..
	echo "INFO: building ppl7 done"
	sleep 2
}

build_osm ()
{
	echo "INFO: building $PROGNAME..."
	create_dir $1/bin
	PATH="$1/bin:$PATH"
	export PATH
	echo "PATH=$PATH"
	cd $OSMDIR
	echo "INFO: calling $QMAKE"
	$QMAKE
	if [ $? -ne 0 ] ; then
                echo "ERROR: qmake for $PROGNAME failed"
                exit 1
        fi
    echo "INFO: calling $MAKE in `pwd`"
	$MAKE
	if [ $? -ne 0 ] ; then
                echo "ERROR: make for $PROGNAME failed"
                exit 1
        fi
	cp release/OpenStopMotion $1/bin/$PROGNAME
	echo "INFO: building $PROGNAME done"
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

	cd $CUR

	CONFIGURE="--with-pcre=/usr --with-jpeg --with-png --with-libtiff=/usr --with-nasm --without-libmcrypt-prefix"
	build_ppl7 $WORK
	cd $CUR
	build_osm $WORK
	cd $WORK

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
                echo "Categories=Qt;AudioVideo;"
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

check_rpm_package ()
{
	PKG=$1
	echo -n "$PKG: "
	rpm -qa | grep $PKG >/dev/null 2>&1
	if [ $? -ne 0 ] ; then
		echo "no"
		MISSING="$MISSING $PKG"
	else
		echo "ok"
	fi
}

write_desktop_application ()
{
	PREFIX=$1
	FILE=$2
	(
                echo "[Desktop Entry]"
                echo "Encoding=UTF-8"
                echo "Name=$PROGNAME"
                echo "Comment=$DESCRIPTION"
                echo "Exec=$PREFIX/bin/$PROGNAME"
                echo "Terminal=false"
                echo "Type=Application"
                echo "Categories=GTK;GNOME;AudioVideo;"
                echo "Icon=$PREFIX/share/pixmaps/$PROGNAME.png"
        ) > $FILE
}

write_specfile ()
{
	(
	echo "%define __spec_prep_post true"
	echo "%define __spec_prep_pre true"
	echo "%define __spec_build_post true"
	echo "%define __spec_build_pre true"
	echo "%define __spec_install_post true"
	echo "%define __spec_install_pre true"
	echo "%define __spec_clean_post true"
	echo "%define __spec_clean_pre true"
	echo "%define _binary_filedigest_algorithm 1"
	echo "%define _binary_payload w9.gzdio"
	echo ""
	echo "Name: $PROGNAME"
	echo "Version: $VERSION"
	echo "Release: $REVISION"
	echo "Summary: $DESCRIPTION"
	echo "BuildArch: $ARCH"
	echo "AutoReqProv: yes"
	echo "BuildRoot: %buildroot"
	echo ""
	echo "Prefix: /"
	echo ""
	echo "Group: Applications/Multimedia"
	echo "License: GPL3"
	echo "Vendor: $MAINTAINER"
	echo "URL: $HOMEPAGE"
	echo "Packager: $MAINTAINER"
	echo ""
	echo "%description"
	cat ../README.TXT
	echo ""
	echo "%prep"
	echo "# noop"
	echo ""
	echo "%build"
	echo "# noop"
	echo ""
	echo "%install"
	echo "[ ! -d $BUILD/usr/bin ] && mkdir -p $BUILD/usr/bin"
	echo "cp $WORK/package/usr/bin/$PROGNAME $BUILD/usr/bin/$PROGNAME"
	echo "[ ! -d $BUILD/usr/share/pixmaps ] && mkdir -p $BUILD/usr/share/pixmaps"
	echo "cp $WORK/package/usr/share/pixmaps/$PROGNAME.png $BUILD/usr/share/pixmaps/$PROGNAME.png"
	echo "[ ! -d $BUILD/usr/share/applications ] && mkdir -p $BUILD/usr/share/applications"
	echo "cp $WORK/package/usr/share/applications/$PROGNAME.desktop $BUILD/usr/share/applications/$PROGNAME.desktop"
	echo ""
	echo "%clean"
	echo "# noop"
	echo ""
	echo "%files"
	echo "%defattr(-,root,root,-)"
	echo "/usr/bin/OpenStopMotion"
	echo "/usr/share/pixmaps/OpenStopMotion.png"
	echo "/usr/share/applications/OpenStopMotion.desktop"
	echo ""
	echo "%changelog"
	) > $PROGNAME.spec
}

build_redhat ()
{
	if [ ! -f packages.checked ] ; then
		#
	        # Check Dependencies
	        #
	        MISSING=""
	        echo "INFO: Checking dependency packages..."
	        check_rpm_package "pcre-devel"
	        check_rpm_package "libjpeg-devel"
	        check_rpm_package "libpng-devel"
	        check_rpm_package "libtiff-devel"
	        check_rpm_package "freetype-devel"
	        check_rpm_package "bzip2-devel"
	        check_rpm_package "zlib-devel"
	        check_rpm_package "qt-devel"
	        check_rpm_package "nasm"
	        check_rpm_package "subversion"
	        check_rpm_package "rpm-devel"
		if [ "$1" = "fpm" ] ; then
		        check_rpm_package "ruby"
		        check_rpm_package "ruby-devel"
		        check_rpm_package "rubygems"
		fi

	        if [ "$MISSING" ] ; then
	                echo "ERROR: Missing packages, please run the following command as root:"
	                echo ""
	                echo "   yum install $MISSING"
			if [ "$1" = "fpm" ] ; then
				fpm -h > /dev/null 2>&1
				if [ $0 -ne 0 ] ; then
		                	echo "   ruby install fpm"
				fi
			fi
	                echo ""
	                exit 1
	        fi
		if [ "$1" = "fpm" ] ; then
		fpm -h > /dev/null 2>&1
			if [ $0 -ne 0 ] ; then
				echo "ERROR: fpm not installed, please run the following command as root:"
		                echo ""
		                echo "   ruby install fpm"
		                echo ""
				exit 1
			fi
		fi
	
		
		touch packages.checked
	fi
        echo "INFO: all required packages are installed"

	CUR=`pwd`
        CONFIGURE="--with-pcre=/usr --with-jpeg --with-png --with-libtiff=/usr --with-nasm --without-libmcrypt-prefix"
        build_ppl7 $CUR
        cd $CUR
        build_osm $CUR

        cd $CUR

        echo "INFO: Build rpm for $DISTRIB_ID $DISTRIB_RELEASE: $DISTRIB_CODENAME"

	create_dir package
	create_dir package/usr/bin
	create_dir package/usr/share/applications
	create_dir package/usr/share/pixmaps
	write_desktop_application "/usr" package/usr/share/applications/$PROGNAME.desktop

	cp osm/resources/icon256x256.png package/usr/share/pixmaps/$PROGNAME.png
	cp bin/$PROGNAME package/usr/bin

	if [ "$1" = "fpm" ] ; then
	cd package
		fpm -p $DISTFILES -t rpm -s dir -n $PROGNAME -v $VERSION \
			--iteration $REVISION --url "$HOMEPAGE" \
			--license "GPL3" --description "$DESCRIPTION" \
			--category "Applications/Multimedia" \
			--maintainer "$MAINTAINER" --vendor "$MAINTAINER" \
			-e ./
	
		cd $WORK
	else
		create_dir build/BUILD
		BUILD=`pwd`/build/BUILD
		write_specfile
		rpmbuild --buildroot=$BUILD --define "_rpmdir ."  -bb $PROGNAME.spec
		if [ $? -ne 0 ] ; then
			echo "ERROR: rpmbuild failed"
			exit 1
		fi
		mv $ARCH/$PROGNAME* $DISTFILES
	fi

}

build_suse ()
{
	if [ ! -f packages.checked ] ; then
		#
	        # Check Dependencies
	        #
	        MISSING=""
	        echo "INFO: Checking dependency packages..."
	        check_rpm_package "pcre-devel"
	        check_rpm_package "libjpeg8-devel"
	        check_rpm_package "libpng15-devel"
	        check_rpm_package "libtiff-devel"
	        check_rpm_package "freetype-devel"
	        check_rpm_package "libbz2-devel"
	        check_rpm_package "zlib-devel"
	        check_rpm_package "libqt4-devel"
	        check_rpm_package "nasm"
	        check_rpm_package "subversion"
	        check_rpm_package "rpm-devel"
	        if [ "$MISSING" ] ; then
	                echo "ERROR: Missing packages, please run the following command as root:"
	                echo "   sudo zypper install $MISSING"
			exit 1
		fi
		touch packages.checked
	fi
        echo "INFO: all required packages are installed"

        cd $WORK
	echo "WORK=$WORK"
        CONFIGURE="--with-pcre=/usr --with-jpeg --with-png --with-libtiff=/usr --with-nasm --without-libmcrypt-prefix"
        build_ppl7 $WORK
        cd $WORK
        build_osm $WORK
        cd $WORK

        echo "INFO: Build rpm for $DISTRIB_ID $DISTRIB_RELEASE: $DISTRIB_CODENAME"
	create_dir package
	create_dir package/usr/bin
	create_dir package/usr/share/applications
	create_dir package/usr/share/pixmaps
	write_desktop_application "/usr" package/usr/share/applications/$PROGNAME.desktop
	cp $OSMDIR/resources/icon256x256.png package/usr/share/pixmaps/$PROGNAME.png
	cp bin/$PROGNAME package/usr/bin
	create_dir build/BUILD
	BUILD=`pwd`/build/BUILD
	write_specfile
	rpmbuild --buildroot=$BUILD --define "_rpmdir ."  -bb $PROGNAME.spec
	if [ $? -ne 0 ] ; then
		echo "ERROR: rpmbuild failed"
		exit 1
	fi
	mv $ARCH/$PROGNAME-$VERSION-$REVISION.`uname -m`.rpm  $DISTFILES
	if [ -d "$TARGETPATH" ] ; then
		cp $DISTFILES/$PROGNAME-$VERSION-$REVISION.`uname -m`.rpm $TARGETPATH
	fi
}

freebsd_dep ()
{
	NAME=`pkg_info | grep "^$1-" | awk '{print $1}'`
	if [ $? -ne 0 ] ; then
		exit 1
	fi
	echo "@pkgdep $NAME"
}

build_freebsd ()
{
	CONFIGURE="--with-pcre=/usr/local --with-libiconv-prefix=/usr/local --with-nasm --with-jpeg --with-png --with-libtiff=/usr/local"
	cd $WORK 
    build_ppl7 $WORK
	cd $WORK
	echo -n "Current Dir="
	pwd
	sleep 2
	build_osm $WORK
	cd $WORK
	echo "INFO: Build FreeBSD-Packet for $DISTRIB_ID $DISTRIB_RELEASE"
	DISTNAME="$PROGNAME-$VERSION-$DISTRIB_ID-$DISTRIB_RELEASE-$PLATFORM"
	create_dir package
	create_dir package/bin
	create_dir package/share/applications
	create_dir package/share/pixmaps
	
		(
                echo "[Desktop Entry]"
                echo "Encoding=UTF-8"
                echo "Name=$PROGNAME"
                echo "Comment=$DESCRIPTION"
                echo "Exec=/usr/local/bin/$PROGNAME"
                echo "Terminal=false"
                echo "Type=Application"
                echo "Categories=GTK;GNOME;AudioVideo;"
                echo "Icon=/usr/local/share/pixmaps/$PROGNAME.png"
        ) > package/share/applications/$PROGNAME.desktop
	cp osm/resources/icon256x256.png package/share/pixmaps/$PROGNAME.png
	cp bin/$PROGNAME package/bin
	(
		echo "@cwd /usr/local"
		echo "@srcdir $WORK/package"
		freebsd_dep pcre
		freebsd_dep png
		freebsd_dep tiff
		freebsd_dep jpeg
		freebsd_dep freetype2
		freebsd_dep libiconv
		freebsd_dep qt4-gui
		freebsd_dep qt4-corelib
		freebsd_dep qt4-opengl
		echo "share/applications/$PROGNAME.desktop"
		echo "share/pixmaps/$PROGNAME.png"
		echo "bin/$PROGNAME"
	) > pkg_list
	
	pkg_create -v -d ../README.TXT -p package -f pkg_list -c "-$DESCRIPTION" $PROGNAME-$VERSION
	cp $PROGNAME-$VERSION.tbz $DISTFILES/$DISTNAME.tbz	
}



write_source_specfile() {
	FILE=$1
	BUILDREQUIRES=$2
	COMMENT=`cat $WORK/../README.TXT| grep -v "^===.*" `
	(
	echo "Name:		$PROGNAME"
	echo "Version:	$VERSION"
	echo "Release:	$REVISION"
	echo "Summary:	$DESCRIPTION"
	echo "Autoreq:  yes"
	echo ""
	echo "Group:		Applications/Multimedia"
	echo "License:	GPL3"
	echo "URL:		$HOMEPAGE"
	echo "Source:	$PROGNAME-%{version}-src.tar.bz2"
	echo "BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)"
	echo ""
	echo "BuildRequires:	$BUILDREQUIRES"
	echo ""	
	echo "%description"
	echo "$COMMENT"
	echo ""
	echo "%prep"
	echo "%setup -q -n %{name}-%{version}"
	echo ""
	echo "%build"
	echo "WORK=\`pwd\`"
	echo "mkdir -p bin lib include"
	echo "cd ppl7"
	echo "./configure --prefix=\$WORK --with-pcre=/usr --with-jpeg --with-png --with-libtiff=/usr --with-nasm --without-libmcrypt-prefix"
	echo "make install"
	echo "cd ../osm"
	echo "PATH=\"\$WORK/bin:\$PATH\""
	echo "$QMAKE"
	echo "make -j2 release"
	echo "cp release/OpenStopMotion \$WORK/bin/$PROGNAME"
	echo ""
	echo "%install"
	echo "rm -rf \$RPM_BUILD_ROOT"
	echo "mkdir -p \$RPM_BUILD_ROOT/usr/bin"
	echo "mkdir -p \$RPM_BUILD_ROOT/usr/share/pixmaps"
	echo "mkdir -p \$RPM_BUILD_ROOT/usr/share/applications"
	echo "cp bin/$PROGNAME \$RPM_BUILD_ROOT/usr/bin"
	echo "cp osm/resources/icon256x256.png \$RPM_BUILD_ROOT/usr/share/pixmaps/$PROGNAME.png"
	echo ""
	echo "# Desktop menu entry"
	echo "cat > %{name}.desktop << EOF"
	echo "[Desktop Entry]"
	echo "Name=$PROGNAME"
	echo "Comment=$DESCRIPTION"
	echo "Exec=/usr/bin/$PROGNAME"
	echo "Icon=/usr/share/pixmaps/$PROGNAME.png"
	echo "Terminal=0"
	echo "Type=Application"
	echo "Categories=Qt;AudioVideo;"
	echo "EOF"
	echo ""
	echo "desktop-file-install --vendor \"Patrick F.-Productions\" \\"
  	echo "  --dir \$RPM_BUILD_ROOT/usr/share/applications \\"
  	echo "  --add-category Qt \\"
  	echo "  --add-category AudioVideo \\"
  	echo "%{name}.desktop"
  	echo ""
	echo "%clean"
	echo "rm -rf \$RPM_BUILD_ROOT"
	echo ""
	echo "%files"
	echo "%defattr(-,root,root,-)"
	echo "/usr/bin/$PROGNAME"
	echo "/usr/share/pixmaps/$PROGNAME.png"
	echo "/usr/share/applications/$PROGNAME"
	echo "%doc"
	echo ""
	echo "%changelog"
	echo ""
	) > $FILE
	
}


build_srpm() {
	if [ ! -d ~.rpmmacros ] ; then
		echo "Bereite RPM-Buildsystem vor..."
		mkdir -p ~/rpmbuild/BUILD ~/rpmbuild/RPMS ~/rpmbuild/SOURCES ~/rpmbuild/SPECS ~/rpmbuild/SRPMS
		if [ $? -ne 0 ] ; then
			echo "Konnte RPM-Verzeichnisse nicht anlegen: ~/rpmbuild/{BUILD,RPMS,S{OURCE,PEC,RPM}S}"
			exit 1
		fi
		echo "%_topdir $HOME/rpmbuild" > ~/.rpmmacros
	fi
	cd $WORK
	BUILDREQUIRES="desktop-file-utils, gcc, gcc-c++, libgcc, bzip2-devel, zlib-devel, libjpeg-devel, libpng-devel, nasm, freetype-devel, libtiff-devel, libstdc++-devel, qt-devel, glibc-devel, pcre-devel"
	write_source_specfile "$WORK/$PROGNAME.spec" "$BUILDREQUIRES"
	
	cp $WORK/$PROGNAME.spec $DISTFILES/$PROGNAME-$VERSION.spec
	
	BUILDREQUIRES="desktop-file-utils, gcc, gcc-c++, libgcc_s1, libbz2-devel, zlib-devel, libjpeg8-devel, libpng15-devel, nasm, freetype-devel, libtiff-devel, libstdc++-devel, libqt4-devel, glibc-devel, pcre-devel"
	write_source_specfile "$DISTFILES/$PROGNAME-$VERSION-suse.spec" "$BUILDREQUIRES"
	
	
	
	TOPDIR=`cat ~/.rpmmacros | grep "%_topdir" | grep -v grep | awk {'print $2'}`
	if [ -z "$TOPDIR" ] ; then
		echo "%_topdir ist nicht in ~/.rpmmacros vorhanden"
		exit 1;
	fi
	if [ ! -d "$TOPDIR" ] ; then
		echo "%_topdir ist nicht vorhanden: $TOPDIR"
		exit 1
	fi
	if [ ! -d "$TOPDIR/SOURCES" ] ; then
		echo "%_topdir/SOURCES ist nicht vorhanden: $TOPDIR/SOURCES"
		exit 1
	fi
	cp $DISTFILES/$PROGNAME-$VERSION-src.tar.bz2 $TOPDIR/SOURCES
	cd $WORK
	rpmbuild -bs $PROGNAME.spec
	if [ $? -ne 0 ] ; then
		echo "Build fehlgeschlagen"
		exit 1
	fi
	cp $TOPDIR/SRPMS/$PROGNAME-$VERSION-$REVISION.src.rpm $DISTFILES
	
	if [ -d "$TARGETPATH" ] ; then
		cp $TOPDIR/SRPMS/$PROGNAME-$VERSION-$REVISION.src.rpm $TARGETPATH
		cp $DISTFILES/$PROGNAME-$VERSION.spec $TARGETPATH
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
elif [ -f /etc/SuSE-release ] ; then
	DISTRIB_ID=`head -n1 /etc/SuSE-release | awk '{print $1}'`
	DISTRIB_RELEASE=`head -n1 /etc/SuSE-release | awk '{print $2}'`
	QMAKE=qmake
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
if [ "$ARCH" = "x86_64" ] || [ "$ARCH" = "amd64" ] ; then
        PLATFORM="amd64"
fi


create_dir $DISTFILES

	
if [ -f "OpenStopMotion.pro" ] ; then
	cd $CUR
	#rm -rf $WORK
	create_dir $WORK
	
	gather_sources $WORK
	if [ "$1" = "source" ] ; then
		rm -rf $PROGNAME-$VERSION
		create_dir "$WORK/$PROGNAME-$VERSION"
		gather_sources "$WORK/$PROGNAME-$VERSION"
		cd $WORK
		cat ../build_package.sh | sed -e "s/^VERSION=.*$/VERSION=$VERSION/" >$PROGNAME-$VERSION/build_package.sh
		cp ../*.TXT "$PROGNAME-$VERSION"
		tar -cjf $DISTFILES/$PROGNAME-$VERSION-src.tar.bz2 --exclude .svn "$PROGNAME-$VERSION"
		if [ -d "$TARGETPATH" ] ; then
			cp $DISTFILES/$PROGNAME-$VERSION-src.tar.bz2 $TARGETPATH
		fi
		if [ `uname` = "FreeBSD" ] ; then
			cd $DISTFILES
			sha256 $PROGNAME-$VERSION-src.tar.bz2 > $CUR/FreeBSD/openstopmotion/distinfo
			echo "SIZE ($PROGNAME-$VERSION-src.tar.bz2) = `stat -f \"%z\" $PROGNAME-$VERSION-src.tar.bz2`" >> $CUR/FreeBSD/openstopmotion/distinfo
			cd $CUR/FreeBSD/openstopmotion; make clean;
			cd ..
			shar `find openstopmotion | grep -v ".svn" `| sed "s/^XPORTVERSION=.*$/XPORTVERSION=	$VERSION/" > $DISTFILES/$PROGNAME-$VERSION-FreeBSD-Port.shar
			if [ -d "$TARGETPATH" ] ; then
				cp $DISTFILES/$PROGNAME-$VERSION-FreeBSD-Port.shar $TARGETPATH
			fi
			cd $WORK
		fi
		build_srpm
		exit 0
	fi
fi
if [ ! -f $CUR/OpenStopMotion.pro ] ; then
	WORK=$CUR
	cd $WORK
fi

echo "Build $PROGNAME $VERSION for: $DISTRIB_ID $DISTRIB_RELEASE..."
echo ""


if [ "$DISTRIB_ID" = "Ubuntu" ] ; then
	build_debian
elif [ "$DISTRIB_ID" = "Debian" ] ; then
	build_debian
elif [ "$DISTRIB_ID" = "FreeBSD" ] ; then
    WORK=$CUR/tmp
	build_freebsd
elif [ "$DISTRIB_ID" = "CentOS" ] ; then
	build_redhat $1
elif [ "$DISTRIB_ID" = "Fedora" ] ; then
	build_redhat $1
elif [ "$DISTRIB_ID" = "RedHat" ] ; then
	build_redhat $1
elif [ "$DISTRIB_ID" = "openSUSE" ] ; then
	build_suse $1
else
	echo "ERROR: no automated build for this system"
	echo "INFO: DISTRIB_ID=$DISTRIB_ID"
	exit 1
fi


exit 0





