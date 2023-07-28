#include "check_sys.h"

WhatchDog wd;

void setup() {
  Serial.begin(9600);
  wd.start_setup();
//  wd.zero_all_eeprom();    //обнуление всех нужных энергонезависимых переменных

  wd.whatch_eeprom();    //выводим энергонезависимые переменные

}

void loop(){
  wd.start_loop();
  wd.prt_countBlock();
  delay(4000);
  for(int i=0; i<20; i++)
  { 
    if(wd.check_block()){
      delay(1000);
      if(i==0)
        while(1);
      if(i==3)
        while(1);
      if(i==4)
        while(1);        
      if(i==14)
        while(1);
      Serial.println(i);
    }
  }
}