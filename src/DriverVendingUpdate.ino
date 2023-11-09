#include <TimerOne.h> 
//Config I/O
const int OE =  7;
const int SHCP =  6;
const int STCP =  5;
const int DS =  4;
const int dropss = 3;
const int encoder =2;
const int endrop = 9;
byte current_state =HIGH, last_current_state=HIGH;
byte drop_state =HIGH, last_drop_state=HIGH;
byte drop = HIGH;
int counter = 0;
int timer=0, st=5;
int tool_number;
int num1, num2, check_serial=0, check_tool=0;
String inString = "";
//define value set

void setup() 
{
  Serial.begin(9600); // Start the Serial to show results in Serial Monitor
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  pinMode(SHCP, OUTPUT);// Latch PIN
  digitalWrite(SHCP, LOW);
  pinMode(STCP, OUTPUT);// Clock PIN
  digitalWrite(STCP, LOW);
  pinMode(DS, OUTPUT); //Data PIN
  digitalWrite(DS, LOW);
  pinMode(OE, OUTPUT);
  digitalWrite(DS, LOW);
  motor_off();
  pinMode(dropss, INPUT);
  pinMode(encoder, INPUT);
  pinMode(endrop, OUTPUT);
  //digitalWrite(endrop, HIGH);

//    set timer interrupt
  Timer1.initialize(1000000); // khởi tạo timer 1 đến 1 giây
  Timer1.attachInterrupt(clk); // khai báo ngắt timer 1
   
}
  
void loop() 
{
  while (Serial.available() > 0)
  {
    read_serial();
  }
  
  if(st<3)
  {
    current_monitor();
  }
  if(drop == LOW){
    drop_monitor();
  }
}
  
//read buttom change
void current_monitor() 
{
  current_state = digitalRead(encoder);
  if(timer>4)
  {
    st=3;
    motor_off();
  }
  if (current_state != last_current_state) 
  {
    last_current_state = current_state;
    switch (counter)
    {
      //Motor Start Event
      case 0:
      {
        if (current_state == LOW)
        {
          Serial.println("121");//Motor Start
          counter++;
          st++;
        }
      }
      break;
      //Push SW1 Event
      case 1:
      {
        if (current_state == HIGH)
        {
          //Serial.println("Push SW 1");
          counter++;
          st++;
        }
      }
      break;

      //Push SW2 Event Stop Motor
      case 2:
      {
        if (current_state == HIGH)
        {
          //Serial.println("Push SW 2");
          Serial.println("122");
          motor_off();
          counter++;
          st++;
        }
      }
      break;
    }
  }
}

void drop_monitor() 
{
  drop_state = digitalRead(dropss);
  if(timer>6)
  {
    drop = HIGH;
    motor_off();
    Serial.println("124");//Action Fail
    //digitalWrite(endrop, HIGH);
  }
  if (drop_state != last_drop_state) 
  {
    last_drop_state = drop_state;
    if (drop_state == LOW) 
    {
      drop = HIGH;
      Serial.println("123");//Get Tool Suscess
      //digitalWrite(endrop, HIGH); 
    }
  }
}
  
void read_serial()
{
  int inChar = Serial.read();
  if(inChar == ',')
  {
    check_serial=1;
    num1=inString.toInt();
    inString = "";
  }
  else if(inChar == '\n')
  {
    if(check_serial==1)
    {
      num2=inString.toInt();
      check_serial=0;
      inString = "";
    }
    else 
    {
      num1 = inString.toInt();
      num2 = 0;
      check_serial=0;
      inString = "";
    }

    if(num1 == 100) Serial.println("120");

    if(num1 == 101)
    {
      if(num2>0 && num2<=60)
      {
        tool_number=num2;
        st=0;
        timer=0;
        counter=0;
        drop = LOW;
        Shiftout_Slot(tool_number);
      }
    }
  }
  else inString += (char)inChar;
}

void Shiftout_Slot(int slot)
{
  int data[16];
  int row, column;
  if(slot > 0 and slot <= 60)
  {
    row = (slot -1)/10;
    column = (slot -1)%10;
    for(int i = 0; i<10;i++){
      if(i== column) data[i] = 1;
      else data[i] = 0;
    }
    for(int i = 10; i<16;i++){
      if((i-10) == row) data[i] = 1;
      else data[i] = 0;
    }
    for(int i =0;i<16;i++){
      if(data[15-i]==0) digitalWrite(DS, LOW);
      else digitalWrite(DS, HIGH);
      digitalWrite(SHCP, LOW);
      digitalWrite(SHCP, HIGH);
    }
    digitalWrite(STCP, LOW);
    digitalWrite(STCP, HIGH);
  }
  else
  {
    motor_off();
  }
}

void clk()
{
  timer++;
  if(timer>20) timer=0;
}

void motor_off()
{
  for(int i =0;i<16;i++)
  {
      digitalWrite(DS, LOW);
      digitalWrite(SHCP, LOW);
      digitalWrite(SHCP, HIGH);
  }
  digitalWrite(STCP, LOW);
  digitalWrite(STCP, HIGH);
}
