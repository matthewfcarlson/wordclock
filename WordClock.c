#include <TinyWireM.h>   

#define SHIFT_DATA_PIN   3              // ATtiny Pin 3
#define SHIFT_CLOCK_PIN  4
#define SHIFT_OUTPUT_PIN 1
#define DS3231_I2C_ADDRESS 0x68

//The LEDS defined
#define LED1  0x00000001
#define LED2  0x00000002
#define LED3  0x00000004
#define LED4  0x00000008
#define LED5  0x00000010
#define LED6  0x00000020
#define LED7  0x00000040
#define LED8  0x00000080
#define LED9  0x00000100
#define LED10 0x00000200
#define LED11 0x00000400
#define LED12 0x00000800
#define LED13 0x00001000
#define LED14 0x00002000
#define LED15 0x00004000
#define LED16 0x00008000
#define LED17 0x00010000
#define LED18 0x00020000
#define LED19 0x00040000
#define LED20 0x00080000
#define LED21 0x00100000
#define LED22 0x00200000
#define LED23 0x00400000
#define LED24 0x00800000
#define LED25 0x01000000
#define LED26 0x02000000
#define LED27 0x04000000
#define LED28 0x08000000
#define LED29 0x10000000
#define LED30 0x20000000
#define LED31 0x40000000
#define LED32 0x80000000

#define DISP_NIGHT   LED2 
#define DISP_TWELVE  LED3
#define DISP_NINE    LED4
#define DISP_TWO     LED5
#define DISP_ELEVEN  LED6
#define DISP_SIX     LED7
#define DISP_NAME1   LED8

#define DISP_BIRTH   LED10 
#define DISP_DAY     LED10 // Correct
#define DISP_QUARTER LED11 // Correct
#define DISP_ONE     LED12 // Correct
#define DISP_TEN     LED13 // Correct
#define DISP_HAPPY   LED14 // Correct
#define DISP_FIVE    LED15 // Correct
#define DISP_FOUR    LED16 // Correct
//17 is blank?
#define DISP_TWENTY  LED18 // Correct
#define DISP_PAST    LED19 // Correct
#define DISP_MINUTES LED20 // Correct
#define DISP_HALF    LED21 // Correct
#define DISP_TO      LED22 // Correct
#define DISP_THREE   LED23 // Correct
//blank for 24 25
#define DISP_EIGHT   LED26 //Correct
#define DISP_FIVEH   LED27 //Correct
#define DISP_TENH    LED28 //added an hour to distingquish ten minutes and ten hours
#define DISP_SEVEN   LED29 //Correct
#define DISP_GOOD    LED30 //Correct
#define DISP_NAME2   LED31 //Correct
#define DISP_OCLOCK  LED32 //Correct





void setup()
{
  //starts the i2c library
  TinyWireM.begin();
  //set the pin modes
  pinMode(SHIFT_DATA_PIN, OUTPUT);
  pinMode(SHIFT_CLOCK_PIN, OUTPUT);
  pinMode(SHIFT_OUTPUT_PIN, OUTPUT);
  digitalWrite(SHIFT_OUTPUT_PIN,LOW);
  //setDS3231time(0,42,10,5,20,8,15); //seconds minutes hour dayofweek dayofmonth month year
}

//For testing purposes
uint32_t count = 0;
uint32_t displays[] = {DISP_TEN, DISP_HALF, DISP_QUARTER, DISP_TWENTY, DISP_FIVE, DISP_MINUTES, DISP_HAPPY, DISP_PAST, DISP_TO, DISP_ONE, DISP_BIRTH, DISP_THREE, DISP_ELEVEN, DISP_FOUR, DISP_DAY, DISP_TWO, DISP_NAME1, DISP_EIGHT, DISP_NINE, DISP_SEVEN, DISP_FIVEH, DISP_SIX, DISP_NAME2, DISP_TENH, DISP_TWELVE, DISP_GOOD, DISP_NIGHT, DISP_OCLOCK, DISP_OCLOCK, DISP_OCLOCK, DISP_OCLOCK, DISP_OCLOCK, DISP_OCLOCK};
//uint32_t displays[] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8, LED9, LED10, LED11, LED12, LED13, LED14, LED15, LED16, LED17, LED18, LED19, LED20, LED21, LED22, LED23, LED24};

