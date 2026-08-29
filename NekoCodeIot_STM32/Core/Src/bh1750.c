
#include "main.h"
#include "i2c.h"

#include "bh1750.h"

uint8_t BH1750_Mtreg; // Default conversion factor for high resolution mode
bh1750_mode BH1750_Mode; // Default mode

BH1750_STATUS bh1750_init(BH1750_HandleTypeDef *dev, I2C_HandleTypeDef *hi2c, uint8_t address)
{
    dev->hi2c = hi2c;
    dev->address = address;

    if(bh1750_reset(dev) == BH1750_OK)
    {
        if(BH1750_OK == bh1750_setMTreg(dev, BH1750_DEFAULT_MTREG)) // Set default MTReg value
        {
            return BH1750_OK;
        }
    }
    return BH1750_ERROR;
}

BH1750_STATUS bh1750_setMTreg(BH1750_HandleTypeDef *dev, uint8_t  MTReg)
{
    uint8_t cmd_high;
    uint8_t cmd_low;

    if(MTReg < 31 || MTReg > 254)
    {
        return BH1750_ERROR; // Invalid MTReg value
    }

    BH1750_Mtreg = MTReg;

    cmd_high = 0x40 | (MTReg >> 5); // High 3 bits
    cmd_low = 0x60 | (MTReg & 0x1F);

    if(HAL_I2C_Master_Transmit(dev->hi2c, dev->address, &cmd_high, 1, 100) != HAL_OK)
    {
        return BH1750_ERROR;
    }

    if(HAL_I2C_Master_Transmit(dev->hi2c, dev->address, &cmd_low, 1, 100) != HAL_OK)
    {
        return BH1750_ERROR;
    }

    return BH1750_OK;
 
}


BH1750_STATUS bh1750_reset(BH1750_HandleTypeDef *dev)
{
    if(HAL_OK ==   HAL_I2C_Master_Transmit(dev->hi2c, dev->address, (uint8_t[]){BH1750_RESET}, 1, 100))
    {
        return BH1750_OK;
    }
  
    return BH1750_ERROR;
 
}

BH1750_STATUS bh1750_powerState(BH1750_HandleTypeDef *dev, uint8_t powerOn)
{
    if(HAL_OK ==   HAL_I2C_Master_Transmit(dev->hi2c, dev->address, &powerOn, 1, 100))
    {
        return BH1750_OK;
    }
  
    return BH1750_ERROR;
}

BH1750_STATUS bh1750_setMode(BH1750_HandleTypeDef *dev, bh1750_mode mode)
{
    if(mode != CONTINUOUS_HIGH_RES_MODE && mode != CONTINUOUS_HIGH_RES_MODE_2 &&
       mode != CONTINUOUS_LOW_RES_MODE && mode != ONETIME_HIGH_RES_MODE &&
       mode != ONETIME_HIGH_RES_MODE_2 && mode != CONTINUOUS_LOW_RES_MODE_2)
    {
        return BH1750_ERROR; // Invalid mode
    }
    // Start measurement in continuous high resolution mode
    HAL_I2C_Master_Transmit(dev->hi2c, dev->address, &mode, 1, 100);
    HAL_Delay(180); // Wait for measurement to be taken

    return BH1750_OK; // Success
}


BH1750_STATUS bh1750_readLux(BH1750_HandleTypeDef *dev, float *lux)
{
    uint8_t data[2] = {0};
    float raw;


    // Read 2 bytes of data
    if (HAL_I2C_Master_Receive(dev->hi2c, dev->address, data, 2, 1000) == HAL_OK) {
            HAL_Delay(180); 
            raw = (data[0] << 8) | data[1];

            if(BH1750_Mtreg != BH1750_DEFAULT_MTREG) {
                // Adjust conversion factor based on MTReg value
                raw *= (float)((uint8_t)BH1750_DEFAULT_MTREG/(float)BH1750_Mtreg);
            } 

            if(BH1750_Mode == CONTINUOUS_LOW_RES_MODE_2 || BH1750_Mode == ONETIME_HIGH_RES_MODE_2) {
                raw /= 2.0; // Low resolution mode has half the sensitivity
            }


            // Convert the raw data to lux
            
            *lux = raw / (float)BH1750_CONVERSION_FACTOR;
    }


    return BH1750_ERROR; // Error reading data
}