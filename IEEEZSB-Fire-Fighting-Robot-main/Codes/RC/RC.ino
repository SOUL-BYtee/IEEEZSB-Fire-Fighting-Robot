// تعريف المحركات
#define IN1 3  
#define IN2 5  
#define IN3 6  
#define IN4 9  

// متغير استقبال بيانات البلوتوث
int data;
int Speed = 130;

void setup() {
  Serial.begin(9600);

  // تعريف أطراف المحركات
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  delay(1000);
}

void loop() {
  // استقبال البيانات من البلوتوث
  if (Serial.available() > 0) {
    data = Serial.read();
    Serial.print("Received: ");
    Serial.println(data);
  }

  delay(30);

  if (Serial.available()) {
    data = Serial.read();
    Serial.println(data);
  }
  if (data == 1) {
    analogWrite(IN1, 255);
    analogWrite(IN2, 0);
    analogWrite(IN3, 255);
    analogWrite(IN4, 0);
  }

  else if (data == 2) {
    analogWrite(IN1, 0);
    analogWrite(IN2, 255);
    analogWrite(IN3, 0);
    analogWrite(IN4, 255);
  }
    else if (data == 4) {
    analogWrite(IN1, 255);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
  }
    else if (data == 3) {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, 255);
    analogWrite(IN4, 0);
  }

   else if(data == 5) {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
    }

}
