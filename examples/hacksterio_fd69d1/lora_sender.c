/*
Original code by: https://github.com/Snootlab/lora_chisterapi
Edited by: Ramin Sangesari
*/

/*-----------------------------------------*/
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
/*-----------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <RH_RF95.h>

RH_RF95 rf95;

/* The address of the node which is 10 by default */
uint8_t node_number = 10;
uint8_t msg[2] = {10, 0};
int run = 1;
/*-----------------------------------------*/
DIR *dir;
struct dirent *dirent;
char dev[16];      // Dev ID
char devPath[128]; // Path to device
char buf[256];     // Data from device
char tmpData[6];   // Temp C * 1000 reported by device 
char path[] = "/sys/bus/w1/devices"; 
ssize_t numRead;
/*-----------------------------------------*/


/* Send a message every 3 seconds */
void sigalarm_handler(int signal)
{
    msg[0] = node_number;
    msg[1]++;
 
    rf95.send(msg, sizeof(msg));
    rf95.waitPacketSent();
    printf("Send!\n");
    alarm(3);
}

/* Signal the end of the software */
void sigint_handler(int signal)
{
    run = 0;
}

void setup()
{ 

	dir = opendir (path);
	if (dir != NULL){
		while ((dirent = readdir (dir)))
		// 1-wire devices are links beginning with 28-
		if (dirent->d_type == DT_LNK && strstr(dirent->d_name, "28-") != NULL) { 
			strcpy(dev, dirent->d_name);
		//	printf("\nDevice: %s\n", dev);
		}
		(void) closedir (dir);
		}
		else{
			perror ("Couldn't open the w1 devices directory");
			//return 1;
		}
	// Assemble path to OneWire device
	sprintf(devPath, "%s/%s/w1_slave", path, dev);
	
	
    wiringPiSetupGpio();

    if (!rf95.init()) 
    {
        fprintf(stderr, "Init failed\n");
        exit(1);
    }

    /* Tx power is from +5 to +23 dBm */
    rf95.setTxPower(23);
    /* There are different configurations
     * you can find in lib/radiohead/RH_RF95.h 
     * at line 437 
     */
    rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
    rf95.setFrequency(868.0); /* Mhz */
}

void loop()
{
	int fd = open(devPath, O_RDONLY);
	if(fd == -1){
		perror ("Couldn't open the w1 device.");
		//return 1;   
	}
	while((numRead = read(fd, buf, 256)) > 0){
		strncpy(tmpData, strstr(buf, "t=") + 2, 5); 
		float tempC = strtof(tmpData, NULL);
		tempC = tempC / 1000;
	//printf("%.2f\n", tempC);
	
	/*-----------------------------------------*/
	int NegativeNumber;
	char SignNumber;

	int intpart = (int)tempC;
    double decpart = tempC - intpart;
    //printf("Num = %f, intpart = %d, decpart = %f\n", x, intpart, decpart);

    decpart = decpart * 100;
    int w = int (decpart);
    if (w<0){
        w = w * (-1);
    }


	if (intpart<0){
		NegativeNumber = intpart * (-1);
		SignNumber = 'N';
	}
	else {
        SignNumber = 'P';
        NegativeNumber = intpart;

	}
	if (intpart==0){SignNumber='Z';}

	//printf("Sign : %c, int Number = %d\n",SignNumber,NegativeNumber);
    //printf("Part1 = %d, Part2 = %d\n",intpart, w);

	/*-----------------------------------------*/

	//	printf("Device: %s  - ", dev); 			
	//	printf("%.3f F\n\n", (tempC / 1000) * 9 / 5 + 32);
	}
close(fd);


sleep(1000);
}

int main(int argc, char **argv)
{
    if( argc == 2 )
        node_number = atoi(argv[1]);
    
	
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, sigalarm_handler);

    alarm(3);

    setup();

    while( run )
    {
        loop();
        usleep(1);
    }

    return EXIT_SUCCESS;
}

