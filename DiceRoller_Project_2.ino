#include <Servo.h>
/* The Pins below should be attach to L298N for the stepper to cut the bag */
/* Responsible Person: Sun Yi */
#define STP1 22
#define STP2 24
#define STP3 26
#define STP4 28
#define SV_BLADE 10// 90cut
/* The Pins below should be attach to TB6600 for the stepper to lift the bag */
/* Responsible Person: Cao Pengyi */
#define DIR 30
#define PUL 32
/* The Pins below should be attach to L298N for the electromagnet to hold the bag */
/* Responsible Person: Cao Pengyi */
#define EM1 34
#define EM2 36
/* The Pins below should be attach to Servo to open the bag */
/* Responsible Person: Ye Chenhao */
#define SVL 12//sevro left 110stright ~ 80
#define SVR 13//servo right 70stright ~ 100
#define SV1 2//servo 1 0stright~90
#define SV2 3//servo 2 90stright~0
#define SV3 4//servo 3 90stright~0
#define SV4 5//servo 4 0stright~90
/* The Pins below should be attach to LED */
#define LED_power 52//green
#define LED_begin 50//yellow
#define LED_auto 48//yellow
#define LED_unclear 46//red
#define LED_full 44//red
#define LED_time 42//red
#define LED_close 50//yellow

/* The Pins below should be attach to sensors */
#define US1_Echo A5//full
#define US2_Echo A4//unclear
#define US1_Trig 40
#define US2_Trig 38
#define TS1 A1//begin
#define TS2 A2//close
#define TS3 A3//turn off
#define TIME_SV 3000
#define TIME_WAIT 5000
#define T 100
#define T2 1800
#define PERIOD 86400000
#define FULL_DISTANCE 30//??
#define UNCLEAR_DISTANCE 20//??
#define LEFT_STR 110
#define RIGHT_STR 70
#define LEFT_CT 70
#define RIGHT_CT 110
Servo svl,svr;
Servo sv_blade;
Servo sv1, sv2, sv3, sv4;
bool auto_switch = false;
int auto_last = LOW;
unsigned long long int  last = 0;
void setup() 
{
  pinMode(STP1,OUTPUT);
  pinMode(STP2,OUTPUT);
  pinMode(STP3,OUTPUT);
  pinMode(STP4,OUTPUT);
  pinMode(DIR,OUTPUT);
  pinMode(PUL,OUTPUT);
  pinMode(EM1,OUTPUT);
  pinMode(EM2,OUTPUT);
  pinMode(LED_power,OUTPUT);
  pinMode(LED_begin,OUTPUT);
  pinMode(LED_unclear,OUTPUT);
  pinMode(LED_full,OUTPUT);
  pinMode(LED_time,OUTPUT);
  pinMode(LED_auto,OUTPUT);
  //pinMode(LED_close,OUTPUT);
  pinMode(US1_Trig,OUTPUT);
  pinMode(US2_Trig,OUTPUT);
  svl.attach(SVL);
  svr.attach(SVR);
  sv1.attach(SV1);
  sv2.attach(SV2);
  sv3.attach(SV3);
  sv4.attach(SV4);
  sv_blade.attach(SV_BLADE);
  pinMode(TS1,INPUT);//begin
  pinMode(TS2,INPUT);//turn off
  pinMode(TS3,INPUT);//close
  pinMode(US1_Echo,INPUT);
  pinMode(US2_Echo,INPUT);  
  digitalWrite(LED_power,HIGH);
  digitalWrite(LED_begin,LOW);
  digitalWrite(LED_unclear,LOW);
  digitalWrite(LED_full,LOW);
  digitalWrite(LED_time,LOW);
  digitalWrite(LED_auto,LOW);
  digitalWrite(LED_close,LOW);
  digitalWrite(US1_Trig,LOW);
  digitalWrite(US2_Trig,LOW);
  digitalWrite(DIR,LOW);
  digitalWrite(PUL,LOW);
  digitalWrite(EM1,LOW);
  digitalWrite(EM2,LOW);
  digitalWrite(US1_Trig,LOW);
  digitalWrite(US2_Trig,LOW);
  svl.write(LEFT_STR);
  svr.write(RIGHT_STR);
  unlock();
  uncut();
  sv_blade.write(0);
  delay(3000);
}

