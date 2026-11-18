#include "bme680.h"
#include "spi.h"
#include "gpio.h"

#include <stdint.h>
#define BME680_SPI_MEM_PAGE0_VALUE 0x00U
#define BME680_SPI_MEM_PAGE1_VALUE 0x10U

static BME680_Status writeSpiRegister(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value);
static BME680_Status writeSpiRegisterRaw(SPI_HandleTypeDef* spiHandle, uint8_t regAddr, uint8_t value);
static BME680_Status setSpiMemoryPage(SPI_HandleTypeDef* spiHandle, uint8_t regAddr);
static BME680_Status readSpi8bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t *retValue);
static BME680_Status readSpi16bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue);
static BME680_Status readSpi16bitRegisterLSBFirst(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue);
static BME680_Status readSpi24bitRegister(SPI_HandleTypeDef* spiHandle, uint32_t *retValue, uint8_t tx);
//static BME680_Status readSpi10bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue);


BME680_Status BME680_Init(BME680_HandleTypeDef *bme680, SPI_HandleTypeDef *hspi)
{
    if (bme680 == NULL || hspi == NULL) {
        return BME680_ERROR;
    }
    uint8_t chip_id = 0;
    uint16_t raw_t2 = 0;
    uint8_t raw_t3 = 0;
    uint16_t raw_p2 = 0;
    uint8_t raw_p3 = 0;
    uint16_t raw_p4 = 0;
    uint16_t raw_p5 = 0;
    uint8_t raw_p6 = 0;
    uint8_t raw_p7 = 0;
    uint16_t raw_p8 = 0;
    uint16_t raw_p9 = 0;
    uint8_t raw_p10 = 0;
    uint8_t raw_h2_msb = 0;
    uint8_t raw_h_shared = 0;
    uint8_t raw_h1_msb = 0;
    uint8_t raw_h3 = 0;
    uint8_t raw_h4 = 0;
    uint8_t raw_h5 = 0;
    uint8_t raw_h7= 0;

    bme680->hspi = hspi;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_ID, &chip_id)) {
        return BME680_ERROR;
    }
    if (chip_id != 0x61U) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_T1, &bme680->par_t1)) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_T2, &raw_t2)) {
        return BME680_ERROR;
    }
    bme680->par_t2 = (int16_t)raw_t2;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_T3, &raw_t3)) {
        return BME680_ERROR;
    }
    bme680->par_t3 = (int8_t)raw_t3;

    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_P1, &bme680->par_p1)) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_P2, &raw_p2)) {
        return BME680_ERROR;
    }
    bme680->par_p2 = (int16_t)raw_p2;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_P3, &raw_p3)) {
        return BME680_ERROR;
    }
    bme680->par_p3 = (int8_t)raw_p3;

    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_P4, &raw_p4)) {
        return BME680_ERROR;
    }
    bme680->par_p4 = (int16_t)raw_p4;

    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_P5, &raw_p5)) {
        return BME680_ERROR;
    }
    bme680->par_p5 = (int16_t)raw_p5;
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_P6, &raw_p6)) {
        return BME680_ERROR;
    }
    bme680->par_p6 = (int8_t)raw_p6;
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_P7, &raw_p7)) {
        return BME680_ERROR;
    }
    bme680->par_p7 = (int8_t)raw_p7;

    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_P8, &raw_p8)) {
        return BME680_ERROR;
    }
    bme680->par_p8 = (int16_t)raw_p8;

    if (BME680_OK != readSpi16bitRegisterLSBFirst(bme680->hspi, BME680_CDM_P9, &raw_p9)) {
        return BME680_ERROR;
    }
    bme680->par_p9 = (int16_t)raw_p9;

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_P10, &bme680->par_p10)) {
        return BME680_ERROR;
    }
    

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H2_MSB, &raw_h2_msb) ||
    BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H1_H2_LSB, &raw_h_shared) ||
    BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H1_MSB, &raw_h1_msb)) 
    {
        return BME680_ERROR;
    }
    bme680->par_h1 = ((uint16_t)raw_h1_msb << 4) | (raw_h_shared & 0x0F);
    bme680->par_h2 = ((uint16_t)raw_h2_msb << 4) | ((raw_h_shared >> 4) & 0x0F);
    
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H3, &raw_h3)) {
        return BME680_ERROR;
    }
    bme680->par_h3 = (int8_t)raw_h3;
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H4, &raw_h4)) {
        return BME680_ERROR;
    }
    bme680->par_h4 = (int8_t)raw_h4;
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H5, &raw_h5)) {
        return BME680_ERROR;
    }
    bme680->par_h5 = (int8_t)raw_h5;
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H6, &bme680->par_h6)) {
        return BME680_ERROR;
    }
    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_CDM_H7, &raw_h7)) {
        return BME680_ERROR;
    }
    bme680->par_h7 = (int8_t)raw_h7;
    return BME680_OK;
}

