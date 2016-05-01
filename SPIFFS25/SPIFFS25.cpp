/************************************************************************************
 * 	
 * 	Name    : SPIFFS25.cpp                        
 * 	Author  : Noah Shibley                         
 * 	Date    : Aug 17th, 2013                                   
 * 	Version : 0.1                                              
 * 	Notes   : Based on SST code from: www.Beat707.com design. (Rugged Circuits and Wusik)      
 * 
 * 	
 * 
 ***********************************************************************************/

#include "SPIFFS25.h"
 
 
 mySPIFFS::mySPIFFS(uint8_t cs, uint8_t wp, uint8_t ho) {
	 _cs = cs;
	 _wp = wp;
	 _ho = ho;
 }

 
int mySPIFFS::begin() {
  _flash.begin(_cs, _wp, _ho);
  delay(200);
  return mount();
}


int mySPIFFS::mount() {
  
  cfg = {0};
  cfg.phys_size = SPI_FLASH_TOT_SIZE;
  cfg.phys_addr = 0; // start spiffs at start of spi flash
  
  cfg.phys_erase_block = SPI_FLASH_64K_SIZE; // according to datasheet
  cfg.log_block_size = SPI_FLASH_64K_SIZE; // let us not complicate things
  
  cfg.log_page_size = SPI_FLASH_PAGE_SIZE; // as we said

  cfg.hal_read_f  = my_spiffs_read;
  cfg.hal_write_f = my_spiffs_write;
  cfg.hal_erase_f = my_spiffs_erase;

  int res = SPIFFS_mount(&fs,
                         &cfg,
                         spiffs_work_buf,
                         spiffs_fds,
                         sizeof(spiffs_fds),
                         spiffs_cache_buf,
                         sizeof(spiffs_cache_buf),
                         0);
  
  Serial.println("Mounting...");
  return res;
}

SPIFFS25::SPIFFS25(){
 
}
 

void SPIFFS25::begin(int chipSelect,int writeProtect,int hold){
	
	//set pin #s
	FLASH_SSn = chipSelect;
	FLASH_Wp = writeProtect;
	FLASH_Hold = hold; 
	
	  pinMode(FLASH_Wp, OUTPUT); 
	  digitalWriteFast(FLASH_Wp, HIGH); //write protect off

	  pinMode(FLASH_Hold, OUTPUT); 
	  digitalWriteFast(FLASH_Hold, HIGH); //mem hold off
	
	  pinMode(FLASH_SSn, OUTPUT); //chip select 
	  
	  SPIFIFO.begin(FLASH_SSn, SPI_CLOCK_24MHz, SPI_MODE3);
	
	  digitalWrite(FLASH_SSn, HIGH);
	  digitalWrite(FLASH_SSn, LOW);
	  
	  if (FLASH_SSn == 10) {         // PTC4
			p = 0x01;
		} else if (FLASH_SSn == 2) {   // PTD0
			p = 0x01;
		} else if (FLASH_SSn == 9) {   // PTC3
			p = 0x02;
		} else if (FLASH_SSn == 6) {   // PTD4
			p = 0x02;
		} else if (FLASH_SSn == 20) {  // PTD5
			p = 0x04;
		} else if (FLASH_SSn == 23) {  // PTC2
			p = 0x04;
		} else if (FLASH_SSn == 21) {  // PTD6
			p = 0x08;
		} else if (FLASH_SSn == 22) {  // PTC1
			p = 0x08;
		} else if (FLASH_SSn == 15) {  // PTC0
			p = 0x10;
		} else {
			p = 0;
		}
	  
	  init(); 
	  readID();
	
}
 

void SPIFFS25::update(){
	

}

// ======================================================================================= //

void SPIFFS25::waitUntilDone()
{
  uint8_t data = 0;
  while (1)
  {
	nop();
    //digitalWriteFast(FLASH_SSn,LOW);
	clearRXFIFO();
    SPIFIFO.write(0x05, 1);
	SPIFIFO.write(0x00);
	//nop();
	(void) SPIFIFO.read();
    data = SPIFIFO.read();
	//digitalWriteFast(FLASH_SSn,HIGH);
    if (!bitRead(data,0)) break;
    
  }
}

// ======================================================================================= //

void SPIFFS25::init()
{
  enable();
  
  SPIFIFO.write(0x50); //enable write status register instruction
  
  delay(50);
  
  SPIFIFO.write(0x01, 1); //write the status register instruction
  SPIFIFO.write(0x00, 0); //write the status register instruction
  
  delay(50);
  disable();
}

// ======================================================================================= //

void SPIFFS25::readID()
{
  uint8_t id, mtype, dev;
  
  enable();
  //digitalWriteFast(FLASH_SSn,LOW);
  clearRXFIFO();
  SPIFIFO.write(0x9F, 1); // Read ID command -@- 1 !!
  //SPIFIFO.write(0x00, 1);
  //SPIFIFO.write(0x00, 1);
  //SPIFIFO.write(0x00, 0);
  
  (void) SPIFIFO.read();
  //id = SPIFIFO.read();
  //mtype = SPIFIFO.read();
  //dev = SPIFIFO.read();
  
  id = receive(p);
  mtype = receive(p);
  dev = receive(p);
  
  SPIFIFO.write(0x00, 0);
  
  disable();
  char buf[16] = {0};
  sprintf(buf, "%02X %02X %02X", id, mtype, dev);
  
  Serial.print("SPI ID ");
  Serial.println(buf);
  
}

