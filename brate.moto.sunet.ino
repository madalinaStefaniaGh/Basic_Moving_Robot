#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ctype.h>
 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
 
#define BT_RX_PIN 2
#define BT_TX_PIN 3
#define DF_RX_PIN 5 
#define DF_TX_PIN 4 
 
SoftwareSerial btSerial(BT_RX_PIN, BT_TX_PIN);
SoftwareSerial dfSerial(DF_RX_PIN, DF_TX_PIN);
DFRobotDFPlayerMini player;
 
// Servo channels
#define LEFT_W_PIN   0 
#define RIGHT_W_PIN  1 
#define LEFT_ARM_PIN  2  // bratul stang
#define RIGHT_ARM_PIN 3  // bratul drept
 

#define PWM_STOP_LEFT   299 
#define PWM_FWD_LEFT    360
#define PWM_REV_LEFT    284
 
#define PWM_STOP_RIGHT  299 
#define PWM_FWD_RIGHT   340 
#define PWM_REV_RIGHT   266  

//  brate 180 grade
#define ARM_DOWN 220
#define ARM_UP   450
#define ARM_MID  300
 
#define TIMEOUT_THRESHOLD 200 
 
char command;
unsigned long lastCommandTime = 0;
int soundToggle = 1; 
 
void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  dfSerial.begin(9600);
 
  pwm.begin();
  pwm.setPWMFreq(48);
 
  Serial.println("Initializare DFPlayer...");
  if (!player.begin(dfSerial, false, false)) { 
    Serial.println("DFPlayer Error!");
  } else {
    delay(500); 
    player.volume(26); 
    Serial.println("DFPlayer OK!");
  }
 
  stopWheels();
  //brate la mijloc
  pwm.setPWM(LEFT_ARM_PIN, 0, ARM_MID);
  pwm.setPWM(RIGHT_ARM_PIN, 0, ARM_MID);
}
 
void loop() {
  btSerial.listen(); 
 
  while (btSerial.available() > 0) {
    char incoming = btSerial.read();
    
    if (isalpha(incoming)) {
      command = toupper(incoming);
      lastCommandTime = millis();
      executeCommand(command);
    }
  }
 
  if (millis() - lastCommandTime > TIMEOUT_THRESHOLD) {
    stopWheels();
  }
}
 
void executeCommand(char cmd) {
  switch(cmd) {
    case 'F': moveForward(); break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft(); break;
    case 'R': turnRight(); break;
    case 'P': stopWheels(); break;
    
    // comenzile pentru brate
    case 'U': pwm.setPWM(LEFT_ARM_PIN, 0, ARM_UP); break;   // brat stang sus
    case 'D': pwm.setPWM(LEFT_ARM_PIN, 0, ARM_DOWN); break; // brat stang jos
    case 'J': pwm.setPWM(RIGHT_ARM_PIN, 0, ARM_UP); break;  // brat drept sus
    case 'K': pwm.setPWM(RIGHT_ARM_PIN, 0, ARM_DOWN); break;// brat drept jos
    
    case 'V': 
      if (soundToggle == 1) {
        player.play(1);
        soundToggle = 2;
      } else {
        player.play(2);
        soundToggle = 1;
      }
      delay(50); 
      break;
 
    default:
      stopWheels();
      break;
  }
}
 
void moveForward() {
  pwm.setPWM(LEFT_W_PIN, 0, PWM_FWD_LEFT);
  pwm.setPWM(RIGHT_W_PIN, 0, PWM_REV_RIGHT);
}
 
void moveBackward() {
  pwm.setPWM(LEFT_W_PIN, 0, PWM_REV_LEFT);
  pwm.setPWM(RIGHT_W_PIN, 0, PWM_FWD_RIGHT);
}
 
void turnRight() {
  pwm.setPWM(LEFT_W_PIN, 0, PWM_FWD_LEFT);   
  pwm.setPWM(RIGHT_W_PIN, 0, PWM_FWD_RIGHT); 
}
 
void turnLeft() {
  pwm.setPWM(LEFT_W_PIN, 0, PWM_REV_LEFT);   
  pwm.setPWM(RIGHT_W_PIN, 0, PWM_REV_RIGHT); 
}
 
void stopWheels() {
  pwm.setPWM(LEFT_W_PIN, 0, PWM_STOP_LEFT);
  pwm.setPWM(RIGHT_W_PIN, 0, PWM_STOP_RIGHT);
}