// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* \li File:               OWISWBitFunctions.c
* \li Compiler:           IAR EWAAVR 3.20a
* \li Support mail:       avr@atmel.com
*
* \li Supported devices:  All AVRs.
*
* \li Application Note:   AVR318 - Dallas 1-Wire(R) master.
*                         
*
* \li Description:        Polled software only implementation of the basic 
*                         bit-level signalling in the 1-Wire(R) protocol.
*
*                         $Revision: 1.7 $
*                         $Date: Thursday, August 19, 2004 14:27:18 UTC $
****************************************************************************/

//#include <Modules\Defines\Defines.h>
#include <OWIPolled.h>
#include <OWIBitFunctions.h>
//#include <NvmDomainsCfg.h>


//#include <ioavr.h>
//#include <inavr.h>

//#include <PortDef.h>

/*! \brief Initialization of the one wire bus(es). (Software only driver)
 *  
 *  This function initializes the 1-Wire bus(es) by releasing it and
 *  waiting until any presence sinals are finished.
 *
 *  \param  pins    A bitmask of the buses to initialize.
 */
void OWI_Init(uint8_t pins)
{
    OWI_RELEASE_BUS(pins);
    // The first rising edge can be interpreted by a slave as the end of a
    // Reset pulse. Delay for the required reset recovery time (H) to be 
    // sure that the real reset is interpreted correctly.
    __delay_cycles(OWI_DELAY_H_STD_MODE);
}

void OWI_Reset(uint8_t pins)
{
  OWI_PULL_BUS_LOW(pins);
  __delay_cycles(OWI_DELAY_H_STD_MODE);
  OWI_RELEASE_BUS(pins);
}
/*! \brief  Write a '1' bit to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '1' bit on the 1-Wire
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void OWI_WriteBit1(uint8_t pins)
{
//TEST    uint8_t intState;
    
    // Disable interrupts.
//TEST    intState = __save_interrupt();
    __disable_interrupt();
    
    // Drive bus low and delay.
	PIOA->PIO_SODR = PIO_PA28;
    //OWI_PULL_BUS_LOW(pins);
    __delay_cycles(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
	PIOA->PIO_CODR = PIO_PA28;
    //OWI_RELEASE_BUS(pins);
    __delay_cycles(OWI_DELAY_B_STD_MODE);
    
    // Restore interrupts.
//TEST    __restore_interrupt(intState);
    __enable_interrupt();
}


/*! \brief  Write a '0' to the bus(es). (Software only driver)
 *
 *  Generates the waveform for transmission of a '0' bit on the 1-Wire(R)
 *  bus.
 *
 *  \param  pins    A bitmask of the buses to write to.
 */
void OWI_WriteBit0(unsigned char pins)
{
//TEST    uint8_t intState;
    
    // Disable interrupts.
//TEST    intState = __save_interrupt();
    __disable_interrupt();
    
    // Drive bus low and delay.
	PIOA->PIO_SODR = PIO_PA28;
    //OWI_PULL_BUS_LOW(pins);
    __delay_cycles(OWI_DELAY_C_STD_MODE);
    
    // Release bus and delay.
	PIOA->PIO_CODR = PIO_PA28;
    //OWI_RELEASE_BUS(pins);
    __delay_cycles((uint32_t)OWI_DELAY_D_STD_MODE);
    
    // Restore interrupts.
//TEST    __restore_interrupt(intState);
    __enable_interrupt();
}


/*! \brief  Read a bit from the bus(es). (Software only driver)
 *
 *  Generates the waveform for reception of a bit on the 1-Wire(R) bus(es).
 *
 *  \param  pins    A bitmask of the bus(es) to read from.
 *
 *  \return A bitmask of the buses where a '1' was read.
 */
uint8_t OWI_ReadBit(uint8_t pins)
{
//TEST    uint8_t intState;
    uint8_t bitsRead;
    
    // Disable interrupts.
//TEST    intState = __save_interrupt();
    __disable_interrupt();
	    
    // Drive bus low and delay.
	PIOA->PIO_SODR = PIO_PA28;
    //OWI_PULL_BUS_LOW(pins);
    __delay_cycles(OWI_DELAY_A_STD_MODE);
    
    // Release bus and delay.
	PIOA->PIO_CODR = PIO_PA28;
    //OWI_RELEASE_BUS(pins);
    __delay_cycles(OWI_DELAY_E_STD_MODE);
    
    // Sample bus and delay.
	bitsRead = ioport_get_pin_level(PIO_PA29_IDX);
    __delay_cycles(OWI_DELAY_F_STD_MODE);
    
    // Restore interrupts.
//TEST    __restore_interrupt(intState);
    __enable_interrupt();
    
    return bitsRead;
}


/*! \brief  Send a Reset signal and listen for Presence signal. (software
 *  only driver)
 *
 *  Generates the waveform for transmission of a Reset pulse on the 
 *  1-Wire(R) bus and listens for presence signals.
 *
 *  \param  pins    A bitmask of the buses to send the Reset signal on.
 *
 *  \return A bitmask of the buses where a presence signal was detected.
 */
uint8_t OWI_DetectPresence(uint8_t pins)
{
//TEST    uint8_t intState;
    uint8_t presenceDetected;
    
    // Disable interrupts.
//TEST    intState = __save_interrupt();
    __disable_interrupt();
    
    // Drive bus low and delay.
	PIOA->PIO_SODR = PIO_PA28;
    //OWI_PULL_BUS_LOW(pins);
	//PIOA->PIO_ODR = PIO_PA28;
	//PIOA->PIO_PDR = PIO_PA28;
	//pio_set_pin_high(PIO_PA28_IDX);
   __delay_cycles(OWI_DELAY_H_STD_MODE);
    
//	delay_cycles(1);
    // Release bus and delay.
	//PIOA->PIO_OER = PIO_PA28;
	//PIOA->PIO_PER = PIO_PA28;
	//pio_set_pin_low(PIO_PA28_IDX);
	PIOA->PIO_CODR = PIO_PA28;
    //OWI_RELEASE_BUS(pins);
    __delay_cycles(OWI_DELAY_I_STD_MODE);
    
    // Sample bus to detect presence signal and delay.
	presenceDetected = !ioport_get_pin_level(PIO_PA29_IDX);
    //presenceDetected = ((~OWI_PIN) & pins);
    __delay_cycles(OWI_DELAY_J_STD_MODE);
    
    // Restore interrupts.
//TEST    __restore_interrupt(intState);
    __enable_interrupt();
    return presenceDetected;
}

void OWI_PULL_BUS_LOW(uint8_t bitMask)
{
 //pio_set_output(PIOA, PIO_PA28, HIGH, DISABLE, DISABLE);
 PIOA->PIO_SODR = PIO_PA28;
 //PIOA->PIO_PDR = PIO_PA28;
 //PIOA->PIO_ODR = PIO_PA28;
}

void OWI_PULL_BUS_HIGH(uint8_t bitMask)
{
  //pio_set_output(PIOA, PIO_PA28, LOW, DISABLE, DISABLE);
  PIOA->PIO_CODR = PIO_PA28;
  //PIOA->PIO_PER = PIO_PA28;
  //PIOA->PIO_OER = PIO_PA28;
  
}

void OWI_RELEASE_BUS(uint8_t bitMask)
{
  //pio_set_output(PIOA, PIO_PA28, LOW, DISABLE, DISABLE);
  PIOA->PIO_CODR = PIO_PA28;
  //PIOA->PIO_PER = PIO_PA28;
  //PIOA->PIO_OER = PIO_PA28;
  
}
