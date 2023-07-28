#include "avr/eeprom.h"
#include "avr/wdt.h"

#define REP 0

void setup() 
{
	wdt_disable();
	Serial.begin(9600);
  if (REP)
  {
    eeprom_update_byte(0,0);
    Serial.println("Проверка сброшена\nПоменяйте REP на 0 и загрузите скетч повторно");
    while(1);
  }
	if( eeprom_read_byte(0) == 1 )
	{
			Serial.println("Микроконтроллер поддерживает whatchdog");
			while(1);
	}
	Serial.println("Проверка энергонезависимых переменных...");
	delay(400);
	if(check_eeprom())
		Serial.println("Микроконтроллер поддерживает энергонезависимые переменные");
	else
	{
		Serial.println("Микроконтроллер не поддерживает энергонезависимые переменные");
		while(1);
	}
	delay(500);
	Serial.println("Проверка whatchdog\nПодождите 8 секунд...");
  wdt_enable (WDTO_4S);
}

long time=millis();
int i = 8;

void loop()
{
	if ( i > 0 )
  {
		if ( millis() - time > 1000)
		{
			time=millis();
			Serial.println(i--);
		}
  }
	else
	{
		Serial.println("Микроконтроллер не поддерживает whatchdog\n");
		while(1);
	}
}


int check_eeprom()
{
  eeprom_update_byte(0, 0);
	if (eeprom_read_byte(0) == 0)
	{
  	eeprom_update_byte(0, 1);
		if (eeprom_read_byte(0))
		{
			return 1;
		}
	}
	return 0;
}
