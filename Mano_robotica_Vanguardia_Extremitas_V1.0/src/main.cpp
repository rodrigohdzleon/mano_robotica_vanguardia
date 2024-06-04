// Librerias necesarias
#include <Arduino.h>
#include <Servo.h>
#include <Q2HX711.h>

// Variables para sensores de presión barométrica
const byte MPS_OUT_pin1 = 2; // OUT data pin for sensor 1
const byte MPS_SCK_pin1 = 3; // clock data pin for sensor 1
const byte MPS_OUT_pin2 = 4; // OUT data pin for sensor 2
const byte MPS_SCK_pin2 = 5; // clock data pin for sensor 2
int avg_size = 10; // #pts to average over
int soplo_counter_sensor1 = 1; // Counter for the number of blows detected by sensor 1
int soplo_counter_sensor2 = 0; // Counter for the number of blows detected by sensor 2
bool above_threshold1 = false; // Flag to track if sensor 1 reading is above threshold
bool above_threshold2 = false; // Flag to track if sensor 2 reading is above threshold
Q2HX711 MPS20N0040D_1(MPS_OUT_pin1, MPS_SCK_pin1); // start comm with the first HX710B
Q2HX711 MPS20N0040D_2(MPS_OUT_pin2, MPS_SCK_pin2); // start comm with the second HX710B

// Pines de los motores
int pin_rot_pulgar = 7;
int pin_menique_anular = 8;
int pin_indice_medio_mitad = 9;
int pin_pulgar = 10;
int pin_menique_anular_mitad = 11;
int pin_medio = 12;
int pin_indice = 13;

// Pines de los LEDs
int LED1 = 36;
int LED2 = 37;
int LED3 = 38;
int LED4 = 39;
int LED5 = 40; 

// Se asignan los servomotores con su nombre respectivo
Servo indice;
Servo medio;
Servo menique_anular;
Servo menique_anular_mitad;
Servo pulgar;
Servo indice_medio_mitad;
Servo rot_pulgar;

// Pines para sensores de presión
int pin_fuerza_pulgar = 0;
int pin_fuerza_indice = 1;
int pin_fuerza_medio = 2;
int pin_fuerza_menique_anular = 3;

// Valor detectado por sensor de presión
int lectura_fuerza_indice;
int lectura_fuerza_medio;
int lectura_fuerza_menique_anular;
int lectura_fuerza_pulgar;

// Angulos actuales de los servomotores
int angulo_actual_indice;
int angulo_actual_medio;
int angulo_actual_menique_anular;
int angulo_actual_pulgar;
int angulo_actual_menique_anular_mitad;

// Estados iniciales de los motores
int estado_indice;
int estado_medio;
int estado_menique_anular;
int estado_pulgar;
int estado_menique_anular_mitad;

