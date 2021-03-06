/*********************************************************************
 * imu.h
 ********************************************************************/

//***********************************************************
//* Externals
//***********************************************************

extern int16_t	angle[2];
extern float accSmooth[NUMBEROFAXIS];

extern void imu_update(uint32_t period);
extern void reset_IMU(void);

extern const uint8_t LPF_lookup[7] PROGMEM;
extern const uint8_t LPF_lookup_HS[7] PROGMEM;