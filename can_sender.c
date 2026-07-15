#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdint.h>


int main( int argc, char *argv[]){

	if (argc != 4) {
    printf("Usage: %s <interface> <can_id_hex> <data_hex>\n", argv[0]);
    exit(1);
	}
	
	char *interfaceName = argv[1];
	long id = strtol(argv[2], NULL, 16);

	struct can_frame frame;
	frame.can_id = (canid_t)id;
	frame.can_dlc=strlen(argv[3]) / 2;
	if (frame.can_dlc > 8) frame.can_dlc = 8; /* CAN data max 8 bytes */

	char byteString[3];
	for (int j = 0; j < frame.can_dlc; j++){

		byteString[0] = argv[3][j * 2];
		byteString[1] = argv[3][j * 2 + 1];
		byteString[2] = '\0';
		frame.data[j] = (uint8_t)strtol(byteString, NULL, 16);
		/* or simply:
		sscanf(hexPair, "%2hhx", &frame.data[i]); */
	}

	int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0) { 
	perror("socket failed");
	exit(1); 
	}
	
	printf("CAN socket created successfully...\n");

	struct ifreq ifr;
	strcpy(ifr.ifr_name, interfaceName);
	ioctl(s, SIOCGIFINDEX, &ifr);

	printf("Interface index for %s: %d\n", interfaceName, ifr.ifr_ifindex);

	struct sockaddr_can addr;
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind failed");
		exit(1);
	}
	printf("Bound to %s successfully...\n", interfaceName);

	int nbytes = write(s, &frame, sizeof(struct can_frame));
	if (nbytes < 0) {
   	 perror("write failed");
  	  exit(1);
	}
	printf("Frame sent: ID=%X, DLC=%d\n", frame.can_id, frame.can_dlc);
    
	return 0 ;
}