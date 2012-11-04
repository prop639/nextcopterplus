//***********************************************************
//* eeprom.c
//***********************************************************

//***********************************************************
//* Includes
//***********************************************************

#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include "..\inc\io_cfg.h"
#include <util/delay.h>
#include "..\inc\mixer.h"
#include "..\inc\menu_ext.h"

//************************************************************
// Prototypes
//************************************************************

void Initial_EEPROM_Config_Load(void);
void Save_Config_to_EEPROM(void);
void Set_EEPROM_Default_Config(void);
void eeprom_write_byte_changed( uint8_t * addr, uint8_t value );
void eeprom_write_block_changes( const uint8_t * src, void * dest, uint16_t size );

//************************************************************
// Defines
//************************************************************

#define EEPROM_DATA_START_POS 0	// Make sure Rolf's signature is over-written for safety
#define MAGIC_NUMBER 0x05		// eePROM signature - change for each eePROM structure change 0x05 = V1.1b1
								// to force factory reset

//************************************************************
// Code
//************************************************************

uint8_t	JR[MAX_RC_CHANNELS] PROGMEM 	= {0,1,2,3,4,5,6,7}; 	// JR/Spektrum channel sequence
uint8_t	FUTABA[MAX_RC_CHANNELS] PROGMEM = {2,0,1,3,4,5,6,7}; 	// Futaba channel sequence

void Set_EEPROM_Default_Config(void)
{
	uint8_t i;

	for (i = 0; i < MAX_RC_CHANNELS; i++)
	{
		Config.ChannelOrder[i] = pgm_read_byte(&JR[i]);
		Config.Channel[i].value = 3750;
		Config.RxChannelZeroOffset[i] = 3750;
	}
	//
	get_preset_mix(AEROPLANE_MIX);		// Load AEROPLANE default mix
	//
	Config.RxMode = PWM1;				// Default to PWM1
	Config.TxSeq = JRSEQ;
	//
	Config.AccRollZeroTrim	= 0;		// User-set ACC trim
	Config.AccPitchZeroTrim	= 0;
	Config.AccRollZero	= 621;			// Acc calibration defaults
	Config.AccPitchZero	= 623;
	Config.AccZedZero	= 767;
	//
	Config.Roll.P_mult = 60;			// PID defaults			
	Config.Roll.I_mult = 0;
	Config.Roll.D_mult = 0;
	Config.Pitch.P_mult = 60;
	Config.Pitch.I_mult = 0;
	Config.Pitch.D_mult = 0;
	Config.Yaw.P_mult = 60;
	Config.Yaw.I_mult = 0;
	Config.Yaw.D_mult = 0;
	//
	Config.A_Roll_P_mult = 30;
	Config.A_Pitch_P_mult = 30;
	Config.Acc_LPF = 8;
	Config.CF_factor = 30;
	Config.AutoCenter = OFF;

	for (i = 0; i < 3; i++)
	{
		Config.I_Limits[i] = 0;
		Config.Raw_I_Limits[i] = 0;
	}
	//
	Config.StabMode = STABCHAN;			// DISABLED = 0, AUTOCHAN, STABCHAN, THREEPOS, ALWAYSON
	Config.AutoMode = AUTOCHAN;			// DISABLED = 0, AUTOCHAN, STABCHAN, THREEPOS, ALWAYSON
	Config.PowerTrigger = 0; 			// 7.33V for 2S, 10.8V for 3S are good values here
	Config.BatteryCells = 3;			// Default to 3S
	Config.BatteryType = LIPO;
	Config.MinVoltage = 360;
	Config.MaxVoltage = 420;

	Config.Modes = 20; 					// LVA mode (1) = buzzer (bit 4), 
										// Stability mode (0) (bit 3),
										// Pot mode (1) = inactive (bit 2) eeprom mode
										// (bit 1) (0) unused
										// Autolevel (0) = inactive (bit 0);
	Config.StabChan = GEAR;				// Channel GEAR switches stability by default
	Config.AutoChan = GEAR;				// Channel AUX2 switches autolevel by default
	Config.FlapChan = THROTTLE;			// Most people will use the throttle input I imagine
	Config.Stabtrigger = 3500;
	Config.Autotrigger = 3500;
	Config.LaunchMode = OFF;			// Launch mode on/off
	Config.LaunchThrPos = 0;			// Launch mode throttle position
	Config.Launchtrigger = 3500;		// Respanned launch trigger

	Config.AileronExpo = 0;				// Amount of expo on Aileron channel
	Config.ElevatorExpo = 0;			// Amount of expo on Elevator channel
	Config.RudderExpo = 0;				// Amount of expo on Rudder channel
	Config.Differential = 0;			// Amount of differential on Aileron channels
	Config.MixMode = AEROPLANE;			// Aeroplane/Flying Wing/Manual

	Config.CamStab = OFF;
	Config.RCMix = OFF;					// RC mixer defaults to OFF for speed

	Config.Orientation = 0;				// Horizontal / vertical
	Config.Contrast = 38;				// Contrast
	Config.Status_timer = 5;			// Refresh timeout
	Config.LMA_enable = 1;				// Default to 1 minute

	Config.Servo_rate = LOW;			// Default to LOW (50Hz)

	Config.Preset1 = 0;
	Config.Preset2 = 0;
	Config.Preset3 = 0;
	Config.Preset4 = 0;
}

void Save_Config_to_EEPROM(void)
{
	// Write to eeProm
	cli();
	eeprom_write_block_changes((const void*) &Config, (void*) EEPROM_DATA_START_POS, sizeof(CONFIG_STRUCT));	
	sei();
	menu_beep(1);
	LED1 = !LED1;
	_delay_ms(500);
	LED1 = !LED1;
}

void eeprom_write_byte_changed( uint8_t * addr, uint8_t value )
{ 
	if(eeprom_read_byte(addr) != value)
	{
		eeprom_write_byte( addr, value );
	}
}

void eeprom_write_block_changes( const uint8_t * src, void * dest, uint16_t size )
{ 
	uint16_t len;

	for(len=0;len<size;len++)
	{
		eeprom_write_byte_changed( dest, *src );
		src++;
		dest++;
	}
}

void Initial_EEPROM_Config_Load(void)
{
	// Load last settings from EEPROM
	if(eeprom_read_byte((uint8_t*) EEPROM_DATA_START_POS )!= MAGIC_NUMBER)
	{
		Config.setup = MAGIC_NUMBER;
		Set_EEPROM_Default_Config();
		// Write to eeProm
		Save_Config_to_EEPROM();
	} 
	else 
	{
		// Read eeProm
		eeprom_read_block(&Config, (void*) EEPROM_DATA_START_POS, sizeof(CONFIG_STRUCT)); 
	}
}

