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

       
#define DEFAULT_CONVERSION_TIME RS_TIMER_T1_SEC
#define DEFAULT_TEMPERATURE_READ_TIME RS_TIMER_T1_SEC*60

static boolean ConversionStarted = FALSE;

        case ONEWIRE_TEMPERATURE_READ_TIMER:
        {

          OWI_device* DevicePtr = NULL; 
	  if(SearchBuses(DevicePtr, 1, 1) == SEARCH_SUCCESSFUL)
          {

            if(DevicePtr)
            {
              if(ConversionStarted)
              {       
                int16 Temp=0;
                ConversionStarted = FALSE;
 
                switch(DevicePtr->id[0])
                {
                  
                  case DS18B20_FAMILY_ID:                 
                  case DS18S20_FAMILY_ID:
                  {
                    Temp = DS1820_ReadTemperature(DevicePtr->bus, DevicePtr->id);
                    if(((Temp == TEMP_RESET_VALUE_B) || (Temp == TEMP_RESET_VALUE_S) || (Temp == TEMP_MAX_VALUE)) && (Counter < MAX_TEMP_READ_TIME))
                    {
                      Counter++;
                      if(DS1820_StartConversion(DevicePtr->bus, DevicePtr->id))
                      {
                        ConversionStarted = TRUE;
                        RsTimerStart(ONEWIRE_TEMPERATURE_READ_TIMER, 2*DEFAULT_CONVERSION_TIME);
                      }
                      else
                      {
                        Counter = 0;
                        RsTimerStart(ONEWIRE_TEMPERATURE_READ_TIMER, DEFAULT_TEMPERATURE_READ_TIME);            
                      }
                    }
                    else
                    {
                      uint8 DataLength = 8;
                      uint8 *Data = MemAlloc(OsCurrentTask, DataLength);
                      if(Data)
                      {
                        memcpy(Data, DevicePtr->id, DataLength);
                        OneWireTempInd(ClientTask, Temp, DataLength, Data); //Send indication with temperature ID and temperature
                      }
                    }
                  }
                  break;
                } 
              }
              else
              {
                switch(DevicePtr->id[0])
                {
                  case DS18B20_FAMILY_ID:
                  case DS18S20_FAMILY_ID:
                  {
                    if(DS1820_StartConversion(DevicePtr->bus, DevicePtr->id))
                    {
                      ConversionStarted = TRUE;
                      RsTimerStart(ONEWIRE_TEMPERATURE_READ_TIMER, DEFAULT_CONVERSION_TIME);
                    }
                  }
                  break;

                  default:
                  break;
                }
              } 
            }
          }



boolean DS1820_StartConversion(uint8 bus, uint8 * id)
{    
    // Reset, presence.
  boolean ReturnValue = FALSE;
  if(OWI_DetectPresence(bus))
  {
    // Match the id found earlier.
    OWI_MatchRom(id, bus);
    // Send start conversion command.
    if(id[0] != DS2438_FAMILY_ID)
    {
      OWI_SendByte(DS18S20_START_CONVERSION, bus);
    }
    else
    {
      OWI_SendByte(DS2438_START_CONVERSION_INTERNAL, bus);
    }
    OWI_RELEASE_BUS(bus);
    ReturnValue = TRUE;
  }
  return(ReturnValue);
}

int16 DS1820_ReadTemperature(uint8 bus, uint8 * id)
{
  int16 temperature = DS18S20_ERROR;
  uint8 bytesLeft = 0;
  uint8 Array[9];
  uint8 Counter = 0;

  if(OWI_DetectPresence(bus))
  {
      // Match id again.
    OWI_MatchRom(id, bus);
    // Send READ SCRATCHPAD command.
    switch(id[0])
    {
      case DS18B20_FAMILY_ID:
      case DS18S20_FAMILY_ID:
        OWI_SendByte(DS18S20_READ_SCRATCHPAD, bus);
        bytesLeft = 9;
        while (bytesLeft > 0)
        {
         // Place the received data in memory.
           Array[Counter++] = OWI_ReceiveByte(bus);
           bytesLeft--;
        }
        if(OWI_CheckScratcpadCRC(Array) == OWI_CRC_OK)
        {
          // Read only two first bytes (temperature low, temperature high)
          // and place them in the 16 bit temperature variable.
          temperature = Array[0];
          temperature |= (Array[1] << 8);
//          return(temperature);
        }
        else
        {
          temperature = DS18S20_CRC_ERROR;
        }
        break;
    
      default:
        temperature = DS18S20_ERROR;
        break;
    }
  }
  return(temperature);
}
