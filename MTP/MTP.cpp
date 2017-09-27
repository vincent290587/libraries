#include "MTP.h"

#if !defined(USB_MTPDISK) && !defined(USB_MTPDISK_SERIAL)
#error "You need to select USB Type: 'MTP Disk (Experimental)'"
#endif

SdFat SD;

// TODO:
//   support multiple storages
//   support serialflash
//   partial object fetch/receive
//   events (notify usb host when local storage changes)

// These should probably be weak.
void mtp_yield() {}
void mtp_lock_storage(bool lock) { }

