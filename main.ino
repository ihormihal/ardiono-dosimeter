int count = 0;
int freq = 0;
const int interval = 5000; //5 sec
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
    if(millis()%interval == 0){
        freq = count;
        count = 0; 
    }
    Serial.println(freq);
}