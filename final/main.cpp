#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Thread thread_RPC;
Thread thread_ping;
Ticker servo_ticker;
Timer t;

DigitalInOut ping(D13);
PwmOut pin5(D5), pin6(D6);
BufferedSerial uart(D1, D0);
BufferedSerial xbee(D10, D9);
BBCar car(pin5, pin6, servo_ticker);

int find_line, cnt_line, find_tag, cnt_tag;
float Ping = 100;

void function_RPC();
void function_ping();

int main() {
    xbee.set_baud(9600);
    uart.set_baud(9600);
    
    thread_RPC.start(function_RPC);
    thread_ping.start(function_ping);

    ThisThread::sleep_for(2s);
    /*************************************************************************/
    printf("start(1) follow the line \n");
    xbee.write("start(1) follow the line\n", 25);
    while (find_line) {
        if (cnt_line > 160) car.go(90 - (cnt_line-160) / 2, 90 - (cnt_line-160) / 4);
        else car.go(cnt_line / 4 + 50, cnt_line / 2 + 10);
    }
    car.stop();
    printf("end(1)\n");
    xbee.write("end(1)\n", 7);
    ThisThread::sleep_for(1s);

    /*************************************************************************/
    if (Ping < 30) {
        printf("start(2) circle around the object\n");
        xbee.write("start(2) circle around the object\n", 34);
        car.turn(-100, 0);
        for (int i = 0; i < 9; i++) {
            ThisThread::sleep_for(70ms);
        }
        car.stop();
        
        while (!find_tag) car.go(100, 30);
        car.stop();
        printf("end(2)\n");
        xbee.write("end(2)\n", 7);
        ThisThread::sleep_for(1s);
    }

    /*************************************************************************/
    if (find_tag) {
        printf("start(3) use a AprilTag and PING to calibrate the location\n");
        xbee.write("start(3) use a AprilTag and PING to calibrate the location\n", 59);
        while (Ping > 70) {
            if (cnt_tag > 0) car.go(50, 100);
            else if (cnt_tag < 0) car.go(100, 50);
            else car.go(100, 100);
        }
        car.stop();
        printf("end(3)\n");
        xbee.write("end(3)\n", 7);
        ThisThread::sleep_for(1s);
    }
    
    /*************************************************************************/
    printf("start(4) follow another line to a destination\n");
    xbee.write("start(4) follow another line to a destination\n", 46);
    while (find_line) {
        if (cnt_line > 160) car.go(90 - (cnt_line-160) / 2, 90 - (cnt_line-160) / 4);
        else car.go(cnt_line / 4 + 50, cnt_line / 2 + 10);
    }
    car.stop();
    printf("end(4)\n");
    xbee.write("end(4)\n", 7);
}

void function_RPC() {
    char buf[256], outbuf[256];
    FILE *devin = fdopen(&uart, "r");
    FILE *devout = fdopen(&uart, "w");
    while (1) {
        memset(buf, 0, 256);
        for (int i = 0; i < 256; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                //printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //printf("%s\r\n", buf);
        RPC::call(buf, outbuf);
    }
}

void function_ping() {
    float val;

    while(1) {
        ping.output();
        ping = 0; wait_us(200);
        ping = 1; wait_us(5);
        ping = 0; wait_us(5);

        ping.input();
        while(ping.read() == 0);
        t.start();
        while(ping.read() == 1);
        val = t.read();
        Ping = val*17700.4f;
        //printf("Ping = %lf\r\n", Ping);
        t.stop();
        t.reset();

        ThisThread::sleep_for(500ms);
    }
}

void OpenMV_scan (Arguments *in, Reply *out) {
    find_line = in->getArg<double>();
    cnt_line  = in->getArg<double>();
    find_tag  = in->getArg<double>();
    cnt_tag   = in->getArg<double>();

    //printf("%d, %d, %d, %d\n", find_line, cnt_line, find_tag, cnt_tag);
}
RPCFunction openMV_scan(&OpenMV_scan, "OpenMV_scan");