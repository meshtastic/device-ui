
#include "comms/LinuxSerialClient.h"
#include "linux/LinuxSerial.h"
#include "util/ILog.h"

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 115200
#endif

LinuxSerialClient::LinuxSerialClient(const char *tty) : UARTClient("lnxser"), tty(tty) {}

void LinuxSerialClient::init(void)
{
    ILOG_INFO("LinuxSerialClient::setPath %s with %d baud", tty, SERIAL_BAUD);
    LinuxSerial *serial = new LinuxSerial;
    serial->setPath(tty);
    serial->begin(SERIAL_BAUD);
    _serial = serial;

    time(&lastReceived);
    SerialClient::init();
}
