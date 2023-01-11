#include <sys/io.h>
#include <stdio.h>
#include "pci.h"

void decode(unsigned char elem1, unsigned char elem2, unsigned char elem3);

//fun for comparing id and getting struct with wendor | dev
PCI_VENTABLE* getVenTableEl(unsigned short venId) {
	for (int i = 0; i < PCI_VENTABLE_LEN; i++) {
		if (PciVenTable[i].VendorId == venId) {
			return PciVenTable + i;
		}
	}
	return 0;
}

PCI_DEVTABLE* getDevTableEl(unsigned short venId, unsigned short devId) {
	for (int i = 0; i < PCI_DEVTABLE_LEN; i++) {
		if (PciDevTable[i].VendorId == venId && PciDevTable[i].DeviceId == devId) {
			return PciDevTable + i;
		}
	}
	return 0;
}

//fun getting full addr (bus + dev + funcNO) and decoding information into table
void decodeDevice(unsigned addr, unsigned busNo, unsigned devNo, unsigned funcNo) {
	outl(addr, 0xCF8);
	int result = inl(0xCFC);
	unsigned short devId = *((unsigned short*) (&result) + 1);
	unsigned short venId = *((unsigned short*) (&result));
	PCI_VENTABLE *ventableEl = getVenTableEl(venId);
	PCI_DEVTABLE *devtableEl = getDevTableEl(venId, devId);

	printf("%10d\t%10d\t%10d\t", busNo, devNo, funcNo);
	printf("%04x\t%04x\t", venId, devId);
	printf("%40s\t", ventableEl->VendorName);
	if (devtableEl) {
		printf("%10s\n", devtableEl->DeviceName);
	} else {
		printf("%10s\n", "--------");
	}
}

int main() {
  //Access
	if (iopl(3)) {
		printf("Priv. error! Use sudo instead.\n");
		return 1;
	}

	//one|res   |busNo  |devN|fn|regNo|zeros
	unsigned addr = 0b10000000000000000000000000000000;
	int result;

	printf("%10s\t%10s\t%10s\t%4s\t%4s\t%40s\t%10s\n", "busNo", "devNo",
			"funcNo", "ven", "dev", "Vendor name", "Device name");
	for (int i = 0; i < 160; i++)
		printf("-");
	printf("\n");

	for (unsigned i = 0; i < 256; i++) {
		//256 bus
		addr &= 0b11111111000000001111111111111111;
		unsigned buf = i << 16;
		addr |= buf;

		for (unsigned j = 0; j < 32; j++) {
		//32 devices
			addr &= 0b11111111111111110000011111111111;
			buf = j << 11;
			addr |= buf;

			for (unsigned k = 0; k < 8; k++) {
			//8 function
				addr &= 0b11111111111111111111100011111111;
				buf = k << 8;
				addr |= buf;
				outl(addr, 0xCF8);
				result = inl(0xCFC);
				if (result != -1) {
					//If bus, device or function
					decodeDevice(addr, i, j, k);
					// check if bridge
					unsigned brAddr = addr;
					unsigned regNo = 3;
					regNo = regNo << 2;
					brAddr |= regNo;
					outl(brAddr, 0xCF8);
					unsigned reg3 = inl(0xCFC);
					int isBridge = reg3 & 0x00010000;
					//4th position from left - bridge

					if (isBridge) {
						printf("Bridge!!!\n");
						// read reg15
						regNo = 2;
						brAddr = addr;
						regNo = regNo << 2;
						brAddr |= regNo;
						outl(brAddr, 0xCF8);
						unsigned reg2 = inl(0xCFC);
						printf("%08X\n", reg2);
						unsigned char *arr = (unsigned char*) &reg2;
						printf("Сlass Code: %02X%02X%02X\n", arr[3], arr[2],
								arr[1]);

              decode(arr[3], arr[2], arr[1]);
						//interrupt pin and interrupt line
					} else {
						regNo = 15;
						regNo = regNo << 2;
						brAddr = addr;
						brAddr |= regNo;
						outl(brAddr, 0xCF8);
						unsigned reg15_1 = inl(0xCFC);
						unsigned char *arr1 = (unsigned char*) &reg15_1;
						printf("\nInterrupt line: %02X\n", arr1[0]);

						regNo = 2;
						regNo = regNo << 2;
						brAddr = addr;
						brAddr |= regNo;
						outl(brAddr, 0xCF8);
						unsigned reg2_2 = inl(0xCFC);
						unsigned char *arr2 = (unsigned char*) &reg2_2;
						printf("Сlass Code: %02X%02X%02X\n", arr2[3], arr2[2],
								arr2[1]);
               decode(arr2[3], arr2[2], arr2[1]);
					}
					printf("\n\n");
				}
			}
		}
	}
	return 0;
}

