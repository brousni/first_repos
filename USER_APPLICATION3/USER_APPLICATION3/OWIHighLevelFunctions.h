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

//#include <Modules\Defines\Defines.h>

void OWI_SendByte(uint8_t data, uint8_t pins);
uint8_t OWI_ReceiveByte(uint8_t pin);
void OWI_SkipRom(uint8_t pins);
void OWI_ReadRom(uint8_t * romValue, uint8_t pins);
void OWI_MatchRom(uint8_t * romValue, uint8_t pins);
uint8_t OWI_SearchRom(uint8_t * bitPattern, uint8_t lastDeviation, uint8_t pins);


#endif