BME680_Status BME680_SetMode(BME680_HandleTypeDef *bme680, uint8_t mode)
{
    uint8_t ctrl_meas_check = 0;
    uint8_t ctrl_meas = 0;

    if (bme680 == NULL || mode > 1U) {
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

    // if ((ctrl_meas_check & 0x03U) != mode) {
    //     return BME680_ERROR;
    // }
    return BME680_OK;
}

BME680_Status BME680_SetTemperatureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling)
{
    uint8_t ctrl_temp_check = 0;
    uint8_t ctrl_meas = 0;
    if (bme680 == NULL || oversampling > 5U) 
    {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_meas)) {
        return BME680_ERROR;
    }
    ctrl_meas &= (uint8_t)~0xE0U;
    ctrl_meas |= (uint8_t)(oversampling << 5);

    if (BME680_OK != writeSpiRegister(bme680->hspi, BME680_REG_CTRL_MEAS, ctrl_meas)) {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi8bitRegister(bme680->hspi, BME680_REG_CTRL_MEAS, &ctrl_temp_check)) {
        return BME680_ERROR;
    }
    if (((ctrl_temp_check & 0xE0U) >> 5) != oversampling) {
        return BME680_ERROR;
    }



    return BME680_OK;

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
    if (((ctrl_press_check & 0x1CU) >> 2) != oversampling) {
        return BME680_ERROR;
    }

    return BME680_OK;
}



BME680_Status BME680_ReadTemperature(BME680_HandleTypeDef *bme680, int32_t *temperature)
{
    if (bme680 == NULL || temperature == NULL || bme680->hspi == NULL) {
        return BME680_ERROR;
    }
    uint32_t adc_temp = 0;

    if (BME680_OK != readSpi24bitRegister(bme680->hspi, &adc_temp, BME680_REG_TEMP_MSB)) {
        return BME680_ERROR;
    }

    /* readSpi24bitRegister already returns the 20-bit ADC value. */
    int64_t var1 = ((int32_t)adc_temp >> 3) - ((int32_t)bme680->par_t1 << 1);
    int64_t var2 = ((int64_t)var1 * (int32_t)bme680->par_t2) >> 11;
    int64_t var3 = (((int64_t)var1 >> 1) * (var1 >> 1)) >> 12;
    var3 = ((int64_t)var3 * ((int32_t)bme680->par_t3 * 16)) >> 14; // było << 4 jest 16, dlatego że jest to bezpieczenijsze, bo nie ma ryzyka przepełnienia 32 bitów

    bme680->t_fine = var2 + var3;
   *temperature = (bme680->t_fine * 5 + 128) >> 8;
    // *temperature = adc_temp;

    return BME680_OK;
}



