/*Блок сопряжения БН-3
Вход:
 - дискретные каналы (XX)
 - аналоговые каналы (XX)
Выход:
 - RS-485 последовательный порт ModbusRTU (19200)
*/
#include "ModbusRtu.h"


#define BAUD      19200 //Скорость последовательного порта
#define MBADDR    1     //MODBUS Адрес (0-127)
#define TXEN      10    //RS485 RE-DE
#define LED       13    //Светоиод на Arduino

//uint16_t mbdata[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//для поиска тега раскомментировать тут и в программе область заполнения таблицы регистров
//тогда значения в тегах будут 1,2,3,4 и таким образом можно определить куда "упал" какой канал
uint16_t mbdata[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

int8_t state = 0;   //Статус MB
unsigned long tus;  //Счетчик millis

Modbus slave(MBADDR, 0, TXEN);

void setup() {
  pinMode(LED, OUTPUT);  
  slave.begin(BAUD);

  //Высвечиваем диодом MB адрес устройства
  for (int i=0; i<MBADDR; i++){
    digitalWrite(LED, HIGH); delay(200);
    digitalWrite(LED, LOW); delay(200);
  }
}

void loop() {
  state = slave.poll( mbdata, 16 ); //Обработчик MODBUS

  //Заполнение таблицы регистров
  /*
  mbdata[0] = analogRead( A0 ); //Считывание ADC0 в MB массив
  mbdata[1] = analogRead( A1 ); //Считывание ADC1 в MB массив
  mbdata[2] = analogRead( A2 ); //Считывание ADC2 в MB массив
  mbdata[3] = analogRead( A3 ); //Считывание ADC3 в MB массив
  mbdata[4] = analogRead( A4 ); //Считывание ADC4 в MB массив
  mbdata[5] = analogRead( A5 ); //Считывание ADC5 в MB массив
  mbdata[6] = analogRead( A6 ); //Считывание ADC6 в MB массив
  mbdata[7] = analogRead( A7 ); //Считывание ADC7 в MB массив
  */
  mbdata[13] = slave.getInCnt();   //Диагностика подключения MODBUS
  mbdata[14] = slave.getOutCnt();
  mbdata[15] = slave.getErrCnt();
  
  //Высвечиваем диодом статус modbus - пакет получен без ошибок
  if (state > 4) {
    tus = millis() + 100;
    digitalWrite(LED, HIGH);
  }
  if (millis() > tus) digitalWrite(LED, LOW);
}
