#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_INC 13
#define BUTTON_DEC 14
#define DEBOUNCE_TIME 75

Adafruit_SSD1306 display(128, 32, &Wire, -1);
int counter = 0;

int prevState_inc;
int prevState_dec;
int currentState_inc;
int currentState_dec;

unsigned long lastDebounceTime = 0;
int lastFlickerableState_inc = LOW;
int lastFlickerableState_dec = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_INC, INPUT_PULLUP);
  pinMode(BUTTON_DEC, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void loop() {
  currentState_inc = digitalRead(BUTTON_INC);
  currentState_dec = digitalRead(BUTTON_DEC);

  if((currentState_inc != lastFlickerableState_inc) || (currentState_dec != lastFlickerableState_dec)){
    lastDebounceTime = millis();
    lastFlickerableState_inc = currentState_inc;
    lastFlickerableState_dec = currentState_dec;    
  }

  if((millis() - lastDebounceTime) > DEBOUNCE_TIME){
    
    if ((currentState_inc == LOW) && (prevState_inc == HIGH)) {
      counter++;
      Serial.println(counter);
    }


    if ((prevState_dec == HIGH) && (currentState_dec == LOW) && (counter != 0)) { //prevent negative value
      counter--;
      Serial.println(counter);
    }


    if((digitalRead(BUTTON_INC) == LOW) && (digitalRead(BUTTON_DEC) == LOW) ){
      counter=0;
      Serial.println(counter);
    }


    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(16, 7);
    display.print("count: ");
    display.println(counter);
    display.display();

    prevState_inc = currentState_inc;
    prevState_dec = currentState_dec;
  }
}