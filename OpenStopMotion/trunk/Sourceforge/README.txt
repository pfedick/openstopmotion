OpenStopMotion is a capture program for creating stopmotion video clips. It
runs on Linux and FreeBSD and it can capture from all webcams supported by
the kernel.

In this directory, you can find the sourcecode of OpenStopMotion in two
formats:

*-src.tar.bz2
    This file contains the plain sourcecode of OpenStopMotion and its
    dependency ppl7. You can find compile and installation instructions
    inside this file.

*.src.rpm
    This file contains the sourcecode of OpenStopMotion and its
    dependency ppl7 in the format of a source rpm for linux. You
    can build a binary rpm for you system by calling
    
       rpmbuild --rebuild OpenStopMotion-0.6.4-1.src.rpm
       
    After that, you will find the binary package here:
    
       cd ~/rpmbuild/RPMS/`uname -m`/
    

There are also precomiled packages for Ubuntu, CentOS, Fedora and openSUSE
in the subdirectories.

You can find a FreeBSD-Port in the FreeBSD subfolder. 
    