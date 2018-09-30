//----------------НАСТРОЙКИ------------------
#define fuse_delay 3                                   // задержка запала, в секундах (сколько секунд на запал будет подаваться ток)

#define default_minutes 10                             // предустановленные минуты (для режима игры №1)
#define default_seconds 30                             // предустановленные секунды (для режима игры №1)
#define default_minutes2 1                             // предустановленные минуты (для режима игры №1)
#define default_seconds2 0                             // предустановленные секунды (для режима игры №1)
//----------------НАСТРОЙКИ------------------

#include "GyverTM1637.h"                               //библиотека дисплея
#include "Keypad.h"                                    //библиотека клавиатуры
#include <EEPROM.h>

//------ПОДКЛЮЧЕНИЕ-------
#define CLK 11
#define DIO 12
#define button_pin 3
#define speaker_pin 2
#define relay_pin A0
//------ПОДКЛЮЧЕНИЕ-------

boolean relay_state = 0;                               //1 для обратного реле, 0 для обычного
boolean dots_flag;
int pass;
byte FAIL[] = {_F, _A, _1, _L};                        //надписи
byte tire[] = {0x40, 0x40, 0x40, 0x40};
byte PASS[] = {_P, _A, _S, _S};
byte TEST[] = {_t, _E, _S, _t};
byte zero[] = {0, 0, 0, 0};
byte BOOM[] = {_d, _e, _a, _d};
byte LIVE[] = {_L, _1, _F, _E};
byte ENTER[] = {_E, _N, _t, _r};
byte G1[]   = {_empty, _G, _1, _empty};
byte G2[]   = {_empty, _G, _2, _empty};
byte G3[]   = {_empty, _G, _3, _empty};
boolean initial, initial2, pass_ent_f, time_ent_f, stopw_m_f, def_f, button_flag, gamemode;
byte charge_mode;
byte discharge_time = 5;
unsigned long last_time, def_time;
int minutes, seconds, milseconds;
int pass_int, time_int;
const byte ROWS = 4;                                  //4 строки у клавиатуры
const byte COLS = 4;                                  //4 столбца
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 4, 5, 7};                    //Подключены строки (4 пина)
byte colPins[COLS] = {8, 6, 13, 10};                  //подключены столбцы (4 пина)

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); //иниициализировать клавиатуру
GyverTM1637 disp(CLK, DIO);                           //иниициализировать дисплей disp

void setup() {
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(speaker_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, relay_state);
  digitalWrite(speaker_pin, 0);
  Serial.begin(9600);
  disp.brightness(7);                                 //яркость (0-7)
  keypad.setHoldTime(5000);
  pass = EEPROM_int_read(0);
}

void print_disp(int x[]) {                            //функция для удобной работы с дисплеем (на вход полаётся массив из 4 чисел, они выводятся на дисплей)
  for (int i = 0; i <= 3; i++) {
    disp.display(i, x[i]);                            //вывести на дисплей содержимое
  }
}
void change_pass() {
  disp.point(0);                                      //выключить двоеточие
  disp.displayByte(PASS);                             //выводим на дисплей PASS
  delay(500);
  disp.displayByte(tire);                             //выводим на дисплей ----
  String pass_str;
  for (int i = 0; i <= 3; i++) {                      //приём цифр с клавиатуры
    char key = keypad.waitForKey();
    disp.display(i, key - '0');                       //преобразование символов в цифры
    pass_str = pass_str + key;                        //цифры с клавиатуры принимаются строку (STRING)
  }
  pass_int = pass_str.toInt();                        //преобразование строки в 4х значное число (INT)
  pass = pass_int;

  EEPROM_int_write(0, pass);
}

void loop() {
  if (initial == 0) {                                 //инициализация, поднимается флажок на ввод пароля (пароль на включение)
    pass_ent_f = 1;
    initial = 1;
  }

  if (pass_ent_f == 1) {                              //проверка флага: ввод пароля на включение
    disp.displayByte(tire);
    if (digitalRead(button_pin) == 0 && button_flag == 0) {
      button_flag = 1;
    }
    if (digitalRead(button_pin) == 1 && button_flag == 1) {
      change_pass();
      button_flag = 0;
    }

    char key = keypad.getKey();
    if (key == 'A') {
      gamemode = 0;
      disp.displayByte(G1);                               //показываем что выбран режим 1
      delay(1000);
      pass_ent();                                         //ввод пароля на включение
    }
    if (key == 'B') {
      gamemode = 1;
      disp.displayByte(G2);                               //показываем что выбран режим 2
      delay(1000);
      pass_ent();                                         //ввод пароля на включение
    }
    if (key == 'C') {
      gamemode = 2;
      disp.displayByte(G3);                               //показываем что выбран режим 2
      delay(1000);
      pass_ent();                                         //ввод пароля на включение
    }
    if (key == '*') {                                     //Тестируем исправность реле
      disp.displayByte(TEST);
      boom();
      disp.displayByte(tire); 
    }
  }
    if (time_ent_f == 1) {                            //проверка флага: ввод времени таймера
      time_ent();                                     //ввод времени таймера
    time_ent_f = 0;                                   //опустить флаг
  }
  if (stopw_m_f == 1) {                               //проверка флага: обратный отсчёт
    stopw_m();                                        //обратный отсчёт
  }
  delay(3);
}

