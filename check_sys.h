#include <avr/eeprom.h>
#include <avr/wdt.h>

#define COUNTBLOK 11        //количество блоков в программе


class WhatchDog
{
    int countBlock;
	int nowBlock=0;
	
	public:
		WhatchDog(char*);
		~WhatchDog();
};

WhatchDog::WhatchDog(int time=8, int countBlock=30)
{
	wdt_disable();
	switch (time)
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
	end_block(0);
}

WhatchDog::~WhatchDog()
{
;
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
void start_whatchdog()    //инициализация сторожевого таймера
{                         //требуется ставить эту функцию самой первой в setup
  wdt_disable();
  wdt_enable (WDTO_8S);  
}


void check_start()
{
  int i=0;
  int a=0;
  int k;
  for(;(i<COUNTBLOK) && (a==0); i++)  //ищем ненулевые блоки(если хотя бы один равен единице, то программа работала и произошла аппаратная перезагрузка)
    if ( eeprom_read_byte(i)!=0)
    {
        a=1;
        break;
    }
  if(a!=0)
  {
      for(i=0;(i<COUNTBLOK); i++) //ищем блок, на котором все остановилось, это будет первый нулевой блок
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



void start_loop()   
{
  for(int i=0; i<COUNTBLOK; i++)
    if(eeprom_read_byte(i) == 1)
      eeprom_update_byte(i, 0);
}


void end_block(int i)   //Ставится !!!ВНУТРИ!!! выполняемого
{                       //блока в самом конце его выполнения
     eeprom_update_byte(i, 1);
     wdt_reset();
}



bool check_block(int i)
{
  if ( eeprom_read_byte(i))
     return false;
  else
     return true; 
}


void whatch_eeprom()
{
  for(int i=0; i<COUNTBLOK; i++)
  {
    Serial.print(i);
    Serial.print("=");
    Serial.println(eeprom_read_byte(i));
  }
}

void zero_all_eeprom()
{

  for(int i=0; i<COUNTBLOK; i++)
  {
    eeprom_update_byte(i, 0);
  }  
  
}
