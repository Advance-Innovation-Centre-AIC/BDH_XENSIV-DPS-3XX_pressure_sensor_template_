/*
 * https://github.com/edgeimpulse/mtb-example-edgeimpulse-continuous-motion
 * https://github.com/Infineon/CY8CKIT-028-SENSE
 * https://github.com/Infineon/display-oled-ssd1306
 * https://github.com/Infineon/sensor-xensiv-dps3xx
 * https://github.com/infineon/sensor-orientation-bmx160
 * https://www.infineon.com/dgdl/Infineon-CY8CEVAL-062S2_Kit_Guide-UserManual-v01_00-EN.pdf
 *
 * Troubleshooting
 * BMX160 chip ID
 * This project includes a modified version of the BMI160 library in order to have
 * the expected CHIPID for the BMX160(0xD8) accelerometer on board the IoT SENSE Expansion Kit.
 * The CHIPID was updated at BMI160_CHIP_ID in the bmi160_defs.h file.
 *
 * at mtb_shared folder --> /mtb_shared --> BMI160_driver/ --> bmi160_v3.9.1/ --> bmi160_defs.h
 *
 */

#include "cyhal.h"
#include "cybsp.h"
#include "cy_result.h"
#include "cy_retarget_io.h"
#include "mtb_bmx160.h"
#include "main.h"
#include "sensor.h"

#ifdef CY_BMX_160_IMU_I2C
static mtb_bmx160_t motion_sensor;
static mtb_bmx160_data_t data;
static cyhal_i2c_t i2c;
cyhal_i2c_cfg_t i2c_cfg = {
    .is_slave = false,
    .address = 0,
    .frequencyhal_hz = 400000
};

#define IMU_I2C_SDA (CYBSP_I2C_SDA)
#define IMU_I2C_SCL (CYBSP_I2C_SCL)
#endif

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

#ifdef CY_BMX_160_IMU_I2C
    /* Initialize i2c for motion sensor */
    printf("Initializing the I2C...\n");
    result = cyhal_i2c_init(&i2c, IMU_I2C_SDA, IMU_I2C_SCL, NULL);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    printf("Configuring the I2C...\n");
    result = cyhal_i2c_configure(&i2c, &i2c_cfg);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize motion sensor */
    printf("Initializing the BMX160...\n");
    result = mtb_bmx160_init_i2c(&motion_sensor, &i2c, MTB_BMX160_ADDRESS_DEFAULT);
    //CY_ASSERT(result == CY_RSLT_SUCCESS);
    if(CY_RSLT_SUCCESS != result)
           {
    		printf("Debug: BMX160 Initialization [Error code: 0x%lx]\n", (long unsigned int)result);
            return result;
           }else
   		    printf("Debug: BMX160 Initialization Success\n");
#endif

#ifdef CY_BMX_160_IMU_SPI
    result = sensor_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);
#endif

    for (;;)
    {

#ifdef CY_BMX_160_IMU_I2C
    	/* Get the accel, gyro, and mag data and print the results to the UART */
        mtb_bmx160_read(&motion_sensor, &data);

        printf("(I2C)Accel: X:%6d Y:%6d Z:%6d\r\n", data.accel.x, data.accel.y, data.accel.z);
        printf("(I2C)Gyro : X:%6d Y:%6d Z:%6d\r\n\r\n", data.gyro.x, data.gyro.y, data.gyro.z);
        printf("(I2C)Mag  : X:%6d Y:%6d Z:%6d\r\n\r\n", data.mag.x, data.mag.y, data.mag.z);

        cyhal_system_delay_ms(100);
#endif

#ifdef CY_BMX_160_IMU_SPI
        result = sensor_get_data();
        CY_ASSERT(result == CY_RSLT_SUCCESS);
        cyhal_system_delay_ms(100);
#endif

    }
}
/* [] END OF FILE */
