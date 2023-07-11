/**
 * Definition of I/O pins
 */

#ifndef __SELECTIO_H
#define __SELECTIO_H

#define DOES_NOT_APPLY                    (0xFF)
#define SIGNAL_LOW                        false
#define SIGNAL_HIGH                       true

#define LL_GPIO_AF_DOES_NOT_APPLY  LL_GPIO_AF_0

// For nucleo STM32L432KC

// PA9 is debug transmit, use for HOST port
// This define must be here so macros nest properly
#define HOST_TX                          HOST_TX
#define HOST_TX_ENABLE_FN                LL_AHB2_GRP1_EnableClock 
#define HOST_TX_ENABLE_SELECT            LL_AHB2_GRP1_PERIPH_GPIOA
#define HOST_TX_PORT_NAME                A
#define HOST_TX_PORT_BASE                GPIOA
#define HOST_TX_PIN_MASK                 LL_GPIO_PIN_9
#define HOST_TX_PIN_MODE                 LL_GPIO_MODE_ALTERNATE
#define HOST_TX_AF_NUMBER                LL_GPIO_AF_7
#define HOST_TX_PIN_INIT_STATE           SIGNAL_LOW
#define HOST_TX_PIN_ACTIVE_STATE         SIGNAL_HIGH
#define HOST_TX_PIN_INACTIVE_STATE       SIGNAL_LOW
#define HOST_TX_PIN_DRIVE                LL_GPIO_OUTPUT_PUSHPULL
#define HOST_TX_PIN_PULLSTATE            LL_GPIO_PULL_NO
#define HOST_TX_PIN_SPEED                LL_GPIO_SPEED_FREQ_VERY_HIGH

// PA10 is debug receive
#define HOST_RX                          HOST_RX
#define HOST_RX_ENABLE_FN                LL_AHB2_GRP1_EnableClock 
#define HOST_RX_ENABLE_SELECT            LL_AHB2_GRP1_PERIPH_GPIOA
#define HOST_RX_PORT_NAME                A
#define HOST_RX_PORT_BASE                GPIOA
#define HOST_RX_PIN_MASK                 LL_GPIO_PIN_10
#define HOST_RX_PIN_MODE                 LL_GPIO_MODE_ALTERNATE
#define HOST_RX_AF_NUMBER                LL_GPIO_AF_7
#define HOST_RX_PIN_INIT_STATE           SIGNAL_LOW
#define HOST_RX_PIN_ACTIVE_STATE         SIGNAL_HIGH
#define HOST_RX_PIN_INACTIVE_STATE       SIGNAL_LOW
#define HOST_RX_PIN_DRIVE                LL_GPIO_OUTPUT_PUSHPULL
#define HOST_RX_PIN_PULLSTATE            LL_GPIO_PULL_NO
#define HOST_RX_PIN_SPEED                LL_GPIO_SPEED_FREQ_VERY_HIGH


// Virtual com port on STLINK

#define VCP_TX                          VCP_TX
#define VCP_TX_ENABLE_FN                LL_AHB2_GRP1_EnableClock 
#define VCP_TX_ENABLE_SELECT            LL_AHB2_GRP1_PERIPH_GPIOA
#define VCP_TX_PORT_NAME                A
#define VCP_TX_PORT_BASE                GPIOA
#define VCP_TX_PIN_MASK                 LL_GPIO_PIN_2
#define VCP_TX_PIN_MODE                 LL_GPIO_MODE_ALTERNATE
#define VCP_TX_AF_NUMBER                LL_GPIO_AF_7
#define VCP_TX_PIN_INIT_STATE           SIGNAL_LOW
#define VCP_TX_PIN_ACTIVE_STATE         SIGNAL_HIGH
#define VCP_TX_PIN_INACTIVE_STATE       SIGNAL_LOW
#define VCP_TX_PIN_DRIVE                LL_GPIO_OUTPUT_PUSHPULL
#define VCP_TX_PIN_PULLSTATE            LL_GPIO_PULL_NO
#define VCP_TX_PIN_SPEED                LL_GPIO_SPEED_FREQ_VERY_HIGH

#define VCP_RX                          VCP_RX
#define VCP_RX_ENABLE_FN                LL_AHB2_GRP1_EnableClock 
#define VCP_RX_ENABLE_SELECT            LL_AHB2_GRP1_PERIPH_GPIOA
#define VCP_RX_PORT_NAME                A
#define VCP_RX_PORT_BASE                GPIOA
#define VCP_RX_PIN_MASK                 LL_GPIO_PIN_15
#define VCP_RX_PIN_MODE                 LL_GPIO_MODE_ALTERNATE
#define VCP_RX_AF_NUMBER                LL_GPIO_AF_3
#define VCP_RX_PIN_INIT_STATE           SIGNAL_LOW
#define VCP_RX_PIN_ACTIVE_STATE         SIGNAL_HIGH
#define VCP_RX_PIN_INACTIVE_STATE       SIGNAL_LOW
#define VCP_RX_PIN_DRIVE                LL_GPIO_OUTPUT_PUSHPULL
#define VCP_RX_PIN_PULLSTATE            LL_GPIO_PULL_NO
#define VCP_RX_PIN_SPEED                LL_GPIO_SPEED_FREQ_VERY_HIGH


#define GREEN_LED                        GREEN_LED
#define GREEN_LED_ENABLE_FN              LL_AHB2_GRP1_EnableClock 
#define GREEN_LED_ENABLE_SELECT          LL_AHB2_GRP1_PERIPH_GPIOB
#define GREEN_LED_PORT_NAME              A
#define GREEN_LED_PORT_BASE              GPIOB
#define GREEN_LED_PIN_MASK               LL_GPIO_PIN_3
#define GREEN_LED_PIN_MODE               LL_GPIO_MODE_INPUT
#define GREEN_LED_AF_NUMBER              LL_GPIO_AF_DOES_NOT_APPLY
#define GREEN_LED_PIN_INIT_STATE         SIGNAL_LOW
#define GREEN_LED_PIN_ACTIVE_STATE       SIGNAL_HIGH
#define GREEN_LED_PIN_INACTIVE_STATE     SIGNAL_LOW
#define GREEN_LED_PIN_DRIVE              LL_GPIO_OUTPUT_PUSHPULL
#define GREEN_LED_PIN_PULLSTATE          LL_GPIO_PULL_NO
#define GREEN_LED_PIN_SPEED              LL_GPIO_SPEED_FREQ_VERY_HIGH


#endif
