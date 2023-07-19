 #include "check_sistem.h"

void setup() {
  //zero_all_eeprom();    //обнуление всех нужных энергонезависимых переменных
  start_whatchdog();  //инициализируем whatch_dog timer
  Serial.begin(9600);
  check_start();      //проверяем системы до перезапуска
  whatch_eeprom();    //выводим энергонезависимые переменные
  delay(4000);
  wdt_reset();        //обнуляем таймер whatch_dog
}

void loop(){
  start_loop();       //запуск цикла
  delay(4000);
  for(int i=0; i<20; i++)
  { 
    if(check_block(i)){ //проверяем, заблокирован ли блок
      delay(1000);
      if(i==3)
        while(1);
      end_block(i);     //окончание блока
      Serial.println(i);
    }
  }
}
