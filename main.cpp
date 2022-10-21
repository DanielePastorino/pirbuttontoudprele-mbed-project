#include "mbed.h"
#include "EthernetInterface.h"
#include "ThisThread.h"
#include "utility.h"

// Parameters
//#define _RELE     //decommentare per attivare rele
#define DHCP false
#define IP "192.168.45.30"
#define GATEWAY "192.168.45.1"
#define MASK "255.255.255.0"
#define TOIP "192.168.45.10"
#define UDPPORT 3039
#define UDPPORTRECEIVER 12003
#define MAINTHREADWAIT 250ms
#define MSG_UDP_PIR_ON "run" 
#define MSG_UDP_PIR_OFF "0" 

EthernetInterface eth;
UDPSocket sock;
SocketAddress from_addr;
SocketAddress auxip;
SocketAddress auxgwt;

#ifdef _RELE
UDPSocket sockReceiverRele;
Thread threadUDPreceive;
DigitalOut rele[] = {D8, D9};
const int numRele = sizeof(rele) / sizeof(DigitalOut);
char buffer[256] = {'\0'}; // così diventa una stringa xche finisce con \0
#endif

DigitalIn pir(D4); // PIR

#ifdef _RELE
/*
 * Il Tread di ricezione UDP del tipo 0010, 1011, ecc...
 */
void receiveUdpThread() {
  while (true) {
    int n = sockReceiverRele.recv(&buffer, sizeof(buffer));

    if (n > 0) {
      buffer[n] = '\0';
      debug(">Received: %s\n\r", buffer);

      for (int i = 0; i < strlen(buffer); i++) {
        int val = charToInt(buffer[i]);
        if (i < numRele) {
          debug("Set rele %d to %d\r\n", i, val);
          rele[i] = val;
        }
      }
    }
  }
}
#endif

/*******************
 *       MAIN      *
 *******************/

int main(void) {
    debug("#START\r\n");

    #ifdef _RELE 
    /* init rele */
    for(int i=0; i<numRele; i++)
    {
    rele[i] = 0;
    }
    #endif
    
    int stato = 0;
    int eth_status = 0;
    eth.disconnect();

    if (DHCP) {
        eth_status = eth.connect(); // connetto in DHCP
        eth.get_ip_address(&auxip);
        const char *ip = auxip.get_ip_address();
        eth.get_gateway(&auxgwt);
        const char *gtw = auxgwt.get_ip_address();
        debug("Ip (DHCP): %s - %s\r\n", ip, gtw);
    } else {
        eth.set_network(IP, MASK, GATEWAY);
        eth_status = eth.connect(); // connetto in Static IP
        eth.get_ip_address(&auxip);
        const char *ip1 = auxip.get_ip_address();
        eth.get_gateway(&auxgwt);
        const char *gtw1 = auxgwt.get_ip_address();
        debug("Ip (Static): %s - %s\r\n", ip1, gtw1);
    }

    // apro socket
    SocketAddress addr(TOIP, UDPPORT);
    debug("Socket Open!\r\n");
    sock.open(&eth);
    sock.connect(addr);
    debug("Socket connesso alla porta %d (trasmissione) send to: %s\r\n", UDPPORT, TOIP);

    #ifdef _RELE
    // socket ricezione per RELE
    sockReceiverRele.open(&eth);
    sockReceiverRele.bind(UDPPORTRECEIVER);
    debug("[RECV-RELE] Socket Open! Port: %d (ricezione)\r\n", UDPPORTRECEIVER);
    threadUDPreceive.start(receiveUdpThread);
    #endif

    allLedOff();
    debug("#READY\n");

    while (1) {
        eth_status = eth.connect(); // leggo lo stato della connessione eth

        if (pir == 1 && stato == 0) {
            allLedOn();
            stato = 1;
            if (eth_status == NSAPI_ERROR_IS_CONNECTED) {
                printf("%s\r\n", MSG_UDP_PIR_ON);
                sock.send(MSG_UDP_PIR_ON, sizeof(MSG_UDP_PIR_ON)-1); //cosi' elimino /0 in fondo alla stringa
            }
        } else if (pir == 0 && stato == 1) {
            allLedOff();
            stato = 0;
            /*
            if (eth_status == NSAPI_ERROR_IS_CONNECTED) {
                printf("%s\r\n", MSG_UDP_PIR_OFF);
                sock.send(MSG_UDP_PIR_OFF, sizeof(MSG_UDP_PIR_OFF)-1); //cosi' elimino /0 in fondo alla stringa
            }
            */
        }
        ThisThread::sleep_for(MAINTHREADWAIT);
    }
}

