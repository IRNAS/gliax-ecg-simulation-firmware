#include "mbed.h"
#include "ecg_simulator.h"

//#define DEBUG

DigitalOut led(LED1);
Serial pc(USBTX, USBRX);

int main() 
{
    //Initialize ecg
    ecg_sender_init();
    //Initialize serial communication
    pc.baud(115200);
#ifdef DEBUG
    pc.printf("Start...\n");
#endif

    while(1) 
    {
        //Send ecg data periodically
        ecg_sender_send(&pc); 
        wait(0.002048);
    }
}
