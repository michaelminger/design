//*****************************************************************************
//
// LM3S1138 Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifndef _LM3S1138_PINMAP_H_
#define _LM3S1138_PINMAP_H_


#define C0O_PERIPH              (SYSCTL_PERIPH_GPIOF)
#define C0O_PORT                (GPIO_PORTF_BASE)
#define C0O_PIN                 (GPIO_PIN_4)

#define C0_MINUS_PERIPH         (SYSCTL_PERIPH_GPIOB)
#define C0_MINUS_PORT           (GPIO_PORTB_BASE)
#define C0_MINUS_PIN            (GPIO_PIN_4)

#define C0_PLUS_PERIPH          (SYSCTL_PERIPH_GPIOB)
#define C0_PLUS_PORT            (GPIO_PORTB_BASE)
#define C0_PLUS_PIN             (GPIO_PIN_6)

#define C1O_PERIPH              (SYSCTL_PERIPH_GPIOF)
#define C1O_PORT                (GPIO_PORTF_BASE)
#define C1O_PIN                 (GPIO_PIN_5)

#define C1_MINUS_PERIPH         (SYSCTL_PERIPH_GPIOB)
#define C1_MINUS_PORT           (GPIO_PORTB_BASE)
#define C1_MINUS_PIN            (GPIO_PIN_5)

#define C1_PLUS_PERIPH          (SYSCTL_PERIPH_GPIOC)
#define C1_PLUS_PORT            (GPIO_PORTC_BASE)
#define C1_PLUS_PIN             (GPIO_PIN_5)

#define C2O_PERIPH              (SYSCTL_PERIPH_GPIOC)
#define C2O_PORT                (GPIO_PORTC_BASE)
#define C2O_PIN                 (GPIO_PIN_6)

#define C2_MINUS_PERIPH         (SYSCTL_PERIPH_GPIOC)
#define C2_MINUS_PORT           (GPIO_PORTC_BASE)
#define C2_MINUS_PIN            (GPIO_PIN_7)

#define C2_PLUS_PERIPH          (SYSCTL_PERIPH_GPIOC)
#define C2_PLUS_PORT            (GPIO_PORTC_BASE)
#define C2_PLUS_PIN             (GPIO_PIN_6)

#define CCP0_PERIPH             (SYSCTL_PERIPH_GPIOB)
#define CCP0_PORT               (GPIO_PORTB_BASE)
#define CCP0_PIN                (GPIO_PIN_0)

#define CCP1_PERIPH             (SYSCTL_PERIPH_GPIOF)
#define CCP1_PORT               (GPIO_PORTF_BASE)
#define CCP1_PIN                (GPIO_PIN_6)

#define CCP2_PERIPH             (SYSCTL_PERIPH_GPIOB)
#define CCP2_PORT               (GPIO_PORTB_BASE)
#define CCP2_PIN                (GPIO_PIN_1)

#define CCP3_PERIPH             (SYSCTL_PERIPH_GPIOG)
#define CCP3_PORT               (GPIO_PORTG_BASE)
#define CCP3_PIN                (GPIO_PIN_4)

#define CCP4_PERIPH             (SYSCTL_PERIPH_GPIOF)
#define CCP4_PORT               (GPIO_PORTF_BASE)
#define CCP4_PIN                (GPIO_PIN_7)

#define CCP5_PERIPH             (SYSCTL_PERIPH_GPIOC)
#define CCP5_PORT               (GPIO_PORTC_BASE)
#define CCP5_PIN                (GPIO_PIN_4)

#define I2C0SCL_PERIPH          (SYSCTL_PERIPH_GPIOB)
#define I2C0SCL_PORT            (GPIO_PORTB_BASE)
#define I2C0SCL_PIN             (GPIO_PIN_2)

#define I2C0SDA_PERIPH          (SYSCTL_PERIPH_GPIOB)
#define I2C0SDA_PORT            (GPIO_PORTB_BASE)
#define I2C0SDA_PIN             (GPIO_PIN_3)

#define I2C1SCL_PERIPH          (SYSCTL_PERIPH_GPIOA)
#define I2C1SCL_PORT            (GPIO_PORTA_BASE)
#define I2C1SCL_PIN             (GPIO_PIN_6)

#define I2C1SDA_PERIPH          (SYSCTL_PERIPH_GPIOA)
#define I2C1SDA_PORT            (GPIO_PORTA_BASE)
#define I2C1SDA_PIN             (GPIO_PIN_7)

#define SSI0CLK_PERIPH          (SYSCTL_PERIPH_GPIOA)
#define SSI0CLK_PORT            (GPIO_PORTA_BASE)
#define SSI0CLK_PIN             (GPIO_PIN_2)

