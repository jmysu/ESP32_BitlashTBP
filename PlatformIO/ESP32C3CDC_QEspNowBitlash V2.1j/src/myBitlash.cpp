#ifndef MYBITLASH_H
#define MYBITLASH_H
#include <Arduino.h>
#include <BitlashAPI.h>
#include "_QEspNowScript.h"
#include "myTinyBasicPlus.h"
//-----------------------------------------------------------------------------
//Bitlash user function
numvar func_echo(void) {
    for (int i=1; i <= getarg(0); i++) {
        if (isstringarg(i)) sp((const char *) getstringarg(i));
        else printInteger(getarg(i), 0, ' ');
        speol();
    }
    return 0;
}

numvar func_tbp(void) {
    iMyScriptType = SCRIPT_TINYBASICPLUS; //for loop-processing
    setupTinyBasicPlus();
    processTinyBasicPlus();  //process some jobs after TinyBasicPlus init...
    return 0;
}
#endif