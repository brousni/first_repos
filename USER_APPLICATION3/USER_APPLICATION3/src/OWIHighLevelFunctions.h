// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* \li File:               OWIHighLevelFunctions.h
* \li Compiler:           IAR EWAAVR 3.20a
* \li Support mail:       avr@atmel.com
*
* \li Supported devices:  All AVRs.
*
* \li Application Note:   AVR318 - Dallas 1-Wire(R) master.
*                         
*
* \li Description:        Header file for OWIHighLevelFunctions.c
*
*                         $Revision: 1.7 $
*                         $Date: Thursday, August 19, 2004 14:27:18 UTC $
****************************************************************************/

#ifndef _OWI_ROM_FUNCTIONS_H_
#define _OWI_ROM_FUNCTIONS_H_
#include <asf.h>

#define DS18S20_FAMILY_ID                0x10
#define DS18S20_START_CONVERSION         0x44
#define DS18S20_READ_SCRATCHPAD          0xbe
#define DS18S20_ERROR                    -1000   // Return code. Outside temperature range.
#define DS18S20_CRC_ERROR                -2000
#define DS18S20_CONVERSION_ERROR         -3000

#define DS18B20_FAMILY_ID                0x28
#define DS18B20_START_CONVERSION         0x44
#define DS18B20_READ_SCRATCHPAD          0xbe
#define DS18B20_ERROR                    -1000   // Return code. Outside temperature range.

#define DS2438_FAMILY_ID                 0x26
#define DS2438_START_CONVERSION_INTERNAL 0x44
#define DS2438_START_CONVERSION_EXTERNAL 0xB4
#define DS2438_READ_SCRATCHPAD           0xBE
#define DS2438_WRITE_SCRATCHPAD          0x4E
#define DS2438_COPY_SCRATCHPAD           0x48
#define DS2438_RECALL_MEMORY             0xB8
#define DS2438_TEMPERATUR_PAGE           0x00
#define DS2438_CONFIGURATION_PAGE        0x00
#define DS2438_CONV_ID                   0xF0
#define DS2438_VOLT_VDD_ID               0xF1
#define DS2438_VOLT_VAD_ID               0xF2
#define DS2438_STATUS_REG                0x00

#define ONE_WIRE_ID_LENGTH 0x08

#define DS1990_FAMILY_ID                0x01
#define DS1991_FAMILY_ID                0x02
#define DS1982_FAMILY_ID                0x09
#define DS1971_FAMILY_ID                0x14

#define DS2413_FAMILY_ID                0x3A
#define DS2413_PIO_ACCESS_READ          0xF5 // Command to read the gpio
#define DS2413_PIO_ACCESS_WRITE		0x5A //	Command to write the gpio
#define DS2413_CONFIRMATION_PATTERN     0xAA

#define DS2413_PIOA_IN    BIT0
#define DS2413_PIOA_OUT   BIT1
#define DS2413_PIOB_IN    BIT2
#define DS2413_PIOB_OUT   BIT3

#define DS2431_FAMILY_ID                0x2D
#define DS2431_EEPROM_READ              0xF0  // Read data from eeprom into scratcpad, can be followed by 1 byte start address, iButton will increment address automatical. If no address is send a reset pulse will read the entire eeprom to scratchpad
#define DS2431_EEPROM_SIZE              0x80
#define DS2431_SCRATCHPAD_SIZE          0x20
#define DS2431_SCRATCHPAD_COPY		0x55
#define DS2431_SCRATCHPAD_READ          0xAA
#define DS2431_SCRATCHPAD_WRITE         0xAA
#define DS2431_VALIDATION_KEY           0x00

#define DS_IBUTTON_SCRATCHPAD_COPY      0x55  // Will copy the entire content of the scratchpad to eeprom, must be followed by validation 0xA5, afterwards the line must be held high for at least 10ms
#define DS_IBUTTON_SCRATCHPAD_WRITE 	0x0F  // Write data to scratchpad, must be followed by 1 byte start address, iButton will increment address automatical
#define DS_IBUTTON_SCRATCHPAD_READ      0xAA  // Read data from scratchpad, must be followed by 1 byte start address, iButton will increment address automatical
#define DS_IBUTTON_EEPROM_READ		0xF0  // Read data from eeprom into scratcpad, can be followed by 1 byte start address, iButton will increment address automatical. If no address is send a reset pulse will read the entire eeprom to scratchpad
#define DS_IBUTTON_VALIDATION_KEY       0xA5  // Key used to to validate the scratchpad copy and application register lock
#define DS_IBUTTON_READ_APP_REG         0xC3  // If the application register is not locked, data is read into the application scratchpad register, otherwise directly. Command must be followed by 1byte address.
#define DS_IBUTTON_WRITE_APP_REG        0x99  // Write into the application scratchpad register if this is not locked
#define DS_IBUTTON_APP_REG_LOCK         0x5A  // Command to copy the application scratchpad into the register, and lock this
#define DS_IBUTTON_STATUS_REG_READ	0x66

#define DS_IBUTTON_SCRATCHPAD_SIZE      0x20
#define DS_IBUTTON_EEPROM_SIZE          0x20  // 256 bit
#define DS_IBUTTON_APP_REG_SIZE         0x08  // 8 byte of application register

#define DS2405_FAMILY_ID                0x05
#define DS2405_ENABLE_OUTPUT            0x01
#define DS2405_DISABLE_OUTPUT           0x00

#define SEARCH_SUCCESSFUL               0x00
#define SEARCH_NO_UNIT_FOUND            0xFF
#define SEARCH_CRC_ERROR                0x01

#define TEMP_RESET_VALUE_S              0x00AA
#define TEMP_RESET_VALUE_B              0x0550
#define TEMP_MAX_VALUE                  0x7FF

//#include <Modules\Defines\Defines.h>

typedef struct
{
	uint8_t bus;      //!< A bitmask of the bus the device is connected to.
	uint8_t id[8];    //!< The 64 bit identifier.
	bool CRC_Failure;  //Indicates that the CRC checksum is ok or not
	bool Presence;
	uint8_t Eeprom[8]; // Used for dallas key with eeprom options
} OWI_device;


void OWI_SendByte(uint8_t data, uint8_t pins);
uint8_t OWI_ReceiveByte(uint8_t pin);
void OWI_SkipRom(uint8_t pins);
void OWI_ReadRom(uint8_t * romValue, uint8_t pins);
void OWI_MatchRom(uint8_t * romValue, uint8_t pins);
uint8_t OWI_SearchRom(uint8_t * bitPattern, uint8_t lastDeviation, uint8_t pins);


#endif