BME680_Status BME680_ReadHumidity(BME680_HandleTypeDef *bme680, int32_t *humidity)
{
    if (bme680 == NULL || humidity == NULL || bme680->hspi == NULL) {
        return BME680_ERROR;
    }

    uint16_t adc_hum = 0;
    int32_t hum_adc;
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    int32_t var6;
    int32_t temp_scaled;
    int32_t calc_hum;

    if (BME680_OK != readSpi16bitRegister(bme680->hspi, BME680_REG_HUM_MSB, &adc_hum)) {
        return BME680_ERROR;
    }

    hum_adc = (int32_t)adc_hum;
    temp_scaled = (((int32_t)bme680->t_fine * 5) + 128) >> 8;
    var1 = hum_adc - ((int32_t)bme680->par_h1 * 16) -
           (((temp_scaled * (int32_t)bme680->par_h3) / 100) >> 1);
    var2 = ((int32_t)bme680->par_h2 *
            (((temp_scaled * (int32_t)bme680->par_h4) / 100) +
             (((temp_scaled * ((temp_scaled * (int32_t)bme680->par_h5) / 100)) >> 6) / 100) +
             (int32_t)(1 << 14))) >> 10;
    var3 = var1 * var2;
    var4 = (int32_t)bme680->par_h6 << 7;
    var4 = (var4 + ((temp_scaled * (int32_t)bme680->par_h7) / 100)) >> 4;
    var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
    var6 = (var4 * var5) >> 1;
    calc_hum = (((var3 + var6) >> 10) * 1000) >> 12;

    if (calc_hum > 100000) {
        calc_hum = 100000;
    } else if (calc_hum < 0) {
        calc_hum = 0;
    }
    *humidity = calc_hum;
    return BME680_OK;
}

BME680_Status BME680_ReadPressure(
    BME680_HandleTypeDef *bme680,
    int32_t *pressure)
{
    if (bme680 == NULL || pressure == NULL ||
        bme680->hspi == NULL) {
        return BME680_ERROR;
    }

    uint32_t adc_press = 0;
    int64_t var1;
    int64_t var2;
    int64_t var3;
    int64_t calc_pres;

    /* Odczyt 20-bitowego RAW ciśnienia */
    if (readSpi24bitRegister(
            bme680->hspi,
            &adc_press,
            BME680_REG_PRESS_MSB) != BME680_OK) {
        return BME680_ERROR;
    }

    if (adc_press > 0xFFFFFU || bme680->par_p1 == 0U) {
        return BME680_ERROR;
    }

    /* t_fine musi pochodzić z aktualnego pomiaru temperatury */
    var1 = ((int64_t)bme680->t_fine >> 1) - 64000;

    /* Pierwszy blok: współczynniki P4, P5, P6 */
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11)
           * (int64_t)bme680->par_p6;
    var2 >>= 2;

    var2 += var1 * (int64_t)bme680->par_p5 * 2;

    var2 = (var2 >> 2) +
           ((int64_t)bme680->par_p4 * 65536);

    /* Drugi blok: współczynniki P1, P2, P3 */
    var3 = (((var1 >> 2) * (var1 >> 2)) >> 13)
           * ((int64_t)bme680->par_p3 * 32);
    var3 >>= 3;

    var3 += ((int64_t)bme680->par_p2 * var1) >> 1;

    var1 = var3 >> 18;

    var1 = (((int64_t)32768 + var1) *
            (int64_t)bme680->par_p1) >> 15;

    /* Zabezpieczenie dzielenia */
    if (var1 <= 0) {
        return BME680_ERROR;
    }

    /* Główne obliczenie ciśnienia */
    calc_pres = ((int64_t)1048576 -
                 (int64_t)adc_press -
                 (var2 >> 12)) * 3125;

    /* int64_t pozwala wykonać mnożenie przed dzieleniem */
    calc_pres = (calc_pres * 2) / var1;

    /*
     * Kontrola wartości pośredniej.
     * 1 000 000 Pa to szeroka granica bezpieczeństwa
     * przed dalszymi obliczeniami wielomianowymi.
     */
    if (calc_pres <= 0 || calc_pres > 1000000) {
        return BME680_ERROR;
    }

    /* Końcowa korekta: współczynniki P7-P10 */
    var1 = ((int64_t)bme680->par_p9 *
            (((calc_pres >> 3) *
              (calc_pres >> 3)) >> 13)) >> 12;

    var2 = (((calc_pres >> 2) *
             (int64_t)bme680->par_p8)) >> 13;

    var3 = (((calc_pres >> 8) *
             (calc_pres >> 8) *
             (calc_pres >> 8) *
             (int64_t)bme680->par_p10)) >> 17;

    calc_pres += (var1 + var2 + var3 +
                 ((int64_t)bme680->par_p7 * 128)) >> 4;

    /* Kontrola wyniku przed konwersją na int32_t */
    if (calc_pres <= 0 || calc_pres > INT32_MAX) {
        return BME680_ERROR;
    }

    *pressure = (int32_t)calc_pres;

    return BME680_OK;
}