void pass_ent() {                                     //ввод пароля на включение
  disp.point(0);                                      //выключить двоеточие
  disp.displayByte(PASS);                             //выводим на дисплей PASS
  delay(500);
  disp.displayByte(tire);                             //выводим на дисплей ----
  String pass_str;
  for (int i = 0; i <= 3; i++) {                      //приём цифр с клавиатуры
    char key = keypad.waitForKey();
    disp.display(i, key - '0');                       //преобразование символов в цифры
    pass_str = pass_str + key;
        if (key == 'D') {                             //Планируется, что при нажатии D сбросятся введенные цифры
        disp.clear();
        disp.displayByte(tire);
    }                                                 //цифры с клавиатуры принимаются строку (STRING)
  }
  pass_int = pass_str.toInt();                        //преобразование строки в 4х значное число (INT)
  delay(500);

  if (pass_int == pass) {                             //если пароль совпал
    disp.displayByte(tire);
    pass_ent_f = 0;
    time_ent_f = 1;                                   //мигаем, флажочки опускаем    
  }
  else {                                              //не совпал
    disp.displayByte(FAIL);                           //выводим FAIL
    delay(500);
  }
}

void pass_ent_defuse() {                              //ввод пароля на разминирование
  disp.point(0);                                      //выключить двоеточие
  disp.displayByte(tire);                             //выводим на дисплей ----
  String pass_str;
  byte i = 0;
  while (i < 4) {
    get_time();
    disp.point(0);
    char key = keypad.getKey();
    if (key > 0) {
      disp.display(i, key - '0');                     //преобразование символов в цифры
      pass_str = pass_str + key;                      //цифры с клавиатуры принимаются строку (STRING)
      i++;
    } 
  }
  pass_int = pass_str.toInt();                        //преобразование строки в 4х значное число (INT)

  if (pass_int == pass) {                             //если пароль совпал
    charge_mode = 2;                                  //флажочек на обезвреживание
  }
  else {                                              //не совпал
    disp.displayByte(FAIL);                           //выводим FAIL
    delay(200);
  }
}

void time_ent() {
  if (gamemode == 0) {
    disp.point(1);                                    //включить двоеточие
    disp.displayByte(zero);
    delay(500);
    disp.displayByte(tire);                           //выводим на дисплей тире
    String time_str;
    for (int i = 0; i <= 3; i++) {
      char key = keypad.waitForKey();
      disp.display(i, key - '0');
      time_str = time_str + key;
    }
    minutes = (time_str[0] - '0') * 10 + (time_str[1] - '0'); //разбиение введенного числа на минуты и секунды
    seconds = (time_str[2] - '0') * 10 + (time_str[3] - '0');
  }
  else if (gamemode == 1) {
    minutes = default_minutes;
    seconds = default_seconds;
  }
  else if (gamemode == 2){                            //кто-нибудь, допишите сюда адекватный режим!!!
  }
  if (seconds > 59) {
    seconds = 59;
  }
  milseconds = seconds * 10;
  stopw_m_f = 1;
}

void stopw_m() {                                      //обратный отсчёт
  if (initial2 == 0) {
    last_time = millis();
    charge_mode = 0;
    initial2 = 1;
  }
  get_time();
  char pressedKey = keypad.getKey();
  if (pressedKey == '#'){
    pass_ent_defuse();
  }
  if (pressedKey == '*'){
    disp.point(0);
    disp.displayByte(BOOM); 
    boom();
    stopw_m_f = 0;
    pass_ent_f = 1;
    keypad.waitForKey();
    initial2 = 0;
  }

  switch (charge_mode) {
    case 0: disp.displayClockScroll(minutes, seconds, 25);
      break;
    case 1: disp.point(0); 
      disp.displayByte(BOOM); 
      boom();
      stopw_m_f = 0;
      pass_ent_f = 1;
      keypad.waitForKey();
      initial2 = 0;
      break;
    case 2: disp.point(0);
      disp.displayByte(LIVE);
      defuse();
      stopw_m_f = 0;
      pass_ent_f = 1;
      initial2 = 0;
      keypad.waitForKey();
      break;
  }
}

void get_time() {                                   //функция таймера обратного отсчёта
  if (millis() - last_time >= 500) {
    if (minutes == 0 && seconds <= 59) {            //если минут 0 и секунд меньше 15
      tone(speaker_pin, 4000, 100);                 //пищать!
    }
    last_time = millis();
    milseconds++;
    if (milseconds == 1) {
      disp.point(0);
    } else {
      disp.point(1);
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
      if (minutes <30 && minutes > 0) {              //если меньше 30 минут
        tone(speaker_pin, 4000, 100);                //пищать!
      }
    }
  }
}

                                                     // в случае взрыва
void boom() {
  digitalWrite(relay_pin, !relay_state);             //дать ток на реле
  tone(speaker_pin, 50, 1000);                       //ОРАТЬ типо БЩЩЩЩ (звук взрыва)
  delay((int)fuse_delay * 1000);                     //задержка (чтобы запал прогрелся)
  digitalWrite(relay_pin, relay_state);              //убрать ток с реле
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

