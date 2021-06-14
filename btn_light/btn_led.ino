// Author:  Ori Shinsholker & Michal Tamir

#define PUSH_BTN 35
#define LED_RED 15
#define LED_BLUE 12

int count=0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
    Serial.begin(115200);

    pinMode(LED_RED, OUTPUT); // RED
    pinMode(LED_BLUE, OUTPUT); // BLUE
    pinMode(PUSH_BTN, INPUT); //BOTTUNN
}

// the loop function runs over and over again forever
void loop() {
  

  if (digitalRead(PUSH_BTN) == HIGH){
  
      Serial.println("BTN  HIGH");
      if(count%5==0){
        Serial.println("Blue");
        digitalWrite(LED_RED, LOW);    // turn the red LED off
        digitalWrite(LED_BLUE,HIGH); // turn the Blue LED on (HIGH is the voltage level)
      }
      else{
        Serial.println("Red");
        digitalWrite(LED_RED, HIGH);   // turn the red LED on (HIGH is the voltage level)
        digitalWrite(LED_BLUE, LOW);   // turn the blue LED off
      }
      Serial.print("Cont is: ");
      Serial.println(count);

      count++;
      delay(1000);

  }
  if (digitalRead(PUSH_BTN) == LOW){
    Serial.println("BTN  LOW");
    digitalWrite(LED_RED, LOW); 
    digitalWrite(LED_BLUE,LOW);
  }// turn the LED off by making the voltage LOW
}//loop
