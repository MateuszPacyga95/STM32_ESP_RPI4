#include "bme680.h"
#include "spi.h"
#include "gpio.h"

#define BME680_SPI_MEM_PAGE0_VALUE 0x00U
#define BME680_SPI_MEM_PAGE1_VALUE 0x10U

static BME680_Status writeSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value);
static BME680_Status writeSpiRegisterRaw(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value);
static BME680_Status setSpiMemoryPage(SPI_HandleTypeDef* spiHandle, uint8_t regAddr);
static BME680_Status readSpi8bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t *retValue);
static BME680_Status readSpi16bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue);
static BME680_Status readSpi24bitRegister(SPI_HandleTypeDef* spiHandle, uint32_t *retValue, uint8_t tx);


BME680_Status BME680_Init(BME680_HandleTypeDef *bme680, SPI_HandleTypeDef *hspi)
{
    uint8_t chip_id = 0;
    uint16_t raw_t2 = 0;
    uint8_t raw_t3 = 0;

    bme680->hspi = hspi;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_ID, &chip_id)) {
        return BME680_ERROR;
    }
    if (chip_id != 0x61U) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi16bitRegister(bme680->hspi, BME680_CDM_T1, &bme680->par_t1)) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi16bitRegister(bme680->hspi, BME680_CDM_T2, &raw_t2)) {
        return BME680_ERROR;
    }
    bme680->par_t2 = (int16_t)raw_t2;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_T3, &raw_t3)) {
        return BME680_ERROR;
    }
    bme680->par_t3 = (int8_t)raw_t3;

    return BME680_OK;
}

BME680_Status BME680_SetForcedMode(BME680_HandleTypeDef *bme680, uint8_t mode)
{
    uint8_t ctrl_meas_check = 0;
    uint8_t ctrl_meas = 0;

    if (bme680 == NULL || mode > 3U) {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_meas)) {
        return BME680_ERROR;
    }
    ctrl_meas &= (uint8_t)~0x03U; // Clear the mode bits (bits 0 and 1)
    ctrl_meas |= mode;
        


    if (BME680_OK != writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_meas)) {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_meas_check)) {
        return BME680_ERROR;
    }

    if ((ctrl_meas_check & 0x03U) != mode) {
        return BME680_ERROR;
    }
    return BME680_OK;
}

BME680_Status BME680_SetTemperatureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_meas = 0;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_meas)) {
        return BME680_ERROR;
    }
    ctrl_meas &= (uint8_t)~0xE0U;
    ctrl_meas |= (uint8_t)(oversampling << 5);

    return writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_meas);
}

BME680_Status BME680_SetHumidityOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_hum_check = 0;
    uint8_t ctrl_hum = 0;

    if (bme680 == NULL || oversampling > 5U) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_HUM, &ctrl_hum)) {
        return BME680_ERROR;
    }

    ctrl_hum &= (uint8_t)~0x07U;
    ctrl_hum |= oversampling;
    if (BME680_OK != writeSpiRegister(bme680->hspi, BME680_REG_CTRL_HUM, ctrl_hum)) {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_HUM, &ctrl_hum_check)) {
        return BME680_ERROR;
    }
    if ((ctrl_hum_check & 0x07U) != oversampling) {
        return BME680_ERROR;
    }

    return BME680_OK;
}

BME680_Status BME680_SetPressureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_press_check = 0;
    uint8_t ctrl_press = 0;

    if (bme680 == NULL || oversampling > 5U) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_press)) {
        return BME680_ERROR;
    }

    ctrl_press &= (uint8_t)~0x1CU;
    ctrl_press |= oversampling << 2;
    if (BME680_OK != writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_press)) {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_press_check)) {
        return BME680_ERROR;
    }
    if ((ctrl_press_check & 0x1CU) != oversampling) {
        return BME680_ERROR;
    }

    return BME680_OK;
}

BME680_Status BME680_SetTemperatureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_hum_check = 0;
    uint8_t ctrl_hum = 0;

    if (bme680 == NULL || oversampling > 5U) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_hum)) {
        return BME680_ERROR;
    }

    ctrl_hum &= (uint8_t)~0xE0U;
    ctrl_hum |= oversampling << 5;
    if (BME680_OK != writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_hum)) {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_hum_check)) {
        return BME680_ERROR;
    }
    if ((ctrl_hum_check & 0xE0U) != oversampling) {
        return BME680_ERROR;
    }

    return BME680_OK;
}


