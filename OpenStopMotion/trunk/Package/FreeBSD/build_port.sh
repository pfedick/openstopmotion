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
TARGETPATH=/ftp/build

CUR=`pwd`

if [ -f "OpenStopMotion.pro" ] ; then
	CUR=`pwd`
elif [ -f "build_source.sh" ] ; then
	cd ..
	CUR=`pwd`
elif [ -f "build_port.sh" ] ; then
	cd ../..
	CUR=`pwd`
else
	echo "ERROR: Wrong directory, did not found sources"
	exit 1
fi

WORK=$CUR/tmp
DISTFILES=$CUR/distfiles

SOURCES="$DISTFILES/$PROGNAME-$VERSION-src.tar.bz2"

if [ `uname` != "FreeBSD" ] ; then
	echo "ERROR: can be build on FreeBSD only!"
	exit 0
fi

if [ ! -f "$SOURCES" ] ; then
	echo "ERROR: No source release found, expecting: $SOURCES"
	exit 0
fi

rm -rf $WORK/FreeBSD
mkdir -p $WORK/FreeBSD
cd $CUR/FreeBSD
find openstopmotion | cpio -pdm $WORK/FreeBSD
cd $DISTFILES
sha256 $PROGNAME-$VERSION-src.tar.bz2 > $WORK/FreeBSD/openstopmotion/distinfo
echo "SIZE ($PROGNAME-$VERSION-src.tar.bz2) = `stat -f \"%z\" $PROGNAME-$VERSION-src.tar.bz2`" >> $WORK/FreeBSD/openstopmotion/distinfo
cat $CUR/FreeBSD/openstopmotion/Makefile | sed "s/^PORTVERSION=.*$/PORTVERSION=		$VERSION/" > $WORK/FreeBSD/openstopmotion/Makefile
cd $WORK/FreeBSD/openstopmotion; make clean;
cd ..
shar `find openstopmotion | grep -v ".svn" `> $DISTFILES/$PROGNAME-$VERSION-FreeBSD-Port.shar
if [ -d "$TARGETPATH" ] ; then
	cp $DISTFILES/$PROGNAME-$VERSION-FreeBSD-Port.shar $TARGETPATH
fi

