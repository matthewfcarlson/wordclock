#include <TinyWireM.h>

#define SHIFT_DATA_PIN   3              // ATtiny Pin 3
#define SHIFT_CLOCK_PIN  4
#define SHIFT_OUTPUT_PIN 1
#define DS3231_I2C_ADDRESS 0x68

#define LED1  0x0000001
#define LED2  0x0000002
#define LED3  0x0000004
#define LED4  0x0000008
#define LED5  0x0000010
#define LED6  0x0000020
#define LED7  0x0000040
#define LED8  0x0000080
#define LED9  0x0000100
#define LED10 0x0000200
#define LED11 0x0000400
#define LED12 0x0000800
#define LED13 0x0001000
#define LED14 0x0002000
#define LED15 0x0004000
#define LED16 0x0008000
#define LED17 0x0010000
#define LED18 0x0020000
#define LED19 0x0040000
#define LED20 0x0080000
#define LED21 0x0100000
#define LED22 0x0200000
#define LED23 0x0400000
#define LED24 0x0800000
#define LED25 0x1000000
#define LED26 0x2000000
#define LED27 0x4000000
#define LED28 0x8000000

#define DISP_TEN     LED1
#define DISP_HALF    LED2
#define DISP_QUARTER LED3
#define DISP_TWENTY  LED4
#define DISP_FIVE    LED5
#define DISP_MINUTES LED6
#define DISP_HAPPY   LED7
#define DISP_PAST    LED8
#define DISP_TO      LED9
#define DISP_ONE     LED10
#define DISP_BIRTH   LED11
#define DISP_THREE   LED12
#define DISP_ELEVEN  LED13
#define DISP_FOUR    LED14
#define DISP_DAY     LED15
#define DISP_TWO     LED16
#define DISP_NAME1   LED17
#define DISP_EIGHT   LED18
#define DISP_NINE    LED19
#define DISP_SEVEN   LED20
#define DISP_FIVEH   LED21
#define DISP_SIX     LED22
#define DISP_NAME2   LED23
#define DISP_TENH    LED24 //added an hour to distingquish ten minutes and ten hours
#define DISP_TWELVE  LED25
#define DISP_GOOD    LED26
#define DISP_NIGHT   LED27
#define DISP_OCLOCK  LED28


void setup()
{
    //starts the i2c library
    TinyWireM.begin();
    pinMode(SHIFT_DATA_PIN, OUTPUT);
    pinMode(SHIFT_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_OUTPUT_PIN, OUTPUT);
    
}

void loop()
{
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    // retrieve data from DS3231
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,&year);
    showTime(second, minute, hour, dayOfMonth, month);
    delay(15000); //wait 15000 seconds
}

/* ------------------------------------------------------------------------------------------------
 *  Sleep watchdog ISR
 * ------------------------------------------------------------------------------------------------
 */

ISR(WDT_vect) {
    // Toggle Port B pin 4 output state
    digitalWrite(4,!(digitalRead(4)));
}

/* ------------------------------------------------------------------------------------------------
 *  LED pushing functions
 * ------------------------------------------------------------------------------------------------
 */

//shows the time on LED- determining what
void showTime(byte second, byte minute, byte hour, byte dayOfMonth, byte month){
    setBrightness(minute,hour);
    
    //prep bit stream
    uint32_t configuration = generateDisplayConfiguration(minute, hour, dayOfMonth, month);
    //push out to shift registers
    
    shiftOutByte(configuration);
    shiftOutByte(configuration >> 8);
    shiftOutByte(configuration >> 16);
    shiftOutByte(configuration >> 24);
}

//sets the brightness of the LED's based on the
void setBrightness(byte minute, byte hour){
    
}

void shiftOutBit(bool data){
    digitalWrite(SHIFT_DATA_PIN,data);
    delay(5); //wait 5 ms as per the data sheet
    digitalWrite(SHIFT_CLOCK_PIN,HIGH);
    delay(5); //wait 5 ms as per the data sheet
    digitalWrite(SHIFT_CLOCK_PIN,LOW);
}

void shiftOutByte(byte data){
    for (byte mask = B10000000; mask>0; mask = mask >> 1) { //iterate through bit mask
        shiftOutBit (data & mask);
    }
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
            configure |= DISP_QUARTER | DISP_TO ;
            break;
        case 10: //10 = 50-54
            configure |= DISP_TEN | DISP_MINUTES | DISP_TO;
            break;
        case 11: //11 = 55-59
            configure |= DISP_FIVE | DISP_MINUTES | DISP_PAST;
            break;
        default:
            configure |= DISP_PAST | DISP_TO ;
    }
    //do the hours
    int hoursToDisplay = hour;
    if (configure && DISP_TO) hoursToDisplay ++; //if we are going to a number then we should add to the hour we are showing (3:55 = 5 minutes to 4)
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
    if (hour == 21) configure |= DISP_GOOD | DISP_NIGHT;
    
    //if it's my birthday (oct 01) wish me a happy birthday
    if (month == 10 && dayOfMonth == 1) configure |= DISP_HAPPY | DISP_BIRTH | DISP_DAY | DISP_NAME2;
    
    //if it's her birthday wish her a happy birthday
    if (month == 6 && dayOfMonth == 25) configure |= DISP_HAPPY | DISP_BIRTH | DISP_DAY | DISP_NAME1;
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


//Defunct function that uses Serial- which the ATTiny85 doesn't have
/*
 void displayTime()
 {
 byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
 // retrieve data from DS3231
 readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
 &year);
 // send it to the serial monitor
 Serial.print(hour, DEC);
 // convert the byte variable to a decimal number when displayed
 Serial.print(":");
 if (minute<10)
 {
 Serial.print("0");
 }
 Serial.print(minute, DEC);
 Serial.print(":");
 if (second<10)
 {
 Serial.print("0");
 }
 Serial.print(second, DEC);
 Serial.print(" ");
 Serial.print(dayOfMonth, DEC);
 Serial.print("/");
 Serial.print(month, DEC);
 Serial.print("/");
 Serial.print(year, DEC);
 Serial.print(" Day of week: ");
 switch(dayOfWeek){
 case 1:
 Serial.println("Sunday");
 break;
 case 2:
 Serial.println("Monday");
 break;
 case 3:
 Serial.println("Tuesday");
 break;
 case 4:
 Serial.println("Wednesday");
 break;
 case 5:
 Serial.println("Thursday");
 break;
 case 6:
 Serial.println("Friday");
 break;
 case 7:
 Serial.println("Saturday");
 break;
 }
 }
 */
