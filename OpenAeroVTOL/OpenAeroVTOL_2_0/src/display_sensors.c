//***********************************************************
//* display_sensors.c
//***********************************************************

//***********************************************************
//* Includes
//***********************************************************

#include "compiledefs.h"
#include <avr/io.h>
#include <stdlib.h>
#include "io_cfg.h"
#include "glcd_driver.h"
#include "mugui.h"
#include <avr/pgmspace.h>
#include "glcd_menu.h"
#include "main.h"
#include "isr.h"
#include <util/delay.h>
#include "acc.h"
#include "gyros.h"
#include "acc.h"
#include "menu_ext.h"
#include "i2c.h"
#include "MPU6050.h"
#include "vbat.h"
#include "imu.h"

//************************************************************
// Prototypes
//************************************************************

void Display_sensors(void);

//************************************************************
// Code
//************************************************************

void Display_sensors(void)
{
#ifdef KK21
	//uint8_t	test[2];
#endif 
		
	while(BUTTON1 != 0)
	{
		if (BUTTON4 == 0)
		{
			_delay_ms(500);
#ifdef AIRSPEED
			CalibrateAirspeed();
#endif
			// Calibrate sensors. Stop if gyro cal fails
			if (!CalibrateGyrosSlow())
			{
				clear_buffer(buffer);
				LCD_Display_Text(61,(const unsigned char*)Verdana14,25,25); // "Cal. failed"
				write_buffer(buffer,1);
				_delay_ms(1000);
			}			
			else
			{
				CalibrateAcc(NORMAL);
			}
		}

		if (BUTTON3 == 0)
		{
			_delay_ms(500);
			CalibrateAcc(REVERSED);
		}

		ReadGyros();
		ReadAcc();

		LCD_Display_Text(26,(const unsigned char*)Verdana8,37,0); 	// Gyro
		LCD_Display_Text(30,(const unsigned char*)Verdana8,77,0); 	// Acc
		//
		LCD_Display_Text(27,(const unsigned char*)Verdana8,5,13);	// Roll
		LCD_Display_Text(28,(const unsigned char*)Verdana8,5,23);	// Pitch
		LCD_Display_Text(29,(const unsigned char*)Verdana8,5,33);	// Yaw/Z
		//
		mugui_lcd_puts(itoa(gyroADC[ROLL],pBuffer,10),(const unsigned char*)Verdana8,40,13);
		mugui_lcd_puts(itoa(gyroADC[PITCH],pBuffer,10),(const unsigned char*)Verdana8,40,23);
		mugui_lcd_puts(itoa(gyroADC[YAW],pBuffer,10),(const unsigned char*)Verdana8,40,33);
		mugui_lcd_puts(itoa(accADC[ROLL],pBuffer,10),(const unsigned char*)Verdana8,80,13);
		mugui_lcd_puts(itoa(accADC[PITCH],pBuffer,10),(const unsigned char*)Verdana8,80,23);
		mugui_lcd_puts(itoa(accADC[YAW],pBuffer,10),(const unsigned char*)Verdana8,80,33);
	
#ifdef KK21
		// Use this for checking MPU6050 register values
		//readI2CbyteArray(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_ACCEL_CONFIG, (uint8_t *)test, 1);
		//readI2CbyteArray(MPU60X0_DEFAULT_ADDRESS, MPU60X0_RA_INT_PIN_CFG, (uint8_t *)test, 1);
		//mugui_lcd_puts(itoa(test[0],pBuffer,10),(const unsigned char*)Verdana8,40,45);
#endif
		
#ifdef AIRSPEED
		LCD_Display_Text(53,(const unsigned char*)Verdana8,5,45);		// Airspeed
		mugui_lcd_puts(itoa((GetAirspeed() - Config.AirspeedZero),pBuffer,10),(const unsigned char*)Verdana8,55,45);
#endif

		// Print bottom markers
		LCD_Display_Text(12, (const unsigned char*)Wingdings, 0, 57); 	// Left
		LCD_Display_Text(25, (const unsigned char*)Verdana8, 75, 55); 	// Inverted Calibrate
		LCD_Display_Text(60, (const unsigned char*)Verdana8, 108, 55); 	// Calibrate

		// Update buffer
		write_buffer(buffer,1);
		clear_buffer(buffer);
		_delay_ms(100);
	}
}
