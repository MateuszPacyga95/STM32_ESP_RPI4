
#include "bme680.h"
#include "spi.h"
#include "gpio.h"

static void writeSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t size);
static void readSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t* rx);


void BME680_Init(BME680_HandleTypeDef *bme680, SPI_HandleTypeDef *hspi, uint8_t address)
{
    uint8_t chip_id = 0x00;
    bme680->hspi = hspi;
    bme680->address = address;

    readSpiRegister(bme680->hspi, address, &chip_id); // Read the chip ID register (0xD0)
    if(chip_id != 0x61) {
        Error_Handler();
    } else {
    }
  }


static void readSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t* rx)
{
  tx |= 0x80; // Set MSB for read operation
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET); // Set CS low
  HAL_Delay(1);
  if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK)
  {
      Error_Handler();
  }
  if (HAL_SPI_Receive(spiHandle, rx, 1, HAL_MAX_DELAY) != HAL_OK)
  {
      Error_Handler();
  }
  HAL_Delay(1);
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); // Set CS high

}

static void writeSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t size)
{
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET); // Set CS low
  HAL_Delay(1);
  if (HAL_SPI_Transmit(spiHandle, &tx, size, HAL_MAX_DELAY) != HAL_OK)
  {
      Error_Handler();
  }
  // if (HAL_SPI_Receive(spiHandle, rx, 1, HAL_MAX_DELAY) != HAL_OK)
  // {
  //     printf("SPI receive failed!\r\n");
  //     Error_Handler();
  // }
  HAL_Delay(1);
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); // Set CS high
}