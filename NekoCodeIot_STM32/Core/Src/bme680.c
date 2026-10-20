
#include "bme680.h"
#include "spi.h"
#include "gpio.h"

static void writeSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value);
static uint8_t readSpi8bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx);
static uint16_t readSpi16bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx);
static uint16_t readSpi16bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx);
static uint32_t readSpi24bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx);


BME680_Status BME680_Init(BME680_HandleTypeDef *bme680, SPI_HandleTypeDef *hspi)
{
    uint8_t chip_id = 0x00;
    bme680->hspi = hspi;

    chip_id = readSpi8bitRegister(bme680->hspi, BME680_REG_ID); // Read the chip ID register (0xD0)
    if(chip_id != 0x61) {
       return BME680_ERROR;
    } 
    bme680->par_t1 = readSpi16bitRegister(bme680->hspi, BME680_CDM_T1);
    bme680->par_t2 = readSpi16bitRegister(bme680->hspi, BME680_CDM_T2);
    bme680->par_t3 = readSpi8bitRegister(bme680->hspi, BME680_CDM_T3);
       
    return BME680_OK;

}

BME680_Status BME680_SetForcedMode(BME680_HandleTypeDef *bme680)
{
    uint8_t ctrl_meas = readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS);
    ctrl_meas &= ~0x03; // Clear the mode bits
    ctrl_meas |= BME680_FORCED_MODE; // Set to forced mode
    writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_meas);
    return BME680_OK;
}

BME680_Status BME680_SetTemperatureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_meas = readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS);
    ctrl_meas &= ~0xE0; // Clear the temperature oversampling bits (bits 7:5)
    ctrl_meas |= (oversampling << 5); // Set the temperature oversampling (bits 7:5)
    writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_meas);
    return BME680_OK;
}


BME680_Status BME680_SetPressureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_meas = readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS);
    ctrl_meas &= ~0x1C; // Clear the pressure oversampling bits (bits 4:2)
    ctrl_meas |= (oversampling << 2); // Set the pressure oversampling (bits 4:2)
    writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_meas);
    return BME680_OK;
}

BME680_Status BME680_ReadTemperature(BME680_HandleTypeDef *bme680, int32_t *temperature)
{
    uint32_t adc_temp = readSpi24bitRegister(bme680->hspi, BME680_REG_TEMP_MSB);
    adc_temp >>= 4; // Right shift to get the 20-bit value

    // Compensation formula from the datasheet
    int32_t var1 = ((int32_t)adc_temp >> 3) - ((int32_t)bme680->par_t1 << 1);
    int32_t var2 = (var1 * (int32_t)bme680->par_t2) >> 11;
    int32_t var3 = ((var1 >> 1) * (var1 >> 1)) >> 12;
    var3 = (var3 * (int32_t)bme680->par_t3) >> 14;

    int32_t t_fine = var2 + var3;
    *temperature = (t_fine * 5 + 128) >> 8; // Temperature in Celsius

    return BME680_OK;
}



static uint8_t readSpi8bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx)
{
  tx |= 0x80; // Set MSB for read operation

  uint8_t retVal = 0;
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET); // Set CS low
  HAL_Delay(1);
  if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK)
  {
      Error_Handler();
  }
  if (HAL_SPI_Receive(spiHandle, &retVal, 1, HAL_MAX_DELAY) != HAL_OK)
  {
      Error_Handler();
  }
  HAL_Delay(1);
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); // Set CS high

  return retVal;

}

static uint16_t readSpi16bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx)
{
    tx |= 0x80; // Set MSB for read operation
    uint8_t retVal[2] = {0};
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET); // Set CS low
    HAL_Delay(1);
    if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_SPI_Receive(spiHandle, (uint8_t*)&retVal, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_Delay(1);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); // Set CS high
    return (retVal[1] << 8) | retVal[0];
}

static uint32_t readSpi24bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx)
{
    tx |= 0x80; // Set MSB for read operation
    uint8_t retVal[3] = {0};
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET); // Set CS low
    HAL_Delay(1);
    if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_SPI_Receive(spiHandle, (uint8_t*)&retVal, 3, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_Delay(1);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); // Set CS high
    return (retVal[0] << 12) | (retVal[1] << 4) | (retVal[2] >> 4);
}

static void writeSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value)
{
  uint8_t tx[2] = {regAddr, value};
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET); // Set CS low
  HAL_Delay(1);
  if (HAL_SPI_Transmit(spiHandle, tx, 2, HAL_MAX_DELAY) != HAL_OK)
  {
      Error_Handler();
  }


  HAL_Delay(1);
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); // Set CS high
}