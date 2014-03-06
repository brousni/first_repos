#include <asf.h>
#include <string.h>
#include <afec.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <OWIBitFunctions.h>
#include <OWIHighLevelFunctions.h>
#include <OWIcrc.h>

#define TASK_QUEUE_SIZE 10

typedef struct MailTypeStruct
{
	uint16_t Timeout;
	unsigned portBASE_TYPE uxHighWaterMark;
}MailTypeStruct;

typedef struct
{
	uint8_t			ProtocolVersion;		// 1
	uint8_t			SoftwareVersion;		// 50
	uint8_t			BootType;				// 0
	uint8_t			MessageID;				// 4
	uint32_t		SNO;					// 500000
	uint32_t		Time;
	int32_t			XPos;
	int32_t			YPos;
	uint32_t		KM;						// 0
	uint8_t			PosQ_No;				// 0..255
	uint8_t			GPSSpeed;				// 0
	uint16_t		Direction:9;			// 0
	uint16_t		Settings:7;				// 0x03
	uint8_t			SAT;
	uint8_t			FOM;					// 30
	uint8_t			IO;
	uint8_t	 		AD1;					//
	uint8_t			AD2;					//
	uint8_t	 		GSM;					// 31
	uint16_t		COPS;					// 23802
	uint16_t		ODORun;					// 0
	uint16_t		Run1;					// 0
	uint16_t		Run2;					// 0
	uint16_t		RunMin;					// 0
} ME_Data_t;								// 44 bytes data

ME_Data_t SMS_Value = {1,50,0,4,500000,0,0,0,0,0,0,0,0x03,0,30,0,0,0,31,23802,0,0,0,0};

typedef enum TaskEnumType
{
	init_case = 0,
	GSM_case,
	GPS_case,
	IO_case
}TaskEnumType;

xQueueHandle GSM_case_Queue = 0;
xQueueHandle GPS_case_Queue = 0;
xQueueHandle IO_case_Queue = 0;
xQueueHandle init_case_Queue = 0;

// SERIAL DEFINES FOR GPS
#define UART_SERIAL1				UART1
#define UART_SERIAL1_ID				ID_UART1
#define UART_SERIAL1_BAUDRATE		9600
#define UART_SERIAL1_CHAR_LENGTH	US_MR_CHRL_8_BIT
#define UART_SERIAL1_PARITY			US_MR_PAR_NO
#define UART_SERIAL1_STOP_BIT		US_MR_NBSTOP_1_BIT

static usart_serial_options_t uart1_options = {
	UART_SERIAL1_BAUDRATE,
	UART_SERIAL1_CHAR_LENGTH,
	UART_SERIAL1_PARITY,
	UART_SERIAL1_STOP_BIT
};

// SERIAL DEFINES FOR GSM
#define USART_SERIAL1				USART1
#define USART_SERIAL1_ID			ID_USART1
#define USART_SERIAL1_BAUDRATE		9600 // lige huske at der er noget prescaling her
#define USART_SERIAL1_CHAR_LENGTH	US_MR_CHRL_8_BIT
#define USART_SERIAL1_PARITY		US_MR_PAR_NO
#define USART_SERIAL1_STOP_BIT		US_MR_NBSTOP_1_BIT

const sam_usart_opt_t usart_console_settings = {
	USART_SERIAL1_BAUDRATE,
	USART_SERIAL1_CHAR_LENGTH,
	USART_SERIAL1_PARITY,
	USART_SERIAL1_STOP_BIT,
	US_MR_CHMODE_NORMAL,
	0
};

// SERIAL DEFINES FOR RS232
#define UART_SERIAL0				UART0
#define UART_SERIAL0_ID				ID_UART0
#define UART_SERIAL0_BAUDRATE		9600
#define UART_SERIAL0_CHAR_LENGTH	US_MR_CHRL_8_BIT
#define UART_SERIAL0_PARITY			US_MR_PAR_NO
#define UART_SERIAL0_STOP_BIT		US_MR_NBSTOP_1_BIT

static usart_serial_options_t uart0_options = {
	UART_SERIAL0_BAUDRATE,
	UART_SERIAL0_CHAR_LENGTH,
	UART_SERIAL0_PARITY,
	UART_SERIAL0_STOP_BIT
};

// SPI

//General Variables
long int		init_cnt = 0;
long int		get_hz = 0;
long int		cnt1 = 0;
long int		cnt2 = 0;
long int		ADC_IN1 = 0;
long int		ADC_IN2 = 0;
long int		ADC_IN3 = 0;
long int		ADC_IN4 = 0;
long int		VIN_ADC = 0;
int				reset_flag = 0;
int				cnt1_flag = 0;
int				cnt2_flag = 0;
uint8_t			init_chs = 0;
int				init_flag = 0;
long int		adc_cnt = 0;
long int		adc_get_in = 0;
char			datastr[1000] = "";

uint_fast32_t	rs232= 0;
uint_fast32_t	usartvar = 0;
int				sepcnt = 0;
long int		bufndx = 0;

//GSM Variables
long int		GSM_tick_cnt = 0;
int				smscnt = 0;
char			phnnmb[8] = "";
int				phnnmb_cnt = 0;
char			sms[160];
int				smslen = 7;
int				sms_flag = 0;
int				GSM_init_flag = 0;
int				GSM_wflag = 1;
int				GSM_atcmd_flag = 1;
int				GSM_phnnmb_flag = 0;
uint8_t			received_byte_GSM = 0;
int				GSM_smslen_cnt = 0;
uint_fast32_t	wptr_GSM = 0;
uint_fast32_t	rptr_GSM = 0;
char			instring_GSM[1000] = "";
uint8_t			GSM_chs = 0;
char			GSM_sms[160] = "";
int				GSM_cstmsms_flag = 0;
int				GSM_reset_flag = 0;
int				send_cstm_sms_flag = 0;
int				GSM_seq_flag = 0;

//GPS Variables
long int		GPS_tick_cnt = 0;
int				GPS_status = 0;
int				GPS_start_flag = 0;
int				GPS_wflag = 0;
int				GPS_fix_flag = 0;
int				GPS_send_flag = 0;
uint8_t			received_byte_GPS = 0;
uint_fast32_t	wptr_GPS = 0;
uint_fast32_t	rptr_GPS = 0;
char			instring_GPS[1000] = "";
long int		str_ndx_GPS = 0;
uint8_t			GPS_chs = 0;
//Latitude/longitude
int				llsepamnt1 = 2;
int				llsepamnt2 = 1;
int				oldllndx = 0;
char			latitude[8];
char			longitude[9];
char			llsgn1 = 71;
char			llsgn2 = 71;
char			llsgn3 = 65;
char			llbuf[1000] = "";
char			separator = 44;
uint32_t		smslongitude = 0;
uint32_t		smslatitude	= 0;
//Time
int				utcsepamnt1 = 1;
int				utcsepamnt2 = 8;
int				oldutcndx = 0;
char			utctime[12] = "";
char			utcsgn1 = 82;
char			utcsgn2 = 77;
char			utcsgn3 = 67;
char			utcbuf[1000] = "";
uint32_t		DDMM = 0;
uint32_t		HHMMSS = 0;
//Satellites
int				satsepamnt1 = 1;
int				oldsatndx = 0;
char			sat_nmb = 0;
char			satsgn1 = 71;
char			satsgn2 = 83;
char			satsgn3 = 86;
char			satbuf[10] = "";

//IO Variables
long int		IO_cnt = 0;
long int		out1_cnt = 0;
uint8_t			IO_chs = 0;
int				out1 = 0;
uint8_t			IO_status = 0;
uint32_t		IO_VIN_CALC = 0;

