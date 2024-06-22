# Система автоматического отключения неисправных блоков программы микроконтроллера

### Содержание
1. [Описание логики работы алгоритма](#Описание-логики-работы-алгоритма)
2. [Проверка перед использованием](#проверка-перед-использованием-библиотеки)
3. [Строение](#строение)
   * [Принцип контроля блоков](#принцип-контроля-блоков)
   * [Определение и исключение неисправных блоков](#определение-и-исключение-неисправных-блоков)
   * [Проверка текущего блока](#проверка-текущего-блока)
4. [Принцип работы](#принцип-работы)
   * [подробный разбор check_sysino](#подробный-разбор-check_sysino)
   * [минимальный скетч с библиотекой](#минимальный-скетч-с-библиотекой)
5. [Пример использования](#пример-использования)
___

## Описание логики работы алгоритма
****написать-объяснить****
__

## Проверка перед использованием библиотеки


Перед использованием библиотеки запустите скетч из папки [check_whatchdog_and_eeprom](https://github.com/bezzzubik/whatch_dog_lib/tree/main/check_whatcdog_and_eeprom/main), чтобы проверить, поддерживает ли ваш микроконтроллер whatchdog и eeprom.
___
*   ПРИМЕЧАНИЕ: После проверки требуется поменять ``#define REP 0`` на ``1`` и запустить, чтобы энергонезависимые переменные вернулись в рабочее состояние.
___
Возможные выводы:
___
*   ```
    Проверка сброшена
    Поменяйте REP на 0 и загрузите скетч повторно
    ```
    Требуется поменять в строке 4 ``#define REP 1`` на ``0`` и запустится проверка.
___
*   ```
    Микроконтроллер поддерживает whatchdog
    ```
    Требуется сбросить проверку. То есть поменять ``#define REP 0`` на ``1``, запустить, увидеть вывод с прошлого пункта и сменить обратно.
___
*   ```
    Проверка энергонезависимых переменных...
    Микроконтроллер поддерживает энергонезависимые переменные
    Проверка whatchdog
    Подождите 8 секунд...
    8
    7
    6
    5
    4
    Микроконтроллер поддерживает whatchdog
    ```
    Все системы поддерживаются, библиотеку можно использовать.
___
*   ```
    Проверка энергонезависимых переменных...
    Микроконтроллер не поддерживает энергонезависимые переменные
    ```
    Микроконтроллер не поддерживает энергонезависимые переменные. Библиотека не может быть использована.
___
*   ```
    Проверка энергонезависимых переменных...
    Микроконтроллер поддерживает энергонезависимые переменные
    Проверка whatchdog
    Подождите 8 секунд...
    8
    7
    6
    5
    4
    3
    2
    1
    Микроконтроллер не поддерживает whatchdog
    ```
    Микроконтроллер не поддерживает whatchdog timer. В качестве заменителя можно использовать [внешние модули](https://wiki.iarduino.ru/page/storozhevoy-taymer/). Работа библиотеки и проверка на них пока не поддерживается.
___
<br>


## Строение
Библиотека использует стандартные библиотеки arduino ``eeprom.h`` и ``wdt.h``.

Основана на классе WhatchDog. Состав класса:
```C++
class WhatchDog
{
  int countBlock;
  int nowBlock=0;
  void zero_all_blocks();
  void check_start();
	public:
    void prt_eeprom();
    void prt_eeprom(int);
  	WhatchDog(int,int);
    bool check_block();
    void start_loop();
    void zero_all_eeprom();
    void prt_countBlock();
    void change_eeprom(int, int);
    void change_eeprom(int, int, int);
    bool block_status(int);
    void off_block(int);
    void on_block(int);
    void off_wd();
  	~WhatchDog();
};
```
Разбор методов и переменных класса:
___
* ```int countBlock;```

  Переменная хранит количество блоков, которые используются в программе. Инициализируется в конструкторе WhatchDog, подробнее будет рассмотрено далее.
___
* ```int nowBlock;```

  Переменная хранит номер текущего блока.
___
* ```WhatchDog(int, int)```
  
  Конструктор класса. Подробный код:
    ```C++
    WhatchDog::WhatchDog(int cB=50, int tim=8)
    {
        nowBlock=0;
        countBlock=cB;
        wdt_disable();
        check_start();
        switch (tim)
        {
            case 15:
                wdt_enable (WDTO_15MS);
                break;
            case 30:
                wdt_enable (WDTO_30MS);
                break;
            case 60:
                wdt_enable (WDTO_60MS);
                break;
            case 120:
                wdt_enable (WDTO_120MS);
                break;
            case 250:
                wdt_enable (WDTO_250MS);
                break;
            case 500:
                wdt_enable (WDTO_500MS);
                break;
            case 1:
                wdt_enable (WDTO_1S);
                break;
            case 2:
                wdt_enable (WDTO_2S);
                break;
            case 4:
                wdt_enable (WDTO_4S);
                break;
            default:
                wdt_enable (WDTO_8S);
                break;
        }
    }
    ```
    Конструктор принимает 2 переменные: ``int cB`` и ``int tim``.
    ___
    Переменная ``int cB`` принимает количество блоков в основной программе. Максимальное записанное значение - 50. После полного прохождения  ````void loop()```` объект класса сам определит количество блоков в программе.
    ___
    Переменная ``int tim`` определяет время wd timer, после истечения которого микроконтроллер перезагрузится, поэтому выбирайте это время с расчетом на то, что за это время должен выполнится самый долгий по времени выполнения блок.
     
    Возможные значения переменной: 
    
    - 1, 2, 4, 8 - установка соответствующего времени в секундах;
    - 15, 30, 60, 120, 250, 500 - установка соответствующего времени в миллисекундах.
___
* ``~WhatchDog()``

  Деструктор класса. Практического применения в данный момент нет.
___
* ```void prt_countBlock()```
  
  Метод, который выводит переменную ``int countBlock`` (кол-во блоков)
___
* ``void prt_eeprom()``
  
  Метод выводит значение энергонезависимых переменных. Существует её одноименный аналог, который принимает номер энергонезависимой переменной ``void prt_eeprom(int)``, которую нужно вывести. Применяется для проверки работы библиотеки.
___
* ``bool check_block()``

  Метод возвращает ``True``, если текущий блок не заблокирован, иначе, если в блоке в процессе выполнения программы была обнаружена ошибка и блок был заблокирован, вернет ``False``.
___
* ``void start_loop()``

  Ставится в начале функции ``void loop()``. Обнуляет энергонезависимые переменные, определяет количество блоков программы по текущему значению блока и обнуляет переменную текущего блока.
___
* ``void zero_all_eeprom()``

  Функция обнуляет все энергонезависимые переменные. Требуется для корректной работы библиотеки при перезапуске или первом запуске.
  <br>
  Такой же функционал реализован в скетче ``zero_eeprom()``
___
* ``void zero_all_blocks()``

  Приватный метод класса. Обнуляет энергонезависимые переменные блоков.
___

* ``void change_eeprom(int numVar, int var)``

  Метод принимает на вход номер переменной, которую нужно изменить ``numVar``, и значение, которое нужно установить ``var``.
___

* ``void change_eeprom(int start, int end, int var)``

  Метод принимает на вход интервал энергонезависимых переменных ``[start, end)``, и значение, которое нужно там установить ``var``.
___
* ``bool block_status(int i)``

  Возвращает ``False``, если блок выключен, и ``True``, если блок включен. Не распространяется на блоки, которые не задействованы в программе.
___
* ``void off_block(int)``

  Выключает блок, номер которого передается в функцию. Выключение происходит путем изменения соответствующей номеру блока энергонезависимой переменной на 2.
___
* ``void on_block(int)``

  Включает блок, номер которого передается в функцию. Включение происходит путем изменения соответствующей номеру блока энергонезависимой переменной на 0.
___
* ``void off_wd()``

  Отключает стартовую проверку ``check_start()`` и проверку блоков ``check_block()``.
___

## Принцип работы
### Принцип контроля блоков
Принцип контроля блоков основан на значениях энергонезависимых переменных. Соответвтвия:

* ``0`` - Блок еще не отработал.

* ``1`` - Блок отработал.

* ``2`` - Блок отключен.

В начале выполнения программы требуется обнулить все энергонезависимые переменные, т.е. вызвать перед запуском метод ``void zero_all_eeprom()``. Тогда все энергонезависимые переменные будут обнулены.

Нулевая энергонезависимые переменная предназначена для инициализации библиотеки. Она определяет, что произошла инициализация ``wdt`` и работа библиотеки начата. Нумерация блоков начинается с 1.
___
### Определение и исключение неисправных блоков

Если происходит перезапуск, метод ``void check_start()``, вызываемый в конструкторе начинает искать среди энергонезависимых переменных 1. Если программа уже работала, нулевая энергонезависимая переменная будет равна единице. Если она равна 0, проверка заканчивается. Если же равна 1, значит во время работы одного из блоков произошла неисправность. Так как неисправный блок будет иметь значение энергонезависимой переменной 0, то первый блок со значением 0 будет неисправным. Далее идет поиск этого блока и значение меняется с 0 на 2.

___
### Проверка текущего блока

Проверка текущего блока происходит в функции ``bool check_block()``. Код метода:
```C++
bool WhatchDog::check_block()
{
  if ( !eeprom_read_byte(this->nowBlock) )
         eeprom_update_byte(this->nowBlock, 1);
  wdt_reset();
  this->nowBlock++;
  if ( eeprom_read_byte(this->nowBlock))
     return false;
  else
     return true; 
}
```
 Внутри него меняется значение энергонезависимой переменной отработанного блока с 0 на 1. (именно с 0 на 1, потому что если блок имеет значение 1 или 2, изменения не происходит). Далее обнуляется сторожевой таймер (wdt) и увеличивается значение текущего блока (то есть переходим к проверке уже текущего блока). Если значение равно 0 - возвращается ``true``, в противном случае возвращается ``false``.


## Пример использования

Приме использования библиотеки расположен в скетче ``check_sys.ino``.

### Подробный разбор ``check_sys.ino``

Сначала инициализируем объект класса WhatchDog как глобальную переменную.

```C++
WhatchDog wd;
```
По умолчанию начальное количество блоков в программе - 50, таймер проверки - 8 секунд.

В ``void setup()`` для начальной работы не требуется никаких дополнительных функций.

В ``void loop()`` необходимо первым методом поставить ``wd.start_loop()``.

Далее нужно обернуть выполняемый блок кода в конструкцию:

```C++
if(wd.check_block())
{
  //блок исполняемого кода
}
```

### Минимальный скетч с библиотекой

```C++
#include "check_sys.h"

WhatchDog wd;

void setup() {
    //начальная инициализация ваших датчиков и пинов
}

void loop(){
  wd.start_loop();
  if(wd.check_block())
  {
  //Первый блок выполнения кода(опрос первого датчика)
  }
  if(wd.check_block())
  {
  //Второй блок выполнения кода(опрос второго датчика)
  }
  
  .....
  
  if(wd.check_block())
  {
  //N-ый блок выполнения кода(опрос N-го датчика)
  }
}

```




## Требуемые доработки
- Добавить возможность отключения работы библиотеки или добавить функцию, которая в setup отключала бы все
- Дописать readme, добавить объяснение логики и алгоритма