// Función de agarre convencional
void agarre(){

  // Se prende el LED rojo que indica que hay un agarre activo
  digitalWrite(LED5,HIGH);
  Serial.println("Agarre convencioanl en ejecucion");
  int velocidad_dedos = 20; // Mientras se disminuya, los dedos cierran más rápido

  // Variables para los angulos de cada dedo
  int angulo_menique_anular_mitad = 0;
  int angulo_indice = 0;
  int angulo_medio = 0;
  int angulo_menique_anular = 180;

  // Indican el ángulo máximo al que debe llegar el motor para este agarre
  while (angulo_indice < 140 || angulo_medio < 140 || angulo_menique_anular > 38) {

    // Movimiento del dedo indice hasta su angulo máximo. Es igual que en el while.
    if (angulo_indice < 140) {

      // Se detecta la fuerza del sensor en el dedo indice
      lectura_fuerza_indice = analogRead(pin_fuerza_indice); 

      // Se escribe el ángulo al motor, mientras el sensor del dedo no superen la presión establecida
      if (lectura_fuerza_indice <= 400 && estado_indice == 1) {
        indice.write(angulo_indice);
      }

      // Se lee el angulo actual de motor
      angulo_actual_indice = indice.read();

      // Si se supera la presión establecida, se frena el dedo 5 grados más de lo actual para apretar y ahí se mantiene
      if (lectura_fuerza_indice >= 400) {
        lectura_fuerza_indice = analogRead(pin_fuerza_indice); // Se lee la presión del sensor
        delay(15);
        indice.write(angulo_actual_indice + 5);
        estado_indice = 0; // Se manda su estado a cero para que no siga aumentando el ángulo
      }

      delay(velocidad_dedos); // Delay para que de tiempo de sensar los dedos
      angulo_indice += 2; // Se aumenta en dos el angulo del motor para que sea más rápido

      // Si se llega al ángulo final del dedo, se frena
      if (angulo_indice >= 138) {
        estado_indice = 0;
      }
    }

    // Movimiento del dedo medio hasta su angulo máximo. Es igual que en el while.
    if (angulo_medio < 140) {

      // Se detecta la fuerza del sensor en el dedo medio
      lectura_fuerza_medio = analogRead(pin_fuerza_medio);

      // Se escribe el ángulo al motor, mientras el sensor del dedo no superen la presión establecida
      if (lectura_fuerza_medio <= 400 && estado_medio == 1) {
        medio.write(angulo_medio);
      }

      // Se lee el angulo actual de motor
      angulo_actual_medio = medio.read();

      // Si se supera la presión establecida, se frena el dedo 5 grados más de lo actual para apretar y ahí se mantiene
      if (lectura_fuerza_medio >= 400) {
        lectura_fuerza_medio = analogRead(pin_fuerza_medio); // Se lee la presión del sensor
        delay(15);
        medio.write(angulo_actual_medio + 5);
        estado_medio = 0; // Se manda su estado a cero para que no siga aumentando el ángulo
      }

      delay(velocidad_dedos); // Delay para que de tiempo de sensar los dedos
      angulo_medio += 2; // Se aumenta en dos el angulo del motor para que sea más rápido

      // Si se llega al ángulo final del dedo, se frena
      if (angulo_medio >= 138) {
        estado_medio = 0;
      }
    }

    // Movimiento del dedo meñique y anular hasta su angulo máximo. Es igual que en el while.
    if (angulo_menique_anular > 38) {

      // Se detecta la fuerza del sensor en el dedo anular
      lectura_fuerza_menique_anular = analogRead(pin_fuerza_menique_anular);

      // Se escribe el ángulo a los motores, mientras el sensor del dedo no superen la presión establecida
      if (lectura_fuerza_menique_anular <= 400 && estado_menique_anular == 1 && estado_menique_anular_mitad == 1) {
        menique_anular.write(angulo_menique_anular);
        menique_anular_mitad.write(angulo_menique_anular_mitad);
      }

      // Se lee el angulo actual de los motores
      angulo_actual_menique_anular = menique_anular.read();
      angulo_actual_menique_anular_mitad = menique_anular_mitad.read();

      // Si se supera la presión establecida, se frenan los dedos 5 grados más de lo actual para apretar y ahí se mantienen
      if (lectura_fuerza_menique_anular >= 400) {
        lectura_fuerza_menique_anular = analogRead(pin_fuerza_menique_anular); // Se lee la presión del sensor
        delay(15);
        menique_anular.write(angulo_actual_menique_anular - 5);
        menique_anular_mitad.write(angulo_actual_menique_anular_mitad + 5);
        estado_menique_anular = 0; // Se manda su estado a cero para que no siga aumentando el ángulo
      }

      delay(velocidad_dedos); // Delay para que de tiempo de sensar los dedos
      // Se le manda a los dos motores para que el cable no se salga de un motor
      angulo_menique_anular -= 2; // Se disminuye en dos el angulo del motor para que sea más rápido
      angulo_menique_anular_mitad += 2; // Se aumenta en dos el angulo del motor para que sea más rápido

      // Si se llega al ángulo final del dedo, se frena
      if (angulo_menique_anular <= 40) {
        estado_menique_anular = 0;
        estado_menique_anular_mitad = 0;
      }
    }
  }

  // Delay para que se realice el movimiento de los dedos y el pulgar no se atore
  delay(1000);

  // Movimiento dedo pulgar. Angulo inicial; angulo final; disminución del ángulo
  for (int angulo = 180; angulo > 102; angulo = angulo - 2){

    // Se detecta la fuerza del sensor en el dedo anular
    lectura_fuerza_pulgar = analogRead(pin_fuerza_pulgar);
      
    // Se escribe el ángulo al motor, mientras el sensor del dedo no superen la presión establecida
    if(lectura_fuerza_pulgar <= 400 && estado_pulgar == 1){
      pulgar.write(angulo);
    }

    // Se lee el angulo actual del motor
    angulo_actual_pulgar = pulgar.read();

    // Si se supera la presión establecida, se frena el dedo 5 grados más de lo actual para apretar y ahí se mantienen
    if(lectura_fuerza_pulgar >= 400){
      lectura_fuerza_pulgar = analogRead(pin_fuerza_pulgar); // Se lee la presión del sensor
      delay(15);
      pulgar.write(angulo_actual_pulgar - 5);
      estado_pulgar = 0; // Se manda su estado a cero para que no siga aumentando el ángulo
    }

    delay(velocidad_dedos); // Delay para que de tiempo de sensar los dedos

    // Si se llega al ángulo final del dedo, se frena
    if(angulo <= 102){
      estado_pulgar = 0;
    }
  }
}

