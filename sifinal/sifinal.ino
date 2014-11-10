#include <LiquidCrystal.h>
#include <Bounce2.h>

LiquidCrystal lcd(12,11,5,4,3,2);
// LCD pin set

const unsigned int RESULT_LED = 8;
const unsigned int SPEAKER = 9;
const unsigned int SET_BTN = 10;
const unsigned int ALARM_LED = 13;
const unsigned int SET_DIAL = A1;
// digital/analog pins

Bounce set_btn = Bounce();
// prevent btn debounce

static int timer = 0;
unsigned int dialValue = 0;

unsigned int demoTime = 35;
// demonstration time (approximately 35 second)

boolean isTimer = false;
// timer on/off

unsigned int setTemp;
// auto/manual operation mode


void setdefaultLCD(){
  lcd.begin(16,2);
  lcd.print("TEMP BODY");
  lcd.setCursor(15,0);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("TEMP MATT");
  lcd.setCursor(15,1);
  lcd.print("C");
}


void setup(){

  //  Serial.begin(9600);

  //BT setup
  //  bluetooth.begin(9600);
  //BT setup

    //Buttons setup
  pinMode(SET_BTN, INPUT);
  set_btn.attach(SET_BTN);
  set_btn.interval(20);
  //Buttons setup

  //LCD setup
  setdefaultLCD();
  //LCD setup


  //Speaker setup
  pinMode(SPEAKER, OUTPUT);
  //Speaker setup


  //LED setup
  pinMode(ALARM_LED, OUTPUT);
  pinMode(RESULT_LED, OUTPUT);
  //LED setup

}

int second_cnt;
int btn_cnt = 0;
boolean isError = false;
void loop(){
  if(isError){
    serialEventRun();
  }
  //btn
  if(set_btn.update()){
    if(set_btn.read() == HIGH){
      lcd.clear();
      demo_1();
      lcd.clear();
      if(demo_2()==0){
        lcd.clear();
        demo_3();
        lcd.clear();
      }
      lcd.clear();
      btn_cnt = 0;
      setdefaultLCD();
      btn_cnt ++;
    }
  }

  if(second_cnt == 50){
    float temperature = measureTemperature(analogRead(0));
    if(temperature > 100 || temperature < -1){
      isError = true;
    }
    resultled(RESULT_LED, map(analogRead(0),0,1023,0,255));
    lcd.setCursor(10,0);
    lcd.print(temperature);  
    lcd.setCursor(10,1);
    if(setTemp == 1000){
      lcd.print(temperature-3);
    }
    else{
      lcd.print(setTemp);
    }

    //timer
    timer++;
    if(timer == demoTime && isTimer){

      unsigned int j = (unsigned int) (36.5 - temperature)*10;
      float tmp_temp = temperature;
      unsigned int i;
      for(i = 0 ; i < j ; i ++){
        lcd.setCursor(10,1);
        lcd.print(tmp_temp);
        tmp_temp+=0.1;
        delay(100);
      }
      lcd.setCursor(10,0);
      lcd.print(temperature);
      lcd.setCursor(10,1);
      lcd.print(36.50);

      delay(2000);
      digitalWrite(SPEAKER,HIGH);
      delay(5000);
      digitalWrite(SPEAKER,LOW);
      for(i = j; i > 0 ; i--){
        lcd.setCursor(10,1);
        lcd.print(tmp_temp);
        tmp_temp-=0.1;
        delay(100);
      }

      lcd.clear();
      digitalWrite(RESULT_LED, LOW);

      while(true);

    }
    //timer
  }

  delay(20);
  second_cnt++;

  if(second_cnt == 51){
    second_cnt = 0;
  }
}


float measureTemperature(const int value1){
  double LnR1;
  double Temp1;
  LnR1 = log(10000.0*(1024.0/(double)value1-1)); 
  Temp1 = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * LnR1 * LnR1 ))* LnR1 );
  Temp1 -= 273.15;        
  return Temp1;
}




