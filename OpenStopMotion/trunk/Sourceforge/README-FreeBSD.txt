To install this Port on FreeBSD, you need at least Version 8.0.

OpenStopMotion requires the webcamd daemon, which provides USB device drivers
for hundreds of different USB based webcam and DVB USB devices. It is actualy
a port of "Video4Linux" into userspace on FreeBSD. If you install this port,
the webcamd is also installed as a dependency, but you need to activate it
(see below).

To install this port, download the *.shar-File and put it somewhere on your
FreeBSD-Host. Then call

   sh OpenStopMotion-*-FreeBSD-Port.shar

(replace * with the downloaded version)

This will unpack the shar-File and create a subdirectory called "openstopmotion".
Now you can build the port:

   su root
   cd openstopmotion
   make install
   
To enable the webcamd, please edit /etc/rc.conf and start it:

   echo "webcamd_enable=\"YES\"" >> /etc/rc.conf
   /usr/local/etc/rc.d/webcamd start   