void decode(unsigned char elem1, unsigned char elem2, unsigned char elem3) {
  switch (elem1) {
    case 0x01: {
      printf("Mass Storage Controller");
      break;
    }
    case 0x02 : {
       printf("Network Controller");
      break;
    }
    case 0x03: {
        printf("Display Controller");
      break;
    }
    case 0x04: {
        printf("Multimedia Controller");
      break;
    }
     case 0x05: {
        printf("Memory Controller");
      break;
    }
    case 0x06: {
        printf("Bridge Device");
      break;
    }
    case 0x07: {
        printf("Simple Communication Controllers");
      break;
    }
    case 0x08: {
        printf("Base System Peripherals");
      break;
    }
    case 0x09: {
        printf("Input Devices");
      break;
    }
     case 0x0A: {
        printf("Docking Stations");
      break;
    }
    case 0x0B: {
        printf("Processors");
      break;
    }
    case 0x0C: {
        printf("Serial Bus Controllers");
      break;
    }
    case 0x0D: {
        printf("Wireless Controllers");
      break;
    }
    case 0x0E: {
        printf("Intelligent I/O Controllers");
      break;
    }
    case 0x0F: {
        printf("Satellite Communication Controllers");
      break;
    }
    case 0x10: {
      printf("Encryption/Decryption Controllers");
      break;
    }
    case 0x11: {
      printf("Data Acquisition and Signal Processing Controllers");
      break;
    }
  }
  printf("\t");
//------------
  switch (elem2) {
  case 0x01: {
    switch (elem3) {
        case 0x01: {
          printf("IDE Controller");
          break;
        }
        case 0x02 : {
          printf("Floppy Disk Controller");
          break;
        }
        case 0x03: {
          printf("IPI Bus Controller");
          break;
        }
        case 0x04: {
          printf("RAID Controller");
          break;
        }
        case 0x05: {
          printf("ATA Controller (Single DMA)");
          break;
        }
        case 0x06: {
          printf("Serial ATA (Vendor Specific Interface)");
          break;
        }
        case 0x07: {
          printf("Serial Attached SCSI (SAS)");
          break;
        }
        case 0x80: {
          printf("Other Mass Storage Controller");
          break;
        }
        }
        break;
  }
  case 0x02 : {
    switch (elem3) {
    case 0x01: {
      printf("Token Ring Controller");
      break;
    }
    case 0x02 : {
      printf("FDDI Controller");
      break;
    }
    case 0x03: {
      printf("ATM Controller");
      break;
    }
    case 0x04: {
      printf("ISDN Controller");
      break;
    }
    case 0x05: {
      printf("WorldFip Controller");
      break;
    }
    case 0x06: {
      printf("PICMG 2.14 Multi Computing");
      break;
    }
    case 0x80: {
      printf("Other Network Controller");
      break;
    }
    }
    break;
  }
  case 0x03: {
    switch (elem3) {
    case 0x00: {
      printf("VGA-Compatible Controller");
      break;
    }
    case 0x01 : {
      printf("XGA Controller");
      break;
    }
    case 0x02: {
      printf("3D Controller (Not VGA-Compatible)");
      break;
    }
    case 0x80: {
      printf("Other Display Controller");
      break;
    }
    case 0x04: {
      switch (elem3) {
      case 0x00: {
        printf("Video Device");
        break;
      }
      case 0x01 : {
        printf("Audio Device");
        break;
      }
      case 0x02: {
        printf("Computer Telephony Device");
        break;
      }
      case 0x03: {
        printf("Multimedia Device");
        break;
      }
    break;
      }
    case 0x05: {
      printf("Memory Controller");
      break;
    }
    case 0x06: {
      printf("Bridge Device");
      break;
    }
    case 0x07: {
      printf("Simple Communication Controllers");
      break;
    }
    case 0x08: {
      printf("Base System Peripherals");
      break;
    }
    case 0x09: {
      printf("Input Devices");
      break;
    }
    case 0x0A: {
      printf("Docking Stations");
      break;
    }
    case 0x0B: {
      printf("Processors");
      break;
    }
    case 0x0C: {
      printf("Serial Bus Controllers");
      break;
    }
    case 0x0D: {
      printf("Wireless Controllers");
      break;
    }
    case 0x0E: {
      printf("Intelligent I/O Controllers");
      break;
    }
    case 0x0F: {
      printf("Satellite Communication Controllers");
      break;
    }
    case 0x10: {
      printf("Encryption/Decryption Controllers");
      break;
    }
    case 0x11: {
      printf("Data Acquisition and Signal Processing Controllers");
      break;
    }
    }

    }
  }
  }
  }