#include <SoftwareSerial.h>

#include "definitions.h"
#include "commbus.h"
#include "lcddisplay.h"

SoftwareSerial comm(PIN_COMM_RX, PIN_COMM_TX);

void comm_writecmd(char* line)
{
        comm.write(line);
}

String comm_readcmd()
{
        auto resp = comm.readStringUntil('\n');
        return resp;
}

void comm_init()
{
        Serial.println("Initializing communication bus...");
        lcd_statusmsg("Waiting for", "serial bus...");

        comm.begin(COMM_BAUD_RATE);
        comm.flush();

        comm_writecmd("/SYN\n");
        while(1)
        {
                auto resp = comm_readcmd();
                if(resp.equals("/ACK\n"))
                {
                        break;
                }
                else
                {
                        Serial.print("Received garbage: ");
                        Serial.println(resp);
                }
        }

        Serial.println("Comm link established.");
}
