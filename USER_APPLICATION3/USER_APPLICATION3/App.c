/*! \brief  Perform a 1-Wire search
 *
 *  This function shows how the OWI_SearchRom function can be used to 
 *  discover all slaves on the bus. It will also CRC check the 64 bit
 *  identifiers.
 *
 *  \param  devices Pointer to an array of type OWI_device. The discovered 
 *                  devices will be placed from the beginning of this array.
 *
 *  \param  len     The length of the device array. (Max. number of elements).
 *
 *  \param  buses   Bitmask of the buses to perform search on.
 *
 *  \retval SEARCH_SUCCESSFUL   Search completed successfully.
 *  \retval SEARCH_CRC_ERROR    A CRC error occured. Probably because of noise
 *                              during transmission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <asf.h>
//#include <OWIHighLevelFunctions.h>
//#include <OWIBitFunctions.h>
//#include <OWIcrc.h>
#include <conf_board.h>

bool OneWireIDValid(uint8_t FamilyID)
{
	bool ReturnValue = false;
	switch(FamilyID)
	{
		//case DS18S20_FAMILY_ID:
		//case DS18B20_FAMILY_ID:
		//case DS2438_FAMILY_ID:
		case DS1990_FAMILY_ID:
		case DS1991_FAMILY_ID:
		case DS1971_FAMILY_ID:
		case DS1982_FAMILY_ID:
		case DS2413_FAMILY_ID:
		case DS2431_FAMILY_ID:
		ReturnValue = true;
		break;

		default:
		break;
	}
	return(ReturnValue);
}


uint8_t SearchBuses(OWI_device * NewDevices, uint8_t len, uint8_t buses)
{
    uint8_t i;//, j;
    uint8_t presence;
    uint8_t * newID;
    uint8_t * currentID;
    uint8_t currentBus;
    uint8_t lastDeviation;
    uint8_t numDevices;
    uint8_t ReturnValue = SEARCH_NO_UNIT_FOUND;
    // Initialize all addresses as zero, on bus 0 (does not exist).
    // Do a search on the bus to discover all addresses.    
    for (i = 0; i < len; i++)
    {
      memset(&NewDevices[i], 0, sizeof(OWI_device));

/*      devices[i].bus = 0x00;
      devices[i].CRC_Failure = FALSE;
      devices[i].Presence = FALSE;
      for (j = 0; j < 8; j++)
      {
        devices[i].id[j] = 0x00;
      }*/
    }
    
    // Find the buses with slave devices.
//    presence = OWI_DetectPresence(BUSES);
    
    numDevices = 0;
    newID = NewDevices[0].id;
    
    // Go through all buses with slave devices.
    for (currentBus = 0x01; currentBus; currentBus <<= 1)
    {
      if(currentBus & buses)
      {
        lastDeviation = 0;
        currentID = newID;
        presence = OWI_DetectPresence(currentBus);
        if (currentBus && presence) // Devices available on this bus.
        {
            // Do slave search on each bus, and place identifiers and corresponding
            // bus "addresses" in the array.
            do  
            {
              if(presence)
              {
		uint8_t TestCounter = 0x0A;
                lastDeviation = OWI_SearchRom(newID, lastDeviation, currentBus);
		while((lastDeviation == OWI_ROM_SEARCH_FAILED) && presence && TestCounter)
		{
		  presence = OWI_DetectPresence(currentBus);
		  TestCounter--;
		  lastDeviation = 0;
		  lastDeviation = OWI_SearchRom(newID, lastDeviation, currentBus); // Try again
		} 
		/*if(!TestCounter)
		{
		  lastDeviation = OWI_ROM_SEARCH_FAILED;
		}*/
                currentID = newID;                
                if((numDevices < len) && (lastDeviation!=OWI_ROM_SEARCH_FAILED))
                {
                  if(OWI_CheckRomCRC(newID))
                  {
                    lastDeviation = OWI_ROM_SEARCH_FINISHED;
		    //ReturnValue = SEARCH_CRC_ERROR;
		    NewDevices[numDevices].CRC_Failure = true;
                  }
                  //else if(OneWireIDValid(*newID))
		  {
                    NewDevices[numDevices].CRC_Failure = false;
                    NewDevices[numDevices].bus = currentBus;
                    NewDevices[numDevices].Presence = true;
                    numDevices++;
		    if(numDevices < len)
		    {
                      newID = NewDevices[numDevices].id;
                      //ReturnValue = SEARCH_SUCCESSFUL;                    
		    }
		    else
		    {
		      lastDeviation = OWI_ROM_SEARCH_FINISHED;
		    }
		  }
                  if(lastDeviation != OWI_ROM_SEARCH_FINISHED)
                  {
                    presence = OWI_DetectPresence(currentBus);
                    memcpy(newID, currentID, 8);                
                  }
                }
                else
                {
                  lastDeviation = OWI_ROM_SEARCH_FINISHED;
                }
              }
              else
              {
                lastDeviation = OWI_ROM_SEARCH_FINISHED;
              }
            } while(lastDeviation != OWI_ROM_SEARCH_FINISHED);            
          ReturnValue = SEARCH_SUCCESSFUL;
        }
      }
    }
    return(ReturnValue);
//    return SEARCH_SUCCESSFUL;
}

OWI_device *FindFamily(uint8_t familyID, OWI_device * devices, uint8_t size)
{
    uint8_t i = 0;
    
    // Search through the array.
    while (i < size)
    {
        // Return the pointer if there is a family id match.
        if ((*devices).id[0] == familyID)
        {
            return devices;
        }
        devices++;
        i++;
    }
    // Else, return NULL.
    return NULL;
}
