#include <Arduino_FreeRTOS.h>
#include <Arduino.h>

void TareaLecturaLuz(void *pvParameters);
void TareaEnvioSerial(void *pvParameters);
void TareaAlarma(void *pvParameters);
//void TareaWeb(void *pvParameters);
void TareaLedLectura(void *pvParameters);
void TareaLedAlarma(void *pvParameters);
//
void ISR_BotonLectura();
//void ISR_BotonAlarma();
//
volatile bool lecturaActivada = true;
volatile int ultimoValorLuz = 0;
volatile bool alarmaActivada = false;
volatile int valorPinLedAlarma = LOW;

const int pinLuz = A3;
const int pinLedLectura = 11;
const int pinLedAlarma = 12;
const int pinPul1 = 2;
const int pinPul2 = 3;

TaskHandle_t xHandleTareaLecturaLuz;
TaskHandle_t xHandleTareaEnvioSerial;
TaskHandle_t xHandleTareaAlarma;
TaskHandle_t xHandleTareaLedLectura;
TaskHandle_t xHandleTareaLedAlarma;

void setup() {
  
  pinMode(pinLuz, INPUT);
  pinMode(pinLedLectura, OUTPUT);
  pinMode(pinLedAlarma, OUTPUT);

  pinMode(pinPul1, INPUT);
  pinMode(pinPul2, INPUT);

  attachInterrupt(digitalPinToInterrupt(pinPul1), ISR_BotonLectura, RISING);
  attachInterrupt(digitalPinToInterrupt(pinPul2), ISR_BotonLectura, RISING);
  //attachInterrupt(digitalPinToInterrupt(pinPul2), ISR_BotonAlarma, RISING);  

  xTaskCreate(TareaLecturaLuz, "LecturaLuz", 128, NULL, 3, &xHandleTareaLecturaLuz);
  xTaskCreate(TareaEnvioSerial, "EnvioSerial", 128, NULL, 3, &xHandleTareaEnvioSerial);
  xTaskCreate(TareaAlarma, "Alarma", 128, NULL, 3, &xHandleTareaAlarma);
  //xTaskCreate(TareaWeb, "Web", 256, NULL, 4, NULL);
  xTaskCreate(TareaLedAlarma, "LedAlarma", 128, NULL, 3, &xHandleTareaLedAlarma);
  xTaskCreate(TareaLedLectura, "LedLectura", 128, NULL, 3, &xHandleTareaLedLectura);

  Serial.begin(9600);

  vTaskStartScheduler();
}

void loop() {
  
}

void TareaLecturaLuz(void *pvParameters) {
  while (true) {
    if (lecturaActivada) {
      taskENTER_CRITICAL();
      ultimoValorLuz = analogRead(pinLuz);
      taskEXIT_CRITICAL();
    }
    // aseguramos que se bloquee para que ejecute otra tarea
    vTaskDelay(10 / portTICK_PERIOD_MS); // 1000 / portTICK_PERIOD_MS = 1s
  }
}

void TareaEnvioSerial(void *pvParameters) {
  while (true) {
    if (lecturaActivada) {
      taskENTER_CRITICAL();
      Serial.println(ultimoValorLuz);
      taskEXIT_CRITICAL();
    }
    // bloqueo por 3s
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void TareaAlarma(void *pvParameters) {
  while (true) {
    if (lecturaActivada && ultimoValorLuz > 800) {
      alarmaActivada = true;
    } else {
      alarmaActivada = false;
      // deactivate pin 12
      valorPinLedAlarma = LOW;
      digitalWrite(pinLedAlarma, valorPinLedAlarma);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void TareaLedAlarma(void *pvParameters) {
  while (true) {
    if (alarmaActivada) {
      valorPinLedAlarma = 1 - valorPinLedAlarma;
      digitalWrite(pinLedAlarma, valorPinLedAlarma);
      taskENTER_CRITICAL();
      Serial.println("WAR:Intensidad luminosa supera los 800");
      taskEXIT_CRITICAL();
    }
    // bloqueo por 0.5s
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void TareaLedLectura(void *pvParameters) {
  while (true) {
    if (lecturaActivada) {
      taskENTER_CRITICAL();
      digitalWrite(pinLedLectura, HIGH);
      delay(1);
      digitalWrite(pinLedLectura, LOW);
      taskEXIT_CRITICAL();
    }
    // bloqueo por 1s
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void ISR_BotonLectura() {
  // chequear si otras tareas necesitan ser suspendidas tambien o no
  if (lecturaActivada) {
    taskENTER_CRITICAL();
    lecturaActivada = false;
    vTaskSuspend(xHandleTareaLecturaLuz);
    taskEXIT_CRITICAL();
    //vTaskSuspend(xHandleTareaEnvioSerial);
    //vTaskSuspend(xHandleTareaAlarma);
    //vTaskSuspend(xHandleTareaLedAlarma);
  }
  else {
    taskENTER_CRITICAL();
    lecturaActivada = true;
    vTaskResume(xHandleTareaLecturaLuz);
    taskEXIT_CRITICAL(); 
    //vTaskResume(xHandleTareaEnvioSerial);
    //vTaskResume(xHandleTareaAlarma);
    //vTaskResume(xHandleTareaLedAlarma);
  }
}
//
//void TareaWeb(void *pvParameters) {
//
//}
