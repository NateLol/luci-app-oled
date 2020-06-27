/*
 * Main.c
 *
 *  Created on  : Sep 6, 2017
 *  Author      : Vinay Divakar
 *  Description : Example usage of the SSD1306 Driver API's
 *  Website     : www.deeplyembedded.org
 */

/* Lib Includes */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Header Files */
#include "I2C.h"
#include "SSD1306_OLED.h"
#include "example_app.h"

/* Oh Compiler-Please leave me as is */
volatile unsigned char flag = 0;

/* Alarm Signal Handler */
void ALARMhandler(int sig)
{
    /* Set flag */
    flag = 5;
}

void BreakDeal(int sig)
{
    clearDisplay();
    usleep(1000000);    
    Display();
    exit(0);
}


int main(int argc, char* argv[])
{	
    int display_offset=7;
    int date=atoi(argv[1]);
    int lanip=atoi(argv[2]);
    int cputemp=atoi(argv[3]);
    int cpufreq=atoi(argv[4]);
    int netspeed=atoi(argv[5]);
    int time=atoi(argv[6]);
    int drawline=atoi(argv[7]);
    int drawrect=atoi(argv[8]);
    int fillrect=atoi(argv[9]);
    int drawcircle=atoi(argv[10]);
    int drawroundcircle=atoi(argv[11]);
    int fillroundcircle=atoi(argv[12]);
    int drawtriangle=atoi(argv[13]);
    int filltriangle=atoi(argv[14]);
    int displaybitmap=atoi(argv[15]);
    int displayinvertnormal=atoi(argv[16]);
    int drawbitmapeg=atoi(argv[17]);
    int scroll=atoi(argv[18]);
    char *text=argv[19];   
    char *eth = argv[20];
    


    /* Initialize I2C bus and connect to the I2C Device */
    if(init_i2c_dev(I2C_DEV0_PATH, SSD1306_OLED_ADDR) == 0)
    {
        printf("(Main)i2c-2: Bus Connected to SSD1306\r\n");
    }
    else
    {
        printf("(Main)i2c-2: OOPS! Something Went Wrong\r\n");
        exit(1);
    }

    /* Register the Alarm Handler */
    signal(SIGALRM, ALARMhandler);
    signal(SIGINT, BreakDeal);
    signal(SIGTERM, BreakDeal);    
/* Run SDD1306 Initialization Sequence */
    display_Init_seq();

    /* Clear display */
    clearDisplay();

    // draw a single pixel
//    drawPixel(0, 1, WHITE);
//    Display();
//    usleep(1000000);
//    clearDisplay();

    // draw many lines
    while(1){

	if(scroll){
	    testscrolltext(text);
	    usleep(1000000);
	    clearDisplay();
	}	

        if(drawline){
            testdrawline();
            usleep(1000000);
            clearDisplay();
        }


        // draw rectangles
        if(drawrect){
            testdrawrect();
            usleep(1000000);
            clearDisplay();
        }

        // draw multiple rectangles
        if(fillrect){
            testfillrect();
            usleep(1000000);
            clearDisplay();
        }

        // draw mulitple circles
        if(drawcircle){
            testdrawcircle();
            usleep(1000000);
            clearDisplay();
        }


        // draw a white circle, 10 pixel radius
        if(drawroundcircle){
            testdrawroundrect();
            usleep(1000000);
            clearDisplay();
        }


        // Fill the round rectangle
        if(fillroundcircle){
            testfillroundrect();
            usleep(1000000);
            clearDisplay();
        }

        // Draw triangles
        if(drawtriangle){
            testdrawtriangle();
            usleep(1000000);
            clearDisplay();
        }
        // Fill triangles
        if(filltriangle){
            testfilltriangle();
            usleep(1000000);
            clearDisplay();
        }

        // Display miniature bitmap
        if(displaybitmap){
            display_bitmap();
            Display();
            usleep(1000000);
        };
        // Display Inverted image and normalize it back
        if(displayinvertnormal){
            display_invert_normal();
            clearDisplay();
            usleep(1000000);
            Display();
		
        }

        // Generate Signal after 20 Seconds

        // draw a bitmap icon and 'animate' movement
        if(drawbitmapeg){
	    alarm(10);
	    flag=0;
            testdrawbitmap_eg();
            clearDisplay();
            usleep(1000000);
            Display();
        }

        setTextSize(1);
        setTextColor(WHITE); 
        //setCursor(0,0);   

        // info display
	   for(int i = 1; i < time; i++){

            if(date) {setCursor(display_offset, 8*(date-1)); testdate();}
            if(lanip) {setCursor(display_offset, 8*(date+lanip-1)); testlanip();}
            if(cpufreq) {setCursor(display_offset, 8*(date+lanip+cpufreq-1)); testcpufreq();}
            if(cputemp) {setCursor(display_offset, 8*(date+lanip+cpufreq+cputemp-1));testcputemp();}
            if(netspeed) {setCursor(display_offset, 8*(date+lanip+cpufreq+cputemp+netspeed-1));testnetspeed();}
        	Display();
        	usleep(1000000);
        	clearDisplay();
	   }
    }

}
