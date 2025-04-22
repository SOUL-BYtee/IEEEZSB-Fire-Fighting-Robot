#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(0, 1); // RX, TX
Servo myServo;  // كائن التحكم في السيرفو

#define IN1 3
#define IN2 5
#define IN3 6
#define IN4 9
#define ir_R 4  // حساس اللهب - يمين
#define ir_L 8  // حساس اللهب - يسار
#define pump A0  // المضخة متصلة بالترانزيستور
#define L2 A1
#define L1 A2  
#define middle A3
#define R1 A4
#define R2 A5

// متغير استقبال بيانات البلوتوث
int data;
int Speed = 130;


bool sensorValues[5];
char lastMove = 'F';  
char Mode = 'M';                   // F = للأمام، L = يسار، R = يمين
bool SF, SL, SR;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ir_R, INPUT);
  pinMode(ir_L, INPUT);
  pinMode(pump, OUTPUT);

  myServo.attach(10);  // السيرفو متصل بالبن رقم 10
  myServo.write(90);   // الوضع الافتراضي للسيرفو

  for (byte i = 1; i < 6; i++) {
    pinMode(A0 + i, INPUT);
  }
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.read() == 'U')
    Mode = 'U';
  if (mySerial.read() == 'R' || mySerial.read() == '82')
    Mode = 'R';
  if (mySerial.read() == 'A')
    Mode = 'A';
  Serial.println(Mode);
  switch (Mode) {
    case 'R':
    IR();
    break;
    case 'U':
    manual();
    break;
    case 'A':
    FA();
    break;
    default :
    Serial.println("Else !");
    break;
  }
}

void forward() {
  analogWrite(IN1, 180);
  analogWrite(IN2, 0);
  analogWrite(IN3, 180);
  analogWrite(IN4, 0);
  lastMove = 'F';
}

void right() {
  analogWrite(IN1, 75);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 75);
  lastMove = 'R';
}


void left() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 75);
  analogWrite(IN3, 75);
  analogWrite(IN4, 0);
  lastMove = 'L';
}


void readSensors() {
  for (byte i = 0; i < 5; i++) {
    sensorValues[i] = digitalRead(A1 + i);
  }

  Serial.print("Sensors: ");
  for (byte i = 0; i < 5; i++) {
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void continueLastMove() {
  if (lastMove == 'L') left();
  else if (lastMove == 'R') right();
  else forward(); // احتياطي في حال لم تُسجل حركة سابقة
}


void IR() {
  if (Serial.read() == 'A')
    Mode = 'A';
  else if(Serial.read() == 'U')
    Mode = 'U';
  readSensors(); 
  if (!sensorValues[3] || !sensorValues[4]) { // R1 - يمين بسيط
    right();
  }  
  else if (!sensorValues[1] || !sensorValues[0]) { // L1 - يسار بسيط
    left();
  } 
  else if (!sensorValues[2]) { // الأوسط فقط إذا لم يظهر الجانبين
    forward();
  } 
  else {
    // إذا فقد الخط تمامًا، أكمل في نفس الاتجاه الأخير
    continueLastMove();
  }
}

void manual() {
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

  if (data == 'R')
    Mode = 'R';
  else if (data == 'A')
    Mode = 'A';

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
  else if (data == 3) {
    analogWrite(IN1, 255);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
  }
  else if (data == 4) {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, 255);
    analogWrite(IN4, 0);
  }

  else if (data == 5) {
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
  }
}
void FA() {
  if (Serial.read() == 'R')
    Mode = 'R';
  if (Serial.read() == 'U')
    Mode = 'U';

  SF = digitalRead(ir_R);
  SR = digitalRead(ir_L);

  Serial.print("Right: "); Serial.print(SL);
  Serial.print(" | Front: "); Serial.print(SF);
  Serial.print(" | Left: "); Serial.println(SR);

  delay(100);

  if (!SF) {  // إذا تم الكشف عن نار في المنتصف
    Stop();
    MoveServo('F');
  }
  else if (!SL) {  // إذا تم الكشف عن نار علي اليمين
    Stop();
    MoveServo('R');
  }
  else if (!SR) {  // إذا تم الكشف عن نار على اليسار
    Stop();
    MoveServo('L');
  }
  else {  // لا يوجد حريق، تابع التحرك
    MoveForward();
    digitalWrite(pump, HIGH);
  }
}

void MoveServo(char numS) {
  digitalWrite(pump, LOW);
  switch (numS) {
  case 'R':
    for (int i = 0; i <= 90; i += 3) {
      myServo.write(i);
      delay(30);
    }
    break;
  case 'F':
  for (int i = 20; i <= 150; i += 3) {
      myServo.write(i);
      delay(30);
    }
    break;
  case 'L':
  for (int i = 180; i >= 90; i -= 3) {
      myServo.write(i);
      delay(30);
    }
  break;
  }
  digitalWrite(pump, HIGH);
}

void MoveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}