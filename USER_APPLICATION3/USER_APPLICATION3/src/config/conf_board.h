/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H


#define __delay_cycles(var) delay_cycles(var) //delay_us(var)
// portable_delay_cycles(var) 
#define __disable_interrupt() taskENTER_CRITICAL(); \
                              Disable_global_interrupt()
							  
#define __enable_interrupt() Enable_global_interrupt(); \
                             taskEXIT_CRITICAL()

// Oscillator defines
#define BOARD_FREQ_MAINCK_XTAL          (11059200U)
#define BOARD_FREQ_SLCK_XTAL            (32768U)
#define BOARD_FREQ_SLCK_BYPASS			(32768U)
#define BOARD_FREQ_MAINCK_BYPASS		(11059200U)

#define BOARD_OSC_STARTUP_US            300000

/*
 * USART pins are configured as basic serial port by default.
 * You can enable other pins according application.
 */
/** USART1 pin RX */
#define PIN_USART1_RXD        {PIO_PA21A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RXD_IDX    (PIO_PA21_IDX)
#define PIN_USART1_RXD_FLAGS  (IOPORT_MODE_MUX_A)
/** USART1 pin TX */
#define PIN_USART1_TXD        {PIO_PA22A_TXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_TXD_IDX    (PIO_PA22_IDX)
#define PIN_USART1_TXD_FLAGS  (IOPORT_MODE_MUX_A)
/** USART1 pin CTS */
#define PIN_USART1_CTS        {PIO_PA25A_CTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_CTS_IDX    (PIO_PA25_IDX)
#define PIN_USART1_CTS_FLAGS  (IOPORT_MODE_MUX_A)
/** USART1 pin RTS */
#define PIN_USART1_RTS        {PIO_PA24A_RTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RTS_IDX    (PIO_PA24_IDX)
#define PIN_USART1_RTS_FLAGS  (IOPORT_MODE_MUX_A)
/** USART1 pin SCK */
//#define PIN_USART1_SCK        {PIO_PA23A_SCK1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
//#define PIN_USART1_SCK_IDX    (PIO_PA23_IDX)
//#define PIN_USART1_SCK_FLAGS  (IOPORT_MODE_MUX_A)
/** USART1 pin ENABLE */
//#define PIN_USART1_EN         {PIO_PA23, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
//#define PIN_USART1_EN_IDX     (PIO_PA23_IDX)
//#define PIN_USART1_EN_FLAGS   (0)
//#define PIN_USART1_EN_ACTIVE_LEVEL   IOPORT_PIN_LEVEL_LOW
//#define PIN_USART1_EN_INACTIVE_LEVEL IOPORT_PIN_LEVEL_HIGH

/* Configure USART RXD pin */
#define CONF_BOARD_USART_RXD

/* Configure USART TXD pin */
#define CONF_BOARD_USART_TXD

/* Configure USART CTS pin */
#define CONF_BOARD_USART_CTS

/* Configure USART RTS pin */
#define CONF_BOARD_USART_RTS

/* Configure USART synchronous communication SCK pin */
//#define CONF_BOARD_USART_SCK

/** UART0 pins (UTXD0 and URXD0) definitions, PA10,9. */
#define PINS_UART0        (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
#define PINS_UART0_FLAGS  (IOPORT_MODE_MUX_A)

#define PINS_UART0_PORT   IOPORT_PIOA
#define PINS_UART0_MASK   (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
#define PINS_UART0_PIO    PIOA
#define PINS_UART0_ID     ID_PIOA
#define PINS_UART0_TYPE   PIO_PERIPH_A
#define PINS_UART0_ATTR   PIO_DEFAULT

/** UART1 pins (UTXD1 and URXD1) definitions, PA6,5. */
#define PINS_UART1        (PIO_PA6C_UTXD1 | PIO_PA5C_URXD1)
#define PINS_UART1_FLAGS  (IOPORT_MODE_MUX_C)

#define PINS_UART1_PORT   IOPORT_PIOA
#define PINS_UART1_MASK   (PIO_PA6C_UTXD1 | PIO_PA5C_URXD1)
#define PINS_UART1_PIO    PIOA
#define PINS_UART1_ID     ID_PIOA
#define PINS_UART1_TYPE   PIO_PERIPH_C
#define PINS_UART1_ATTR   PIO_DEFAULT

/* Configure ADM3312 enable pin */
//#define CONF_BOARD_ADM3312_EN
//#define CONF_BOARD_ADM3312_EN_DISABLE_AT_INIT

