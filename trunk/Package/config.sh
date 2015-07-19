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
 

PROGNAME="OpenStopMotion"
VERSION=${VERSION:=trunk}
REVISION="1"
PPL7SOURCE=${PPL7SOURCE:="../../ppl7"}
PPL7REPO="http://svn.code.sf.net/p/pplib/code/lib/trunk"
OSMREPO="https://openstopmotion.googlecode.com/svn/OpenStopMotion/trunk"
OSMDIR="$PROGNAME-$VERSION"
DESCRIPTION="Webcam capture program for creating stopmotion video"
HOMEPAGE="http://www.pfp.de/osm"
MAINTAINER="Patrick Fedick <patrick@pfp.de>"
TARGETPATH=/ftp/build
QMAKE=qmake-qt4
MAKE=make



DISTFILES=$CUR/distfiles
WORK=$CUR/tmp
OSMSOURCE=$CUR


create_dir ()
{
	mkdir -p $1
	if [ $? -ne 0 ] ; then
		echo "ERROR: could not create: $1"
		exit 1
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
elif [ -f /etc/os-release ] ; then
    DISTRIB_ID=`grep "^ID=" /etc/os-release | sed -e "s/ID=//g"`
    DISTRIB_RELEASE=`grep "^VERSION_ID=" /etc/os-release | sed -e "s/VERSION_ID=//g" | sed -e "s/\"//g"`
    if [ "$DISTRIB_ID" = "debian" ] ; then
        DISTRIB_ID="Debian"
    fi	
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





