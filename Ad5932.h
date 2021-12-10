/*
Library AD5932 
*/
#ifndef AD5932_H
#define AD5932_H
#include "Arduino.h"
//Register addresses AD5932
#define AD5932_CONTROL      0b0000000000000000      //CTRL Register
#define AD5932_N_INC        0b0001000000000000      //N of increments
#define AD5932_DELTA_F_LOW  0b0010000000000000      //12 bits delta frequency LSB
#define AD5932_DELTA_F_HIGH 0b0011000000000000      //12 bits delta frequency MSB
#define AD5932_TIME_INC     0b0100000000000000      //duration of increment
#define AD5932_START_F_LOW  0b1100000000000000      //12 bits start frequency LSB
#define AD5932_START_F_HIGH 0b1101000000000000      //12 bits start frequency MSB
#define SIN 0b00000001                        //sinus
#define TRI 0b00000000                        //triangle
//
class Ad5932{
public:
  Ad5932();                             //constructor
  ~Ad5932();                            //destructor
  void begin(uint8_t fsync, uint8_t ctrl, uint8_t interrupt);
                                        //creation and initialization
  void begin(uint8_t fsync, uint8_t ctrl, uint8_t interrupt, uint8_t standby, \
             float oscillator);
                                        //creation and initialization
  void setFrequency(uint32_t frequency);//fixed output frequency
  void setFsync(uint8_t level);        
  void start();                         
  void reset();                         
  void setStandby(uint8_t level);      //standby level 
  void setManuAuto(uint8_t level);     //set manual or auto increment ctrlD5(1) or (0)
  void setControlRegister();            //set CTRL register
  void setStartFreq(uint32_t frequency);//fixed start frequency
  void setIncremFreq(uint32_t frequency);
                                        //fixed delta frequency
  void setNbIncrement(uint16_t nbIncrement);
                                        //fixed number of increments min 2, max 4095
  void setIntervalDuration(float intervalDuration);
                                        //interval duration in micro_s
                                        //minimum value = 2 / oscillator
                                        //maximum value = 2047/oscillateur
private:
  void adWrite(uint16_t value);        //SPI 16 bits
  uint8_t _fsync;                       //pin fsync
  uint8_t _ctrl;                        //pin ctrl
  uint8_t _interrupt;                   //pin interrupt
  uint8_t _standby;                     //pin standby
  uint16_t _controlRegister;            //variable controlRegister
  uint32_t _frequency;                  //variable output frequency
  float _oscillator;                    //variable oscillator
  uint16_t _nbIncrement;                //variable number of increments
  float _intervalDuration;              //variable interval duration
};
#endif
