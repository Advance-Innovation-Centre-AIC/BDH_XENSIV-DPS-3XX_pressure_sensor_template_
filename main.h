/*
 * main.h
 *
 *  Created on: Dec 3, 2022
 *      Author: wiroon
 */

#ifndef MAIN_H_
#define MAIN_H_

#define CY_BMX_160_IMU_SPI
//#define CY_BMX_160_IMU_I2C

#ifdef CY_BMX_160_IMU_SPI
	#define CY_IMU_SPI
#endif

/***************************************
*            Constants
****************************************/
/* SPI related */
#define SPI_FREQ_HZ         (8000000UL)
#define mSPI_MOSI           CYBSP_SPI_MOSI
#define mSPI_MISO           CYBSP_SPI_MISO
#define mSPI_SCLK           CYBSP_SPI_CLK
#define mSPI_SS             CYBSP_SPI_CS

/******************************************************************************
* Macros
******************************************************************************/
/* Macro to check if the result of an operation was successful. When it has
 * failed, print the error message and suspend the task.
 */
#define CHECK_RESULT(result, error_message...)                        \
                     do                                               \
                     {                                                \
                         if ((cy_rslt_t)result != CY_RSLT_SUCCESS)    \
                         {                                            \
                             printf(error_message);                   \
                             vTaskSuspend(NULL);                      \
                         }                                            \
                     } while(0)

/* Check if the EPD and TFT shields are being used with non Pioneer kits and
 * throw a compile-time error accordingly.
 */

#endif /* MAIN_H_ */