// Función para agarre de tipo pinza
void pinza() {

  // Se prende el LED rojo que indica que hay un agarre activo
  digitalWrite(LED5,HIGH);

  Serial.println("Agarre de pinza en ejecucion");
  int velocidad_dedos = 40; // Mientras se disminuya, los dedos cierran más rápido

  // Variables para los angulos de cada dedo
  int angulo_medio = 180;
  int angulo_menique_anular = 0;
  int angulo_pulgar = 180;

  // Indican el ángulo máximo al que debe llegar el motor para este agarre
  while (angulo_medio > 78 || angulo_menique_anular < 140 || angulo_pulgar > 165) {

    // Movimiento de la mitad de los dedos indice y medio hasta su angulo máximo. Es igual que en el while
    if (angulo_medio > 78) {

      // Se detecta la fuerza del sensor en el dedo anular
      lectura_fuerza_medio = analogRead(pin_fuerza_medio);
            
      // Se escribe el ángulo al motor, mientras el sensor del dedo no superen la presión establecida
      if (lectura_fuerza_medio <= 400 && estado_medio == 1) {
        indice_medio_mitad.write(angulo_medio);
      }

      // Se lee el angulo actual del motor
      angulo_actual_medio = indice_medio_mitad.read();

      // Si se supera la presión establecida, se frena el dedo 5 grados más de lo actual para apretar y ahí se mantienen
      if (lectura_fuerza_medio >= 400) {
        lectura_fuerza_medio = analogRead(pin_fuerza_medio); // Se lee la presión del sensor
        delay(15);
        indice_medio_mitad.write(angulo_actual_medio - 5);
        estado_medio = 0; // Se manda su estado a cero para que no siga aumentando el ángulo
      }

      delay(velocidad_dedos); // Delay para que de tiempo de sensar los dedos
      angulo_medio -= 2;

      // Si se llega al ángulo final del dedo, se frena
      if (angulo_medio <= 80) {
        estado_medio = 0;
      }
    }

    // Movimiento de la mitad de los dedos menique y anular hasta su angulo máximo. Es igual que en el while
    if (angulo_menique_anular < 140) {

      // Se detecta la fuerza del sensor en el dedo anular
      lectura_fuerza_menique_anular = analogRead(pin_fuerza_menique_anular);
            
      // Se escribe el ángulo al motor, mientras el sensor del dedo no superen la presión establecida
      if (lectura_fuerza_menique_anular <= 400 && estado_menique_anular == 1) {
        menique_anular_mitad.write(angulo_menique_anular);
      }

      // Se lee el angulo actual del motor
      angulo_actual_menique_anular_mitad = menique_anular_mitad.read();

      // Si se supera la presión establecida, se frena el dedo 5 grados más de lo actual para apretar y ahí se mantienen
      if (lectura_fuerza_menique_anular >= 400) {
        lectura_fuerza_menique_anular = analogRead(pin_fuerza_menique_anular); // Se lee la presión del sensor
        delay(15);
        menique_anular_mitad.write(angulo_actual_menique_anular_mitad + 5);
        estado_menique_anular = 0; // Se manda su estado a cero para que no siga aumentando el ángulo
      }

      delay(velocidad_dedos); // Delay para que de tiempo de sensar los dedos
      angulo_menique_anular += 2;

      // Si se llega al ángulo final del dedo, se frena
      if (angulo_menique_anular >= 138) {
        estado_menique_anular = 0;
      }
    }

    // Movimiento del pulgar hasta su angulo máximo. Es igual que en el while
    if (angulo_pulgar > 165) {

      // Se detecta la fuerza del sensor en el dedo anular
      lectura_fuerza_pulgar = analogRead(pin_fuerza_pulgar);
            
      // Se escribe el ángulo al motor, mientras el sensor del dedo no superen la presión establecida
      if (lectura_fuerza_pulgar <= 400 && estado_pulgar == 1) {
        pulgar.write(angulo_pulgar);
      }

      // Se lee el angulo actual del motor
      angulo_actual_pulgar = pulgar.read();

      // Si se supera la presión establecida, se frena el dedo 5 grados más de lo actual para apretar y ahí se mantienen
      if (lectura_fuerza_pulgar >= 400) {
        lectura_fuerza_pulgar = analogRead(pin_fuerza_pulgar); // Se lee la presión del sensor
        delay(15);
        pulgar.write(angulo_actual_pulgar - 5);
        estado_pulgar = 0; // Se manda su estado a cero para que no siga aumentando el ángulo
      }
            
      delay(velocidad_dedos); // Delay para que de tiempo de sensar los dedos
      angulo_pulgar -= 2;

      // Si se llega al ángulo final del dedo, se frena
      if (angulo_pulgar <= 144) {
        estado_pulgar = 0;
      }
    }
  }
  menique_anular.write(120);
}

