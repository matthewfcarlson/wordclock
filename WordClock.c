#include <TinyWireM.h>   

#define DS3231_I2C_ADDRESS 0x68
#define LED1  0x000001
#define LED2  0x000002
#define LED3  0x000004
#define LED4  0x000008
#define LED5  0x000010
#define LED6  0x000020
#define LED7  0x000040
#define LED8  0x000080
#define LED9  0x000100
#define LED10 0x000200
#define LED11 0x000400
#define LED12 0x000800
#define LED13 0x001000
#define LED14 0x002000
#define LED15 0x004000
#define LED16 0x008000
#define LED17 0x010000
#define LED18 0x020000
#define LED19 0x040000
#define LED20 0x080000
#define LED21 0x100000
#define LED22 0x200000
#define LED23 0x400000
#define LED24 0x800000


#define DISP_TEN     LED1
#define DISP_HALF    LED2
#define DISP_QUARTER LED3
#define DISP_TWENTY  LED4
#define DISP_FIVE    LED5
#define DISP_MINUTES LED6
#define DISP_HAPPY   LED7
#define DISP_PAST    LED8
#define DISP_TO      LED9
#define DISP_THREE   LED10
#define DISP_ELEVEN  LED11
#define DISP_FOUR    LED12
#define DISP_DAY     LED13
#define DISP_TWO     LED14
#define DISP_EIGHT   LED15
#define DISP_BDAY    LED16
#define DISP_NINE    LED17
#define DISP_SEVEN   LED18
#define DISP_FIVE    LED19
#define DISP_SIX     LED20
#define DISP_NAME    LED21
#define DISP_TEN     LED22
#define DISP_TWELVE  LED23
#define DISP_OCLOCK  LED24

#define MASTER TRUE

//The idea was to have to the same code run on both chips but perhaps that isn't the best idea
enum States{
	STATE_INIT,
	MSTATE_GETTIME,
	MSTATE_RENDER,
	MSTATE_CHECKSERVANT,
	MSTATE_RESPOND,
	SSTATE_WAIT,
	SSTATE_DEBOUNCING
	STATE_SLEEP
}



volatile States currentState = STATE_INIT;


void setup()
{
  //starts the i2c library
  TinyWireM.begin();
  currentState = STATE_INIT;
  if (MASTER){ 
	//if we are the master and not the slave
	currentState = MSTATE_GETTIME;
	
	//setup Sleep state
	// temporarily prescale timer to 1s so we can measure current
	WDTCR |= (1<<WDP3); //| (1<<WDP2); // (1<<WDP2) | (1<<WDP0);

	// Enable watchdog timer interrupts
	WDTCR |= (1<<WDTIE);//(1<<WDIE);

	sei(); // Enable global interrupts

	// Use the Power Down sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  }
  else{
	//set up for slave mode
  }
  
}

void loop()
{
	States nextState = currentState;
	
	switch(currentState){
		case STATE_INIT:
			if (MASTER){
				nextState = MSTATE_GETTIME;
				//clear next state
			}
			else{
				nextState = SSTATE_WAIT;
			}
			break;
		case MSTATE_GETTIME:
			break;
		case STATE_INIT:
			break;
		default:
			nextState = STATE_INIT;
		
	}
	
	currentState = nextState;
}

/* ------------------------------------------------------------------------------------------------
 * 	Sleep watchdog ISR
 * ------------------------------------------------------------------------------------------------
 */
ISR(WDT_vect) {
	// Toggle Port B pin 4 output state
	digitalWrite(4,!(digitalRead(4)));
}

/* ------------------------------------------------------------------------------------------------
 * 	LED pushing functions
 * ------------------------------------------------------------------------------------------------
 */

//shows the time on LED- determining what 
void showTime(byte second, byte minute, byte hour, byte dayOfMonth, byte month, byte year){
	setBrightness(minute,hour);
	//prep bit stream
	
	//push out to shift registers
}

//sets the brightness of the LED's based on the 
void setBrightness(byte minute, byte hour){
	
}

/* ------------------------------------------------------------------------------------------------
 * 	Functions for the DS3231
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