BME680_Status BME680_ReadGasRaw(BME680_HandleTypeDef *bme680,uint16_t *gas_raw)
{
    uint16_t adc_gas = 0;
    if (bme680 == NULL || gas_raw == NULL) 
    {
        return BME680_ERROR;
    }

    if (BME680_OK != readSpi16bitRegister(bme680->hspi,  BME680_GAS_R_MSB, &adc_gas)) {
        return BME680_ERROR;
    }
    *gas_raw = (adc_gas >> 6);

    return BME680_OK;
}



static BME680_Status readSpi8bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint8_t *retValue)
{
    uint8_t retVal = 0;

    if(spiHandle == NULL || retValue == NULL)
    {
        return BME680_ERROR;
    }

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


//static BME680_Status readSpi10bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue)
//{
//    uint8_t retVal[2] = {0};
//
//    if(spiHandle == NULL || retValue == NULL)
//    {
//        return BME680_ERROR;
//    }
//
//    if (tx != BME680_REG_STATUS &&
//        BME680_OK != setSpiMemoryPage(spiHandle, tx)) {
//        return BME680_ERROR;
//    }
//    tx |= 0x80U;
//
//    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
//    HAL_Delay(1);
//    if (HAL_SPI_Transmit(spiHandle, &tx, 1, HAL_MAX_DELAY) != HAL_OK) {
//        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
//        return BME680_ERROR;
//    }
//    if (HAL_SPI_Receive(spiHandle, retVal, 2, HAL_MAX_DELAY) != HAL_OK) {
//        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
//        return BME680_ERROR;
//    }
//    HAL_Delay(1);
//    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
//
//    *retValue = (uint16_t)retVal[0] << 8 | ((uint16_t)retVal[1]>>6);
//    return BME680_OK;
//}

static BME680_Status readSpi16bitRegister(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue)
{
    uint8_t retVal[2] = {0};

    if(spiHandle == NULL || retValue == NULL)
    {
        return BME680_ERROR;
    }

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

    *retValue = (uint16_t)retVal[0] << 8 | ((uint16_t)retVal[1] );
    return BME680_OK;
}

static BME680_Status readSpi16bitRegisterLSBFirst(SPI_HandleTypeDef* spiHandle, uint8_t tx, uint16_t *retValue)
{
    uint8_t retVal[2] = {0};

    if(spiHandle == NULL || retValue == NULL)
    {
        return BME680_ERROR;
    }

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

    *retValue = (uint16_t)retVal[1] << 8 | ((uint16_t)retVal[0] );
    return BME680_OK;
}


static BME680_Status readSpi24bitRegister(SPI_HandleTypeDef* spiHandle, uint32_t *retValue, uint8_t tx)
{
    uint8_t retVal[3] = {0};

    if(spiHandle == NULL || retValue == NULL)
    {
        return BME680_ERROR;
    }

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