
#define updateInterval 1000

const unsigned int intervals[4] ={5000, 10000, 30000, 60000}; //60 sec is max
int interval = 0;

unsigned long time = 0;
int count = 0; // quantums per one updated interval
unsigned long totalCount = 0; // total quantums
unsigned long counter[60];
float freq = 0;
float averageFreq = 0;

boolean signalFlag = false; //flag for signal
boolean btnFlag = false; //flag for set interval button

void setup()
{
	Serial.begin(9600);
	pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
}

void setInterval()
{
    interval++;
    if(interval = sizeof(intervals) - 1){
        interval = 0;
    } else {
        interval++;
    }
}

int t = 0; //updatedInterval index in whole calculated interval
void tick()
{
    counter[t] = count;
    totalCount += count;
    count = 0; //reset for new interval
    t++;
    if(t == 5) t = 0;
    int inInterval = 0; //quantums in whole one calculated interval
    for(int i=0; i < 5; i++){
        inInterval += counter[i];
    }
    freq = inInterval*60000/intervals[interval];
    averageFreq = totalCount*60000/time;
}

int tickTimeFlag = 0;
void loop()
{
    time = millis();

    boolean btnValue = digitalRead(3);
	if(btnValue && btnValue != btnFlag){
        setInterval();
	}
	btnFlag = btnValue;

	boolean signalValue = digitalRead(2);
	if(signalValue && signalValue != signalFlag){
        count++;
	}
	signalFlag = signalValue;

	if(time >= updateInterval && time%updateInterval == 0 && tickTimeFlag != time){
        tick();
        tickTimeFlag = time;
	}
}