//The main control loop
void loop()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;  
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,&year);
  showTime(second, minute, hour, dayOfMonth, month);
  //wait 15000 seconds
  delay(15000);

  //Testing the configuration routine
  //second, minute, hour, dayOfMonth, month
  /*showTime(0, 1, 9, 1, 1);
  delay(5000);
  showTime(0, 42, 9, 1, 1);
  delay(5000);
  showTime(0, 20, 15, 1, 1);
  delay(5000);
  showTime(0, 11, 21, 1, 10);
  delay(5000);
  showTime(0, 55, 11, 25, 6);
  delay(5000);
  showTime(0, 55, 9, 25, 6);
  delay(5000);
  
  for (int hours = 1; hours < 24; hours ++){
    showTime(5, 12, hours, 1, 1);
    delay(2000);
  }*/
  
  
  
}

/* ------------------------------------------------------------------------------------------------
 *  LED pushing functions
 * ------------------------------------------------------------------------------------------------
 */

//shows the time on LED- determining what 
void showTime(byte second, byte minute, byte hour, byte dayOfMonth, byte month){
  static uint32_t oldConfig;
  
  //prep bit stream
  uint32_t configuration = generateDisplayConfiguration(minute, hour, dayOfMonth, month);

  //push out to shift registers if anything has changed
  if (oldConfig != configuration){
    setBrightness(minute,hour, false); //slowly dim
    digitalWrite(SHIFT_OUTPUT_PIN,HIGH); //turn off the output enable signal
    delay(2); //make sure it has time to reach ground since it has a bit of inductance
    shiftOutUint32(configuration);
    setBrightness(minute,hour, true );
  }
  oldConfig = configuration;
}

//sets the brightness of the LED's based on the time and if we need to go up or down
void setBrightness(byte minute, byte hour, bool rampUp){
  static int oldBrightness = 255;
  int brightness = 200;
  if (hour > 21 && hour < 5) brightness = 230;
  if (hour > 22 && hour <3)  brightness = 240;
  if (rampUp == true){
    for (int i= 255; i> brightness; i--){
      analogWrite(SHIFT_OUTPUT_PIN,i);
      delay(7);
    }
    analogWrite(SHIFT_OUTPUT_PIN,brightness);
  }
  else{
    for (int i= oldBrightness; i<255; i++){
      analogWrite(SHIFT_OUTPUT_PIN,i);
      delay(3);
    }
  }
  
  oldBrightness = brightness;
}

void shiftOutByte(byte data, bool last){
  digitalWrite(SHIFT_CLOCK_PIN,LOW);
  shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, data);  
  if (last) {
    digitalWrite(SHIFT_DATA_PIN, data&1);
    digitalWrite(SHIFT_CLOCK_PIN,HIGH);
    delay(1);
    digitalWrite(SHIFT_CLOCK_PIN,LOW);
  }
  
}
void shiftOutByte(byte data){
  shiftOutByte(data & 0xff,true);
}

void shiftOutUint32(uint32_t data){
  shiftOutByte(data >> 24 & 0xff,false);
  shiftOutByte(data >> 16 & 0xff,false);
  shiftOutByte(data >> 8 & 0xff, false);
  shiftOutByte(data & 0xff,true);
  
}

