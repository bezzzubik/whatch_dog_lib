#include <avr/eeprom.h>
#include <avr/wdt.h>

#define COUNT 50


void setup()
{
  wdt_disable();
  for(int i=0; i < COUNT; i++)
  {
    eeprom_update_byte(i, 0);
  }
  Serial.print("Обнулено энергонезависимых переменных: ");
  Serial.println(COUNT);
}

void loop()
{

}