#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdint.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define UDS_REQUEST_ID   0x7E0
#define UDS_RESPONSE_ID  0x7E8
#define DID_ENGINE_SPEED 0x1111

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <interface>\n", argv[0]);
        exit(1);
    }
    char *interfaceName = argv[1];

    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket failed");
        exit(1);
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, interfaceName);
    ioctl(s, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    printf("Bound to %s successfully...\n", interfaceName);

    /* Build UDS request: SID 0x22 (Read Data By Identifier), DID = 0x1111 */
    struct can_frame reqFrame;
    memset(&reqFrame, 0, sizeof(reqFrame));
    reqFrame.can_id = UDS_REQUEST_ID;
    reqFrame.can_dlc = 8;
    reqFrame.data[0] = 0x03;              /* ISO-TP single frame, length = 3 */
    reqFrame.data[1] = 0x22;              /* SID: Read Data By Identifier   */
    reqFrame.data[2] = (DID_ENGINE_SPEED >> 8) & 0xFF; /* DID high byte      */
    reqFrame.data[3] = DID_ENGINE_SPEED & 0xFF;        /* DID low byte       */

    int nbytes = write(s, &reqFrame, sizeof(struct can_frame));
    if (nbytes < 0) {
        perror("write failed");
        exit(1);
    }
    printf("UDS Request sent: SID=0x22, DID=0x%04X\n", DID_ENGINE_SPEED);

    /* Wait for the response, ignoring any frame that isn't our response ID */
    struct can_frame respFrame;
    while (1) {
        int rbytes = read(s, &respFrame, sizeof(struct can_frame));
        if (rbytes < 0) {
            perror("read failed");
            exit(1);
        }
        if (respFrame.can_id != UDS_RESPONSE_ID) {
            continue; /* not our response, keep waiting */
        }
        break;
    }

    uint8_t sfLen = respFrame.data[0] & 0x0F;
    if (sfLen < 5) {
        printf("Unexpected response length: %d\n", sfLen);
        close(s);
        return 1;
    }

    uint8_t respSid = respFrame.data[1];
    uint16_t respDid = ((uint16_t)respFrame.data[2] << 8) | respFrame.data[3];
    uint16_t value = ((uint16_t)respFrame.data[4] << 8) | respFrame.data[5];

    if (respSid == 0x62 && respDid == DID_ENGINE_SPEED) {
        printf("UDS Response: Positive, DID=0x%04X, Value=%u\n", respDid, value);
    } else if (respSid == 0x7F) {
        printf("UDS Response: Negative, NRC=0x%02X\n", respFrame.data[3]);
    } else {
        printf("UDS Response: Unrecognized SID=0x%02X\n", respSid);
    }

    close(s);
    return 0;
}