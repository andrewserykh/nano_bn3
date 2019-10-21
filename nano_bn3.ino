/*Блок сопряжения БН-3 (v1.2)
Вход:
 - дискретные каналы (D2, D3, D4, D5) = управляется землей
 - аналоговые каналы (A0, A1, A2, A3) = 4..20 mA
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

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  
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
  mbdata[0] = int_digitalRead(2); //Считывание D2 в MB массив
  mbdata[1] = int_digitalRead(3); //Считывание D3 в MB массив
  mbdata[2] = int_digitalRead(4); //Считывание D4 в MB массив
  mbdata[3] = int_digitalRead(5); //Считывание D5 в MB массив

  mbdata[4] = map( int_analogReadMedian (A0,5), 204, 1023, 0, 255 ); //Считывание ADC0 в MB массив
  mbdata[5] = map( int_analogReadMedian (A1,5), 204, 1023, 0, 255 ); //Считывание ADC1 в MB массив
  mbdata[6] = map( int_analogReadMedian (A2,5), 204, 1023, 0, 255 ); //Считывание ADC2 в MB массив
  mbdata[7] = map( int_analogReadMedian (A3,5), 204, 1023, 0, 255 ); //Считывание ADC3 в MB массив

  mbdata[8] = 0;
  mbdata[9] = 0;
  mbdata[10] = 0;
  mbdata[11] = 0;
  mbdata[12] = 0;
   
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

int int_digitalRead(int pin){
  if ( digitalRead(pin)==HIGH) {
    return 0;  
  } else {
    return 1;
  }
  return 0;
}

//получить среднее значение из АЦП
int int_analogReadM(int pin, int samples){
  // переменная для хранения суммы считанных значений
  int sum = 0;
  // чтение и складывание значений
  for (int i = 0; i < samples; i++){
    sum = sum + analogRead(pin);
  }
  // делим сумму значений на количество измерений
  sum = sum/samples;
  // возвращаем среднее значение
  return sum;
}

//получить медианное значение из АЦП
int int_analogReadMedian (int pin, int samples){
  // массив для хранения данных
  int raw[samples];
  // считываем вход и помещаем величину в ячейки массива
  for (int i = 0; i < samples; i++){
    raw[i] = analogRead(pin);
  }
  // сортируем массив по возрастанию значений в ячейках
  int temp = 0; // временная переменная

  for (int i = 0; i < samples; i++){
    for (int j = 0; j < samples - 1; j++){
      if (raw[j] > raw[j + 1]){
        temp = raw[j];
        raw[j] = raw[j + 1];
        raw[j + 1] = temp;
      }
    }
  }
  // возвращаем значение средней ячейки массива
  return raw[samples/2];
}
