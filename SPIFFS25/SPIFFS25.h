/************************************************************************************
 * 	
 * 	Name    : SPIFFS25.h                        
 * 	Author  : Noah Shibley                         
 * 	Date    : Aug 17th, 2013                                    
 * 	Version : 0.1                                              
 * 	Notes   : Nor Serial Flash 16Mbit (2Mbytes) - S71295 (SST) 2097152 bytes.
 *			  Based on SST code from: www.Beat707.com design. (Rugged Circuits and Wusik)
 *			  Should work for SST25VF004, SST25VF016, SST25VF064, etc.                 
 * 
 * 	
 * 
 ***********************************************************************************/


//include guard
#ifndef SST25VF_H
#define SST25VF_H

#include "WProgram.h"
#include <SPIFIFO.h>
#include <spiffs.h>

#define SPI_FLASH_TOT_SIZE       2097152*32 //this chip contains this much storage
#define SPI_FLASH_SEC_SIZE           4096UL //each sector is 4096 bytes
#define SPI_FLASH_32K_SIZE          32768UL //32K block
#define SPI_FLASH_64K_SIZE          65536UL //64K block
#define SPI_FLASH_PAGE_SIZE             256

class SPIFFS25 {
 
public: 

	SPIFFS25();
    void begin(int chipSelect,int writeProtect,int hold);
    void update();
	void readID();
	void totalErase();
	void globalUnlock();
	void sectorErase(uint8_t sectorAddress);
	void block32Erase(uint8_t sectorAddress);
	void block64Erase(uint8_t sectorAddress);
	
	void dumpArray(uint32_t pageAddress, uint32_t size);
	
	void readInit(uint32_t address);
	void writeInit(uint32_t address);
	uint8_t readNext();
	void readFinish();
	void readArray(uint32_t address,uint8_t dataBuffer[],uint16_t dataLength);
	
	void writeByte(uint32_t address, uint8_t data, uint8_t cont);
	uint32_t writeArray(uint32_t address,const uint8_t dataBuffer[],uint16_t dataLength);
	uint32_t writeArrayDump(uint32_t address,const uint8_t dataBuffer[],uint16_t dataLength);
	void finish();
	void waitUntilDone();

private: 
	
	void init();
	char buf[16];
	uint8_t FLASH_SSn; //chip select pin
	uint8_t FLASH_Wp;  //Write protection pin
	uint8_t FLASH_Hold; //read/write hold pin (pause)
    uint8_t p;
	
	
	void setAddress(uint32_t addr, uint8_t cont);
	void enable()    { nop(); }
	void disable()   { nop(); }
	
	inline void volatile nop(void) { asm __volatile__ ("nop"); }
	
	/** SPI receive a byte */
    uint8_t receive(uint8_t pcs) {
	  uint16_t wTimeout = 0xffff;
      //SPI0_SR = SPI_SR_TCF;
	  // vidage buffer
	  //while (!(KINETISK_SPI0.SR & SPI_SR_EOQF)) ; && (--wTimeout)
      SPI0_PUSHR = 0x00 | (pcs << 16) | SPI_PUSHR_CONT;
      while ((KINETISK_SPI0.SR & (15 << 4)) == 0) {}
      return KINETISK_SPI0.POPR;
    }
	
	void clearRXFIFO() {
		SPI0_MCR |= SPI_MCR_CLR_RXF;
	}
	
 
}; 

#if defined(__cplusplus)
extern "C" {
#endif
extern s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src);
extern s32_t my_spiffs_erase(u32_t addr, u32_t size);
extern s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst);
#if defined(__cplusplus)
}
#endif

class mySPIFFS {
	
public:
	mySPIFFS(uint8_t cs, uint8_t wp, uint8_t ho);
	int begin();
	int mount();
	SPIFFS25 getFlash() {return _flash;}
	
	spiffs_file fopen(const char *path, spiffs_flags flags, spiffs_mode mode) {
		return SPIFFS_open(&fs, path, flags, mode);
	}
	s32_t fwrite(spiffs_file fh, void *buf, s32_t len) {
		return SPIFFS_write(&fs, fh, buf, len);
	}
	
	s32_t fclose(spiffs_file fh) {
		return SPIFFS_close(&fs, fh);
	}
	
	s32_t errno() {return SPIFFS_errno(&fs);}
	
	void format () {SPIFFS_format(&fs);}
	void unmount () {SPIFFS_unmount(&fs);}
	//Returns nonzero if spiffs is mounted, or zero if unmounted.
	u8_t mounted () {return SPIFFS_mounted(&fs);}
	
private:
    uint8_t _cs, _wp, _ho;
	spiffs_config cfg;
    SPIFFS25 _flash;
	spiffs fs;
	u8_t spiffs_work_buf[SPI_FLASH_PAGE_SIZE * 2];
    u8_t spiffs_fds[32 * 4];
    u8_t spiffs_cache_buf[(SPI_FLASH_PAGE_SIZE + 32) * 4];
};
 
#endif