void pinza2(){
  digitalWrite(LED5,HIGH);
  indice_medio_mitad.write(70);
  menique_anular_mitad.write(170);
  //menique_anular.write(150);
  pulgar.write(160);
}

// Función de agarre pinch
void pinch(){
  // Se prende el LED rojo que indica que hay un agarre activo
  digitalWrite(LED5,HIGH);
  Serial.println("Agarre pinch en ejecucion");

  pulgar.write(130);
  delay(1000);
  // Se mandan los motores a su posición para el agarre
  indice_medio_mitad.write(85);
  delay(1000); // Delay para que el pulgar vaya después
  medio.write(40);

}

// Función de agarre hook
void hook(){

  // Se prende el LED rojo que indica que hay un agarre activo
  digitalWrite(LED5,HIGH);
  Serial.println("Agarre hook en ejecucion");
  // Se mandan los motores a su posición para el agarre
  indice.write(100);
  medio.write(120);
  menique_anular.write(40);

}

// Función para abrir la mano
void openHand(){

  // Se apaga el LED rojo que indica que hay un agarre activo
  digitalWrite(LED5,LOW);

  // Se llevan los motores a su posición inicial y sus estados a 1
  indice.write(0);
  estado_indice = 1;

  medio.write(0);
  estado_medio = 1;

  menique_anular.write(180);
  estado_menique_anular = 1;

  menique_anular_mitad.write(0);
  estado_menique_anular_mitad = 1;

  pulgar.write(180);
  estado_pulgar = 1;

  indice_medio_mitad.write(180);

}

// Función para saludar
void saludo(){
  menique_anular_mitad.write(120);
  delay(90);
  indice_medio_mitad.write(100);
  delay(500);
  menique_anular_mitad.write(0);
  delay(90);
  indice_medio_mitad.write(180);
  delay(500);
  menique_anular_mitad.write(120);
  delay(90);
  indice_medio_mitad.write(100);
  delay(500);
  menique_anular_mitad.write(0);
  delay(90);
  indice_medio_mitad.write(180);
}

void setup(){

  Serial.begin(9600); // Empieza la comunicación serial en 9600

  // Pins para leds
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  // Se apaga el LED rojo que indica un agarre activo
  digitalWrite(LED5,LOW);

  indice.attach(pin_indice); // Se asigna el motor del dedo indice a su pin respectivo
  indice.write(0); // Se lleva el motor a su posición inicial con la mano abierta
  estado_indice = 1; // Se asigna 1 a su estado actual para poder moverse

  medio.attach(pin_medio); // Se asigna el motor del dedo medio a su pin respectivo
  medio.write(0); // Se lleva el motor a su posición inicial con la mano abierta
  estado_medio = 1; // Se asigna 1 a su estado actual para poder moverse

  menique_anular.attach(pin_menique_anular); // Se asigna el motor de los dedos meñique y anular a su pin respectivo
  menique_anular.write(180); // Se lleva el motor a su posición inicial con la mano abierta
  estado_menique_anular = 1; // Se asigna 1 a su estado actual para poder moverse

  pulgar.attach(pin_pulgar); // Se asigna el motor del dedo pulgar a su pin respectivo
  pulgar.write(180); // Se lleva el motor a su posición inicial con la mano abierta
  estado_pulgar = 1; // Se asigna 1 a su estado actual para poder moverse

  menique_anular_mitad.attach(pin_menique_anular_mitad); // Se asigna el motor de los dedos meñique y anular mitad a su pin respectivo
  menique_anular_mitad.write(0); // Se lleva el motor a su posición inicial con la mano abierta
  estado_menique_anular_mitad = 1; // Se asigna 1 a su estado actual para poder moverse

  indice_medio_mitad.attach(pin_indice_medio_mitad); // Se asigna el motor de los dedos indice y medio mitad a su pin respectivo
  indice_medio_mitad.write(180); // Se lleva el motor a su posición inicial con la mano abierta

  rot_pulgar.attach(pin_rot_pulgar); // Se asigna el motor de la rotación del pulgar a su pin respectivo
  rot_pulgar.write(10); // Se lleva el motor a su posición inicial con la mano abierta

  soplo_counter_sensor2 = 2; // Se asigna el valor de 2 para que empiece en posición abierta

  delay(2000);
  //saludo(); // Saludo inicial del robot

}

