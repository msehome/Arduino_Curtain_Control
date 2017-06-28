#include <RCSwitch.h>
#include <PWM.h>

#define OFF 0
#define UP    1
#define DOWN   2
#define BRAKE 3

#define CS_THRESHOLD 22   // Definition of safety current (Check: "1.3 Monster Shield Example").

//LIMIT SWITCH PINS
 
#define LOWER_SWITCH 9
#define UPPER_SWITCH 10

//MOTOR 1
#define MOTOR_A1_PIN 7
#define MOTOR_B1_PIN 8

#define PWM_MOTOR_1 5

#define CURRENT_SEN_1 A2

#define EN_PIN_1 A0

#define MOTOR_1 0

short usSpeed_up = 255;  //default motor speed up
short usSpeed_down = 105;  //default motor speed down

unsigned short usMotor_Status = OFF;

boolean is_going_up = false;
boolean is_going_down = false;

int upTimer = 0;
int downTimer = 0;

int32_t frequency = 20000; //frequency (in Hz)

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  InitTimersSafe();
  bool success = SetPinFrequencySafe(PWM_MOTOR_1, frequency);
  Serial.println(success); 
  //receviver setup Digital #2
  mySwitch.enableReceive(0);  
  
  //SET LED
  pinMode(LED_BUILTIN, OUTPUT);  
  
  //SET Ultrasonic Sensors
  /*pinMode(trigPin_up, OUTPUT);
  pinMode(trigPin_down, OUTPUT);
  pinMode(echoPin_up, INPUT);
  pinMode(echoPin_down, INPUT);
  */

  //SET LIMIT SWITCHES
  pinMode(UPPER_SWITCH, INPUT);
  pinMode(LOWER_SWITCH, INPUT);
  
  //MOTOR DRIVER SETUP
  pinMode(MOTOR_A1_PIN, OUTPUT);
  pinMode(MOTOR_B1_PIN, OUTPUT);

  pinMode(PWM_MOTOR_1, OUTPUT);

  pinMode(CURRENT_SEN_1, INPUT);

  pinMode(EN_PIN_1, OUTPUT);
}

void Stop()
{
  is_going_up = false;
  is_going_down = false;
  Serial.println("Stop");
  usMotor_Status = BRAKE;
  motorGo(usMotor_Status, 50);
  delay(100);
  usMotor_Status = OFF;
  motorGo(usMotor_Status, 0);
  
}

void motorGo(uint8_t direct, uint8_t pwm)
{
    if(direct == UP)
    {
      digitalWrite(MOTOR_A1_PIN, LOW); 
      digitalWrite(MOTOR_B1_PIN, HIGH);
      Serial.println("motoGo UP");
    }
    else if(direct == DOWN)
    {
      digitalWrite(MOTOR_A1_PIN, HIGH);
      digitalWrite(MOTOR_B1_PIN, LOW);
      Serial.println("motoGo UP");      
    }
    else if(direct == BRAKE)
    {
      digitalWrite(MOTOR_A1_PIN, HIGH);
      digitalWrite(MOTOR_B1_PIN, HIGH);
      Serial.println("motoGo Stop");            
    }    
    else if(direct == OFF)
    {
      digitalWrite(MOTOR_A1_PIN, LOW);
      digitalWrite(MOTOR_B1_PIN, LOW);
      Serial.println("motoGo Stop");            
    }    
    analogWrite(PWM_MOTOR_1, pwm); 
}

boolean overcurrent()
{
  float I_current_value;
  I_current_value = (analogRead(CURRENT_SEN_1)*5*11370/1500); // get I in milliamps
  //I_current_value = I_current_value/1000 ;
  Serial.print("Current: ");
  Serial.println(I_current_value);
  if (I_current_value >= CS_THRESHOLD)
    return true;
  else
    return false;
}
//Ping for upper ultrasonic distance
int ping_up()
{
  /*long duration_up;
  long distance_up;
  digitalWrite(trigPin_up, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_up, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_up, LOW);
  duration_up = pulseIn(echoPin_up, HIGH);
  distance_up = (duration_up/2) / 29.1;
  Serial.print("distance up:");
  Serial.println(distance_up);
  return distance_up;*/
  return digitalRead(UPPER_SWITCH);
}

//Ping for lower ultrasonic distance
int ping_down()
{
  /*long duration_down;
  long distance_down;
  digitalWrite(trigPin_down, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_down, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_down, LOW);
  duration_down = pulseIn(echoPin_down, HIGH);
  distance_down = (duration_down/2) / 29.1;
  Serial.print("distance down:");
  Serial.println(distance_down);
  return distance_down;*/
  return digitalRead(LOWER_SWITCH);
}

void loop() {
  
  digitalWrite(EN_PIN_1, HIGH);
  /*Serial.print("Down");
  Serial.println(ping_down());
  Serial.print("Up");
  Serial.println(ping_up());*/
  if (is_going_up)
  {      
      if (ping_up() == 1 || overcurrent() || upTimer >= 100)
      {
          delay(140);
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println("Arrived to the top");
          Serial.println(upTimer);            
      }
      else
      {
      //  motorGo(UP,usSpeed);
        Serial.println("Is going up...");
      }
      upTimer++;
  }
    
  if (is_going_down)
  {
      if (ping_down() == 0 || overcurrent() || downTimer >= 112)
      {
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println(ping_down());
          Serial.println("Arrived to the bottom");  
          Serial.println(downTimer);            
      }
      else
      {
       // motorGo(DOWN,usSpeed);
        Serial.println("Is going down...");
      }
      downTimer++;
  }
    
  if (mySwitch.available()) {
    //output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    Serial.println(mySwitch.getReceivedValue());
   
    
    if (mySwitch.getReceivedValue() == 5591232) //continous up
    {
        upTimer = 0;
        if (ping_up() == 1)
        {
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println("Arrived to the top");
        }
        else
        {
          digitalWrite(LED_BUILTIN, HIGH);
          motorGo(UP,usSpeed_up);
          is_going_up = true;
        }
    }
    else if (mySwitch.getReceivedValue() == 5591088) //continous down
    {
        downTimer = 0;
        if (ping_down() == 0)
        {
          delay(10);
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println(ping_down());
          Serial.println("Arrived to the bottom");
        }
        else
        {
          digitalWrite(LED_BUILTIN, HIGH);
          motorGo(DOWN,usSpeed_down);
          is_going_down = true;
        }
    }
    else if (mySwitch.getReceivedValue() == 5591043) //single step up
    {
        upTimer = 0;
        if (ping_up() == 1)
        {
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println("Arrived to the top");
        }
        else
        {
          digitalWrite(LED_BUILTIN, HIGH);
          motorGo(UP,usSpeed_up);
          is_going_up = true;
          delay(500);
          Stop();
        }      
    }
    else if (mySwitch.getReceivedValue() == 5591052) //single step down
    {
        downTimer = 0;
        if (ping_down() == 0)
        {
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println("Arrived to the bottom");
        }
        else
        {
          digitalWrite(LED_BUILTIN, HIGH);
          motorGo(DOWN,usSpeed_down);
          is_going_down = true;
          delay(500);
          Stop();
        }            
    }
    else if (mySwitch.getReceivedValue() == 5591808)
    {
      Stop();
      Serial.println("Stop");
    }
    mySwitch.resetAvailable();
    
  }   
  delay(50);
}
