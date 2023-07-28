#include <avr/eeprom.h>
#include <avr/wdt.h>


class WhatchDog
{
  int countBlock;
	int nowBlock=0;
	void zero_all_blocks();
  void check_start();
	public:
   void whatch_eeprom();
    void start_setup();
		WhatchDog(int,int);
    bool check_block();
    void start_loop();
    void zero_all_eeprom();
    void prt_countBlock();
		~WhatchDog();
};


void WhatchDog::prt_countBlock()
{
  Serial.println(this->countBlock);
}


void WhatchDog::start_loop()
{
  zero_all_blocks();
  if(this->nowBlock==0)
    return;
  this->countBlock=this->nowBlock;
  this->nowBlock=0;
}


void WhatchDog::zero_all_blocks()
{
  for(int i=1; i<this->countBlock; i++)
    if(eeprom_read_byte(i) == 1)
      eeprom_update_byte(i, 0);
}

void WhatchDog::start_setup()
{
  check_start();
  this->nowBlock=0;
}


WhatchDog::WhatchDog(int tim=8, int cB=50)
{
  nowBlock=0;
  countBlock=cB;
  wdt_disable();
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

WhatchDog::~WhatchDog()
{
;
}

void WhatchDog::check_start()
{
  int i=0;
  int a=0;
  int k;
  Serial.print("Количество блоков:");
  Serial.println(this->countBlock);
  for(;(i<this->countBlock) && (a==0); i++)  //ищем ненулевые блоки(если хотя бы один равен единице, то программа работала и произошла аппаратная перезагрузка)
    if ( eeprom_read_byte(i)!=0)
    {
        a=1;
        break;
    }
  if(a==1)
    Serial.println("Найден блок");
  else
    Serial.println("Блок не найден");

  if(a!=0)
  {
      for(i=0;(i<this->countBlock); i++) //ищем блок, на котором все остановилось, это будет первый нулевой блок
          if(eeprom_read_byte(i) == 0)
          {
            a=i;
            break;
          }
      eeprom_update_byte(a, 2);
      Serial.print("System ");
      Serial.print(a);
      Serial.println(" is off");
  }
}



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



/*
  У каждого блока есть 3 состояние:
  0 - блок еще не отработал
  1 - блок отработал
  2 - блок заморожен

*/


/* Возможные значения для константы
  WDTO_15MS
  WDTO_30MS
  WDTO_60MS
  WDTO_120MS
  WDTO_250MS
  WDTO_500MS
  WDTO_1S
  WDTO_2S
  WDTO_4S
  WDTO_8S
*/


void WhatchDog::whatch_eeprom()
{
  for(int i=0; i<countBlock; i++)
  {
    Serial.print(i);
    Serial.print("=");
    Serial.println(eeprom_read_byte(i));
  }
}


void WhatchDog::zero_all_eeprom()
{
  wdt_disable();
  for(int i=0; i< this->countBlock; i++)
  {
    eeprom_update_byte(i, 0);
  }
  Serial.println("Энергонезависимые переменные обнулены");
  while(1);
}