// ======================================================================================= //

void SPIFFS25::totalErase()
{
  nop();
  
  SPIFIFO.write(0x06);//write enable instruction
  
  SPIFIFO.write(0xC7); // Erase Chip //
  
  waitUntilDone();
  
  nop();
}

void SPIFFS25::globalUnlock()
{
  SPIFIFO.write(0x06);//write enable instruction
  
  SPIFIFO.write(0x98); // Erase Chip //
 
  nop();
}

// ======================================================================================= //

void SPIFFS25::setAddress(uint32_t addr, uint8_t cont)
{
  SPIFIFO.write(addr >> 16, 1);
  SPIFIFO.write(addr >> 8, 1);  
  SPIFIFO.write(addr, cont);
}


// ======================================================================================= //

uint8_t SPIFFS25::readNext() { 
	return receive(p);
}

// ======================================================================================= //

void SPIFFS25::finish()
{
  SPIFIFO.write(0x00, 0);
}

// ======================================================================================= //
void SPIFFS25::writeInit(uint32_t address)
{
  enable();
  clearRXFIFO();
  SPIFIFO.write(0x06);//write enable instruction
  
  SPIFIFO.write(0x02, 1); // Write Byte //
  setAddress(address, 1);
  
}

void SPIFFS25::writeByte(uint32_t address, uint8_t data, uint8_t cont)
{
  SPIFIFO.write(data, cont);
  
  if (!cont) {
	  nop();
	  waitUntilDone();
  }
}

uint32_t SPIFFS25::writeArray(uint32_t address,const uint8_t dataBuffer[],uint16_t dataLength)
{
	enable();
	clearRXFIFO();
	writeInit(address);
	
	for(uint16_t i=0;i<dataLength;i++)
     {
       writeByte((uint32_t)address+i, dataBuffer[i], i != dataLength-1);
     }
  
  return address + dataLength;
}

uint32_t SPIFFS25::writeArrayDump(uint32_t address,const uint8_t dataBuffer[],uint16_t dataLength)
{
	enable();
	clearRXFIFO();
	writeInit(address);
	
	for(uint16_t i=0;i<dataLength;i++)
     {

	   if (dataBuffer[i] != 0xFF) Serial.println(String(dataBuffer[i], HEX) + String("@") + String(address+i, HEX));

       writeByte((uint32_t)address+i, dataBuffer[i], i != dataLength-1);
     }
  
  return address + dataLength;
}


// ======================================================================================= //

void SPIFFS25::readInit(uint32_t address)
{
  nop();
  
  clearRXFIFO();
  SPIFIFO.write(0x03, 1); // Read Memory - 25/33 Mhz //
  
  setAddress(address, 1);
  
  (void) SPIFIFO.read(); // vidage buffer
  (void) SPIFIFO.read(); // vidage buffer
  (void) SPIFIFO.read(); // vidage buffer
  (void) SPIFIFO.read(); // vidage buffer
  
  //clearRXFIFO();
}


void SPIFFS25::readArray(uint32_t address,uint8_t dataBuffer[],uint16_t dataLength)
{
	readInit(address);

    for (uint16_t i=0; i<dataLength; ++i)
    {
	  SPIFIFO.write(0x00, 1);
      dataBuffer[i] = SPIFIFO.read();
    }

    finish();
}

// ======================================================================================= //

void SPIFFS25::sectorErase(uint8_t sectorAddress)
{
  enable();
  
  SPIFIFO.write(0x06);//write enable instruction
  
  SPIFIFO.write(0x20, 1); // Erase 4KB Sector //
  setAddress(SPI_FLASH_SEC_SIZE*long(sectorAddress), 0);
  
  nop();
  
  waitUntilDone();

  nop();
}

void SPIFFS25::block32Erase(uint8_t sectorAddress)
{
  enable();
  
  SPIFIFO.write(0x06);//write enable instruction
  
  SPIFIFO.write(0x52, 1); // Erase 32KB block //
  setAddress(SPI_FLASH_32K_SIZE*long(sectorAddress), 0);
  
  nop();
  
  waitUntilDone();

  nop();
}

void SPIFFS25::block64Erase(uint8_t sectorAddress)
{
  enable();
  
  SPIFIFO.write(0x06);//write enable instruction
  
  SPIFIFO.write(0xD8, 1); // Erase 64KB block //
  setAddress(SPI_FLASH_64K_SIZE*long(sectorAddress), 0);
  
  nop();
  
  waitUntilDone();

  nop();
}

void SPIFFS25::dumpArray(uint32_t pageAddress, uint32_t size)
{
	uint16_t j;
	uint8_t page[256] = {0};
	
	readArray(pageAddress, page, size);
	for (j=0; j < size; j++) {
		Serial.print(String(page[j], HEX) + String("@") + String(pageAddress+j, HEX) + " ");
	}
	Serial.println("");
}
