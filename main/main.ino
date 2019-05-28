
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define updateInterval 1000
#define L_LED 13
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}

//dosimetr variables
const unsigned int intervals[4] = {5000, 10000, 30000, 60000}; //60 sec is max
const unsigned int intervalsCount = 4;
int interval = 0;

unsigned long time = 0;
int count = 0;                // quantums per one updated interval
unsigned long totalCount = 0; // total quantums
unsigned long counter[60];
float freq = 0;
float averageFreq = 0;

boolean signalFlag = false; //flag for signal
boolean btnFlag = false;    //flag for set interval button


void displayData()
{
    display.clearDisplay();

    display.setTextSize(1);  // Normal 1:1 pixel scale
    display.setCursor(0, 0); // Start at top-left corner
    display.println(utf8rus("Счет "+String(intervals[interval]/1000)+"с"));

    display.setTextSize(2);   // Normal 1:1 pixel scale
    display.setCursor(0, 12); // Start at top-left corner
    display.println(utf8rus(String(round(freq))+" мкР/ч"));
    display.display();

    display.setTextSize(1);   // Normal 1:1 pixel scale
    display.setCursor(64, 0); // Start at top-left corner
    display.println(utf8rus(String(totalCount)+" имп"));
    display.display();
}

void setup()
{
    Serial.begin(9600);
    display.cp437(true);
    display.setTextColor(WHITE);
    pinMode(L_LED, OUTPUT);
    pinMode(3, INPUT_PULLUP);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }
}

void changeInterval()
{
   
    if (interval == intervalsCount - 1)
    {
        interval = 0;
    }
    else
    {
        interval = interval+1;
    }
    
    displayData();
}

int t = 0; //updatedInterval index in whole calculated interval
void tick()
{
    counter[t] = count;
    totalCount += count;
    count = 0; //reset for new interval
    t++;
    if (t == 5)
        t = 0;
    int inInterval = 0; //quantums in whole one calculated interval
    for (int i = 0; i < 5; i++)
    {
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

    boolean btnValue = !digitalRead(3);
    
    if (btnValue && btnValue != btnFlag)
    {
        changeInterval();
    }
    btnFlag = btnValue;
    
    int analogValue = analogRead(3);
    boolean signalValue = analogValue > 10;
    if (signalValue) {
      digitalWrite(L_LED, HIGH);
    } else {
      digitalWrite(L_LED, LOW);
    }
    if (signalValue && signalValue != signalFlag)
    {
        count++;
    }
    signalFlag = signalValue;

    if (time >= updateInterval && time % updateInterval == 0 && tickTimeFlag != time)
    {
        tick();
        tickTimeFlag = time;
    }
}