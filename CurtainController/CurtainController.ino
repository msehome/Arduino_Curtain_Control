#include <RCSwitch.h>

#define trigPin_up 9
#define echoPin_up 10
#define trigPin_down 11
#define echoPin_down 12

#define BRAKE 0
#define UP    1
#define DOWN   2
#define CS_THRESHOLD 15   // Definition of safety current (Check: "1.3 Monster Shield Example").

//MOTOR 1
#define MOTOR_A1_PIN 7
#define MOTOR_B1_PIN 8

#define PWM_MOTOR_1 5

#define CURRENT_SEN_1 A2

#define EN_PIN_1 A0

#define MOTOR_1 0

short usSpeed_up = 150;  //default motor speed up
short usSpeed_down = 50;  //default motor speed down

unsigned short usMotor_Status = BRAKE;

boolean is_going_up = false;
boolean is_going_down = false;

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  
  //receviver setup Digital #2
  mySwitch.enableReceive(0);  
  
  //SET LED
  pinMode(LED_BUILTIN, OUTPUT);  
  
  //SET Ultrasonic Sensors
  pinMode(trigPin_up, OUTPUT);
  pinMode(trigPin_down, OUTPUT);
  pinMode(echoPin_up, INPUT);
  pinMode(echoPin_down, INPUT);
  
  //Motor Driver Setup
  pinMode(MOTOR_A1_PIN, OUTPUT);
  pinMode(MOTOR_B1_PIN, OUTPUT);

  pinMode(PWM_MOTOR_1, OUTPUT);

  pinMode(CURRENT_SEN_1, OUTPUT);

  pinMode(EN_PIN_1, OUTPUT);
}

void Stop()
{
  is_going_up = false;
  is_going_down = false;
  Serial.println("Stop");
  usMotor_Status = BRAKE;
  motorGo(usMotor_Status, 0);
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
    else
    {
      digitalWrite(MOTOR_A1_PIN, LOW);
      digitalWrite(MOTOR_B1_PIN, LOW);
      Serial.println("motoGo Stop");            
    }    
    analogWrite(PWM_MOTOR_1, pwm); 
}

//Ping for upper ultrasonic distance
int ping_up()
{
  long duration_up;
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
  return distance_up;
}

//Ping for lower ultrasonic distance
int ping_down()
{
  long duration_down;
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
  return distance_down;
}

void loop() {
  digitalWrite(EN_PIN_1, HIGH);
  if (is_going_up)
    {
      if (ping_up() > 40)
      {
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println("Arrived to the top");              
      }
      else
      {
        //motorGo(UP,usSpeed);
        Serial.println("Is going up...");
      }
    }
    
    if (is_going_down)
    {
      if (ping_down() < 20)
      {
          digitalWrite(LED_BUILTIN, LOW);
          Stop();
          Serial.println("Arrived to the bottom");              
      }
      else
      {
        //motorGo(DOWN,usSpeed);
        Serial.println("Is going down...");
      }
    }
  if (mySwitch.available()) {
    //output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    Serial.println(mySwitch.getReceivedValue());
   
    
    if (mySwitch.getReceivedValue() == 5591232) //continous up
    {
        if (ping_up() > 40)
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
        if (ping_down() < 20)
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
        }
    }
    else if (mySwitch.getReceivedValue() == 5591043) //single step up
    {
        if (ping_up() < 20)
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
          delay(1000);
          Stop();
        }      
    }
    else if (mySwitch.getReceivedValue() == 5591052) //single step down
    {
        if (ping_down() < 20)
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
          delay(1000);
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
   
  delay(100);
}
