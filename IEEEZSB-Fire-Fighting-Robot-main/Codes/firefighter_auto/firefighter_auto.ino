#include <Servo.h>

#define motor1_1 3  // محرك 1 - أمامي
#define motor1_2 5  // محرك 1 - خلفي
#define motor2_1 6  // محرك 2 - أمامي
#define motor2_2 9  // محرك 2 - خلفي

#define ir_R 4  // حساس اللهب - يمين
#define ir_L 8  // حساس اللهب - يسار

#define pump A0  // المضخة متصلة بالترانزيستور

Servo myServo;  // كائن التحكم في السيرفو

int s1, s3;
const int threshold = 3;  // عدد القراءات المطلوبة لتأكيد وجود النار

void setup() {
  Serial.begin(9600);

  pinMode(motor1_1, OUTPUT);
  pinMode(motor1_2, OUTPUT);
  pinMode(motor2_1, OUTPUT);
  pinMode(motor2_2, OUTPUT);

  pinMode(ir_R, INPUT);
  pinMode(ir_L, INPUT);

  pinMode(pump, OUTPUT);

  myServo.attach(10);
  myServo.write(90);  // الوضع الافتراضي للسيرفو

  Serial.println("System Initialized...");
}

void loop() {
  s1 = ReadSensor(ir_R);
  s3 = ReadSensor(ir_L);

  Serial.print("Right: "); Serial.print(s1);
  Serial.print(" | Left: "); Serial.println(s3);

  delay(100);

  if (s1 == LOW) {  // نار على اليمين
    TurnRight();
    Stop();
    delay(300);
    ExtinguishFire(130); // إطفاء النار مع حركة السيرفو
    MoveBackward();
  } 
  else if (s3 == LOW) {  // نار على اليسار
    TurnLeft();
    Stop();
    delay(300);
    ExtinguishFire(20); // إطفاء النار مع حركة السيرفو
    MoveBackward();
  } 
  else {  // لا يوجد حريق، تابع التحرك
    digitalWrite(pump, LOW);
    MoveForward();
  }
}

// قراءة الحساس عدة مرات لتأكيد القرار
int ReadSensor(int pin) {
  int count = 0;
  for (int i = 0; i < threshold; i++) {
    count += digitalRead(pin);
    delay(5);
  }
  return (count >= threshold / 2) ? HIGH : LOW;
}

// إطفاء الحريق مع تحريك السيرفو بسرعة
void ExtinguishFire(int targetAngle) {
  myServo.write(targetAngle);  // تحريك السيرفو ناحية النار
  delay(300);  // انتظار لوصوله للمكان

  unsigned long startTime = millis();
  digitalWrite(pump, HIGH); // تشغيل المضخة

  while (millis() - startTime < 3000) { // تشغيل الإطفاء لمدة 3 ثوانٍ
    for (int angle = targetAngle - 10; angle <= targetAngle + 10; angle += 5) {
      myServo.write(angle);
      delay(50);  // حركة سريعة يمين وشمال
    }
    for (int angle = targetAngle + 10; angle >= targetAngle - 10; angle -= 5) {
      myServo.write(angle);
      delay(50);
    }
  }

  digitalWrite(pump, LOW); // إطفاء المضخة
  delay(200);

  myServo.write(90); // إعادة السيرفو للوضع الطبيعي بسرعة
}

void MoveForward() {
  analogWrite(motor1_1, 80);
  analogWrite(motor1_2, 0);
  analogWrite(motor2_1, 80);
  analogWrite(motor2_2, 0);
}

void MoveBackward() {
  analogWrite(motor1_1, 0);
  analogWrite(motor1_2, 80);
  analogWrite(motor2_1, 0);
  analogWrite(motor2_2, 80);
  delay(700);
  Stop();
}

void TurnRight() {
  analogWrite(motor1_1, 80);
  analogWrite(motor1_2, 0);
  analogWrite(motor2_1, 0);
  analogWrite(motor2_2, 80);
  delay(500);
}

void TurnLeft() {
  analogWrite(motor1_1, 0);
  analogWrite(motor1_2, 80);
  analogWrite(motor2_1, 80);
  analogWrite(motor2_2, 0);
  delay(500);
}

void Stop() {
  analogWrite(motor1_1, 0);
  analogWrite(motor1_2, 0);
  analogWrite(motor2_1, 0);
  analogWrite(motor2_2, 0);
}