boolean demo = false;

void demo_1(){
  ledon(ALARM_LED);
  demo = false;
  lcd.setCursor(0,0);
  lcd.print("Initial Temp.");

  lcd.setCursor(12,1);
  lcd.print("AUTO");

  while(true){
    if(set_btn.update()){
      if(set_btn.read()==HIGH){
        demo = true; 
        ledon(ALARM_LED);
        setTemp = 1000;
        break;
      }
    }

    if(analogRead(SET_DIAL) == 50){
      break;
    }
  }
  lcd.setCursor(12,1);
  lcd.print("    ");

  while(!demo){
    lcd.setCursor(13,1);
    dialValue = map(analogRead(SET_DIAL), 0, 1023, 36, 61);
    lcd.print(dialValue);
    lcd.setCursor(15,1);
    lcd.print("C");

    if(set_btn.update()){
      if(set_btn.read()==HIGH){
        demo = true; 
        ledon(ALARM_LED);
        setTemp = dialValue;
      }
    }
  }
}

unsigned int demo_2(){
  ledon(ALARM_LED);
  demo = false;
  lcd.setCursor(0,0);
  lcd.print("wake up alarm");
  int i = 0;

  while(!demo){
    if(set_btn.update()){
      if(set_btn.read()==HIGH){
        demo = true; 
        ledon(ALARM_LED);
        return i;
      }
    }
    dialValue = analogRead(SET_DIAL);
    if(dialValue < 500){
      lcd.setCursor(0,1);
      lcd.print("######ON######");
      i = 0;
      isTimer = true;
    }
    else{
      lcd.setCursor(0,1);
      lcd.print("######OFF#####");
      i = 1;
      isTimer = false;
    }
  }



}

unsigned int Hour;
unsigned int Minute;
boolean apm; // false->am, true->pm

void demo_3(){
  ledon(ALARM_LED);
  demo = false;
  lcd.setCursor(0,0);
  lcd.print("wake up time");
  lcd.setCursor(14,1);
  lcd.print("AM");

  while(!demo){
    if(set_btn.update()){
      if(set_btn.read()==HIGH){
        demo = true; 
        Hour = dialValue;
        ledon(ALARM_LED);
      }
    }
    dialValue = map(analogRead(SET_DIAL), 0, 1023, 0, 13);
    if(dialValue < 10){
      lcd.setCursor(9,1);
      lcd.print(" ");
      lcd.setCursor(10,1);
      lcd.print(dialValue);
    }
    else{
      lcd.setCursor(9,1);
      lcd.print(dialValue);
    }
    lcd.setCursor(11,1);
    lcd.print(":");


  }
  demo = false;
  while(!demo){
    if(set_btn.update()){
      if(set_btn.read()==HIGH){
        demo = true; 
        Minute = dialValue;
        ledon(ALARM_LED);
      }
    }
    dialValue = map(analogRead(SET_DIAL), 0, 1023, 0, 60);
    if(dialValue < 10){
      lcd.setCursor(12,1);
      lcd.print(" ");
      lcd.setCursor(13,1);
      lcd.print(dialValue);
    }
    else{
      lcd.setCursor(12,1);
      lcd.print(dialValue);
    }
  }
  demo = false;
  while(!demo){
    if(set_btn.update()){
      if(set_btn.read()==HIGH){
        demo = true; 
        ledon(ALARM_LED);
        //        demoTime = Hour * 60* 60 * 1000 + Minute * 60 * 1000;
        //        In real!
      }
    }
    dialValue = analogRead(SET_DIAL);
    if(dialValue < 500){
      lcd.setCursor(14,1);
      lcd.print("AM");
      apm = false;
    }
    else{
      lcd.setCursor(14,1);
      lcd.print("PM");
      apm = true;
    }
  }
}


void ledon(const int val){
  digitalWrite(val,HIGH);
  delay(500);
  digitalWrite(val,LOW);
}


void resultled(const int val, const int brightness){
  analogWrite(val, brightness);
}
