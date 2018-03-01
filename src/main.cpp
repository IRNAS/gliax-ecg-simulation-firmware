#include "mbed.h"
#include "ecg_simulator.h"

#define LED_GREEN   p21
#define LED_RED     p22
#define LED_BLUE    p23
#define BUTTON_PIN  p17

//#define UART_TX     p9
#define UART_TX     p3
//#define UART_RX     p11
#define UART_RX     p4

DigitalOut blue(LED_BLUE);
DigitalOut green(LED_GREEN);
DigitalOut red(LED_RED);
InterruptIn button(BUTTON_PIN);

Serial pc(UART_TX, UART_RX);

void detect(void)
{
    pc.printf("Button pressed\n");  
    green = !green;
}

int main() {
    green = 1;
    red = 1;
    button.fall(detect);

    ecg_sender_init();

    pc.baud(9600);
    pc.printf("Start...\n");

    ecg_sender_send(&pc); 
    while(1) {
        blue = 1;
        wait(0.2);
        blue = 0;
        wait(0.2);
    }
}