/** SPI MISO pin definition. */
#define SPI_MISO_GPIO         (PIO_PA12_IDX)
#define SPI_MISO_FLAGS        (IOPORT_MODE_MUX_A)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO         (PIO_PA13_IDX)
#define SPI_MOSI_FLAGS        (IOPORT_MODE_MUX_A)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO         (PIO_PA14_IDX)
#define SPI_SPCK_FLAGS        (IOPORT_MODE_MUX_A)

// 1-WIRE SETUP

#define     OWI_PIN_0       0x01
#define     OWI_PIN_1       0x02
#define     OWI_PIN_2       0x04
#define     OWI_PIN_3       0x08
#define     OWI_PIN_4       0x10
#define     OWI_PIN_5       0x20
#define     OWI_PIN_6       0x40
#define     OWI_PIN_7       0x80

#define     OWI_PORT        PORTC   //!< 1-Wire PORT Data register.
#define     OWI_PIN         PINC    //!< 1-Wire Input pin register.
#define     OWI_DDR         DDRC    //!< 1-Wire Data direction register.
#define     OWI_PORT_pin    PORTC0
#define     BUSES   (OWI_PIN_0) //!< Buses to search.

#define     OWO_DDR         DDRD
#define     OWO_PORT        PORTD
#define     OWO_PORT_PIN    PORTD7

#define     OWI_READ_PORT() GET_PORT(OWI_PIN, OWI_PORT_pin)  
#define MAX_DEVICES 10       //!< Max number of devices to search for.

#define ONE_WIRE_ID_LENGTH 0x08

#define DS1990_FAMILY_ID                0x01
#define DS1991_FAMILY_ID                0x02
#define DS1982_FAMILY_ID                0x09
#define DS1971_FAMILY_ID                0x14

#define DS2413_FAMILY_ID                0x3A
#define DS2413_PIO_ACCESS_READ          0xF5 // Command to read the gpio
#define DS2413_PIO_ACCESS_WRITE                       0x5A //    Command to write the gpio
#define DS2413_CONFIRMATION_PATTERN     0xAA

#define DS2413_PIOA_IN    BIT0
#define DS2413_PIOA_OUT   BIT1
#define DS2413_PIOB_IN    BIT2
#define DS2413_PIOB_OUT   BIT3

#define DS2431_FAMILY_ID                0x2D
#define DS2431_EEPROM_READ              0xF0  // Read data from eeprom into scratcpad, can be followed by 1 byte start address, iButton will increment address automatical. If no address is send a reset pulse will read the entire eeprom to scratchpad
#define DS2431_EEPROM_SIZE              0x80
#define DS2431_SCRATCHPAD_SIZE          0x20
#define DS2431_SCRATCHPAD_COPY                        0x55
#define DS2431_SCRATCHPAD_READ          0xAA
#define DS2431_SCRATCHPAD_WRITE         0xAA
#define DS2431_VALIDATION_KEY           0x00

#define DS_IBUTTON_SCRATCHPAD_COPY      0x55  // Will copy the entire content of the scratchpad to eeprom, must be followed by validation 0xA5, afterwards the line must be held high for at least 10ms
#define DS_IBUTTON_SCRATCHPAD_WRITE         0x0F  // Write data to scratchpad, must be followed by 1 byte start address, iButton will increment address automatical
#define DS_IBUTTON_SCRATCHPAD_READ      0xAA  // Read data from scratchpad, must be followed by 1 byte start address, iButton will increment address automatical
#define DS_IBUTTON_EEPROM_READ                        0xF0  // Read data from eeprom into scratcpad, can be followed by 1 byte start address, iButton will increment address automatical. If no address is send a reset pulse will read the entire eeprom to scratchpad
#define DS_IBUTTON_VALIDATION_KEY       0xA5  // Key used to to validate the scratchpad copy and application register lock
#define DS_IBUTTON_READ_APP_REG         0xC3  // If the application register is not locked, data is read into the application scratchpad register, otherwise directly. Command must be followed by 1byte address.
#define DS_IBUTTON_WRITE_APP_REG        0x99  // Write into the application scratchpad register if this is not locked
#define DS_IBUTTON_APP_REG_LOCK         0x5A  // Command to copy the application scratchpad into the register, and lock this
#define DS_IBUTTON_STATUS_REG_READ                    0x66

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

#define MAX_DEVICES 10       //!< Max number of devices to search for.


#endif // CONF_BOARD_H
