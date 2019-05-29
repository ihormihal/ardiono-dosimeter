
//use "flags" for prevent duplication triggers
/*
СБМ-20
12 мкР/ч = 18 имп/мин ------------ 100 мкР/ч = 150 имп/мин
10 000 мкР/ч = 25 200 имп/мин----- 100 мкР/ч = 252 имп/мин (Ra224)
14 400 мкР/ч = 65 имп/мкР или 260 имп/с (Cs137)
Ra226 - 29 имп.с/ мр/ч ---- 1740 имп.мин / мр/ч
Co60 - 22 имп.с/ мр/ч ----  1320 имп.мин / мр/ч
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4
#define L_LED 13 //L-led pin
#define UPDATE_INTERVAL 5000 //interval for update display
#define BTN_INTERVAL_PIN 3 //digital pin 3 for button
#define ANALOG_SIGNAL_PIN 3 //signal input pin
#define SIGNAL_TRESHOLD 10
#define SBM20_NORM 0.7 // SBM-20 uH/h / quantums

//dosimetr variables
const unsigned int intervals[4] = { 10000, 30000, 60000, 12000 }; //60 sec is max
const unsigned int intervalsCount = 4;
int interval = 0; //interval index

unsigned long time = 0; //time from start in ms
unsigned int count = 0; // quantums per one updated interval
unsigned long inInterval = 0; //quantums in whole one calculated interval
unsigned long totalCount = 0; // total quantums
unsigned long counter[60]; //intervals container length = updated_intervals_count (see below)
float freq = 0; //quantums per minute in whole one calculated interval
float averageFreq = 0; //average quantums per minute for whole working time

boolean signalFlag = false; //flag for signal
boolean btnFlag = false; //flag for set interval button

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String utf8rus(String source) {
    int i, k;
    String target;
    unsigned char n;
    char m[2] = { '0', '\0' };

    k = source.length();
    i = 0;

    while (i < k) {
        n = source[i];
        i++;

        if (n >= 0xC0) {
            switch (n) {
            case 0xD0: {
                n = source[i];
                i++;
                if (n == 0x81) {
                    n = 0xA8;
                    break;
                }
                if (n >= 0x90 && n <= 0xBF)
                    n = n + 0x30;
                break;
            }
            case 0xD1: {
                n = source[i];
                i++;
                if (n == 0x91) {
                    n = 0xB8;
                    break;
                }
                if (n >= 0x80 && n <= 0x8F)
                    n = n + 0x70;
                break;
            }
            }
        }
        m[0] = n;
        target = target + String(m);
    }
    return target;
}



void displayData()
{
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(utf8rus(String(intervals[interval] / 1000) + "с"));

    display.setTextSize(2);
    display.setCursor(0, 12);

	String mainText = '0 мкР/ч';
	unsigned int dose = round(freq*SBM20_NORM);
	if(freq > 1000){
		mainText = String(dose/1000, 2) + " мР/ч";
	}else{
		mainText = String(dose) + " мкР/ч";
	}
    display.println(utf8rus(mainText));

    display.setTextSize(1);
    display.setCursor(40, 0);
    display.println(utf8rus(String(inInterval) + " имп"));

    display.display();
}

void setup()
{
    Serial.begin(9600);
    display.cp437(true);
    display.setTextColor(WHITE);
    pinMode(L_LED, OUTPUT);
    pinMode(BTN_INTERVAL_PIN, INPUT_PULLUP);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }
}

void changeInterval()
{

    if (interval == intervalsCount - 1) {
        interval = 0;
    } else {
        interval = interval + 1;
    }

    displayData();
}

int t = 0; //updatedInterval index in whole calculated interval
void tick()
{
	int updated_intervals_count = intervals[interval]/UPDATE_INTERVAL; //updated intervals in calculated interval
    counter[t] = count;
    totalCount += count;
	inInterval = 0;
    count = 0; //reset for new interval
    t++;
    if (t == updated_intervals_count)
        t = 0;
    
    for (int i = 0; i < updated_intervals_count; i++) {
        inInterval += counter[i];
    }
    freq = inInterval * 60000 / intervals[interval];
    averageFreq = totalCount * 60000 / time;
    displayData();
}

int tickTimeFlag = 0;
void loop()
{
    time = millis();

    boolean btnValue = !digitalRead(BTN_INTERVAL_PIN);
    if (btnValue && btnValue != btnFlag)
        changeInterval();
    btnFlag = btnValue;

    int analogValue = analogRead(ANALOG_SIGNAL_PIN);
    boolean signalValue = analogValue > SIGNAL_TRESHOLD;
	
    if (signalValue) {
        digitalWrite(L_LED, HIGH);
		if(signalValue != signalFlag)
			count++;
    } else {
        digitalWrite(L_LED, LOW);
    }
    signalFlag = signalValue;

	//for each update interval call tick() function
    if (time >= UPDATE_INTERVAL && time % UPDATE_INTERVAL == 0 && tickTimeFlag != time) {
        tick();
        tickTimeFlag = time;
    }
}