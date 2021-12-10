
#include <SPI.h>                        //library SPI
#include "Ad5932.h"                     //header Ad5932
//class call
Ad5932::Ad5932() : 
  _oscillator(0), _fsync(0), _ctrl(0), _interrupt(0), _standby(0),
  _controlRegister(), _frequency(5), _nbIncrement(2), _intervalDuration(0)
{
}
Ad5932::~Ad5932()
{
}
/* ************************************************************************** */
void Ad5932::begin(uint8_t fsync, uint8_t ctrl, uint8_t interrupt, \
                   uint8_t standby, float oscillator){
/* ************************************************************************** */
/* initialization with oscillator and standby */
  _fsync = fsync;                       
  _ctrl = ctrl;                         //pin start
  _interrupt = interrupt;               //pin reset
  _standby = standby;                   
  _oscillator = oscillator;             
  _controlRegister = 0x7FF;             //default 0b0000 0111 1111 1111
  pinMode(_fsync, OUTPUT);              
  pinMode(_ctrl, OUTPUT);               
  pinMode(_interrupt, OUTPUT);          
  pinMode(_standby, OUTPUT);           
  digitalWrite(_fsync, HIGH);           
  digitalWrite(_ctrl, LOW);             
  digitalWrite(_interrupt, LOW);        
  digitalWrite(_standby, LOW);          
  SPI.begin();                          //init SPI
  SPI.setBitOrder(MSBFIRST);            //MSB first
  SPI.setDataMode(SPI_MODE1);           //CPOL = 0, CPHA = 1
  SPI.setClockDivider(SPI_CLOCK_DIV128);//f/128
  //SPI_CLOCK_DIV4-SPI_CLOCK_DIV16-SPI_CLOCK_DIV32-SPI_CLOCK_DIV64
}
/* ************************************************************************** */
void Ad5932::setIntervalDuration(float intervalDuration){
/* ************************************************************************** */
/* Set duration of increment interval */
  uint16_t _interval = 0;
  _intervalDuration = intervalDuration * 1.0E-6;
                                        //in s
  if(_intervalDuration < 2.0 / _oscillator)//less than minimum duration
    _intervalDuration = 2.0 / _oscillator; //minimum duration
  else if(_intervalDuration > 500.0 * 2047 / _oscillator)
                                        //bigger than maximum duration
    _intervalDuration = 500.0 * 2047 / _oscillator;
                                        //maximum duration
  if(_intervalDuration <= 2047.0 / _oscillator){
    _interval = (uint16_t)(_intervalDuration  * _oscillator);
    _interval |= 0x0000;                //x1
  }else{
    _interval = (uint16_t)(_intervalDuration / 500.0  * _oscillator);
    _interval |= 0x2000;
    _interval |= 0x1800;                //x500 with D13 at 0
                                        
  }
  adWrite(AD5932_TIME_INC | _interval); //writing
  delay(1);                             
}
/* ************************************************************************** */
void Ad5932::setNbIncrement(uint16_t nbIncrement){
/* ************************************************************************** */
/* Set number of increments */
  _nbIncrement = nbIncrement;           //assign
  if(_nbIncrement < 2)
    _nbIncrement = 2;                   //minimum value = 2
  else if(_nbIncrement > 4095)
    _nbIncrement = 4095;                //maximum value = 4095
  adWrite(AD5932_N_INC | _nbIncrement); // writting
  delay(1);                            
}
/* ************************************************************************** */
void Ad5932::setIncremFreq(uint32_t frequency){
/* ************************************************************************** */
/* Set size of increment in frequency (delta f) */
  uint16_t _high = 0;                   
  uint16_t _low = 0;                    
  float _mclkFactor = 0.0;              //variable coefficient
  uint32_t _freqWord = 0;               //variable coefficient M
  _mclkFactor = 16777216.0 / _oscillator;
                                        //coefficient calcul freqWord
  _frequency = frequency;
  if(_frequency < 5)                    //frequency < 5 Hz
    _frequency = 5;                     //frequency minimum 5 Hz
  else if(_frequency > (uint32_t)(_oscillator / 2))
                                        //frequency > oscillator/2
    _frequency = (uint32_t)(_oscillator / 2);
                                        //fréquence maxi = oscillator /2
  _freqWord = (uint32_t)(_frequency * _mclkFactor);
                                        //coefficient M
  _high = (uint16_t)(_freqWord >> 12 & 0x0FFF);
                                        //12 MSB bits
  _low = (uint16_t)(_freqWord & 0x0FFF);//12 LSB bits
  adWrite(AD5932_DELTA_F_LOW | _low);   //writting 12 LSB bits
  delay(1);                             
  adWrite(AD5932_DELTA_F_HIGH | _high); //writting 12 MSB bits
  delay(1);                             
}
/* ************************************************************************** */
void Ad5932::setStartFreq(uint32_t frequency){
/* ************************************************************************** */
/* Set the start frequency */
  uint16_t _high = 0;                   
  uint16_t _low = 0;                    
  float _mclkFactor = 0.0;              //variable coefficient
  uint32_t _freqWord = 0;               //variable coefficient M
  _mclkFactor = 16777216.0 / _oscillator;
                                        //coefficient calcul freqWord
  _frequency = frequency;
  if(_frequency < 5)                    //frequency < 5 Hz
    _frequency = 5;                     //frequency minmum 5 Hz
  else if(_frequency > (uint32_t)(_oscillator / 2))
                                        //frequency > oscillator/2
    _frequency = (uint32_t)(_oscillator / 2);
                                        //frequency maximum = oscillator /2
  _freqWord = (uint32_t)(_frequency * _mclkFactor);
                                        //coefficient M
  _high = (uint16_t)(_freqWord >> 12 & 0x0FFF);
                                        //12 MSB bits 
  _low = (uint16_t)(_freqWord & 0x0FFF);//12 LSB bits
  adWrite(AD5932_START_F_LOW | _low);   //writting 12 LSB bits
  delay(1);                             
  adWrite(AD5932_START_F_HIGH | _high); //writing 12 MSB bits
  delay(1);                             
}
/* ************************************************************************** */
void Ad5932::setControlRegister(){
/* ************************************************************************** */
/* Update control register */
  reset();                              
  adWrite(AD5932_CONTROL | _controlRegister);
  delay(50);                            
}
/* ************************************************************************** */
void Ad5932::setManuAuto(uint8_t level){
/* ************************************************************************** */
/* Change to auto mode */
  uint8_t _level = 0;                  
  _level = level;                     //assign
  if(_level == HIGH){                  
    _controlRegister |= (1 << 5);       //set bit D5 to 1 (sets auto mode on)
  }else{                                
    _controlRegister &= ~(1 << 5);      //set bit D5 to 0
  }
}
/* ************************************************************************** */
void Ad5932::setFrequency(uint32_t frequency){
/* ************************************************************************** */
/* Set start frequency */
  uint16_t _high = 0;                   
  uint16_t _low = 0;                    
  float _mclkFactor = 0.0;              //variable coefficient
  uint32_t _freqWord = 0;               //variable coefficient M
  _mclkFactor = 16777216.0 / _oscillator;
                                        //coefficient calcul freqWord
  _frequency = frequency;
  if(_frequency < 5)                    //frequency < 5 Hz
    _frequency = 5;                     //frequency minimum 5 Hz
  else if(_frequency > (uint32_t)(_oscillator / 2))
                                        //frequency > oscillator/2
    _frequency = (uint32_t)(_oscillator / 2);
                                        //frequency maximum = oscillator /2
  _freqWord = (uint32_t)(_frequency * _mclkFactor);
                                        //coefficient M
  _high = (uint16_t)(_freqWord >> 12 & 0x0FFF);
                                        //12 MSB bits
  _low = (uint16_t)(_freqWord & 0x0FFF);//12 LSB bits

  reset();                              
  adWrite(AD5932_CONTROL | _controlRegister);
                                        //writing control register
  delay(50);                           
  adWrite(AD5932_START_F_LOW | _low);   //writing 12 LSB
  delay(1);                             
  adWrite(AD5932_START_F_HIGH | _high); //writing 12 MSB
  delay(1);                             
  start();                              //enable CTRL pin
}
/* ************************************************************************** */
void Ad5932::adWrite(uint16_t value){
/* ************************************************************************** */
/* shiftout SPI of 16 bits by 2 x 8 bits */
  uint16_t _value = 0;                 //variable
  _value = value;                     //assign
  setFsync(LOW);                        //set LOW to statrt writing
  delay(1);                             
  SPI.transfer((uint8_t)(_value >> 8 & 0x00FF)); 
                                        //transfer
  SPI.transfer((uint8_t)(_value & 0x00FF));
                                        
  delay(1);                             
  setFsync(HIGH);                       //set HIGH when we stop writing
}
/* ************************************************************************** */
void Ad5932::start(){
/* ************************************************************************** */
/* start - enabling CTRL pin from HIGH to LOW */
  digitalWrite(_ctrl, HIGH);            
  //delay(50);                            
  //digitalWrite(_ctrl, LOW);            
}
/* ************************************************************************** */
void Ad5932::reset(){
/* ************************************************************************** */
/* reset - enabling INTERRUPT pin HIGH to LOW */
  digitalWrite(_interrupt, HIGH);       
  delay(5);                             
  digitalWrite(_interrupt, LOW);        
}
/* ************************************************************************** */
void Ad5932::setFsync(uint8_t level){
/* ************************************************************************** */
/* set */
  if(level)                            //level = 1 where HIGH
    digitalWrite(_fsync, HIGH);         
  else                                  //level = 0 where LOW
    digitalWrite(_fsync, LOW);          //sortie LOW
}
/* ************************************************************************** */
void Ad5932::setStandby(uint8_t level){
/* ************************************************************************** */
/* Set standby */
  if(!_standby)                         //_standby = 0,  not declared
    return;                             
  if(level)                            //level = 1 where HIGH
    digitalWrite(_standby, HIGH);       
  else                                  //level = 0 where LOW
    digitalWrite(_standby, LOW);        
}


