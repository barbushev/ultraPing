#define trigPin 2
#define echoPin 3    //- configured for interrupts on pin 3

const uint32_t outOfRangeMs = 38; // at 38ms the distance is 38000 / 58 = 655 cm
const uint32_t delayBetweenPingMs = 100;
const uint8_t avgTotalVals = 5;

uint32_t pingStartTime = 0;      //used to monitor for ping timeout
volatile uint32_t measTimeUs = 0;
volatile bool pingComplete = false;
volatile bool pingStarted = false;

//function prototypes
void triggerPing(void);
inline uint32_t calcDistanceMm(uint32_t microSec);
inline uint32_t calcDistanceIn(uint32_t microSec);


void setup()
{
	Serial.begin(115200);
	pinMode(trigPin, OUTPUT);
	setupInt();
}

/*
	Formula: uS / 58 = centimeters or uS / 148 = inch; or 
	the range = high level time * velocity(340M / S) / 2; we suggest to use over 60ms
	measurement cycle, in order to prevent trigger signal to the echo signal.
	If no obstacle is detected, the output pin will give a 38ms high level signal
*/

void loop()
{
	if ((!pingComplete) && (!pingStarted))
	{
		triggerPing();
		pingStarted = true;
		pingStartTime = millis();
	}

	if (pingComplete)
	{
		uint32_t distance = calcDistanceMm(measTimeUs);
		Serial.print(distance);
		Serial.println(" mm");

		delay(delayBetweenPingMs);
		pingComplete = false;
	}

	if ((pingStarted) && ((pingStartTime + outOfRangeMs) < millis()))
	{ 
		Serial.println("Out of range");
		pingStarted = false;
	}
}

//convert micro seconds to centimeteres
inline uint32_t calcDistanceMm(uint32_t microSec)
{
	return (microSec / 5.8);
}


//convert micro seconds to inches
inline uint32_t calcDistanceIn(uint32_t microSec)
{
	return (microSec / 148);
}


void triggerPing(void)
{
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
}


//PD3 - arduino Pin3
void setupInt()
{
	DDRD = (DDRD | ~_BV(PD3)); // Set pin direction to input (when set to 0 = input, 1 = output)
	EICRA = _BV(ISC10); // Any logical change on ISC1 generates an interrupt request
	EIMSK = _BV(INT1);  // Enable external interrupt request 1
}


// INT1 interrupt handling 
ISR(INT1_vect)
{
	static uint32_t startTimeUs = 0;
	if (PIND & (_BV(PD3))) //if Pin 3 is set, output is high. Start measuring
	{
		startTimeUs = micros();
	}
	else
	{
		measTimeUs = micros() - startTimeUs; //should do something about rollover
		pingComplete = true;
		pingStarted = false;
	}
}