void loop() {

  // Variables para los sensores de presión barométrica
  float avg_val_1 = 0.0; // variable for averaging for sensor 1
  float avg_val_2 = 0.0; // variable for averaging for sensor 2
  
  // Read data from sensor 1
  for (int ii = 0; ii < avg_size; ii++) {
    avg_val_1 += MPS20N0040D_1.read(); // add multiple ADC readings
  }
  avg_val_1 /= avg_size;
  Serial.print("Sensor 1: ");
  Serial.println(avg_val_1, 0); // print out the average

  Serial.println(analogRead(pin_fuerza_indice));
  Serial.println(analogRead(pin_fuerza_medio));
  Serial.println(analogRead(pin_fuerza_menique_anular));
  Serial.println(analogRead(pin_fuerza_pulgar));

  // Se prende el LED del agarre convencional
  if (soplo_counter_sensor1 == 1){
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  }

  // Se prende el LED del agarre pinza
  if (soplo_counter_sensor1 == 2){
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  }

  // Se prende el LED del agarre pinch
  if (soplo_counter_sensor1 == 3){
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);
  }

  // Se prende el LED del agarre hook
  if (soplo_counter_sensor1 == 4){
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, HIGH);
  }
  
  // Check if sensor 1 reading crosses the threshold
  if (avg_val_1 >= 9000000 && !above_threshold1 && soplo_counter_sensor2 == 2) {
    above_threshold1 = true; // Set flag to indicate sensor reading is above threshold
    soplo_counter_sensor1++; // Increment the blow counter for sensor 1
    
    if (soplo_counter_sensor1 > 4) {
      soplo_counter_sensor1 = 1; // Reset counter if it exceeds 4
    }
    Serial.print("Soplo detectado en Sensor 1. Contador: ");
    Serial.println(soplo_counter_sensor1);
  }

  // Check if sensor 1 reading falls below the threshold
  if (avg_val_1 < 9000000 && above_threshold1) {
    above_threshold1 = false; // Reset flag
  }

  // Read data from sensor 2
  for (int ii = 0; ii < avg_size; ii++) {
    avg_val_2 += MPS20N0040D_2.read(); // add multiple ADC readings
  }

  avg_val_2 /= avg_size;
  Serial.print("Sensor 2: ");
  Serial.println(avg_val_2, 0); // print out the average

  // Check if sensor 2 reading crosses the threshold
  if (avg_val_2 >= 10000000 && !above_threshold2) {
    above_threshold2 = true; // Set flag to indicate sensor reading is above threshold
    soplo_counter_sensor2++; // Increment the blow counter for sensor 2
    if (soplo_counter_sensor2 > 2) {
      soplo_counter_sensor2 = 1; // Reset counter if it exceeds 2
    }
    Serial.print("Soplo detectado en Sensor 2. Contador: ");
    Serial.println(soplo_counter_sensor2);

    // Ejecutar función basada en el valor del contador del sensor 2
    if (soplo_counter_sensor2 == 1) {

      // Agarre convencional
      if (soplo_counter_sensor1 == 1){
        rot_pulgar.write(65);
        delay(2000);
        agarre();
      }

      // Agarre pinza
      else if (soplo_counter_sensor1 == 2){
        rot_pulgar.write(65);
        delay(2000);
        pinza2();
      }

      // Agarre pinch
      else if (soplo_counter_sensor1 == 3){
        rot_pulgar.write(60);
        delay(2000);
        pinch();
      }

      // Agarre hook
      else if (soplo_counter_sensor1 == 4){
        rot_pulgar.write(10);
        delay(2000);
        hook();
      }
    }

    else if (soplo_counter_sensor2 == 2){
      openHand();
    }
  }

   // Check if sensor 2 reading falls below the threshold
  if (avg_val_2 < 10000000 && above_threshold2) {
    above_threshold2 = false; // Reset flag
  }

  delay(50); // delay before next loop iteration, ajustar según sea necesario

}
