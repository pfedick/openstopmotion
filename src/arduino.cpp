/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick <patrick@pfp.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "arduino.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>  /* POSIX terminal control definitions */
#include <errno.h>


Arduino::Arduino()
{
	fd=0;
	listener=0;
	resettime=4000;
}

Arduino::~Arduino()
{
	if (fd) close(fd);
	fd=0;
	if (listener) close(listener);
	listener=0;
}



// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error
bool Arduino::open(const ppl7::String device, int baud)
{
    struct termios toptions;
    if (fd) close(fd);
    fd=0;


    fd = ::open((const char*)device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        perror("init_serialport: Unable to open port ");
        fd=0;
        return false;
    }

    // Reset des Arduino-Boards abwarten
    ppl7::MSleep(resettime);

    fcntl(fd, F_SETFL, 0);


    if (tcgetattr(fd, &toptions) < 0) {
        perror("init_serialport: Couldn't get term attributes");
        return false;
    }
//    speed_t brate = baud; // let you override switch below if needed
    cfsetispeed(&toptions, baud);
    cfsetospeed(&toptions, baud);



    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    //toptions.c_cc[VMIN]  = 0;
    //toptions.c_cc[VTIME] = 20;





    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
        perror("init_serialport: Couldn't set term attributes");
        return false;
    }
    return true;
}

void Arduino::writeBytes(const char *buffer, int size)
{
	if (!fd) return;
	int bytes=::write(fd,buffer,size);
	//printf ("%d of %d Bytes gesendet\n",bytes,size);
	if (bytes!=size) {
		printf ("ERROR\n");
	}
	return;

	int ret=0;
	while (1) {
		bytes=::read(fd,&ret,1);
		if (bytes==1) {
			if (ret!=1) printf ("ERROR: Wrong answer byte: %d\n",ret);
			return;
		} else if (bytes==0) {
			printf ("ERROR: Connection Closed\n");
			return;
		} else if (bytes==-1 && errno==EAGAIN) {
			printf ("Nix im Buffer, warte...\n");
		} else if (bytes==-1) {
			printf ("Irgendein Fehler\n");
			return;
		} else {
			printf ("Mehr Bytes als gewünscht: %d\n",bytes);
			return;
		}

	}
}

void Arduino::setCounter(int value)
{
	char buffer[3];
	buffer[0]=2;
	buffer[1]=value&255;
	buffer[2]=(value>>8)&255;
	writeBytes(buffer,3);
}
#include "arduino.h"

void Arduino::setLight(int num, int value)
{
	char buffer[3];
	buffer[0]=1;
	buffer[1]=num&255;
	buffer[2]=value&255;
	writeBytes(buffer,3);
}

void Arduino::allLightsOn()
{
	char buffer[1];
	buffer[0]=3;
	writeBytes(buffer,1);
}

void Arduino::allLightsOff()
{
	char buffer[1];
	buffer[0]=4;
	writeBytes(buffer,1);
}

void Arduino::startDemo()
{
	char buffer[1];
	buffer[0]=5;
	writeBytes(buffer,1);
}

