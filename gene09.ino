
//Ad5932.h - Ad5932.cpp

#include "Ad5932.h"                     //library Ad5932
#include <SPI.h>                        //library SPI
uint8_t ad5932Fsync = 7;                //pin fsync
uint8_t ad5932Ctrl = 8;                 //pin ctrl
uint8_t ad5932Interrupt = 6;            //pin interrupt
uint8_t ad5932Standby = 5;              //pin standby
uint32_t startFreq = 2000000;           //start frequency
uint32_t incrementFreq = 1000;           //increment frequency
uint16_t numberIncrement = 4000;        //number of increments, max 4095  
float intervalDuration = 2000.0;        //duration of intervals
/* For a 50 MHz oscillator the minimum duration is t = 2 * 1 / Fosc
t = 2 * 1 / 50E6 = 0.042E-6 s and the maximum duration is
t = 2047 * 500 * 1 / 50E6 = 21323E-6
If the value is outside the range the library corrects these times */
Ad5932 gene;                            //initialize
/* ************************************************************************** */
void setup(){
/* ************************************************************************** */
  gene.begin(ad5932Fsync, ad5932Ctrl, ad5932Interrupt, ad5932Standby, \
             50000000.0);
  gene.setManuAuto(LOW);                //auto increment mode
//  gene.setManuAuto(HIGH);               //mode scan manuel
  gene.setControlRegister();            //write control register
  gene.setStartFreq(startFreq);         //set start frequency
  gene.setIncremFreq(incrementFreq);    //set delta frequency
  gene.setNbIncrement(numberIncrement); //set number of increments
  gene.setIntervalDuration(intervalDuration); //set interval duration
}
/* ************************************************************************** */
void loop(){
             
                                       
    gene.start();                       //setting the CTRL pin HIGH
    delay(200);

}
/* ************************************************************************** */
