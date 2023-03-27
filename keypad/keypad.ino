#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DEBOUNCE_TIME 75
#define ROW_NUM     4
#define COLUMN_NUM  4
#define INDEX       5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

byte currChar;

unsigned long lastClick[ROW_NUM][COLUMN_NUM] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};

unsigned char Key[ROW_NUM][COLUMN_NUM][INDEX] = {
  {{'1'}, {'2', 'a', 'b', 'c'}, {'3', 'd', 'e', 'f'}, {' '}},
  {{'4', 'g', 'h', 'i'}, {'5', 'j', 'k', 'l'}, {'6', 'm', 'n', 'o'}, {'<'}},
  {{'7', 'p', 'q', 'r', 's'}, {'8', 't', 'u', 'v'}, {'9', 'w', 'x', 'y', 'z'}, {'^'}},
  {{'*'}, {'0'}, {'#'}, {'>'}}
};


unsigned short currentState = 0;
unsigned short prevState = LOW;
unsigned short lastFlicker = LOW;
unsigned long lastDebounceTime = 0;
bool isCapslock = false;

String text;

byte rows[ROW_NUM] = {19, 18, 5, 17};
byte columns[COLUMN_NUM] = {16, 4, 12, 15};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.display();

  for(byte i=0;i<4;i++){
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], HIGH);
  }

  for(byte i=0;i<4;i++){
    pinMode(columns[i], INPUT_PULLUP);    
  }

}


unsigned char getKey(){
  unsigned char key = '_';  
  for(byte row = 0; row < ROW_NUM; row++){
    digitalWrite(rows[row], LOW);
    
    for(byte col = 0; col < COLUMN_NUM; col++){
      
      while(digitalRead(columns[col]) == LOW){
        currentState = digitalRead(columns[col]);
        
        if(currentState != lastFlicker){
          lastDebounceTime = millis();
          lastFlicker = currentState;  
        }
        
        if (millis()-lastDebounceTime > DEBOUNCE_TIME){
          if (currentState == LOW && prevState == HIGH){
            
            if((millis() - lastClick[row][col]) < 800 && row < (ROW_NUM - 1) && col < (COLUMN_NUM - 1)){
              bool isStateChanged = false;
              switch (currChar){
                resetKey:
                case 0:
                  currChar++;
                  if(Key[row][col][1] != 0){
                    isStateChanged = true;
                    key = Key[row][col][1];
                  }
                  break;
                case 1:
                  currChar++;
                  if(Key[row][col][2] != 0){
                    isStateChanged = true;
                    key = Key[row][col][2];
                  }
                  break;
                case 2:
                  currChar++;
                  if(Key[row][col][3] != 0){
                    isStateChanged = true;
                    key = Key[row][col][3];
                  }
                  break;
                case 3:
                  currChar++;
                  if(Key[row][col][4] != 0){
                    isStateChanged = true;
                    key = Key[row][col][4];
                  }
                  if(row != 2 && (col != 0 || col !=2)){
                    currChar = 0;
                    isStateChanged = true;
                    key = Key[row][col][0];
                  }
                  break;
                case 4:
                  currChar = 0;
                  key = Key[row][col][0];
                  isStateChanged = true;
                  break;                  
              }
              if(isStateChanged){                
                unsigned short len = text.length();
                text.remove(len-1, 1);
                isStateChanged = false;
              }
            } else {
              key = Key[row][col][0];
              currChar = 0;
            }

            lastClick[row][col] = millis();
            prevState = currentState;
          }
        }
      }
      prevState = HIGH;
    }
    digitalWrite(rows[row], HIGH);
  }
  return key;
}


void restart(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("RESTARTING PROGRAM...");
  display.display();
  delay(3000);
  display.clearDisplay();
  text = "";
}


void passwordChecker(){
  String secretKey = "TeTiTb2021";
  if(text == secretKey){
    display.clearDisplay();
    delay(1000);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,10);
    display.println("ACCESS GRANTED!");
    display.display();
    delay(3000);
    restart();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  char key;
  key = getKey();
  // Serial.println(key);
  if(key=='<'){
    unsigned short len = text.length();
    text.remove(len-1, 1);
    display.clearDisplay();
  }
  else if(key=='^'){
    isCapslock = !isCapslock;
  }
  else if(key=='>'){
    text += '\n';
    display.clearDisplay();
  }
  else if(key!='_'){
    if(isCapslock){
      key = toupper(key);
    }
    text += key;
    display.clearDisplay();
  }
  
  passwordChecker();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Masukkan Password: ");
  display.println(text);
  display.display();
}
