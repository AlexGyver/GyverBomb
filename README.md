# Бомба из Counter-Strike на Arduino

**В СВЯЗИ С СИТУАЦИЕЙ В ПИТЕРЕ, ВИДЕО НАХОДИТСЯ В ДОСТУПЕ ПО ССЫЛКЕ**  
https://youtu.be/5_hCx7RV0-E  

## Папки

**GyverBomb_libs** - папка с файлами библиотек, установить в C:\Program Files\Arduino\libraries\
  
**GyverBomb_v1.2** - прошивка для Ардуино
**GyverBomb_v1.3_keyboard_4x4** - прошивка для новой версии с клавиатурой 4х4 by **Егор Корнышов**

**ВНИМАНИЕ! ПУТЬ К ПАПКЕ СО СКЕТЧЕМ НЕ ДОЛЖЕН СОДЕРЖАТЬ РУССКИХ СИМВОЛОВ!
ВО ИЗБЕЖАНИЕ ПРОБЛЕМ ПОЛОЖИТЕ ПАПКУ В КОРЕНЬ ДИСКА С.**

### Схема подключения
![GyverBomb](https://github.com/AlexGyver/GyverBomb/blob/master/scheme.jpg)

## Нужная инфа
### Страница проекта на моём сайте
http://AlexGyver.ru/gyverbomb/

##  Инструкция по эксплуатации
Для смены/задания пароля СРАЗУ ПОСЛЕ ВКЛЮЧЕНИЯ нажать секретную кнопку. Включаем. * - ввод пароля. 
Сразу после ввода пойдёт обратный отсчёт (режим 1), либо установка таймера вручную (режим 0).
Для разминирования нажать # и ввести пароль. Пароль можно сбрасывать неограниченное число раз (хотя по факту не более 100 тысяч).

### Настройки в прошивке
* fuse_delay - задержка запала, в секундах (сколько секунд на запал будет подаваться ток)
* gamemode - режим игры (0 - установка времени вручную, 1 - предустановленное время)
* default_minutes - предустановленные минуты (для режима игры №1)
* default_seconds - предустановленные секунды (для режима игры №1)

### Статья первые шаги с Arduino
► http://alexgyver.ru/arduino-first/

##  Материалы и компоненты
Почти всё указанное ниже можно найти здесь:  
http://alexgyver.ru/arduino_shop/

* Arduino NANO 328p – искать
* https://ali.ski/tI7blh
* https://ali.ski/O4yTxb
* https://ali.ski/6_rFIS
* https://ali.ski/gb92E-
* Giant4 (Россия)
* Клавиатура https://ali.ski/-JGx-o
* Дисплей – искать
* https://ali.ski/8pQi9M
* https://ali.ski/-nBZs
* https://ali.ski/wumit
* Реле
* https://ali.ski/8U-AdI
* https://ali.ski/icYDvl
* Кнопка https://ali.ski/In2iuf
* Аккумулятор https://alexgyver.ru/18650/
* Батарейный отсек https://ali.ski/LXzk4N
* Зарядник + защита https://ali.ski/pspbG
* Повышайка https://ali.ski/Qqx8jI
* Выключатель https://ali.ski/Vdjvjt
* Усилитель
* https://ali.ski/lAGZr
* https://ali.ski/IwNrn
* Динамик https://ali.ski/r6757
* Паяльники, припой https://alexgyver.ru/all-for-soldering/
