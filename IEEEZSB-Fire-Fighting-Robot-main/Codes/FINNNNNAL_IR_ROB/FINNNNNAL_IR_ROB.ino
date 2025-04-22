#define IN1 3
#define IN2 5
#define IN3 6
#define IN4 9

#define L2 11   
#define L1 7  
#define middle 8  
#define R1 10  
#define R2 4  

int sensorValues[5];
char lastMove = 'F';                     // F = للأمام، L = يسار، R = يمين

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(L2, INPUT);
  pinMode(L1, INPUT);
  pinMode(middle, INPUT);
  pinMode(R1, INPUT);
  pinMode(R2, INPUT);

  Serial.begin(9600);
}

void forward() {
  analogWrite(IN1, 120);
  analogWrite(IN2, 0);
  analogWrite(IN3, 120);
  analogWrite(IN4, 0);
  lastMove = 'F';
}

void right() {
  analogWrite(IN1, 120);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 120);
  lastMove = 'R';
}

// void turn_right() {
//   analogWrite(IN1, 120);
//   analogWrite(IN2, 0);
//   analogWrite(IN3, 0);
//   analogWrite(IN4, 120);
//   lastMove = 'R';
// }

void left() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 120);
  analogWrite(IN3, 120);
  analogWrite(IN4, 0);
  lastMove = 'L';
}

// void turn_left() {
//   analogWrite(IN1, 0);
//   analogWrite(IN2, 120);
//   analogWrite(IN3, 120);
//   analogWrite(IN4, 0);
//   lastMove = 'L';
// }

void readSensors() {
  sensorValues[0] = digitalRead(L2);
  sensorValues[1] = digitalRead(L1);
  sensorValues[2] = digitalRead(middle);
  sensorValues[3] = digitalRead(R1);
  sensorValues[4] = digitalRead(R2);

  Serial.print("Sensors: ");
  for (int i = 0; i < 5; i++) {
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void continueLastMove() {
  if (lastMove == 'L') left();
  else if (lastMove == 'R') right();
  else forward();
}

void loop() {
  readSensors(); 

  // الأولوية للحساسات الجانبية أولًا، ثم الحساس الأوسط
  // if (sensorValues[4] == 0) {      // R2 - يمين حاد
  //   turn_right();
  // } 
  if (sensorValues[3] == 0 || sensorValues[4] == 0) { // R1 - يمين بسيط
    right();
  } 
  // else if (sensorValues[0] == 0) { // L2 - يسار حاد
  //   turn_left();
  // } 
  else if (sensorValues[1] == 0 || sensorValues[0] == 0) { // L1 - يسار بسيط
    left();
  } 
  else if (sensorValues[2] == 0) { // الأوسط فقط إذا لم يظهر الجانبين
    forward();
  } 
  else {
    // إذا فقد الخط تمامًا، أكمل في نفس الاتجاه الأخير
    continueLastMove();
  }
}
