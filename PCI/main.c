#include <sys/io.h>
#include <stdio.h>
#include "pci.h"

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
}