BME680_Status BME680_SetPressureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_meas = 0;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_meas)) {
        return BME680_ERROR;
    }
    ctrl_meas &= (uint8_t)~0x1CU;
    ctrl_meas |= (uint8_t)(oversampling << 2);

    return writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_meas);
}

BME680_Status BME680_ReadTemperature(BME680_HandleTypeDef *bme680, int32_t *temperature)
{
    uint32_t adc_temp = 0;

    if (BME680_OK != readSpi24bitRegister(bme680->hspi, &adc_temp, BME680_REG_TEMP_MSB)) {
        return BME680_ERROR;
    }

    /* readSpi24bitRegister already returns the 20-bit ADC value. */
    int32_t var1 = ((int32_t)adc_temp >> 3) - ((int32_t)bme680->par_t1 << 1);
    int32_t var2 = (var1 * (int32_t)bme680->par_t2) >> 11;
    int32_t var3 = ((var1 >> 1) * (var1 >> 1)) >> 12;
    var3 = (var3 * ((int32_t)bme680->par_t3 << 4)) >> 14;

    int32_t t_fine = var2 + var3;
    *temperature = (t_fine * 5 + 128) >> 8;

    return BME680_OK;
}


static BME680_Status readSpi8bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t *retValue)
{
    uint8_t retVal = 0;

    if (tx != BME680_REG_STATUS &&
        BME680_OK != setSpiMemoryPage(spiHandle, tx)) {
        return BME680_ERROR;
    }
    tx |= 0x80U;

    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
        return BME680_ERROR;
    }
    if (HAL_SPI_Receive(spiHandle, &retVal, 1, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
        return BME680_ERROR;
    }
    HAL_Delay(1);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

    *retValue = retVal;
    return BME680_OK;
}

static BME680_Status readSpi16bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue)
{
    uint8_t retVal[2] = {0};

    if (tx != BME680_REG_STATUS &&
        BME680_OK != setSpiMemoryPage(spiHandle, tx)) {
        return BME680_ERROR;
    }
    tx |= 0x80U; 

    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
        return BME680_ERROR;
    }
    if (HAL_SPI_Receive(spiHandle, retVal, 2, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
        return BME680_ERROR;
    }
    HAL_Delay(1);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

    *retValue = (uint16_t)retVal[0] | ((uint16_t)retVal[1] << 8);
    return BME680_OK;
}

static BME680_Status readSpi24bitRegister(SPI_HandleTypeDef* spiHandle, uint32_t *retValue, uint8_t tx)
{
    uint8_t retVal[3] = {0};

    if (tx != BME680_REG_STATUS &&
        BME680_OK != setSpiMemoryPage(spiHandle, tx)) {
        return BME680_ERROR;
    }
    tx |= 0x80U;

    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
        return BME680_ERROR;
    }
    if (HAL_SPI_Receive(spiHandle, retVal, 3, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
        return BME680_ERROR;
    }
    HAL_Delay(1);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

    *retValue = ((uint32_t)retVal[0] << 12) |
                ((uint32_t)retVal[1] << 4) |
                ((uint32_t)retVal[2] >> 4);
    return BME680_OK;
}

static BME680_Status setSpiMemoryPage(SPI_HandleTypeDef* spiHandle, uint8_t regAddr)
{
    uint8_t pageValue = BME680_SPI_MEM_PAGE0_VALUE;

    /* Low-address registers are on SPI page 1. Chip ID is a page-0 exception. */
    if (regAddr < 0x80U &&
        regAddr != BME680_REG_ID &&
        regAddr != BME680_REG_STATUS) {
        pageValue = BME680_SPI_MEM_PAGE1_VALUE;
    }

    return writeSpiRegisterRaw(spiHandle, BME680_REG_STATUS, pageValue);
}

static BME680_Status writeSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value)
{
    if (regAddr != BME680_REG_STATUS &&
        BME680_OK != setSpiMemoryPage(spiHandle, regAddr)) {
        return BME680_ERROR;
    }

    /* The SPI read/write bit is clear for a write transaction. */
    return writeSpiRegisterRaw(spiHandle, regAddr & 0x7FU, value);
}

static BME680_Status writeSpiRegisterRaw(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value)
{
    uint8_t tx[2] = {regAddr, value};

    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);

    if (HAL_SPI_Transmit(spiHandle, tx, 2, HAL_MAX_DELAY) != HAL_OK) {
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
        return BME680_ERROR;
    }

    HAL_Delay(1);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
    return BME680_OK;
}