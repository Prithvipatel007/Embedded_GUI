// test.cpp
// Lothar Berger - May 2021
//
// COM Test for SPS

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <sys/poll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#define INMSG_MAX_LEN 15
#define OUTMSG_MAX_LEN 15

static char SPS_msg[] = "(------,------)X";

static int SPS_fd = -1;

static const double VCOM_READ_DELAY = 0.050;

// (------,------)X
static void VCOM_read(int fd, char *str)
{
	struct pollfd pollevents;
	pollevents.fd = fd;
	pollevents.events = POLLIN;

	int idx, num;
	char ch;

	for (idx = 0; idx < INMSG_MAX_LEN; idx++)
	{
		// read time-out 500 msec
		if ((poll(&pollevents, 1, 500)) == 0)
		{
			printf("read() time-out\n");
			break;
		}
		else
			num = read(fd, &ch, 1);

		if (num <= 0)
			break;

		str[idx] = ch;
	}

	// clear input buffer
	usleep(VCOM_READ_DELAY * 1000 * 1000);
	tcflush(fd, TCIFLUSH);

	printf("<-%s\n", str);
}

// (------,------)X
static void VCOM_write(int fd, const char *str)
{
	int idx, num;
	char ch;

	printf("->%s\n", str);

	for (idx = 0; idx < OUTMSG_MAX_LEN; idx++)
	{
		num = write(fd, &str[idx], 1);

		if (num <= 0)
			break;
	}

	// clear output buffer
	tcflush(fd, TCOFLUSH);
}

static void VCOM_close(int fd)
{
	tcflush(fd, TCIOFLUSH);

	close(fd);
}

static int VCOM_open(void)
{
	int fd;

	if ((fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY)) == -1)
		return -1;

	fcntl(fd, F_SETFL, O_NONBLOCK);

	struct termios tty;
	memset(&tty, 0, sizeof tty);
	struct termios tty_prev;
	memset(&tty_prev, 0, sizeof tty_prev);

	// error ?
	if (tcgetattr(fd, &tty) != 0)
		return -1;
	tty_prev = tty;

	cfsetospeed(&tty, (speed_t)B9600);
	cfsetispeed(&tty, (speed_t)B9600);

	tty.c_cflag &= ~PARENB; // 8N1
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	tty.c_cflag &= ~CRTSCTS; // no flow control

	tty.c_cflag |= CREAD | CLOCAL; // read on - ignore ctrl lines
	tty.c_cc[VMIN] = 1;			   // read no block
	tty.c_cc[VTIME] = 5;		   // read time-out 500 msec

	cfmakeraw(&tty);

	tcflush(fd, TCIOFLUSH);

	// error ?
	if (tcsetattr(fd, TCSANOW, &tty) != 0)
		return -1;

	return fd;
}

// (------,------)X
char SPS_str(int idx)
{
	return SPS_msg[idx];
}

void SPS_response(void)
{
	VCOM_read(SPS_fd, SPS_msg);
}

double SPS_request(const char *msg)
{
	VCOM_write(SPS_fd, msg);

	return VCOM_READ_DELAY;
}

void SPS_close(void)
{
	VCOM_close(SPS_fd);
}

int SPS_ready(void)
{
	return (SPS_fd != -1);
}

void SPS_open(void)
{
	SPS_fd = VCOM_open();
}

void IDLE_setCycle(void)
{
	double sps_read_delay = SPS_request("(IDLE--,------)X");

	usleep((1.0 - sps_read_delay) * 1000 * 1000);

	SPS_response();

	sps_read_delay = SPS_request("(SET---,+00600)X");

	usleep((1.0 - sps_read_delay) * 1000 * 1000);

	SPS_response();
}

void BUSY_read(void)
{
	double sps_read_delay = SPS_request("(BUSY--,------)X");

	usleep((1.0 - sps_read_delay) * 1000 * 1000);

	SPS_response();

	int n;
	for (n = 0; n < 50; n++)
	{
		double sps_read_delay = SPS_request("(READ--,------)X");

		usleep((1.0 - sps_read_delay) * 1000 * 1000);

		SPS_response();

		string data = (string)SPS_msg;
		string dataX = data.substr(2, 6);
		string dataY = data.substr(9, 13);
		int data_x = stoi(dataX);
		int data_y = stoi(dataY);
		printf("\n");
		cout << "integer is: " << data_x;
		cout << "integer is: " << data_y;
		printf("\n");
	}
}

int main(void)
{
	printf("COM Test for SPS ...\n");

	SPS_open();

	if (!SPS_ready())
	{
		printf("Error: SPS not ready\n");
		return EXIT_FAILURE;
	}

	usleep(1.0 * 1000 * 1000);

	/**
 * 
 * FOR BUSY CASE
 * 
 */

	IDLE_setCycle();
	BUSY_read();

	/**
	 * 
	int n;
	for (n=0; n<10; n++)
	{
		double sps_read_delay = SPS_request("(READ--,------)X");

		usleep((1.0-sps_read_delay)*1000*1000);

		SPS_response();
	}
    **/

	// Working for IDLE
	// double sps_read_delay = SPS_request("(IDLE--,------)X");

	// for (n=0; n<10; n++)
	// {
	// 	double sps_read_delay = SPS_request("(SET---,------)X");

	// 	usleep((1.0-sps_read_delay)*1000*1000);

	// 	SPS_response();
	// }

	SPS_close();

	printf("... done\n");

	return EXIT_SUCCESS;
}