//Generates the configuration to be pushed out on the leds
uint32_t generateDisplayConfiguration(byte minute, byte hour, byte dayOfMonth, byte month){
  uint32_t configure = 0;
  
  //figure out the minutes
  int minuteIncrements = minute / 5; // 0 = 1-4, 1 = 5-9, 2 = 10-14, 3 = 15-19, 4 = 20 - 24, 5 = 25-29, 6 = 30-34, 7 = 35-39, 8 = 40-44, 9 = 45-49, 10 = 50-54, 11 = 55-59
  switch(minuteIncrements){
    case 0: // 0 = 1-4
      configure |= DISP_OCLOCK;
      break;
    case 1: // 1 = 5-9
      configure |= DISP_FIVE | DISP_PAST | DISP_MINUTES;
      break;    
    case 2: // 2 = 10-14,
      configure |= DISP_TEN | DISP_PAST | DISP_MINUTES;
      break;
    case 3: // 3 = 15-19
      configure |= DISP_QUARTER | DISP_PAST ;
      break;
    case 4: //4 = 20 - 24
      configure |= DISP_TWENTY | DISP_PAST | DISP_MINUTES;
      break;
    case 5: //5 = 25-29
      configure |= DISP_TWENTY | DISP_FIVE | DISP_PAST | DISP_MINUTES;
      break;
    case 6: //6 = 30-34
      configure |= DISP_HALF | DISP_PAST;
      break;
    case 7: //7 = 35-39
      configure |= DISP_TWENTY | DISP_FIVE | DISP_TO | DISP_MINUTES;
      break;
    case 8: //8 = 40-44
      configure |= DISP_TWENTY | DISP_TO | DISP_MINUTES;
      break;
    case 9: //9 = 45-49
      configure |= DISP_QUARTER | DISP_TO;
      break;
    case 10: //10 = 50-54
      configure |= DISP_TEN | DISP_MINUTES | DISP_TO;
      break;
    case 11: //11 = 55-59
      configure |= DISP_FIVE | DISP_MINUTES | DISP_TO;
      break;
    default:
      configure |= DISP_PAST | DISP_TO ;
  }
  //do the hours
  int hoursToDisplay = hour;
  if (configure & DISP_TO) hoursToDisplay ++; //if we are going to a number then we should add to the hour we are showing (3:55 = 5 minutes to 4)
  if (hoursToDisplay > 12) hoursToDisplay -=12;

  switch(hoursToDisplay){
    case 1:
      configure |= DISP_ONE;
      break;
    case 2:
      configure |= DISP_TWO;
      break;
    case 3:
      configure |= DISP_THREE;
      break;
    case 4:
      configure |= DISP_FOUR;
      break;
    case 5:
      configure |= DISP_FIVEH;
      break;
    case 6:
      configure |= DISP_SIX;
      break;
    case 7:
      configure |= DISP_SEVEN;
      break;
    case 8:
      configure |= DISP_EIGHT;
      break;
    case 9:
      configure |= DISP_NINE;
      break;
    case 10:
      configure |= DISP_TENH;
      break;
    case 11:
      configure |= DISP_ELEVEN;
      break;
    case 12:
      configure |= DISP_TWELVE;
      break;
  }

  //if it's 9 pm wish Sara a good night
  if (hour == 21 && minute < 24) configure |= DISP_GOOD | DISP_NIGHT;

  //if it's my birthday (oct 01) wish me a happy birthday
  if (month == 10 && dayOfMonth == 1) configure |= DISP_HAPPY | DISP_BIRTH | DISP_DAY | DISP_NAME2;
  if (month == 10 && dayOfMonth == 1 && minute == 42 && hour < 12) configure |= DISP_HAPPY | DISP_BIRTH | DISP_DAY | DISP_NAME2;

  //if it's her birthday wish her a happy birthday 
  if (month == 6 && dayOfMonth == 25) configure |= DISP_HAPPY | DISP_BIRTH | DISP_DAY | DISP_NAME1;
  if (month == 6 && dayOfMonth == 25 && minute == 42 && hour < 12) configure = DISP_HAPPY | DISP_BIRTH | DISP_DAY | DISP_NAME1;

  return configure;
}

/* ------------------------------------------------------------------------------------------------
 *  Functions for the DS3231
 * ------------------------------------------------------------------------------------------------
 */
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  TinyWireM.beginTransmission(DS3231_I2C_ADDRESS);
  TinyWireM.write(0); // set next input to start at the seconds register
  TinyWireM.write(decToBcd(second)); // set seconds
  TinyWireM.write(decToBcd(minute)); // set minutes
  TinyWireM.write(decToBcd(hour)); // set hours
  TinyWireM.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  TinyWireM.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  TinyWireM.write(decToBcd(month)); // set month
  TinyWireM.write(decToBcd(year)); // set year (0 to 99)
  TinyWireM.endTransmission();
}
void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year)
{
  TinyWireM.beginTransmission(DS3231_I2C_ADDRESS);
  TinyWireM.write(0); // set DS3231 register pointer to 00h
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(TinyWireM.read() & 0x7f);
  *minute = bcdToDec(TinyWireM.read());
  *hour = bcdToDec(TinyWireM.read() & 0x3f);
  *dayOfWeek = bcdToDec(TinyWireM.read());
  *dayOfMonth = bcdToDec(TinyWireM.read());
  *month = bcdToDec(TinyWireM.read());
  *year = bcdToDec(TinyWireM.read());
}
