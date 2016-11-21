/*************************************************************************
  GrippyBot Firmware 0.01
  
  Arduino Mega 2560 +
  GrippyBot(5 servo-motors) + 
  Bluetooth + 
  Laser + 
  Audio + 
  IR Range Finder (Sharp)
  
  Code par David Venancio de Campos / 2016
*************************************************************************/
#include <Servo.h> 
#include "pitches.h"
#include "config.h"
////////////////////////////////////////////////////////////////// Globals
Servo servo_pan,
      servo_arm1,
      servo_arm2,
      servo_arm3,
      servo_hand;
 
int pos = 0;
int dataByte=0;
const int melody[][2] = {  //melody (0 for blank note)
  {  NOTE_C4, NOTE_B4 },
  {  NOTE_B4, NOTE_C4 },
};
const int noteDurations[][2] = {  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  {  4, 8 },
  {  8, 4 },
};
float Distance,rez;
String commandBuffer;

///////////////////////////////////////////////////////////////////////////
void setup() 
{ 
  Serial.begin(115200);
  Serial.println("Grippy Bot Initialisation");
  pinMode(LASER_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  attach_servos();
  go_start();
  delay(1000);
}  
//////////////////////////////////////////////////////////////////// LOOP
void loop() 
{ 
  while(!Serial){
    //; //wait for connection
  }
  if (Serial.available()>0) {
    dataByte = Serial.read();
    //analyser le buffer entier
    if(dataByte==10){
      ///////////////////////////////// traitement des commandes
      if(commandBuffer=="go park"){
         go_park();
      }
      if(commandBuffer=="go start"){
         go_start();
      }
      if(commandBuffer=="wait"){
        wait();
      }
      if(commandBuffer=="play im"){
         play_imperial_march();
      }
      if(commandBuffer=="laser"){
         laser_demo();
      }
      if(commandBuffer=="get_distance"){
         rez = get_distance();
         Serial.println(rez);
      }
      
      if(commandBuffer.substring(0,3)=="pan"){
         String argument = commandBuffer.substring(4);
         move_pan(argument.toInt());
      }
      
      if(commandBuffer.substring(0,4)=="arm1"){
         String argument = commandBuffer.substring(5);
         move_arm1(argument.toInt());
      }
      
      if(commandBuffer.substring(0,4)=="arm2"){
         String argument = commandBuffer.substring(5);
         move_arm2(argument.toInt());
      }

      if(commandBuffer.substring(0,4)=="arm3"){
         String argument = commandBuffer.substring(5);
         move_arm3(argument.toInt());
      }
      
      if(commandBuffer.substring(0,7)=="fingers"){
         String argument = commandBuffer.substring(8);
         move_fingers(argument.toInt());
      }
      
      //clearer commandBuffer pour prochaine réception
      commandBuffer = String("");
    }else{
      commandBuffer = String( commandBuffer + char(dataByte) );
    }
    
  }
} 
//////////////////////////////////////////////////////////////////// Arm Moves
void attach_servos(void){
  servo_pan.attach(2);
  servo_arm1.attach(3);
  servo_arm2.attach(4);
  servo_arm3.attach(5);
  servo_hand.attach(6);
}
/////////////////////////////////////
//
void go_start(void){
  Serial.println("START");
  servo_arm1.write(ARM_1_ANGLE_START);
  delay(ARMS_DELAY);
  servo_arm2.write(ARM_2_ANGLE_START);
  delay(ARMS_DELAY);
  servo_arm3.write(ARM_3_ANGLE_START);
  delay(ARMS_DELAY);
  servo_pan.write(PAN_ANGLE_START);
  delay(PAN_DELAY);
}
/////////////////////////////////////
void go_park(void){
  Serial.println("PARK");
  servo_pan.write(PAN_ANGLE_PARK);
  delay(PAN_DELAY);
  servo_arm3.write(ARM_3_ANGLE_PARK);
  delay(ARMS_DELAY);
  servo_arm2.write(ARM_2_ANGLE_PARK);
  delay(ARMS_DELAY);
  servo_arm1.write(ARM_1_ANGLE_PARK);
  delay(ARMS_DELAY);
  servo_hand.write(HAND_ANGLE_PARK);
  delay(ARMS_DELAY);
  digitalWrite(LASER_PIN, LOW);  //turn off laser
}


///////////////////////////////////// Audio
//
void play_audio(int melody_number){ //from sample source code
  if(!AUDIO) return;
  melody_number -= 1;
  for (int thisNote = 0; thisNote < 2; thisNote++) {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[melody_number][thisNote];
    tone(AUDIO_PIN, melody[melody_number][thisNote], noteDuration);
    
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(AUDIO_PIN);
  } 
}

void play_test_melody(){
  if(!AUDIO) return;
  int num_notes = 8;
  int melody_custom[] = {  NOTE_B0,NOTE_C1, 0, NOTE_B3,NOTE_C4, 0, NOTE_B7, NOTE_C8 };
  int noteDurations_custom[] = {  4, 8, 4, 4, 8, 4, 4, 8 };
  for (int thisNote = 0; thisNote < num_notes; thisNote++) {
    int noteDuration = 1000 / noteDurations_custom[thisNote];
    tone(AUDIO_PIN, melody_custom[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(AUDIO_PIN);
  } 
}

void play_imperial_march(void){
     tone(AUDIO_PIN,LA3,Q); 
    delay(1+Q); //delay duration should always be 1 ms more than the note in order to separate them.
    tone(AUDIO_PIN,LA3,Q);
    delay(1+Q);
    tone(AUDIO_PIN,LA3,Q);
    delay(1+Q);
    tone(AUDIO_PIN,F3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    
    tone(AUDIO_PIN,LA3,Q);
    delay(1+Q);
    tone(AUDIO_PIN,F3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    tone(AUDIO_PIN,LA3,H);
    delay(1+H);
    
    tone(AUDIO_PIN,E4,Q); 
    delay(1+Q); 
    tone(AUDIO_PIN,E4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,E4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,F4,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    
    tone(AUDIO_PIN,Ab3,Q);
    delay(1+Q);
    tone(AUDIO_PIN,F3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    tone(AUDIO_PIN,LA3,H);
    delay(1+H);
    
    tone(AUDIO_PIN,LA4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,LA3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,LA3,S);
    delay(1+S);
    tone(AUDIO_PIN,LA4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,Ab4,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,G4,S);
    delay(1+S);
    
    tone(AUDIO_PIN,Gb4,S);
    delay(1+S);
    tone(AUDIO_PIN,E4,S);
    delay(1+S);
    tone(AUDIO_PIN,F4,E);
    delay(1+E);
    delay(1+E);//PAUSE
    tone(AUDIO_PIN,Bb3,E);
    delay(1+E);
    tone(AUDIO_PIN,Eb4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,D4,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,Db4,S);
    delay(1+S);
    
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    tone(AUDIO_PIN,B3,S);
    delay(1+S);
    tone(AUDIO_PIN,C4,E);
    delay(1+E);
    delay(1+E);//PAUSE QUASI FINE RIGA
    tone(AUDIO_PIN,F3,E);
    delay(1+E);
    tone(AUDIO_PIN,Ab3,Q);
    delay(1+Q);
    tone(AUDIO_PIN,F3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,LA3,S);
    delay(1+S);
    
    tone(AUDIO_PIN,C4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,LA3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    tone(AUDIO_PIN,E4,H);
    delay(1+H);
    
    tone(AUDIO_PIN,LA4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,LA3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,LA3,S);
    delay(1+S);
    tone(AUDIO_PIN,LA4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,Ab4,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,G4,S);
    delay(1+S);
    
    tone(AUDIO_PIN,Gb4,S);
    delay(1+S);
    tone(AUDIO_PIN,E4,S);
    delay(1+S);
    tone(AUDIO_PIN,F4,E);
    delay(1+E);
    delay(1+E);//PAUSE
    tone(AUDIO_PIN,Bb3,E);
    delay(1+E);
    tone(AUDIO_PIN,Eb4,Q);
    delay(1+Q);
    tone(AUDIO_PIN,D4,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,Db4,S);
    delay(1+S);
    
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    tone(AUDIO_PIN,B3,S);
    delay(1+S);
    tone(AUDIO_PIN,C4,E);
    delay(1+E);
    delay(1+E);//PAUSE QUASI FINE RIGA
    tone(AUDIO_PIN,F3,E);
    delay(1+E);
    tone(AUDIO_PIN,Ab3,Q);
    delay(1+Q);
    tone(AUDIO_PIN,F3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    
    tone(AUDIO_PIN,LA3,Q);
    delay(1+Q);
    tone(AUDIO_PIN,F3,E+S);
    delay(1+E+S);
    tone(AUDIO_PIN,C4,S);
    delay(1+S);
    tone(AUDIO_PIN,LA3,H);
    delay(1+H);
    
    delay(2*H); 
}


///////////////////////////////////// IR
float get_distance(void){
  float res=0,mean=0;
  float volts;
  for(int yeh=0; yeh<10; yeh++){  //blink laser
    volts = analogRead(IR_PIN);   // value from sensor * (5/1024) - if running 3.3.volts then change 5 to 3.3
    res = map(volts, 700, 0, 0, 80);  //670 = close from the hand. Low value is far up to 80cm with precision.
    mean += res;
    tone(AUDIO_PIN, map( res, 0, 100, 31, 4900), 40);
    blink_laser(1,60);
    //Serial.print("Sharp IR Distance = ");
    //Serial.println(res);
  }
  mean = mean/10;
  //Serial.print("Sharp IR MEAN Distance = ");
  //Serial.println(mean);
  return mean;
}

//
void blink_laser(int num, int delay_time){
  for(int yoh=0; yoh<num; yoh++){
    digitalWrite(LASER_PIN, LOW);
    delay(delay_time/2);
    digitalWrite(LASER_PIN, HIGH); 
    delay(delay_time/2);
  }
}

//wait
void wait(void){
  float dist = get_distance();
  move_fingers(HAND_ANGLE_MAX); //open fingers
  while(dist>20){
    dist = get_distance();
    delay(500);
    //Serial.println("no object");
  }
  Serial.println("object found!");
  move_fingers(HAND_ANGLE_MIN); //close fingers
}

void move_pan(int angle){
  servo_pan.write(angle);
  delay(PAN_DELAY);
}

void move_arm1(int angle){
  servo_arm1.write(angle);
  delay(ARMS_DELAY);
}

void move_arm2(int angle){
  servo_arm2.write(angle);
  delay(ARMS_DELAY);
}

void move_arm3(int angle){
  servo_arm3.write(angle);
  delay(ARMS_DELAY);
}

void move_fingers(int angle){
  servo_hand.write(angle);
  //tone(AUDIO_PIN, NOTE_DS8, 250);
  delay(200);
}

void laser_demo(){
  play_audio(1);
  delay(500);
  for(int k=0; k<5; k++){
    blink_laser(10,100);
  }
  delay(500);
}