//1-WIRE
long int		ow_cnt = 0;
OWI_device		OWI_test_dev;
static bool		ConversionStarted = false;
long int			Counter = 0;

//RS232 Variables
char			instring_RS232[1000] = "";
uint_fast32_t	wptr_RS232 = 0;
uint_fast32_t	rptr_RS232 = 0;
uint8_t			received_byte_RS232 = 0;

//AFEC0 Variables
struct afec_config afec0_cfgdef;
struct afec_ch_config afec0_chdef;
int				afec0_chn_nmb = 0;

//SPI Variables
struct spi_device spi_acc;
uint8_t			spi_data;

// Function prototypes
void test_task(void* taskparam);

bool DS1820_StartConversion(uint8_t bus, uint8_t * id);

int16_t DS1820_ReadTemperature(uint8_t bus, uint8_t * id);

uint8_t SearchBuses(OWI_device * NewDevices, uint8_t len, uint8_t buses);

void UDP2SMS( uint8_t *buffer, char *StrVar, uint16_t MaxSize );

void adc0_callback(void);

//UDP Packing
void UDP2SMS( uint8_t *buffer, char *StrVar, uint16_t MaxSize )
{
	uint8_t i,j;
	uint8_t c1,c2;

	j = 0;
	for (i = 0; i < MaxSize; ++i)
	{
		c1 = (buffer[i] & 0x0F) + 97;
		c2 = ((buffer[i] & 0xF0) / 16) + 65;
		StrVar[j] = c2;
		j++;
		StrVar[j] = c1;
		j++;
	}
	StrVar[j] = 0;
}

#define Prot_v1_Size	44

ME_Data_t	ME_Data;
char		SmsStr[161];
char		UnsStr[161];
char		SmsTel[16] = "1272";
uint32_t		DT;