/* ************************************************************************** */
/*
Circuit AD5932

         |--|_|--|
COMP     |1    16| VOUT
AVDD     |2    15| AGND
DVDD     |3    14| STANDBY
CAP/2.5V |4    13| FSYNC
DGND     |5    12| SCLK
MCLK     |6    11| SDATA
SYNCOUT  |7    10| CTRL
MSBOUT   |8     9| INTERRUPT
         |-------|
Register Address
-----------------
D15 D14 D13 D12
  0 0 0 0 control bits
  0 0 0 1 number of increments
  0 0 1 0 delta frequency 12 least significant bits
  0 0 1 1 delta frequency 12 most significant bits
  0 1 x x increment time
  1 0 x x reserved
  1 1 0 0 start frequency 12 least significant bits
  1 1 0 1 start frequency 12 most significant bits
  1 1 1 0 reserved
  1 1 1 1 reserved
Default control register
-------------------------------
D15 = 0 |
D14 = 0 | adresses of registers
D13 = 0 |
D12 = 0 |
D11 = 0 B24 
D10 = 1 DAC_ENABLE (1 = active - 0 = inactive)
D09 = 1 SINE/TRI (1 = sine - 0 = triangle)
D08 = 1 MSBOUT
D07 = 1 reserved
D06 = 1 reserved
D05 = 1 INT/EXT_INCR (1 = incrementation via CTRL 0 = auto incremebt mode)
D04 = 1 reserved
D03 = 1 SYNCSL 
D02 = 1 SYNCOUT (1 = active - 0 = inactive)
D01 = 1 reserved
D00 = 1 reserved
_controlRegister = | 0x7FF
*/
