/*
 * https://github.com/Infineon/CY8CKIT-028-SENSE
 * https://github.com/Infineon/display-oled-ssd1306
 * https://github.com/Infineon/sensor-xensiv-dps3xx
 * https://github.com/infineon/sensor-orientation-bmx160
 * https://www.infineon.com/dgdl/Infineon-CY8CEVAL-062S2_Kit_Guide-UserManual-v01_00-EN.pdf
 */

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "xensiv_dps3xx_mtb.h"

#define ADC_MAX_VALUE 4095
#define PRESSURE_SENSOR_VOLTAGE 3.3
#define PRESSURE_SENSOR_SLOPE 0.04
#define PRESSURE_SENSOR_OFFSET 0.1

xensiv_dps3xx_t pressure_sensor;
cyhal_i2c_t i2c;
cyhal_i2c_cfg_t i2c_cfg = {
    .is_slave = false,
    .address = 0,
    .frequencyhal_hz = 400000
};

#define DPS_I2C_SDA (P6_1)
#define DPS_I2C_SCL (P6_0)

int main(void)
{
    cy_rslt_t result;
    float voltage, pressure_hPa, pressure_mbar;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize i2c for pressure sensor */
    result = cyhal_i2c_init(&i2c, DPS_I2C_SDA, DPS_I2C_SCL, NULL);
    CY_ASSERT(result == CY_RSLT_SUCCESS);
    result = cyhal_i2c_configure(&i2c, &i2c_cfg);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize pressure sensor */
    result = xensiv_dps3xx_mtb_init_i2c(&pressure_sensor, &i2c, XENSIV_DPS3XX_I2C_ADDR_DEFAULT);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    for (;;)
    {
        /* Get the pressure and temperature data and print the results to the UART */
        float pressure, temperature;
        xensiv_dps3xx_read(&pressure_sensor, &pressure, &temperature);

        // Convert the ADC reading to voltage
            voltage = (float) pressure * PRESSURE_SENSOR_VOLTAGE / ADC_MAX_VALUE;

            // Convert the voltage to pressure in hPa
            pressure_hPa = (voltage - PRESSURE_SENSOR_OFFSET) / PRESSURE_SENSOR_SLOPE;

            // Convert the pressure to mbar
            pressure_mbar = pressure_hPa * 10;

        printf("Pressure   : %8f\r\n", pressure);
        printf("Pressure (hectopascal)   : %8f hPa\r\n", pressure_hPa);
        printf("Pressure (millibar)  : %8f mbar\r\n", pressure_mbar);
        printf("Temperature: %8f Celcius\r\n\r\n", temperature);

        // For Serial Studio
        //printf("/*%.2f,%.2f,%.2f*/\r\n",pressure_hPa, pressure_mbar, temperature);

        cyhal_system_delay_ms(200);
    }
}