void test_task(void* taskparam)
{
	MailTypeStruct QueueItem;
	
	for(;;)
	{
		switch((TaskEnumType) taskparam)
		{
			case init_case:
			xQueueReceive(init_case_Queue, &QueueItem, portMAX_DELAY);
			while(1)
			{
				// Write menu options when GPS on, fix found and GSM initialised
				if(GSM_init_flag == 0 && GPS_fix_flag == 1 && GPS_start_flag == 1 && init_flag == 1)
				{
					usart_serial_write_packet(UART_SERIAL0, "\n\rInit/Status Mode:\n\r\n\r", strlen("\n\rInit/Status Mode:\n\r\n\r"));
					usart_serial_write_packet(UART0, "Press 1 to go to GPS Mode\n\r", strlen("Press 1 to go to GPS Mode\n\r"));
					usart_serial_write_packet(UART0, "Press 2 to go to GSM Mode\n\r", strlen("Press 2 to go to GSM Mode\n\r"));
					usart_serial_write_packet(UART0, "Press 3 to go to I/O Mode\n\r", strlen("Press 3 to go to I/O Mode\n\r"));
					usart_serial_write_packet(UART_SERIAL0, "\n\rConfirm with Enter\n\r\n\r", strlen("\n\rConfirm with Enter\n\r\n\r"));
					init_flag = 0;
				}
				if(reset_flag == 1)
				{
					// BATT OFF
					pio_set_output(PIOD, PIO_PD19, LOW, DISABLE, DISABLE);
					// MODEM OFF
					pio_set_output(PIOB, PIO_PB14, LOW, DISABLE, DISABLE);
					init_flag = 0;
					GPS_wflag = 0;
					cnt1_flag = 0;
					cnt2_flag = 0;
					sms_flag = 0;
					GSM_init_flag = 0;
					GSM_wflag = 1;
					GSM_atcmd_flag = 1;
					GSM_phnnmb_flag = 0;
					GSM_reset_flag = 0;
					send_cstm_sms_flag = 0;
					GSM_seq_flag = 0;
					GPS_start_flag = 0;
					GPS_wflag = 0;
					GPS_fix_flag = 0;
					GPS_send_flag = 0;
					adc_cnt = 0;
					ow_cnt = 0;
					GSM_tick_cnt = 0;
					GPS_tick_cnt = 0;
					cnt1 = 0;
					cnt2 = 0;
					IO_cnt = 0;
					GPS_chs = 0;
					GSM_chs = 0;
					IO_chs = 0;
					init_chs = 0;
					reset_flag = 0;
				}
				if(instring_RS232[wptr_RS232-1] == 13)
				{
					{
						memcpy(&init_chs, &instring_RS232[wptr_RS232-2], 1);
						if(wptr_RS232 == 0)
						{
							memcpy(&init_chs, instring_RS232[998], 1);
						}
						memset(instring_RS232, 0, 1000);
						wptr_RS232 = 0;
					}
				}
				// if 1 go to gps-mode
				if(init_chs == 49)
				{
					init_chs = 0;
					xQueueSendToBack(GPS_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(init_chs == 50)
				{
					init_chs = 0;
					xQueueSendToBack(GSM_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(init_chs == 51)
				{
					init_chs = 0;
					xQueueSendToBack(IO_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(ioport_get_pin_level(PIO_PD23_IDX))
				{
					pio_set_output(PIOB, PIO_PB13, HIGH, DISABLE, DISABLE);
					if(GPS_fix_flag == 0 && GPS_start_flag == 1)
					{
						usart_serial_write_packet(UART0, "GPS Fix Found\n\r", strlen("GPS Fix Found\n\r"));
						GPS_fix_flag = 1;
					}
				}
				if(!ioport_get_pin_level(PIO_PD23_IDX))
				{
					pio_set_output(PIOB, PIO_PB13, LOW, DISABLE, DISABLE);
				}
				// Hvis GPS status-ON er høj. Tænd grøn diode
				if(ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 0)
				{
					pio_set_output(PIOD, PIO_PD29, HIGH, DISABLE, DISABLE);
					usart_serial_write_packet(UART0, "GPS On\n\r", strlen("GPS On\n\r"));
					vTaskDelay(1000 / portTICK_RATE_MS);
					GPS_start_flag = 1;
				} // Sluk grøn diode hvis status-ON er lav
				if(!ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 1)
				{
					pio_set_output(PIOD, PIO_PD29, LOW, DISABLE, DISABLE);
					GPS_start_flag = 0;
					usart_serial_write_packet(UART0, "GPS Off\n\r", strlen("GPS Off\n\r"));
				}
				if(GSM_init_flag == 1)
				{
					usart_serial_write_packet(USART1, "at\+cmgf\=1\r", strlen("at\+cmgf\=1\r"));
					vTaskDelay(1000 / portTICK_RATE_MS);
					//usart_serial_write_packet(USART1, "at\^spow\=1,0,0\r", strlen("at\^spow\=1,0,0\r"));
					//vTaskDelay(1000 / portTICK_RATE_MS);
					usart_serial_write_packet(USART1, "at\+cmee=2\r", strlen("at\+cmee=2\r\r"));
					vTaskDelay(1000 / portTICK_RATE_MS);
					//usart_serial_write_packet(USART1, "at\+creg\=2\r", strlen("at\+creg\=2\r"));
					//vTaskDelay(1000 / portTICK_RATE_MS);
					//usart_serial_write_packet(USART1, "at\+cgsn\r", strlen("at\+cgsn\r"));
					//vTaskDelay(1000 / portTICK_RATE_MS);
					//usart_serial_write_packet(USART1, "at\+ccid\r", strlen("at\+ccid\r"));
					//vTaskDelay(1000 / portTICK_RATE_MS);
					//usart_serial_write_packet(USART1, "at\+cops\=\?\r", strlen("at\+cops\=\?\r"));
					//vTaskDelay(1000 / portTICK_RATE_MS);
					//usart_serial_write_packet(USART1, "AT\+IPR\=9600\r", strlen("AT\+IPR\=9600\r"));
					//vTaskDelay(1000 / portTICK_RATE_MS);
					usart_serial_write_packet(UART0, "GSM Initialised\r\n", strlen("GSM Initialised\r\n"));
					GSM_init_flag = 0;
					init_flag = 1;
				}
				
				
				for(str_ndx_GPS = rptr_GPS; str_ndx_GPS != wptr_GPS; str_ndx_GPS++)
				{
					if(str_ndx_GPS > 999)
					{
						str_ndx_GPS = 0;
					}
					// FIND UTC
					if(instring_GPS[str_ndx_GPS] == utcsgn1 && instring_GPS[str_ndx_GPS+1] == utcsgn2 && instring_GPS[str_ndx_GPS+2] == utcsgn3)
					{
						str_ndx_GPS++;
						if(str_ndx_GPS > 999)
						{
							str_ndx_GPS = 0;
						}
						for(sepcnt = 0; sepcnt < utcsepamnt1; str_ndx_GPS++)
						{
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							if(instring_GPS[str_ndx_GPS] == separator)
							{
								sepcnt++;
							}
						}
						if(sepcnt == utcsepamnt1)
						{
							sepcnt = 0;
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							while(instring_GPS[str_ndx_GPS] != separator)
							{
								datastr[bufndx] = instring_GPS[str_ndx_GPS];
								bufndx++;
								str_ndx_GPS++;
								if(str_ndx_GPS > 999)
								{
									str_ndx_GPS = 0;
								}
							}
						}
						// FIND DATE
						for(sepcnt = 0; sepcnt < utcsepamnt2; str_ndx_GPS++)
						{
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							if(instring_GPS[str_ndx_GPS] == separator)
							{
								sepcnt++;
							}
						}
						if(sepcnt == utcsepamnt2)
						{
							sepcnt = 0;
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							while(instring_GPS[str_ndx_GPS] != separator)
							{
								datastr[bufndx] = instring_GPS[str_ndx_GPS];
								bufndx++;
								str_ndx_GPS++;
								if(str_ndx_GPS > 999)
								{
									str_ndx_GPS = 0;
								}
							}
						}
						memcpy(utctime, datastr, 6);
						oldutcndx = bufndx;
						memcpy(&utctime[6], &datastr[10], 6);
						memset(utcbuf, 0, 1000);
						memcpy(utcbuf, datastr, oldutcndx);
						bufndx = 0;
						memset(datastr, 0, 1000);
					}
					//Find Number of Satellites
					if(instring_GPS[str_ndx_GPS] == satsgn1 && instring_GPS[str_ndx_GPS+1] == satsgn2 && instring_GPS[str_ndx_GPS+2] == satsgn3)
					{
						str_ndx_GPS++;
						if(str_ndx_GPS > 999)
						{
							str_ndx_GPS = 0;
						}
						for(sepcnt = 0; sepcnt < satsepamnt1; str_ndx_GPS++)
						{
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							if(instring_GPS[str_ndx_GPS] == separator)
							{
								sepcnt++;
							}
						}
						if(sepcnt == satsepamnt1)
						{
							sepcnt = 0;
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							while(instring_GPS[str_ndx_GPS] != separator)
							{
								datastr[bufndx] = instring_GPS[str_ndx_GPS];
								bufndx++;
								str_ndx_GPS++;
								if(str_ndx_GPS > 999)
								{
									str_ndx_GPS = 0;
								}
							}
						}
						memcpy(&sat_nmb, &datastr[0], 1);
						memset(satbuf, 0, 10);
						bufndx = 0;
						memset(datastr, 0, 1000);
						SMS_Value.SAT = (sat_nmb - 48);
					}
					// Find LL
					if(instring_GPS[str_ndx_GPS] == llsgn1 && instring_GPS[str_ndx_GPS+1] == llsgn2 && instring_GPS[str_ndx_GPS+2] == llsgn3)
					{
						str_ndx_GPS++;
						if(str_ndx_GPS > 999)
						{
							str_ndx_GPS = 0;
						}
						// Find Latitude
						for(sepcnt = 0; sepcnt < llsepamnt1; str_ndx_GPS++)
						{
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							if(instring_GPS[str_ndx_GPS] == separator)
							{
								sepcnt++;
							}
						}
						if(sepcnt == llsepamnt1)
						{
							sepcnt = 0;
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							while(instring_GPS[str_ndx_GPS] != separator)
							{
								datastr[bufndx] = instring_GPS[str_ndx_GPS];
								bufndx++;
								str_ndx_GPS++;
								if(str_ndx_GPS > 999)
								{
									str_ndx_GPS = 0;
								}
							}
						}
						// Find N/S
						for(sepcnt = 0; sepcnt < llsepamnt2; str_ndx_GPS++)
						{
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							if(instring_GPS[str_ndx_GPS] == separator)
							{
								sepcnt++;
							}
						}
						if(sepcnt == llsepamnt2)
						{
							sepcnt = 0;
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							while(instring_GPS[str_ndx_GPS] != separator)
							{
								datastr[bufndx] = instring_GPS[str_ndx_GPS];
								bufndx++;
								str_ndx_GPS++;
								if(str_ndx_GPS > 999)
								{
									str_ndx_GPS = 0;
								}
							}
						}
						// Find Longitude
						for(sepcnt = 0; sepcnt < llsepamnt2; str_ndx_GPS++)
						{
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							if(instring_GPS[str_ndx_GPS] == separator)
							{
								sepcnt++;
							}
						}
						if(sepcnt == llsepamnt2)
						{
							sepcnt = 0;
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							while(instring_GPS[str_ndx_GPS] != separator)
							{
								datastr[bufndx] = instring_GPS[str_ndx_GPS];
								bufndx++;
								str_ndx_GPS++;
								if(str_ndx_GPS > 999)
								{
									str_ndx_GPS = 0;
								}
							}
						}
						// Find E/W
						for(sepcnt = 0; sepcnt < llsepamnt2; str_ndx_GPS++)
						{
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							if(instring_GPS[str_ndx_GPS] == separator)
							{
								sepcnt++;
							}
						}
						if(sepcnt == llsepamnt2)
						{
							sepcnt = 0;
							if(str_ndx_GPS > 999)
							{
								str_ndx_GPS = 0;
							}
							while(instring_GPS[str_ndx_GPS] != separator)
							{
								datastr[bufndx] = instring_GPS[str_ndx_GPS];
								bufndx++;
								str_ndx_GPS++;
								if(str_ndx_GPS > 999)
								{
									str_ndx_GPS = 0;
								}
							}
						}
						memcpy(latitude, datastr, 4);
						memcpy(&latitude[4], &datastr[5], 4);
						memcpy(longitude, &datastr[10], 5);
						memcpy(&longitude[5], &datastr[16], 4);
						oldllndx = bufndx;
						memset(llbuf, 0, 1000);
						memcpy(llbuf, datastr, oldllndx);
						bufndx = 0;
						memset(datastr, 0, 1000);
						{
							uint8_t i;
							int32_t Valuelong = 0;
							int32_t Valuelat = 0;
							for(i=0; i<sizeof(longitude); i++)
							{
								Valuelong*=10;
								Valuelong += longitude[i] - 0x30;
							}
							SMS_Value.XPos = Valuelong;
							for(i=0; i<sizeof(latitude); i++)
							{
								Valuelat*=10;
								Valuelat += latitude[i] - 0x30;
							}
							SMS_Value.YPos = Valuelat;
						}
						{
							uint8_t i;
							int32_t ValueDDMM = 0;
							int32_t ValueHHMMSS = 0;
							for(i=6; i<10; i++)
							{
								ValueDDMM*=10;
								ValueDDMM += utctime[i] - 0x30;
							}
							SMS_Value.Time = ValueDDMM * 262144;
							for(i=0; i<6; i++)
							{
								ValueHHMMSS*=10;
								ValueHHMMSS += utctime[i] - 0x30;
							}
							SMS_Value.Time += ValueHHMMSS;
						}
						
					}
				}
				rptr_GPS = wptr_GPS;
				
				//TEMP READ
				if(ConversionStarted)
				{
					vTaskDelay(1500 / portTICK_RATE_MS);
					int16_t Temp=0;
					ConversionStarted = false;
					
					switch(OWI_test_dev.id[0])
					{
						
						case DS18B20_FAMILY_ID:
						case DS18S20_FAMILY_ID:
						{
							Temp = DS1820_ReadTemperature(OWI_test_dev.bus, OWI_test_dev.id);
							if(((Temp == TEMP_RESET_VALUE_B) || (Temp == TEMP_RESET_VALUE_S) || (Temp == TEMP_MAX_VALUE)) && (Counter < 10))
							{
								Counter++;
								if(DS1820_StartConversion(OWI_test_dev.bus, OWI_test_dev.id))
								{
									ConversionStarted = true;
								}
								else
								{
									Counter = 0;
								}
							}
							else
							{
								int32_t Temp32 = Temp;
								Temp32*=625;
								Temp32/=10000;
								SMS_Value.BootType = (uint8_t)Temp32;
							}
						}
						break;
					}
				}
				if(cnt2_flag == 1 || sms_flag == 1)
				{
					memcpy ( ( char * ) SmsStr, ( char * ) &SMS_Value, Prot_v1_Size );
					UDP2SMS( ( uint8_t * )SmsStr, UnsStr, Prot_v1_Size );
					strcpy ( SmsStr, "MTC BINRES ");
					strcat ( SmsStr, UnsStr);
					
					SMS_Value.PosQ_No++;
					{
						if(SMS_Value.PosQ_No > 255)
						{
							SMS_Value.PosQ_No = 0;
						}
					}
					usart_serial_write_packet(USART_SERIAL1, "AT+CMGS=\"", strlen("AT+CMGS=\""));
					usart_serial_write_packet(USART_SERIAL1, SmsTel, 4);
					usart_serial_write_packet(USART_SERIAL1, "\"\r", strlen("\"\r"));
					vTaskDelay(100 / portTICK_RATE_MS);
					usart_serial_write_packet(USART_SERIAL1, SmsStr, strlen(SmsStr));
					vTaskDelay(250 / portTICK_RATE_MS);
					usart_serial_write_packet(USART_SERIAL1, "\032", strlen("\032"));
					vTaskDelay(2000);
					smscnt++;
					cnt2_flag = 0;
					sms_flag = 0;
					usart_serial_write_packet(UART0, "SMS sent to Gateway", strlen("SMS sent to Gateway"));
					xQueueSendToBack(GSM_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(send_cstm_sms_flag == 1)
				{
					usart_serial_write_packet(USART_SERIAL1, "AT+CMGS=\"", strlen("AT+CMGS=\""));
					vTaskDelay(250 / portTICK_RATE_MS);
					usart_serial_write_packet(USART_SERIAL1, phnnmb, 8);
					vTaskDelay(250 / portTICK_RATE_MS);
					usart_serial_write_packet(USART_SERIAL1, "\"\r", strlen("\"\r"));
					vTaskDelay(250 / portTICK_RATE_MS);
					usart_serial_write_packet(USART_SERIAL1, GSM_sms, GSM_smslen_cnt);
					vTaskDelay(250 / portTICK_RATE_MS);
					usart_serial_write_packet(USART_SERIAL1, "\032", strlen("\032"));
					send_cstm_sms_flag = 0;
					vTaskDelay(2000 / portTICK_RATE_MS);
					usart_serial_write_packet(UART0, "\n\r", strlen("\n\r"));
					usart_serial_write_packet(UART0, GSM_sms, GSM_smslen_cnt);
					usart_serial_write_packet(UART0, " was sent to receiver\: ", strlen(" was sent to receiver\: "));
					usart_serial_write_packet(UART0, phnnmb, 8);
					usart_serial_write_packet(UART0, "\n\r", strlen("\n\r"));
					xQueueSendToBack(GSM_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
			}
			break;
			
			case GPS_case:
			xQueueReceive(GPS_case_Queue, &QueueItem, portMAX_DELAY);
			usart_serial_write_packet(UART0, "\n\rGPS Mode:\n\r\n\r", strlen("\n\rGPS Mode:\n\r\n\r"));
			usart_serial_write_packet(UART0, "Press 1 to Print Full NMEA. Press 1 again to stop\n\r", strlen("Press 1 to Print Full NMEA. Press 1 again to stop\n\r"));
			usart_serial_write_packet(UART0, "Press 2 to go to GSM Mode\n\r", strlen("Press 2 to go to GSM Mode\n\r"));
			usart_serial_write_packet(UART0, "Press 3 to go to I/O Mode\n\r", strlen("Press 3 to go to I/O Mode\n\r"));
			usart_serial_write_packet(UART0, "Press 0 to Reset to Init/Status Mode\n\r", strlen("Press 0 to Reset to Init/Status Mode\n\r"));
			usart_serial_write_packet(UART_SERIAL0, "\n\rConfirm with Enter\n\r\n\r", strlen("\n\rConfirm with Enter\n\r\n\r"));
			GPS_chs = 0;
			GPS_wflag = 0;
			while(1)
			{
				if(!ioport_get_pin_level(PIO_PD23_IDX))
				{
					pio_set_output(PIOB, PIO_PB13, LOW, DISABLE, DISABLE);
				}
				// Hvis GPS status-ON er høj. Tænd grøn diode
				if(ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 0)
				{
					pio_set_output(PIOD, PIO_PD29, HIGH, DISABLE, DISABLE);
					usart_serial_write_packet(UART0, "GPS On\n\r", strlen("GPS On\n\r"));
					vTaskDelay(1000 / portTICK_RATE_MS);
					GPS_start_flag = 1;
				} // Sluk grøn diode hvis status-ON er lav
				if(!ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 1)
				{
					pio_set_output(PIOD, PIO_PD29, LOW, DISABLE, DISABLE);
					GPS_start_flag = 0;
					usart_serial_write_packet(UART0, "GPS Off\n\r", strlen("GPS Off\n\r"));
				}
				if(cnt2_flag == 1)
				{
					GPS_wflag = 0;
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(sms_flag == 1)
				{
					GPS_wflag = 0;
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(instring_RS232[wptr_RS232-1] == 13)
				{
					{
						memcpy(&GPS_chs, &instring_RS232[wptr_RS232-2], 1);
						if(wptr_RS232 == 0)
						{
							memcpy(&GPS_chs, instring_RS232[998], 1);
						}
						memset(instring_RS232, 0, 1000);
						wptr_RS232 = 0;
					}
				}
				// if 1 Print NMEA or STOP IT!
				if(GPS_chs == 49 && GPS_wflag == 0)
				{
					GPS_wflag = 1;
					GPS_chs = 0;
				}
				if(GPS_chs == 49 && GPS_wflag == 1)
				{
					GPS_wflag = 0;
					GPS_chs = 0;
					xQueueSendToBack(GPS_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				// on 2 Go to GSM Mode
				if(GPS_chs == 50)
				{
					GPS_chs = 0;
					GPS_wflag = 0;
					xQueueSendToBack(GSM_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(GPS_chs == 51)
				{
					GPS_chs = 0;
					GPS_wflag = 0;
					xQueueSendToBack(IO_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(GPS_chs == 48)
				{
					GPS_chs = 0;
					reset_flag = 1;
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
			}
			break;
			
			case GSM_case:
			xQueueReceive(GSM_case_Queue, &QueueItem, portMAX_DELAY);
			usart_serial_write_packet(UART0, "\n\rGSM Mode:\n\r\n\r", strlen("\n\rGSM Mode:\n\r\n\r"));
			usart_serial_write_packet(UART0, "Press 1 to go to I/O Mode\n\r", strlen("Press 1 to go to I/O Mode\n\r"));
			usart_serial_write_packet(UART0, "Press 2 to go to GPS Mode\n\r", strlen("Press 2 to go to GPS Mode\n\r"));
			usart_serial_write_packet(UART0, "Press 3 to turn On/Off Direct MODEM connection Mode Mode\n\r", strlen("Press 3 to turn On/Off Direct MODEM connection Mode Mode\n\r"));
			usart_serial_write_packet(UART0, "Press 4 to send GPS-data to gateway\n\r", strlen("Press 4 to send GPS-data to gateway\n\r"));
			usart_serial_write_packet(UART0, "Press 5 to write SMS\n\r", strlen("Press 5 to write SMS\n\r"));
			usart_serial_write_packet(UART0, "Press 6 to set receiver number\n\r", strlen("Press 6 to set receiver number\n\r"));
			usart_serial_write_packet(UART0, "Press 7 to send sms to receiver\n\r", strlen("Press 7 to send sms to receiver\n\r"));
			usart_serial_write_packet(UART0, "Press 8 to turn sequential position-sms ON/OFF \n\r", strlen("Press 8 to turn sequential position-sms ON/OFF \n\r"));
			usart_serial_write_packet(UART0, "Press 0 to Reset to Init/Status Mode\n\r", strlen("Press 0 to Reset to Init/Status Mode\n\r"));
			usart_serial_write_packet(UART_SERIAL0, "\n\rConfirm with Enter\n\r\n\r", strlen("\n\rConfirm with Enter\n\r\n\r"));
			sms_flag = 0;
			GSM_chs = 0;
			while(1)
			{
				if(!ioport_get_pin_level(PIO_PD23_IDX))
				{
					pio_set_output(PIOB, PIO_PB13, LOW, DISABLE, DISABLE);
				}
				// Hvis GPS status-ON er høj. Tænd grøn diode
				if(ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 0)
				{
					pio_set_output(PIOD, PIO_PD29, HIGH, DISABLE, DISABLE);
					usart_serial_write_packet(UART0, "GPS On\n\r", strlen("GPS On\n\r"));
					vTaskDelay(1000 / portTICK_RATE_MS);
					GPS_start_flag = 1;
				} // Sluk grøn diode hvis status-ON er lav
				if(!ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 1)
				{
					pio_set_output(PIOD, PIO_PD29, LOW, DISABLE, DISABLE);
					GPS_start_flag = 0;
					usart_serial_write_packet(UART0, "GPS Off\n\r", strlen("GPS Off\n\r"));
				}
				if(sms_flag == 1)
				{
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(cnt2_flag == 1)
				{
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(GSM_reset_flag == 1)
				{
					GSM_reset_flag = 0;
					xQueueSendToBack(GSM_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(instring_RS232[wptr_RS232-1] == 13)
				{
					{
						memcpy(&GSM_chs, &instring_RS232[wptr_RS232-2], 1);
						if(wptr_RS232 == 0)
						{
							memcpy(&GSM_chs, instring_RS232[998], 1);
						}
						memset(instring_RS232, 0, 1000);
						wptr_RS232 = 0;
					}
				}
				// on 1 Go to IO Mode
				if(GSM_chs == 49)
				{
					GSM_chs = 0;
					xQueueSendToBack(IO_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				// on 2 Go to GPS Mode
				if(GSM_chs == 50)
				{
					GSM_chs = 0;
					xQueueSendToBack(GPS_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				// on 3 turn On/Off direct connect to GSM
				if(GSM_chs == 51 && GSM_atcmd_flag == 0 && GSM_wflag == 0)
				{
					GSM_atcmd_flag = 1;
					GSM_wflag = 1;
					usart_serial_write_packet(UART0, "Direct GSM Connection On\n\r", strlen("Direct GSM Connection On\n\r"));
					GSM_chs = 0;
				}
				if(GSM_chs == 51 && GSM_atcmd_flag == 1 && GSM_wflag == 1)
				{
					GSM_atcmd_flag = 0;
					GSM_wflag = 0;
					usart_serial_write_packet(UART0, "Direct GSM Connection Off\n\r", strlen("Direct GSM Connection Off\n\r"));
					GSM_chs = 0;
				}
				if(GSM_chs == 52)
				{
					GSM_chs = 0;
					sms_flag = 1;
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(GSM_chs == 53)
				{
					GSM_cstmsms_flag = 1;
					GSM_chs = 0;
					wptr_RS232 = 0;
					GSM_smslen_cnt = 0;
					memset(GSM_sms, 0, 160);
					memset(instring_RS232, 0, 1000);
					received_byte_RS232 = 0;
					usart_serial_write_packet(UART0, "Write SMS and end with \-\n\r", strlen("Write SMS and end with \-\n\r"));
				}
				if(GSM_chs == 54)
				{
					GSM_phnnmb_flag = 1;
					GSM_chs = 0;
					wptr_RS232 = 0;
					phnnmb_cnt = 0;
					memset(phnnmb, 0, 8);
					memset(instring_RS232, 0, 1000);
					received_byte_RS232 = 0;
					usart_serial_write_packet(UART0, "Write receiver number and end with \-\n\r", strlen("Write reciever number and end with \-\n\r"));
				}
				if(GSM_chs == 55)
				{
					GSM_chs = 0;
					send_cstm_sms_flag = 1;
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(GSM_chs == 56 && GSM_seq_flag == 0)
				{
					GSM_chs = 0;
					GSM_seq_flag = 1;
					usart_serial_write_packet(UART0, "Sequential position-sms ON\n\r", strlen("Sequential position-sms ON\n\r"));
				}
				if(GSM_chs == 56 && GSM_seq_flag == 1)
				{
					GSM_chs = 0;
					GSM_seq_flag = 0;
					usart_serial_write_packet(UART0, "Sequential position-sms OFF\n\r", strlen("Sequential position-sms OFF\n\r"));
				}
				if(GSM_chs == 48)
				{
					GSM_chs = 0;
					reset_flag = 1;
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
			}
			break;
			
			case IO_case:
			xQueueReceive(IO_case_Queue, &QueueItem, portMAX_DELAY);
			usart_serial_write_packet(UART0, "\n\rI/O Mode:\n\r\n\r", strlen("\n\rI/O Mode:\n\r\n\r"));
			usart_serial_write_packet(UART0, "Press 1 to go to GPS Mode\n\r", strlen("Press 1 to go to GPS Mode\n\r"));
			usart_serial_write_packet(UART0, "Press 2 to go to GSM Mode\n\r", strlen("Press 2 to go to GSM Mode\n\r"));
			usart_serial_write_packet(UART0, "Press 3 to monitor IN1 ADC\n\r", strlen("Press 3 to monitor IN1 ADC\n\r"));
			usart_serial_write_packet(UART0, "Press 4 to monitor IN2\n\r", strlen("Press 4 to monitor IN2\n\r"));
			usart_serial_write_packet(UART0, "Press 5 to monitor IN3 ADC\n\r", strlen("Press 5 to monitor IN3 ADC\n\r"));
			usart_serial_write_packet(UART0, "Press 6 to monitor IN4\n\r", strlen("Press 6 to monitor IN4\n\r"));
			usart_serial_write_packet(UART0, "Press 7 to set OUT1 HIGH/LOW\n\r", strlen("Press 7 to set OUT1 HIGH/LOW\n\r"));
			usart_serial_write_packet(UART0, "Press 0 to Reset to Init/Status Mode\n\r", strlen("Press 0 to Reset to Init/Status Mode\n\r"));
			usart_serial_write_packet(UART_SERIAL0, "\n\rConfirm with Enter\n\r\n\r", strlen("\n\rConfirm with Enter\n\r\n\r"));
			IO_chs = 0;
			while(1)
			{
				if(!ioport_get_pin_level(PIO_PD23_IDX))
				{
					pio_set_output(PIOB, PIO_PB13, LOW, DISABLE, DISABLE);
				}
				// Hvis GPS status-ON er høj. Tænd grøn diode
				if(ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 0)
				{
					pio_set_output(PIOD, PIO_PD29, HIGH, DISABLE, DISABLE);
					usart_serial_write_packet(UART0, "GPS On\n\r", strlen("GPS On\n\r"));
					vTaskDelay(1000 / portTICK_RATE_MS);
					GPS_start_flag = 1;
				} // Sluk grøn diode hvis status-ON er lav
				if(!ioport_get_pin_level(PIO_PD24_IDX) && GPS_start_flag == 1)
				{
					pio_set_output(PIOD, PIO_PD29, LOW, DISABLE, DISABLE);
					GPS_start_flag = 0;
					usart_serial_write_packet(UART0, "GPS Off\n\r", strlen("GPS Off\n\r"));
				}
				if(sms_flag == 1)
				{
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(cnt2_flag == 1)
				{
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(instring_RS232[wptr_RS232-1] == 13)
				{
					{
						memcpy(&IO_chs, &instring_RS232[wptr_RS232-2], 1);
						if(wptr_RS232 == 0)
						{
							memcpy(&IO_chs, instring_RS232[998], 1);
						}
						memset(instring_RS232, 0, 1000);
						wptr_RS232 = 0;
					}
				}
				// on 1 Go to GPS Mode
				if(IO_chs == 49)
				{
					IO_chs = 0;
					xQueueSendToBack(GPS_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				// on 2 Go to GSM Mode
				if(IO_chs == 50)
				{
					IO_chs = 0;
					xQueueSendToBack(GSM_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
				if(IO_chs == 51)
				{
					if(ioport_get_pin_level(PIO_PA19_IDX))
					{
						usart_serial_write_packet(UART0, "IN1 ADC says\: I\'m HIGH\n\r", strlen("IN1 ADC says\: I\'m HIGH\n\r"));
						//cnt3 = afec_get_latest_value(AFEC0);
						IO_chs = 0;
					}
					if(!ioport_get_pin_level(PIO_PA19_IDX))
					{
						usart_serial_write_packet(UART0, "IN1 ADC says\: I\'m LOW\n\r", strlen("IN1 ADC says\: I\'m LOW\n\r"));
						IO_chs = 0;
					}
				}
				if(IO_chs == 52)
				{
					if(ioport_get_pin_level(PIO_PA18_IDX))
					{
						usart_serial_write_packet(UART0, "IN2 says\: I\'m HIGH\n\r", strlen("IN2 says\: I\'m HIGH\n\r"));
						IO_chs = 0;
					}
					if(!ioport_get_pin_level(PIO_PA18_IDX))
					{
						usart_serial_write_packet(UART0, "IN2 says\: I\'m LOW\n\r", strlen("IN2 says\: I\'m LOW\n\r"));
						IO_chs = 0;
					}
				}
				if(IO_chs == 53)
				{
					if(ioport_get_pin_level(PIO_PA17_IDX))
					{
						usart_serial_write_packet(UART0, "IN3 ADC says\: I\'m HIGH\n\r", strlen("IN3 ADC says\: I\'m HIGH\n\r"));
						IO_chs = 0;
					}
					if(!ioport_get_pin_level(PIO_PA17_IDX))
					{
						usart_serial_write_packet(UART0, "IN3 ADC says\: I\'m LOW\n\r", strlen("IN3 ADC says\: I\'m LOW\n\r"));
						IO_chs = 0;
					}
				}
				if(IO_chs == 54)
				{
					if(ioport_get_pin_level(PIO_PA20_IDX))
					{
						usart_serial_write_packet(UART0, "IN4 says\: I\'m HIGH\n\r", strlen("IN4 says\: I\'m HIGH\n\r"));
						IO_chs = 0;
					}
					if(!ioport_get_pin_level(PIO_PA20_IDX))
					{
						usart_serial_write_packet(UART0, "IN4 says\: I\'m LOW\n\r", strlen("IN4 says\: I\'m LOW\n\r"));
						IO_chs = 0;
					}
				}
				if(IO_chs == 55 && ioport_get_pin_level(PIO_PD30_IDX))
				{
					usart_serial_write_packet(UART0, "OUT1 set LOW\n\r", strlen("OUT1 set LOW\n\r"));
					pio_set_output(PIOD, PIO_PD30, LOW, DISABLE, DISABLE);
					vTaskDelay(2 / portTICK_RATE_MS);
					pio_set_input(PIOD, PIO_PD30, PIO_DEFAULT);
					IO_chs = 0;
				}
				if(IO_chs == 55 && !ioport_get_pin_level(PIO_PD30_IDX))
				{
					usart_serial_write_packet(UART0, "OUT1 set HIGH\n\r", strlen("OUT1 set HIGH\n\r"));
					pio_set_output(PIOD, PIO_PD30, HIGH, DISABLE, DISABLE);
					vTaskDelay(2 / portTICK_RATE_MS);
					pio_set_input(PIOD, PIO_PD30, PIO_DEFAULT);
					IO_chs = 0;
				}
				if(IO_chs == 48)
				{
					reset_flag = 1;
					IO_chs = 0;
					xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
					break;
				}
			}
			default:
			break;
		}
	}
}

int main (void)
{
	board_init();
	sysclk_init();
	
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOB);
	pmc_enable_periph_clk(ID_PIOC);
	pmc_enable_periph_clk(ID_PIOD);
	
	//pio_set_output(PIOA, PIO_PA6, HIGH, DISABLE, DISABLE);1
	
	
	// BATT OFF
	pio_set_output(PIOD, PIO_PD19, LOW, DISABLE, DISABLE);
	// MODEM OFF
	pio_set_output(PIOB, PIO_PB14, LOW, DISABLE, DISABLE);
	
	pio_set_output(PIOD, PIO_PD22, LOW, DISABLE, DISABLE);
	pio_set_output(PIOD, PIO_PD29, LOW, DISABLE, DISABLE);
	pio_set_output(PIOB, PIO_PB13, LOW, DISABLE, DISABLE);
	
	//Set Inputs 1,2,3,4
	//IN1 ADC
	pio_set_output(PIOA, PIO_PA19, LOW, DISABLE, DISABLE);
	////IN2
	pio_set_output(PIOA, PIO_PA18, LOW, DISABLE, DISABLE);
	////IN3 ADC
	pio_set_output(PIOA, PIO_PA17, LOW, DISABLE, DISABLE);
	////IN4 Counter
	pio_set_output(PIOA, PIO_PA20, LOW, DISABLE, DISABLE);
	
	//Set up ADC
	afec_enable(AFEC0);
	afec_get_config_defaults(&afec0_cfgdef);
	afec_ch_get_config_defaults(&afec0_chdef);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_1, &afec0_chdef);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_2, &afec0_chdef);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_3, &afec0_chdef);
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_5, &afec0_chdef);
	//Range 1-2
	pio_set_output(PIOD, PIO_PD27, LOW, PIO_DEFAULT, PIO_DEFAULT);
	afec_init(AFEC0, &afec0_cfgdef);
	afec_set_trigger(AFEC0, AFEC_TRIG_SW);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_2, 0x800);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_5, 0x800);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_3, 0x800);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_0, 0x800);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_1, 0x800);
	afec_set_callback(AFEC0, AFEC_INTERRUPT_DATA_READY, adc0_callback, 1);
	afec_channel_enable(AFEC0, AFEC_CHANNEL_2);
	//Range 3-4
	pio_set_output(PIOD, PIO_PD0, LOW, PIO_DEFAULT, PIO_DEFAULT);
	
	sysclk_enable_peripheral_clock(USART_SERIAL1_ID);
	sysclk_enable_peripheral_clock(UART_SERIAL1_ID);
	sysclk_enable_peripheral_clock(UART_SERIAL0_ID);
	
	//usart_init_rs232(USART_SERIAL1, &usart_console_settings, sysclk_get_cpu_hz());
	usart_init_hw_handshaking(USART_SERIAL1, &usart_console_settings,  sysclk_get_cpu_hz());
	usart_serial_init(UART_SERIAL1, &uart1_options);
	usart_serial_init(UART_SERIAL0, &uart0_options);
	
	usart_enable_tx(USART_SERIAL1);
	usart_enable_rx(USART_SERIAL1);
	
	uart_enable_tx(UART_SERIAL1);
	uart_enable_rx(UART_SERIAL1);
	
	uart_enable_tx(UART_SERIAL1);
	uart_enable_rx(UART_SERIAL1);
	
	usart_enable_interrupt(USART_SERIAL1, US_CSR_RXRDY);
	uart_enable_interrupt(UART_SERIAL1, UART_SR_RXRDY);
	uart_enable_interrupt(UART_SERIAL0, UART_SR_RXRDY);
	afec_enable_interrupt(AFEC0, AFEC_INTERRUPT_DATA_READY);
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART1_IRQn);
	NVIC_EnableIRQ(USART1_IRQn);

	get_hz = sysclk_get_cpu_hz();
	delay_init(get_hz);
	
	MailTypeStruct QueueItem;
	
	init_case_Queue = xQueueCreate(TASK_QUEUE_SIZE, sizeof(MailTypeStruct));
	GPS_case_Queue = xQueueCreate(TASK_QUEUE_SIZE, sizeof(MailTypeStruct));
	GSM_case_Queue = xQueueCreate(TASK_QUEUE_SIZE, sizeof(MailTypeStruct));
	IO_case_Queue = xQueueCreate(TASK_QUEUE_SIZE, sizeof(MailTypeStruct));
	
	xTaskCreate(test_task, "init_case", 500, init_case, 2, NULL);
	xTaskCreate(test_task, "GPS_case", 500, GPS_case, 2, NULL);
	xTaskCreate(test_task, "GSM_case", 500, GSM_case, 2, NULL);
	xTaskCreate(test_task, "IO_case", 500, IO_case, 2, NULL);
	
	QueueItem.Timeout = 1000;
	
	// MODEM RTS
	pio_set_output(PIOA, PIO_PA24, LOW, DISABLE, DISABLE);
	
	pio_set_output(PIOD, PIO_PD22, HIGH, DISABLE, DISABLE);
	
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO12;
	pio_set_output(PIOB, PIO_PB12, HIGH, PIO_DEFAULT, PIO_DEFAULT);
	pio_set_output(PIOD, PIO_PD17, LOW, PIO_DEFAULT, PIO_DEFAULT);
	
	//Set RS232
	//MODE HIGH
	pio_set_output(PIOD, PIO_PD6, HIGH, PIO_DEFAULT, PIO_DEFAULT);
	//PS HIGH
	pio_set_output(PIOD, PIO_PD7, HIGH, PIO_DEFAULT, PIO_DEFAULT);
	
	pio_set_input(PIOD, PIO_PD24, PIO_DEFAULT);
	pio_set_input(PIOD, PIO_PD23, PIO_DEFAULT);
	pio_set_input(PIOA, PIO_PA25, PIO_DEFAULT);
	
	//1-WIRE
	pio_set_input(PIOA, PIO_PA29, PIO_DEFAULT);
	pio_set_output(PIOD, PIO_PD28, LOW, PIO_DEFAULT, PIO_DEFAULT);
	pio_set_output(PIOA, PIO_PA28, LOW, PIO_DEFAULT, PIO_DEFAULT);

	
	//setup SPI
	spi_master_init(SPI);
	spi_master_setup_device(SPI, &spi_acc, SPI_MODE_3, 1000000, 0);
	spi_enable(SPI);
	//CS ACC
	pio_set_output(PIOA, PIO_PA11, LOW, PIO_DEFAULT, PIO_DEFAULT);
	
	
	
	usart_serial_write_packet(UART0, "\033[2J", strlen("\033[2J"));
	usart_serial_write_packet(UART0, "Initialising:\n\r\n\r", strlen("Initialising:\n\r\n\r"));
	
	xQueueSendToBack(init_case_Queue, &QueueItem, portMAX_DELAY);
	vTaskStartScheduler();
	for(;;);
}

void UART0_Handler(void)
{
	received_byte_RS232 = "";

	usart_serial_getchar(UART_SERIAL0, &received_byte_RS232);
	if(GSM_atcmd_flag == 1)
	{
		usart_serial_putchar(USART_SERIAL1, received_byte_RS232);
	}
	instring_RS232[wptr_RS232++] = received_byte_RS232;
	if(GSM_cstmsms_flag == 1)
	{
		if(received_byte_RS232 == 45)
		{
			GSM_cstmsms_flag = 0;
			GSM_reset_flag = 1;
			usart_serial_write_packet(UART0, "Custom SMS says: \n\r", strlen("Custom SMS says: \n\r"));
			usart_serial_write_packet(UART0, GSM_sms, GSM_smslen_cnt);
			usart_serial_write_packet(UART0, "\n\r", strlen("\n\r"));
		}
		if(received_byte_RS232 != 45)
		{
			GSM_sms[GSM_smslen_cnt++] = received_byte_RS232;
			usart_serial_putchar(UART_SERIAL0, received_byte_RS232);
		}
	}
	if(GSM_phnnmb_flag == 1)
	{
		if(received_byte_RS232 == 45)
		{
			GSM_phnnmb_flag = 0;
			GSM_reset_flag = 1;
			usart_serial_write_packet(UART0, "Receiver number set to: \n\r", strlen("Receiver number set to: \n\r"));
			usart_serial_write_packet(UART0, phnnmb, 8);
			usart_serial_write_packet(UART0, "\n\r", strlen("\n\r"));
		}
		if(received_byte_RS232 != 45)
		{
			phnnmb[phnnmb_cnt++] = received_byte_RS232;
			usart_serial_putchar(UART_SERIAL0, received_byte_RS232);
		}
	}
	if(wptr_RS232 > 999)
	{
		wptr_RS232 = 0;
	}
}

void UART1_Handler(void)
{
	received_byte_GPS = "";
	usart_serial_getchar(UART_SERIAL1, &received_byte_GPS);
	if(GPS_wflag == 1)
	{
		usart_serial_putchar(UART_SERIAL0, received_byte_GPS);
	}
	instring_GPS[wptr_GPS++] = received_byte_GPS;
	if(wptr_GPS > 999)
	{
		wptr_GPS = 0;
	}
}

void USART1_Handler(void)
{
	if(USART1, US_CSR_RXRDY)
	{
		received_byte_GSM = "";
		usart_serial_getchar(USART_SERIAL1, &received_byte_GSM);
		if(GSM_wflag == 1)
		{
			usart_serial_putchar(UART_SERIAL0, received_byte_GSM);
		}
		instring_GSM[wptr_GSM++] = received_byte_GSM;
		if(wptr_GSM > 999)
		{
			wptr_GSM = 0;
		}
	}
}

//ADC0 DATA COLLECT
void adc0_callback(void)
{
	{
		afec0_chn_nmb = afec_get_latest_chan_num(AFEC0);
		if(afec0_chn_nmb == 2)
		{
			ADC_IN1 = afec_get_latest_value(AFEC0);
			afec_channel_disable(AFEC0, AFEC_CHANNEL_2);
			afec_channel_enable(AFEC0, AFEC_CHANNEL_1);
			afec_start_software_conversion(AFEC0);
		}
		if(afec0_chn_nmb == 1)
		{
			ADC_IN2 = afec_get_latest_value(AFEC0);
			afec_channel_disable(AFEC0, AFEC_CHANNEL_1);
			afec_channel_enable(AFEC0, AFEC_CHANNEL_0);
			afec_start_software_conversion(AFEC0);
		}
		if(afec0_chn_nmb == 0)
		{
			ADC_IN3 = afec_get_latest_value(AFEC0);
			afec_channel_disable(AFEC0, AFEC_CHANNEL_0);
			afec_channel_enable(AFEC0, AFEC_CHANNEL_3);
			afec_start_software_conversion(AFEC0);
		}
		if(afec0_chn_nmb == 3)
		{
			ADC_IN4 = afec_get_latest_value(AFEC0);
			afec_channel_disable(AFEC0, AFEC_CHANNEL_3);
			afec_channel_enable(AFEC0, AFEC_CHANNEL_5);
			afec_start_software_conversion(AFEC0);
		}
		if(afec0_chn_nmb == 5)
		{
			VIN_ADC = afec_get_latest_value(AFEC0);
			afec_channel_disable(AFEC0, AFEC_CHANNEL_5);
			IO_VIN_CALC = ((VIN_ADC)*(3191))/1000;
			SMS_Value.AD1 = (((IO_VIN_CALC * 34)+9850)/19700);
		}
	}
}

bool DS1820_StartConversion(uint8_t bus, uint8_t * id)
{
	// Reset, presence.
	bool ReturnValue = false;
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
		ReturnValue = true;
	}
	return(ReturnValue);
}

int16_t DS1820_ReadTemperature(uint8_t bus, uint8_t * id)
{
	int16_t temperature = DS18S20_ERROR;
	uint8_t bytesLeft = 0;
	uint8_t Array[9];
	uint8_t Counter = 0;

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

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	while (1)
	{
	}
}

void vApplicationMallocFailedHook( void )
{
	while(1);
}

void vApplicationTickHook( void )
{
	if(init_cnt < 30001)
	{
		init_cnt++;
	}
	adc_cnt++;
	ow_cnt++;
	GSM_tick_cnt++;
	GPS_tick_cnt++;
	cnt1++;
	if(GSM_seq_flag)
	{
		cnt2++;
	}
	if(init_cnt > 30000)
	{
		IO_cnt++;
	}
	// Pulse to start the GPS
	if(GPS_tick_cnt == 1000 && GPS_start_flag == 0)
	{
		pio_set_output(PIOD, PIO_PD26, HIGH, DISABLE, DISABLE);
	}
	if(GPS_tick_cnt == 1001 && GPS_start_flag == 0)
	{
		pio_set_output(PIOD, PIO_PD26, LOW, DISABLE, DISABLE);
	}
	// Power on the MODEM
	if(GSM_tick_cnt == 1005)
	{
		// BATT ON
		pio_set_output(PIOD, PIO_PD19, HIGH, DISABLE, DISABLE);
		// MODEM ON
		pio_set_output(PIOB, PIO_PB14, HIGH, DISABLE, DISABLE);
	}
	// After 10 seconds. Initialise the MODEM
	if(GSM_tick_cnt == 10000)
	{
		GSM_init_flag = 1;
	}
	// if GPS not started. Try again
	if(GPS_tick_cnt == 2001 && GPS_start_flag == 0)
	{
		GPS_tick_cnt = 0;
	}
	//Enable IN1
	if(adc_cnt == 1000)
	{
		afec_channel_enable(AFEC0, AFEC_CHANNEL_2);		afec_start_software_conversion(AFEC0);
		adc_cnt = 0;
	}
	if(ow_cnt == 20000)
	{
		if(SearchBuses(&OWI_test_dev, 1, 1) != OWI_ROM_SEARCH_FAILED)
		{
			switch(OWI_test_dev.id[0])
			{
				case DS18B20_FAMILY_ID:
				case DS18S20_FAMILY_ID:
				{
					if(DS1820_StartConversion(OWI_test_dev.bus, OWI_test_dev.id))
					{
						ConversionStarted = true;
					}
				}
			}
		}
		ow_cnt = 0;
	}
	//Check inputs every 3/4 second
	if(IO_cnt == 750)
	{
		pio_set_input(PIOA, PIO_PA19, PIO_DEFAULT);
		pio_set_input(PIOA, PIO_PA18, PIO_DEFAULT);
		pio_set_input(PIOA, PIO_PA17, PIO_DEFAULT);
		pio_set_input(PIOA, PIO_PA20, PIO_DEFAULT);
		
		if(ioport_get_pin_level(PIO_PA19_IDX))
		{
			if(!(IO_status & (1<<0)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status |= (1 << 0);
			}
		}
		else
		{
			if((IO_status & (1<<0)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status &= ~(1 << 0);
			}
		}
		if(ioport_get_pin_level(PIO_PA18_IDX))
		{
			if(!(IO_status & (1<<1)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status |= (1 << 1);
			}
		}
		else
		{
			if((IO_status & (1<<1)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status &= ~(1 << 1);
			}
		}
		if(ioport_get_pin_level(PIO_PA17_IDX))
		{
			if(!(IO_status & (1<<2)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status |= (1 << 2);
			}
		}
		else
		{
			if((IO_status & (1<<2)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status &= ~(1 << 2);
			}
		}
		if(ioport_get_pin_level(PIO_PA20_IDX))
		{
			if(!(IO_status & (1<<3)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status |= (1 << 3);
			}
		}
		else
		{
			if((IO_status & (1<<3)))
			{
				sms_flag = 1;
				SMS_Value.MessageID = 6;
				IO_status &= ~(1 << 3);
			}
		}
		SMS_Value.IO = IO_status;
		IO_cnt = 0;
	}
	if(cnt2 == 120000)
	{
		SMS_Value.MessageID = 2;
		cnt2_flag = 1;
		cnt2 = 0;
	}
	//Accelerometer
	//if(cnt1 == 500)
	//{
	//spi_write_single(SPI, 0x8f);
	//spi_read_single(SPI, &spi_data);
	//cnt1=0;
	//}
}
