#include "mbed.h"
#include "test.h"

DigitalOut myled(LED1);

int value = 1;

int main() {
    while(1) {
        myled = value;
        value = test_toggle_value(value);
        wait(0.2);
        myled = value;
        value = test_toggle_value(value);
        wait(0.2);
    }
}
