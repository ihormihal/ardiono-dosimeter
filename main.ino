const unsigned int updateInterval = 1000; //1 sec
const unsigned int interval = 5000; //calculated interval 5s

float freq = 0;
float allTimeFreq = 0;

unsigned long count = 0;
unsigned long prevCount = 0;
boolean state = false;

void setup()
{
	Serial.begin(9600);
	pinMode(2, INPUT_PULLUP);
}

void loop()
{
	boolean currentState = digitalRead(2);
	if(currentState && currentState != state){
        count++;
	}
	state = currentState;
	
	//reset on interval
	unsigned long time = millis();
	if(time%updateInterval == 0){
        allTimeFreq = count*60000/(time+1);
        if(time > interval){
            freq = (count - prevCount)*60000/interval;
        }
        prevCount = count;
	}
}