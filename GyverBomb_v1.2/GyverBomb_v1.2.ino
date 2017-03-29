/*
  Created 2017
  by AlexGyver
  AlexGyver Home Labs Inc.
  Имитация бомбы из CS, с паролем, таймером, пищалкой и электрозапалом.
  Для смены/задания пароля СРАЗУ ПОСЛЕ ВКЛЮЧЕНИЯ нажать секретную кнопку.
  Включаем. * - ввод пароля. Сразу после ввода пойдёт обратный отсчёт (режим 1), либо
  установка таймера вручную (режим 0). Для разминирования нажать # и ввести пароль
  Пароль можно сбрасывать неограниченное число раз (хотя по факту не более 100 тысяч).
*/
//----------------НАСТРОЙКИ------------------
#define fuse_delay 3  // задержка запала, в секундах (сколько секунд на запал будет подаваться ток)
#define gamemode 0    // режим игры (0 - установка времени вручную, 1 - предустановленное время)

#define default_minutes 10 // предустановленные минуты (для режима игры №1)
#define default_seconds 30 // предустановленные секунды (для режима игры №1)
//----------------НАСТРОЙКИ------------------

#include "TM1637.h"  //библиотека дисплея
#include "Keypad.h"  //библиотека клавиатуры
#include <EEPROM.h>

//------ПОДКЛЮЧЕНИЕ-------
#define CLK 11
#define DIO 12
#define button_pin 3
#define speaker_pin 2
#define relay_pin A0
//------ПОДКЛЮЧЕНИЕ-------

boolean relay_state = 0;        //1 для обратного реле, 0 для обычного
boolean dots_flag;
int pass;
int FAIL[] = {15, 10, 1, 16};   //надпись FAIL, для надписи ЕГОГ {14,19,0,19}
int tire[] = {17, 17, 17, 17};  //надпись ----
int PASS[] = {19, 10, 5, 5};    //надпись PASS
int zero[] = {0, 0, 0, 0};
int BOOM[] = {8, 8, 8, 8};
int DEFUS[] = {0, 14, 15, 1};
boolean initial, initial2, pass_ent_f, time_ent_f, stopw_m_f, def_f, button_flag;
byte charge_mode;
byte discharge_time = 5;
unsigned long last_time, def_time;
int minutes, seconds, milseconds;
int pass_int, time_int;
const byte ROWS = 4; //4 строки у клавиатуры
const byte COLS = 3; //три столбца
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {9, 4, 5, 7};             //Подключены строки (4 пина)
byte colPins[COLS] = {8, 10, 6};               //подключены столбцы (3 пина)

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); //иниициализировать клавиатуру
TM1637 disp(CLK, DIO);                                                    //иниициализировать дисплей disp

void setup() {
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(speaker_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, relay_state);
  digitalWrite(speaker_pin, 0);
  Serial.begin(9600);
  disp.init();  //инициализация дисплея
  disp.set(7);  //яркость (0-7)
  keypad.setHoldTime(5000);
  pass = EEPROM_int_read(0);
}

void print_disp(int x[]) {    //функция для удобной работы с дисплеем (на вход полаётся массив из 4 чисел, они выводятся на дисплей)
  for (int i = 0; i <= 3; i++) {
    disp.display(i, x[i]);    //вывести на дисплей содержимое
  }
}
void change_pass() {
  disp.point(POINT_OFF);     //выключить двоеточие
  print_disp(PASS);          //выводим на дисплей PASS
  delay(500);
  print_disp(tire);     //выводим на дисплей ----
  String pass_str;
  for (int i = 0; i <= 3; i++) { //приём цифр с клавиатуры
    char key = keypad.waitForKey();
    disp.display(i, key - '0'); //преобразование символов в цифры
    pass_str = pass_str + key; //цифры с клавиатуры принимаются строку (STRING)
  }
  pass_int = pass_str.toInt(); //преобразование строки в 4х значное число (INT)
  pass = pass_int;

  EEPROM_int_write(0, pass);
}

void loop() {
  if (initial == 0) { //инициализация, поднимается флажок на ввод пароля (пароль на включение)
    pass_ent_f = 1;
    initial = 1;
  }

  if (pass_ent_f == 1) {   //проверка флага: ввод пароля на включение
    print_disp(tire);
    if (digitalRead(button_pin) == 0 && button_flag == 0) {
      button_flag = 1;
    }
    if (digitalRead(button_pin) == 1 && button_flag == 1) {
      change_pass();
      button_flag = 0;
    }

    char key = keypad.getKey();
    if (key == '*') {
      pass_ent();          //ввод пароля на включение
    }
  }
  if (time_ent_f == 1) {   //проверка флага: ввод времени таймера
    time_ent();            //ввод времени таймера
    time_ent_f = 0;        //опустить флаг
  }
  if (stopw_m_f == 1) {    //проверка флага: обратный отсчёт
    stopw_m();             //обратный отсчёт
  }
  delay(3);
}

