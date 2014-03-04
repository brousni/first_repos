// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* \li File:               OWIBitFunctions.h
* \li Compiler:           IAR EWAAVR 3.20a
* \li Support mail:       avr@atmel.com
*
* \li Supported devices:  All AVRs.
*
* \li Application Note:   AVR318 - Dallas 1-Wire(R) master.
*                         
*
* \li Description:        Header file for OWIBitFunctions.c
*
*                         $Revision: 1.7 $
*                         $Date: Thursday, August 19, 2004 14:27:18 UTC $
****************************************************************************/

#ifndef _OWI_BIT_FUNCTIONS_H_
#define _OWI_BIT_FUNCTIONS_H_
#include <asf.h>

//#include <Modules\Defines\Defines.h>
#include <OWIdefs.h>
#include <OWIPolled.h>

#ifdef OWI_SOFTWARE_DRIVER
void OWI_Init(uint8_t pins);
void OWI_Reset(uint8_t pins);
void OWI_WriteBit1(uint8_t pins);
void OWI_WriteBit0(uint8_t pins);
uint8_t OWI_ReadBit(uint8_t pins);
uint8_t OWI_DetectPresence(uint8_t pins);

void OWI_PULL_BUS_LOW(uint8_t bitMask);
void OWI_PULL_BUS_HIGH(uint8_t bitMask);
void OWI_RELEASE_BUS(uint8_t bitMask);


#endif

#ifdef OWI_UART_DRIVER
uint8_t OWI_TouchBit(unsigned char outValue);

void OWI_Init();
#define OWI_Init(arg) OWI_Init()

void OWI_WriteBit1();
#define OWI_WriteBit1(arg) OWI_WriteBit1()

void OWI_WriteBit0();
#define OWI_WriteBit0(arg) OWI_WriteBit0()

uint8_t OWI_ReadBit();
#define OWI_ReadBit(arg) OWI_ReadBit()

uint8_t OWI_DetectPresence();
#define OWI_DetectPresence(arg) OWI_DetectPresence()
#endif  // OWI_UART_DRIVER

/****************************************************************************
 Macros
****************************************************************************/
/*! \brief Pull 1-Wire bus low.
 *
 *  This macro sets the direction of the 1-Wire pin(s) to output and 
 *  pull the line(s) low.
 *  
 *  \param bitMask  A bitmask of the buses to pull low.
 */
 #if 0
#ifdef USE_DIFFERENT_OWI_OUTPUT

#define OWI_PULL_BUS_LOW(bitMask) \
            OWO_DDR |= (1<<OWO_PORT_PIN); \
            OWO_PORT |= (1<<OWO_PORT_PIN); \
            OWI_DDR |= bitMask; \
            OWI_PORT &= ~bitMask;

#define OWI_PULL_BUS_HIGH(bitMask) \
            OWO_DDR |= (1<<OWO_PORT_PIN); \
            OWO_PORT &= ~(1<<OWO_PORT_PIN); \
            OWI_DDR |= bitMask; \
            OWI_PORT |= bitMask;

#define OWI_RELEASE_BUS(bitMask) \
            OWO_DDR |= (1<<OWO_PORT_PIN); \
            OWO_PORT &= ~(1<<OWO_PORT_PIN); \
            OWI_DDR &= ~bitMask; \
            OWI_PORT &= ~bitMask;

#else

#define OWI_PULL_BUS_LOW(bitMask) \
            OWI_DDR |= bitMask; \
            OWI_PORT &= ~bitMask;
            

#define OWI_PULL_BUS_HIGH(bitMask) \
            OWI_DDR |= bitMask; \
            OWI_PORT |= bitMask;


/*! \def    OWI_RELEASE_BUS(bitMask)
 *
 *  \brief  Release the bus. 
 *
 *  This macro releases the bus and enables the internal pull-up if
 *  it is used.
 *
 *  \param  bitMask A bitmask of the buses to release.
 */
#ifdef OWI_USE_INTERNAL_PULLUP            
// Set 1-Wire pin(s) to input and enable internal pull-up resistor.
#define OWI_RELEASE_BUS(bitMask) \
            OWI_DDR &= ~bitMask; \
            OWI_PORT |= bitMask; 

#else
// Set 1-Wire pin(s) to input mode. No internal pull-up enabled.
#define OWI_RELEASE_BUS(bitMask) \
            OWI_DDR &= ~bitMask; \
            OWI_PORT &= ~bitMask;

#endif

#endif


#endif
#endif