void loop() 
{
  if(digitalRead(TS1==HIGH))
  {
    OPEN();
  }
  if(digitalRead(TS2==HIGH))
  {
    CLOSE();
  }
  auto_read();
}
void auto_read()
{
  int temp = digitalRead(TS3);
  if(temp==HIGH)
  {
    if(auto_last==LOW)//new touch
    {
      auto_switch = !auto_switch;
      Serial.println(auto_switch);
    }
  }
  auto_last = temp;
  AUTO();
}
void OPEN()
{
  if(!UNCLEAR())
  {
    Serial.println("refuse");
    return;
  }
  digitalWrite(LED_begin,HIGH);
  unlock();
  /******* the skew rod goes up *******/
  clipon();
  up();
  delay(500);
  /******* the servos hold the bag *******/
  svl.write(LEFT_CT);
  svr.write(RIGHT_CT);
  delay(TIME_SV);

  /******* the skew rod goes down *******/
  clipoff();
  down();
  clipon();
  /******* the blade cut the bag *******/
  cut();
  cut_goto();
  uncut();
  cut_back();
  clipoff();
  /******* the servos open the bag *******/
  svl.write(LEFT_STR);
  svr.write(RIGHT_STR);
  delay(TIME_SV);

  /******* four servos lock the bag *******/
  lock();
  delay(TIME_SV);
  digitalWrite(LED_begin,LOW);
  last = millis();
  Serial.println("open");
}
void CLOSE()
{
  unlock();
  //four servo unlock
  svl.write(LEFT_CT);
  svr.write(RIGHT_CT);
  delay(TIME_WAIT);
  svl.write(LEFT_STR);
  svr.write(RIGHT_STR);
  Serial.println("close");
}
void lock()
{
  sv1.write(90);//
  sv2.write(0);//
  sv3.write(0);//
  sv4.write(90);//
}
void unlock()
{
  sv1.write(0);//
  sv2.write(90);//
  sv3.write(90);//
  sv4.write(0);//
}
void up()//
{
  int i;
  for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,LOW);
  digitalWrite(PUL,HIGH);
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  }
   for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,LOW);
  digitalWrite(PUL,HIGH);
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  } for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,LOW);
  digitalWrite(PUL,HIGH);
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  } for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,LOW);
  digitalWrite(PUL,HIGH);
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  } for(i=0;i<15000;i++)
  {
  digitalWrite(DIR,LOW);
  digitalWrite(PUL,HIGH);
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  }
}
void down()//
{
   int i;
  for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,HIGH);   
  digitalWrite(PUL,HIGH); /**/
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  Serial.println("down");
  }
  for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,HIGH);   
  digitalWrite(PUL,HIGH); /**/
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  Serial.println("down");
  }
  for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,HIGH);   
  digitalWrite(PUL,HIGH); /**/
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  Serial.println("down");
  }
  for(i=0;i<20000;i++)
  {
  digitalWrite(DIR,HIGH);   
  digitalWrite(PUL,HIGH); /**/
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  Serial.println("down");
  }
  for(i=0;i<15000;i++)
  {
  digitalWrite(DIR,HIGH);   
  digitalWrite(PUL,HIGH); /**/
  delayMicroseconds(T);
  digitalWrite(PUL,LOW);
  delayMicroseconds(T);
  Serial.println("down");
  }
}
void clipon()
{
  digitalWrite(EM1,HIGH);
  digitalWrite(EM2,LOW);
  }
  void clipoff()
{
  digitalWrite(EM1,LOW);
  digitalWrite(EM2,LOW);
  }
void cut()//
{
  sv_blade.write(90);
}
void uncut()//
{
  sv_blade.write(0);
}
void FULL()
{
  float out_in_cm;
  float input_time;
  digitalWrite(US1_Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_Trig,LOW);
  input_time=pulseIn(US1_Echo,HIGH);
  out_in_cm=(input_time/10000)*340/2;
  if(out_in_cm<FULL_DISTANCE)
  {
    digitalWrite(LED_full,HIGH);
  }
  else
  {
    digitalWrite(LED_full,LOW);
  }
}
bool UNCLEAR()
{
  float out_in_cm;
  float input_time;
  digitalWrite(US2_Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(US2_Trig,LOW);
  input_time=pulseIn(US2_Echo,HIGH);
  out_in_cm=(input_time/10000)*340/2;
  if(out_in_cm<UNCLEAR_DISTANCE)
  {
    digitalWrite(LED_unclear,HIGH);//the trash is not clear
    delay(3000);
    digitalWrite(LED_unclear,LOW);
    Serial.println("unclear");
    return false;
  }
  else
  {
    digitalWrite(LED_unclear,LOW);
    Serial.println("clear");
    return true;
  }
}
void AUTO()//time, full
{
  if(auto_switch)
  {
    digitalWrite(LED_auto,HIGH);
    Serial.println("switch on");
  }
  else
  {
    digitalWrite(LED_auto,LOW);
    Serial.println("switch off");
  }
unsigned long long int t = millis();
  if(auto_switch)
  {
    if(t-last>PERIOD)
    {
      digitalWrite(LED_time,HIGH);
    }
    else
    {
      digitalWrite(LED_time,LOW);
    }
    FULL();
  }
}
void cut_goto()
{
  int count=0;
  while(count>=0&&count<=1800){
  for(int i=22;i<30;i+=2)
   {
    digitalWrite(i,HIGH);
    delayMicroseconds(T2);
    digitalWrite(i,LOW); 
   }
   count++;
  }
  for(int i=22;i<30;i+=2){
    digitalWrite(i,LOW);
  }
}
void cut_back()
{
  int count=-1800;
  while(count<0){
    for(int i=28;i>20;i-=2)
   {
    digitalWrite(i,HIGH);
    delayMicroseconds(T2);
    digitalWrite(i,LOW); 
   }
   count++;
  }
  for(int i=28;i>20;i-=2)
  {
    digitalWrite(i,LOW);
  }
}
