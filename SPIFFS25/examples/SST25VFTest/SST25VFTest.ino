#include <SPIFFS25.h>
#include <SPI.h>
// SPIFFS25 SST25VF
#define MemCs 10 //chip select
#define MemWp 18 //write protection
#define MemHold 19 //hold

SPIFFS25 flash;

#define DL 11
uint8_t buffer[DL], buffer2[DL + 10]; //20130903T2046Z
uint8_t indice = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(500);

  flash.begin(MemCs, MemWp, MemHold);

  Serial.println("reading Array Written bytes....");

  for (uint8_t j = 0; j < DL; j++)
  {
    buffer[j] = j;
  }

  flash.totalErase();
  //flash.writeArray(0, buffer, DL);
  //flash.writeArray(DL, buffer, DL);
  //flash.writeArray(2*DL, buffer, DL);
  //flash.writeArray(3*DL, buffer, DL);

  for (uint8_t j = 0; j < 10; j++)
  {
    flash.readArray((DL * j), buffer2, DL);

    Serial.println("--------------------------------");
    for (uint8_t i = 0; i < DL; i++)
    {
      Serial.print(" ");
      Serial.print(buffer2[i], HEX);
      Serial.print("/");
      Serial.print(buffer[i], HEX);
    }
    Serial.println("");
  }

  indice = 0;
}

void loop() {
  flash.writeArray(indice, buffer, DL);
  indice += DL;
  if (indice > 8*DL) {
    flash.sectorErase(0);
    indice = 0;
  }
  
//  flash.readInit((4096UL * 0));
//  uint8_t result = flash.readNext();
//  Serial.print(result, DEC);Serial.print(" ");
//  result = flash.readNext();
//  Serial.print(result, DEC);Serial.print(" ");
//  result = flash.readNext();
//  Serial.print(result, DEC);Serial.print(" ");
//  result = flash.readNext();
//  Serial.print(result, DEC);Serial.print(" ");
//  result = flash.readNext();
//  Serial.println(result, DEC);
//  result = flash.readNext();
//  flash.finish();
for (uint8_t j = 0; j < 10; j++)
  {
    flash.readArray((DL * j), buffer2, DL);

    Serial.println("--------------------------------");
    for (uint8_t i = 0; i < DL; i++)
    {
      Serial.print(" ");
      Serial.print(buffer2[i], HEX);
      Serial.print("/");
      Serial.print(buffer[i], HEX);
    }
    Serial.println("");
  }
  delay(2000);
}