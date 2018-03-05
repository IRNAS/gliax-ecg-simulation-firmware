#include "mbed.h"
#include "ecg_simulator.h"

//#define DEBUG

DigitalOut led1(LED1);
InterruptIn button1(BUTTON1);

Serial pc(USBTX, USBRX);

enum {
    IDLE        = 0,
    SEND_ONCE   = 1,
    SEND_ALWAYS = 2,
};

uint8_t state;

void button1_detect(void)
{
    led1 = 1;
    state = SEND_ONCE;
}

int main() 
{
    //Initialize variables
    state = IDLE;
    button1.fall(button1_detect);
    
    led1 = 0;
    //ecg_sender_init();

    pc.baud(115200);
#ifdef DEBUG
    pc.printf("Start...\n");
#endif

    while(1) 
    {
        switch (state)
        {
            case IDLE:
                led1 = 0;
                break;

            case SEND_ONCE:
                ecg_sender_send(&pc);
                wait(0.5);

                state = IDLE;
                break;
            
            case SEND_ALWAYS:
                ecg_sender_send(&pc);
                wait(0.002048);

                break;
        }
    }
}