#define SSI0FSS_PERIPH          (SYSCTL_PERIPH_GPIOA)
#define SSI0FSS_PORT            (GPIO_PORTA_BASE)
#define SSI0FSS_PIN             (GPIO_PIN_3)

#define SSI0RX_PERIPH           (SYSCTL_PERIPH_GPIOA)
#define SSI0RX_PORT             (GPIO_PORTA_BASE)
#define SSI0RX_PIN              (GPIO_PIN_4)

#define SSI0TX_PERIPH           (SYSCTL_PERIPH_GPIOA)
#define SSI0TX_PORT             (GPIO_PORTA_BASE)
#define SSI0TX_PIN              (GPIO_PIN_5)

#define SSI1CLK_PERIPH          (SYSCTL_PERIPH_GPIOE)
#define SSI1CLK_PORT            (GPIO_PORTE_BASE)
#define SSI1CLK_PIN             (GPIO_PIN_0)

#define SSI1FSS_PERIPH          (SYSCTL_PERIPH_GPIOE)
#define SSI1FSS_PORT            (GPIO_PORTE_BASE)
#define SSI1FSS_PIN             (GPIO_PIN_1)

#define SSI1RX_PERIPH           (SYSCTL_PERIPH_GPIOE)
#define SSI1RX_PORT             (GPIO_PORTE_BASE)
#define SSI1RX_PIN              (GPIO_PIN_2)

#define SSI1TX_PERIPH           (SYSCTL_PERIPH_GPIOE)
#define SSI1TX_PORT             (GPIO_PORTE_BASE)
#define SSI1TX_PIN              (GPIO_PIN_3)

#define SWCLK_PERIPH            (SYSCTL_PERIPH_GPIOC)
#define SWCLK_PORT              (GPIO_PORTC_BASE)
#define SWCLK_PIN               (GPIO_PIN_0)

#define SWDIO_PERIPH            (SYSCTL_PERIPH_GPIOC)
#define SWDIO_PORT              (GPIO_PORTC_BASE)
#define SWDIO_PIN               (GPIO_PIN_1)

#define SWO_PERIPH              (SYSCTL_PERIPH_GPIOC)
#define SWO_PORT                (GPIO_PORTC_BASE)
#define SWO_PIN                 (GPIO_PIN_3)

#define TCK_PERIPH              (SYSCTL_PERIPH_GPIOC)
#define TCK_PORT                (GPIO_PORTC_BASE)
#define TCK_PIN                 (GPIO_PIN_0)

#define TDI_PERIPH              (SYSCTL_PERIPH_GPIOC)
#define TDI_PORT                (GPIO_PORTC_BASE)
#define TDI_PIN                 (GPIO_PIN_2)

#define TDO_PERIPH              (SYSCTL_PERIPH_GPIOC)
#define TDO_PORT                (GPIO_PORTC_BASE)
#define TDO_PIN                 (GPIO_PIN_3)

#define TMS_PERIPH              (SYSCTL_PERIPH_GPIOC)
#define TMS_PORT                (GPIO_PORTC_BASE)
#define TMS_PIN                 (GPIO_PIN_1)

#define TRST_PERIPH             (SYSCTL_PERIPH_GPIOB)
#define TRST_PORT               (GPIO_PORTB_BASE)
#define TRST_PIN                (GPIO_PIN_7)

#define U0RX_PERIPH             (SYSCTL_PERIPH_GPIOA)
#define U0RX_PORT               (GPIO_PORTA_BASE)
#define U0RX_PIN                (GPIO_PIN_0)

#define U0TX_PERIPH             (SYSCTL_PERIPH_GPIOA)
#define U0TX_PORT               (GPIO_PORTA_BASE)
#define U0TX_PIN                (GPIO_PIN_1)

#define U1RX_PERIPH             (SYSCTL_PERIPH_GPIOD)
#define U1RX_PORT               (GPIO_PORTD_BASE)
#define U1RX_PIN                (GPIO_PIN_2)

#define U1TX_PERIPH             (SYSCTL_PERIPH_GPIOD)
#define U1TX_PORT               (GPIO_PORTD_BASE)
#define U1TX_PIN                (GPIO_PIN_3)

#define U2RX_PERIPH             (SYSCTL_PERIPH_GPIOG)
#define U2RX_PORT               (GPIO_PORTG_BASE)
#define U2RX_PIN                (GPIO_PIN_0)

#define U2TX_PERIPH             (SYSCTL_PERIPH_GPIOG)
#define U2TX_PORT               (GPIO_PORTG_BASE)
#define U2TX_PIN                (GPIO_PIN_1)


#endif // _LM3S1138_PINMAP_H_

