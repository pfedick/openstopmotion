#!/bin/sh
#
#
# This file is part of OpenStopMotion by Patrick Fedick
#
# $Author: pafe1234@gmail.com $
# $Revision: 483 $
# $Date: 2014-02-20 20:04:03 +0100 (Do, 20 Feb 2014) $
# $Id: build_package.sh 483 2014-02-20 19:04:03Z pafe1234@gmail.com $
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


create_dir ()
{
	mkdir -p $1
	if [ $? -ne 0 ] ; then
		echo "ERROR: could not create: $1"
		exit 1
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
		find *.m4 autoconf configure configure.ac docs Doxyfile HISTORY.TXT include LICENSE.TXT Makefile.in ppl7-config.in README.TXT resource src tests/gcovr tests/Makefile.in tests/ppl7-tests.h tests/src tests/testdata | cpio -pdm "$TARGET/ppl7" > /dev/null 2>&1
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
	cat $TARGET/osm/OpenStopMotion.pro | sed "s!\`ppl7-config!-L../tmp/lib \`../tmp/bin/ppl7-config!" > $TARGET/osm/OpenStopMotion.pro.tmp
	mv $TARGET/osm/OpenStopMotion.pro.tmp $TARGET/osm/OpenStopMotion.pro
	(
		echo "#ifndef VERSION_H_"
		echo "#define VERSION_H_"
		echo "#define STOPMOCAP_VERSION		\"$VERSION\""
		echo "#endif /* VERSION_H_ */"
	) > $TARGET/osm/include/version.h
	
	cd $CUR
}

create_configure()
{
	TARGET=$1
	cd $WORK/$PROGNAME-$VERSION
	cp $CUR/Package/configure.ac configure.ac
	aclocal
	automake --add-missing --copy
	autoconf
	
}

echo "Building source package for $PROGNAME $VERSION..."
echo ""

rm -rf $WORK

create_dir $WORK
create_dir $DISTFILES
create_dir "$WORK/$PROGNAME-$VERSION"
gather_sources "$WORK/$PROGNAME-$VERSION"
create_configure "$WORK/$PROGNAME-$VERSION"

cd $CUR
cp Package/Makefile.in $WORK/$PROGNAME-$VERSION
cp Package/OpenStopMotion.desktop.in $WORK/$PROGNAME-$VERSION

####cat build_package.sh | sed -e "s/^VERSION=.*$/VERSION=$VERSION/" >$WORK/$PROGNAME-$VERSION/build_package.sh
cp *.TXT $WORK/$PROGNAME-$VERSION

cd $WORK
tar -cjf $DISTFILES/$PROGNAME-$VERSION-src.tar.bz2 --exclude .svn "$PROGNAME-$VERSION"
if [ -d "$TARGETPATH" ] ; then
	cp $DISTFILES/$PROGNAME-$VERSION-src.tar.bz2 $TARGETPATH
fi

echo ""
echo "done: $DISTFILES/$PROGNAME-$VERSION-src.tar.bz2"
		
