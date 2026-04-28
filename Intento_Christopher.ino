#define S1 A0     
#define S2 A1 
#define S3 A2
#define S4 A3
#define S5 A4

#define ENA 5
#define IN1 6
#define IN2 7
#define ENB 9
#define IN3 10
#define IN4 11

// --- Constantes de Control ---
int baseSpeed = 215;    
int maxSpeed = 200;    
float Kp = 35.0;       
float Kd = 40.0;       

// --- AJUSTE DE HARDWARE ---
// Si el motor DERECHO sigue lento, sube este número (ej: 1.3, 1.4)
// Si se pasa de rápido, bájalo (ej: 1.1)
float compensacionDerecha = 1.4; 

int lastError = 0;

void setup() {
  Serial.begin(9600);
  pinMode(S1, INPUT); pinMode(S2, INPUT); pinMode(S3, INPUT);
  pinMode(S4, INPUT); pinMode(S5, INPUT);
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
}

void loop() {
  // 1. Lectura de sensores
  int s1 = !digitalRead(S1);
  int s2 = !digitalRead(S2);
  int s3 = !digitalRead(S3);
  int s4 = !digitalRead(S4);
  int s5 = !digitalRead(S5);

  // 2. Cálculo de Error
  int error = (s1 * -4) + (s2 * -2) + (s3 * 0) + (s4 * 2) + (s5 * 4);

  // 3. Caso: Fuera de la línea

  if (s1 + s2 + s3 + s4 + s5 == 0) {
    delay(120);
    if (lastError < 0)  moveMotors(120, -108);
    else moveMotors(-108, 120);
    return;
  }

  // Si solo el sensor central ve la línea, ignoramos el PD y vamos recto
  if (s3 == 1 && s1 == 0 && s2 == 0 && s4 == 0 && s5 == 0) {
    int vDerecha = baseSpeed * compensacionDerecha;
    moveMotors(baseSpeed, vDerecha);
    lastError = 0;
    return; 
  }

  // 4. Algoritmo PD
  int motorSpeed = (Kp * error) + (Kd * (error - lastError));
  lastError = error;

  int leftSpeed = baseSpeed - motorSpeed;
  // Aplicamos la compensación también aquí
  int rightSpeed = (baseSpeed + motorSpeed) * compensacionDerecha;

  // 5. Ajuste de límites
  leftSpeed = constrain(leftSpeed, -120, maxSpeed);
  rightSpeed = constrain(rightSpeed, -120, maxSpeed);

  moveMotors(leftSpeed, rightSpeed);
}

void moveMotors(int left, int right) {
  // Motor Izquierdo
  if (left >= 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    left *= -1;
  }
  analogWrite(ENA, left);

  // Motor Derecho
  if (right >= 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    right *= -1;
  }
  analogWrite(ENB, right);
}