void pass_ent() {            //ввод пароля на включение
  disp.point(POINT_OFF);     //выключить двоеточие
  print_disp(PASS);          //выводим на дисплей PASS
  delay(500);
  print_disp(tire);          //выводим на дисплей ----
  String pass_str;
  for (int i = 0; i <= 3; i++) {    //приём цифр с клавиатуры
    char key = keypad.waitForKey();
    disp.display(i, key - '0');     //преобразование символов в цифры
    pass_str = pass_str + key;      //цифры с клавиатуры принимаются строку (STRING)
  }
  pass_int = pass_str.toInt(); //преобразование строки в 4х значное число (INT)
  delay(500);

  if (pass_int == pass) {   //если пароль совпал
    print_disp(tire);
    pass_ent_f = 0;
    time_ent_f = 1;         //мигаем, флажочки опускаем    
  }
  else {                //не совпал
    print_disp(FAIL);   //выводим FAIL
    delay(500);
  }
}

void pass_ent_defuse() {     //ввод пароля на разминирование
  disp.point(POINT_OFF);     //выключить двоеточие
  print_disp(tire);          //выводим на дисплей ----
  String pass_str;
  byte i = 0;
  while (i < 4) {
    get_time();
    disp.point(POINT_OFF);
    char key = keypad.getKey();
    if (key > 0) {
      disp.display(i, key - '0'); //преобразование символов в цифры
      pass_str = pass_str + key; //цифры с клавиатуры принимаются строку (STRING)
      i++;
    }
  }
  pass_int = pass_str.toInt(); //преобразование строки в 4х значное число (INT)

  if (pass_int == pass) {                   //если пароль совпал
    charge_mode = 2; //флажочек на обезвреживание
  }
  else {          //не совпал
    print_disp(FAIL);   //выводим FAIL
    delay(200);
  }
}

void time_ent() {
  if (gamemode == 0) {
    disp.point(POINT_ON);     //включить двоеточие
    print_disp(zero);
    delay(500);
    print_disp(tire);   //выводим на дисплей тире
    String time_str;
    for (int i = 0; i <= 3; i++) {
      char key = keypad.waitForKey();
      disp.display(i, key - '0');
      time_str = time_str + key;
    }
    minutes = (time_str[0] - '0') * 10 + (time_str[1] - '0'); //разбиение введенного числа на минуты и секунды
    seconds = (time_str[2] - '0') * 10 + (time_str[3] - '0');
  }
  if (gamemode == 1) {
    minutes = default_minutes;
    seconds = default_seconds;
  }
  if (seconds > 59) {
    seconds = 59;
  }
  milseconds = seconds * 10;
  stopw_m_f = 1;
}

void stopw_m() {  //обратный отсчёт
  if (initial2 == 0) {
    last_time = millis();
    charge_mode = 0;
    initial2 = 1;
  }
  get_time();
  if (keypad.getKey() == '#') {
    pass_ent_defuse();
  }

  switch (charge_mode) {
    case 0: disp.display(0, minutes / 10);
      disp.display(1, minutes % 10);
      disp.display(2, seconds / 10);
      disp.display(3, seconds % 10);
      break;
    case 1: disp.point(POINT_OFF);
      print_disp(BOOM); boom();
      stopw_m_f = 0;
      pass_ent_f = 1;
      keypad.waitForKey();
      initial2 = 0;
      break;
    case 2: disp.point(POINT_OFF);
      print_disp(DEFUS);
      defuse();
      disp.point(POINT_OFF);
      stopw_m_f = 0;
      pass_ent_f = 1;
      initial2 = 0;
      keypad.waitForKey();
      break;
  }
}

void get_time() {     //функция таймера обратного отсчёта
  if (millis() - last_time >= 500) {
    if (minutes == 0 && seconds <= 15) { //если минут 0 и секунд меньше 15
      tone(speaker_pin, 4000, 100);   //пищать!
    }
    last_time = millis();
    milseconds++;
    if (milseconds == 1) {
      disp.point(POINT_OFF);
    } else {
      disp.point(POINT_ON);
    }
    if (milseconds >= 2) {
      milseconds = 0;
      seconds--;
      if (minutes == 0 && seconds < 0) {
        charge_mode = 1;
      }
      if (seconds < 0) {
        seconds = 59;
        minutes--;
      }
      if (minutes == 0 && seconds < 30 && seconds > 15) { //если минут 0 и секунд меньше от 30 до 15
        tone(speaker_pin, 4000, 100);   //пищать!
      }
    }
  }
}

// в случае взрыва
void boom() {
  digitalWrite(relay_pin, !relay_state);   //дать ток на реле
  tone(speaker_pin, 50, 1000);             // ОРАТЬ типо БЩЩЩЩ (звук взрыва)
  delay((int)fuse_delay * 1000);           //задержка (чтобы запал прогрелся)
  digitalWrite(relay_pin, relay_state);    //убрать ток с реле
}

// в случае обезвреживания, играть победное ПИП ПИП ПИП
void defuse() {
  tone(speaker_pin, 4000, 100);
  delay(200);
  tone(speaker_pin, 4000, 100);
  delay(200);
  tone(speaker_pin, 4000, 100);
  delay(200);
}

// чтение EEPROM
int EEPROM_int_read(int addr) {
  byte raw[2];
  for (byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr + i);
  int &num = (int&)raw;
  return num;
}

// запись EEPROM
void EEPROM_int_write(int addr, int num) {
  byte raw[2];
  (int&)raw = num;
  for (byte i = 0; i < 2; i++) EEPROM.write(addr + i, raw[i]);
}

