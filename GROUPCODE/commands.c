#include "lpc_types.h"
#include "serial.h"
#include "commands.h"

#include <string.h>

char cmdBuffer[100];
uint32_t cmdPos;

void readCmdBuff(COMMAND* cmd){
    // Check for serial command from PC
    cmdPos += read_usb_serial_none_blocking(&cmdBuffer[cmdPos], 10);
    // write_usb_serial_blocking(cmdBuffer,100);
    // Parse buffer for commands
    int i;
    int cmdStart = -1;
    int cmdStop = -1;
    for(i=0; i < 100; i++){
        if(cmdBuffer[i] == '/'){
            cmdStart = i;
        } else if(cmdBuffer[i] == ';' && cmdStart > -1){
            cmdStop = i;
            break;
        }
    }
    // If we found a command then extract it and parse it
    if (cmdStart > -1 && cmdStop > -1 && cmdStart < cmdStop){
        char cmdStr[100];
        int ln = cmdStop - cmdStart - 1;
        for(i=0; i<ln; i++){
            cmdStr[i] = cmdBuffer[cmdStart + i + 1];
        }
        cmdStr[i] = 0x00; // extra terminator since we delimit with null bytes
        write_usb_serial_blocking("CMD:",4);
        write_usb_serial_blocking(cmdStr,ln);
        write_usb_serial_blocking("\n\r",2);
        // Shift buffer down now command is read
        for(i=0; i <= cmdStop; i++){
            cmdBuffer[i] = cmdBuffer[cmdStop + i + 1];
        }

        // TODO: Parse the command into the struct
        parseCommand(cmdStr, cmd);

    } else {
        cmd->cmd = CMD_NONE;
    }
}

void parseCommand(char* str, COMMAND* cmd){
    // Command format: A \0 STRING [\0 STRING] \0 \0 (without spaces)
    cmd->cmd = str[0];
    int nextWord = strlen(str) + 1;
    strcpy(cmd->infile, &str[nextWord]);
    nextWord += strlen(&str[nextWord]) + 1;
    if(strlen(&str[nextWord]) > 0){
        strcpy(cmd->outfile, &str[nextWord]);
    }
}
