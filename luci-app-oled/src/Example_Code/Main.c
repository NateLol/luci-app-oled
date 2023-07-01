/*
 * Main.c
 *
 *  Created on  : Sep 6, 2017
 *  Author      : Vinay Divakar
 *  Description : Example usage of the SSD1306 Driver API's
 *  Website     : www.deeplyembedded.org
 */

/* Lib Includes */
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Header Files */
#include "I2C.h"
#include "SSD1306_OLED.h"
#include "example_app.h"

#define NETSPEED_INTERVAR 1000000

/* Oh Compiler-Please leave me as is */
volatile unsigned char flag = 0;

/** Shared variable by the threads */
static unsigned long int __shared_rx_speed = 0;
static unsigned long int __shared_tx_speed = 0;

/** Mutual exclusion of the shared variable */
static pthread_mutex_t __mutex_shared_variable =
    (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

/* thread id */
static pthread_t tid = 0;

static float get_uptime() {
	FILE *fp1;
	float uptime = 0, idletime = 0;
	if ((fp1 = fopen("/proc/uptime", "r")) != NULL) {
		if (fscanf(fp1, "%f %f", &uptime, &idletime))
			;
		fclose(fp1);
	}
	return uptime;
}

static void *pth_netspeed(char *ifname) {
	char rxbytes_path[80];
	char txbytes_path[80];
	unsigned long long int llu_bytes;
	unsigned long int rx_bytes = 0, tx_bytes = 0, last_rx_bytes = 0,
			  last_tx_bytes = 0;
	unsigned long int rx_speed, tx_speed;
	FILE *fp1;
	float last_uptime, uptime;

	sprintf(rxbytes_path, "/sys/class/net/%s/statistics/rx_bytes", ifname);
	sprintf(txbytes_path, "/sys/class/net/%s/statistics/tx_bytes", ifname);
	last_uptime = get_uptime();
	while (1) {
		uptime = get_uptime();
		if ((fp1 = fopen(rxbytes_path, "r")) != NULL) {
			if (fscanf(fp1, "%llu", &llu_bytes))
				;
			fclose(fp1);
			rx_bytes = llu_bytes % ULONG_MAX;
		} else {
			last_uptime = uptime;
			usleep(NETSPEED_INTERVAR);
			continue;
		}

		if ((fp1 = fopen(txbytes_path, "r")) != NULL) {
			if (fscanf(fp1, "%llu", &llu_bytes))
				;
			fclose(fp1);
			tx_bytes = llu_bytes % ULONG_MAX;
		} else {
			last_uptime = uptime;
			usleep(NETSPEED_INTERVAR);
			continue;
		}

		if ((last_rx_bytes == 0 && last_tx_bytes == 0) ||
		    (rx_bytes < last_rx_bytes) || (tx_bytes < last_tx_bytes) ||
		    (uptime <= last_uptime)) {
			last_rx_bytes = rx_bytes;
			last_tx_bytes = tx_bytes;
		} else {
			rx_speed =
			    (rx_bytes - last_rx_bytes) / (uptime - last_uptime);
			tx_speed =
			    (tx_bytes - last_tx_bytes) / (uptime - last_uptime);

			// write shared variables;
			pthread_mutex_lock(&__mutex_shared_variable);
			{
				__shared_rx_speed = rx_speed;
				__shared_tx_speed = tx_speed;
			}
			pthread_mutex_unlock(&__mutex_shared_variable);

			last_rx_bytes = rx_bytes;
			last_tx_bytes = tx_bytes;
		}
		last_uptime = uptime;
		usleep(NETSPEED_INTERVAR);
	}
}

/* Alarm Signal Handler */
void ALARMhandler(int sig) {
	/* Set flag */
	flag = 5;
}

void BreakDeal(int sig) {
	printf("Recived a KILL signal!\n");
	if (tid != 0) {
		pthread_cancel(tid);
		pthread_join(tid, NULL);
	}
	clearDisplay();
	usleep(1000000);
	Display();
	exit(0);
}

int main(int argc, char *argv[]) {
	int date = atoi(argv[1]);
	int lanip = atoi(argv[2]);
	int cputemp = atoi(argv[3]);
	int cpufreq = atoi(argv[4]);
	int netspeed = atoi(argv[5]);
	int time = atoi(argv[6]);
	int drawline = atoi(argv[7]);
	int drawrect = atoi(argv[8]);
	int fillrect = atoi(argv[9]);
	int drawcircle = atoi(argv[10]);
	int drawroundcircle = atoi(argv[11]);
	int fillroundcircle = atoi(argv[12]);
	int drawtriangle = atoi(argv[13]);
	int filltriangle = atoi(argv[14]);
	int displaybitmap = atoi(argv[15]);
	int displayinvertnormal = atoi(argv[16]);
	int drawbitmapeg = atoi(argv[17]);
	int scroll = atoi(argv[18]);
	char *text = argv[19];
	char *eth = argv[20];
	char *path = argv[21];
	int rotate = atoi(argv[22]);
	int needinit = atoi(argv[23]);
	unsigned long int rx_speed, tx_speed;

	if (path == NULL) path = I2C_DEV0_PATH;

	/* Initialize I2C bus and connect to the I2C Device */
	if (init_i2c_dev(path, SSD1306_OLED_ADDR) == 0) {
		printf("I2C: Bus Connected to SSD1306\r\n");
	} else {
		printf("I2C: OOPS! Something Went Wrong\r\n");
		exit(1);
	}

	if (netspeed == 1 && strcmp(eth, "") != 0) {
		pthread_create(&tid, NULL, (void *)pth_netspeed, eth);
	}

	/* Register the Alarm Handler */
	signal(SIGALRM, ALARMhandler);
	signal(SIGINT, BreakDeal);
	signal(SIGTERM, BreakDeal);

	/* Run SDD1306 Initialization Sequence */
	if (needinit == 1) display_Init_seq();

	if (rotate == 1)
		display_rotate();
	else
		display_normal();

	/* Clear display */
	clearDisplay();

	// draw a single pixel
	//    drawPixel(0, 1, WHITE);
	//    Display();
	//    usleep(1000000);
	//    clearDisplay();

	// draw many lines
	while (1) {
		if (scroll) {
			testscrolltext(text);
			usleep(1000000);
			clearDisplay();
		}

		if (drawline) {
			testdrawline();
			usleep(1000000);
			clearDisplay();
		}

		// draw rectangles
		if (drawrect) {
			testdrawrect();
			usleep(1000000);
			clearDisplay();
		}

		// draw multiple rectangles
		if (fillrect) {
			testfillrect();
			usleep(1000000);
			clearDisplay();
		}

		// draw mulitple circles
		if (drawcircle) {
			testdrawcircle();
			usleep(1000000);
			clearDisplay();
		}

		// draw a white circle, 10 pixel radius
		if (drawroundcircle) {
			testdrawroundrect();
			usleep(1000000);
			clearDisplay();
		}

		// Fill the round rectangle
		if (fillroundcircle) {
			testfillroundrect();
			usleep(1000000);
			clearDisplay();
		}

		// Draw triangles
		if (drawtriangle) {
			testdrawtriangle();
			usleep(1000000);
			clearDisplay();
		}
		// Fill triangles
		if (filltriangle) {
			testfilltriangle();
			usleep(1000000);
			clearDisplay();
		}

		// Display miniature bitmap
		if (displaybitmap) {
			display_bitmap();
			Display();
			usleep(1000000);
		};

		// Display Inverted image and normalize it back
		if (displayinvertnormal) {
			display_invert_normal();
			clearDisplay();
			usleep(1000000);
			Display();
		}

		// Generate Signal after 20 Seconds

		// draw a bitmap icon and 'animate' movement
		if (drawbitmapeg) {
			alarm(10);
			flag = 0;
			testdrawbitmap_eg();
			clearDisplay();
			usleep(1000000);
			Display();
		}

		// setCursor(0,0);
		setTextColor(WHITE);

		// info display
		int sum = date + lanip + cpufreq + cputemp + netspeed;
		if (sum == 0) {
			clearDisplay();
			return 0;
		}

		for (int i = 1; i < time; i++) {
			if (sum == 1) {	 // only one item for display
				if (date) testdate(CENTER, 8);
				if (lanip) testlanip(CENTER, 8);
				if (cpufreq) testcpufreq(CENTER, 8);
				if (cputemp) testcputemp(CENTER, 8);
				if (netspeed) {
					// read shared variables;
					pthread_mutex_lock(
					    &__mutex_shared_variable);
					{
						rx_speed = __shared_rx_speed;
						tx_speed = __shared_tx_speed;
					}
					pthread_mutex_unlock(
					    &__mutex_shared_variable);

					testnetspeed(SPLIT, 0, rx_speed,
						     tx_speed);
				}
				Display();
				usleep(1000000);
				clearDisplay();
			} else if (sum == 2) {	// two items for display
				if (date) {
					testdate(CENTER, 16 * (date - 1));
				}
				if (lanip) {
					testlanip(CENTER,
						  16 * (date + lanip - 1));
				}
				if (cpufreq) {
					testcpufreq(CENTER, 16 * (date + lanip +
								  cpufreq - 1));
				}
				if (cputemp) {
					testcputemp(CENTER, 16 * (date + lanip +
								  cpufreq +
								  cputemp - 1));
				}
				if (netspeed) {
					// read shared variables;
					pthread_mutex_lock(
					    &__mutex_shared_variable);
					{
						rx_speed = __shared_rx_speed;
						tx_speed = __shared_tx_speed;
					}
					pthread_mutex_unlock(
					    &__mutex_shared_variable);

					testnetspeed(
					    MERGE,
					    16 * (date + lanip + cpufreq +
						  cputemp + netspeed - 1),
					    rx_speed, tx_speed);
				}
				Display();
				usleep(1000000);
				clearDisplay();
			} else {  // more than two items for display
				if (date) {
					testdate(FULL, 8 * (date - 1));
				}
				if (lanip) {
					testlanip(FULL, 8 * (date + lanip - 1));
				}
				if (cpufreq && cputemp) {
					testcpu(8 * (date + lanip));
					if (netspeed) {
						// read shared variables;
						pthread_mutex_lock(
						    &__mutex_shared_variable);
						{
							rx_speed =
							    __shared_rx_speed;
							tx_speed =
							    __shared_tx_speed;
						}
						pthread_mutex_unlock(
						    &__mutex_shared_variable);

						testnetspeed(
						    FULL,
						    8 * (date + lanip + 1 +
							 netspeed - 1),
						    rx_speed, tx_speed);
					}
				} else {
					if (cpufreq) {
						testcpufreq(FULL,
							    8 * (date + lanip +
								 cpufreq - 1));
					}
					if (cputemp) {
						testcputemp(FULL,
							    8 * (date + lanip +
								 cpufreq +
								 cputemp - 1));
					}
					if (netspeed) {
						// read shared variables;
						pthread_mutex_lock(
						    &__mutex_shared_variable);
						{
							rx_speed =
							    __shared_rx_speed;
							tx_speed =
							    __shared_tx_speed;
						}
						pthread_mutex_unlock(
						    &__mutex_shared_variable);

						testnetspeed(
						    FULL,
						    8 * (date + lanip +
							 cpufreq + cputemp +
							 netspeed - 1),
						    rx_speed, tx_speed);
					}
				}
				Display();
				usleep(1000000);
				clearDisplay();
			}
		}
	}

	if (netspeed == 1 && strcmp(eth, "") != 0) {
		pthread_cancel(tid);
		pthread_join(tid, NULL);
	}
}
