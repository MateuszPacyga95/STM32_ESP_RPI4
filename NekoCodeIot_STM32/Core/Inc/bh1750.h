#include "i2c.h"


#define BH1750_ADDR (0x23 << 1) // Shifted left for HAL I2C functions

#define BH1750_POWER_OFF 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07

#define BH1750_CONVERSION_FACTOR	1.2
#define BH1750_DEFAULT_MTREG 69 // Default MTReg value for high resolution mode

typedef struct 
{
    I2C_HandleTypeDef *hi2c;
    uint8_t address;
} BH1750_HandleTypeDef;

typedef enum {
    BH1750_OK = 0,
    BH1750_ERROR = 1
} BH1750_STATUS;

typedef enum
{
    CONTINUOUS_HIGH_RES_MODE  	= 0x10,
    CONTINUOUS_HIGH_RES_MODE_2 	= 0x11,
    CONTINUOUS_LOW_RES_MODE 	= 0x13,
    ONETIME_HIGH_RES_MODE 		= 0x20,
    ONETIME_HIGH_RES_MODE_2 	= 0x21,
    CONTINUOUS_LOW_RES_MODE_2 	= 0x23
}bh1750_mode;

#define BH1750_CONTINUOUS_HIGH_RES_MODE 0x10
#define BH1750_CONTINUOUS_HIGH_RES_MODE_2 0x11

BH1750_STATUS bh1750_init(BH1750_HandleTypeDef *dev, I2C_HandleTypeDef *hi2c, uint8_t address);
BH1750_STATUS bh1750_readLux(BH1750_HandleTypeDef *dev, float *lux);
BH1750_STATUS bh1750_powerState(BH1750_HandleTypeDef *dev, uint8_t powerOn);
BH1750_STATUS bh1750_reset(BH1750_HandleTypeDef *dev);
BH1750_STATUS bh1750_setMTreg(BH1750_HandleTypeDef *dev, uint8_t MTReg);
BH1750_STATUS bh1750_setMode(BH1750_HandleTypeDef *dev, bh1750_mode mode);