#include "mbed.h"
#include "ecg_simulator.h"

//#define DEBUG

DigitalOut led1(LED1);
DigitalOut led2(LED2);
InterruptIn button1(BUTTON1);
InterruptIn button2(BUTTON2);

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

void button2_detect(void)
{
    led2 = 1;
    state = SEND_ALWAYS;
}

int main() 
{
    //Initialize variables
    state = IDLE;
    button1.fall(button1_detect);
    button2.fall(button2_detect);
    
    led1 = 0;
    led2 = 0;
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
                led2 = 0;
                break;

            case SEND_ONCE:
                ecg_sender_send(&pc);
                wait(0.2);

                state = IDLE;
                break;
            
            case SEND_ALWAYS:
                ecg_sender_send(&pc);
                wait(0.002048);

                break;
        }
        //ecg_sender_send(&pc); 
        //wait(0.002048);
        //blue = !blue;
        //led1 = 1;
        //led2 = 1;
        //wait(0.5);
        //led1 = 0;
        //led2 = 0;
        //wait(0.5);
    }
}
