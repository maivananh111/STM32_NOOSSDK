
#include "startup.h"

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"


#define mkstr(s) #s

rcc_config_t rcc = {
	.hse_frequency   = HSE_VALUE,
	.hsi_frequency   = HSI_VALUE,
	.hsi_trim        = CONFIG_HSI_TRIM_VALUE,
	.osc_source      = CONFIG_OSC_CLOCK_SOURCE,
	.sysclock_source = CONFIG_SYSTEM_CLOCK_MUX,
	.pll_source      = CONFIG_PLL_SOURCE_MUX,
	.sysclock_frequency = CONFIG_SYSTEM_CLOCK_FREQUENCY,
	.ahb_prescaler      = CONFIG_AHB_PRESCALER,
	.apb1_prescaler 	= CONFIG_APB1_PRESCALER,
	.apb2_prescaler 	= CONFIG_APB2_PRESCALER,
	.pll_mul            = CONFIG_PLLMUL,
};

#if CONFIG_USE_SDRAM && ENABLE_FMC
sdram_config_t sdram_conf = {};
#endif /* CONFIG_USE_SDRAM && ENABLE_FMC */


#if CONFIG_USE_LOG_MONITOR
static const char* TAG = (const char *)"STARTUP";
#if ENABLE_USART && CONFIG_LOG_OVER_UART
static USART_TypeDef *log_uart = (USART_TypeDef *)CONFIG_LOG_UART_NUM;
#endif /* ENABLE_USART && LOG_UART */
#endif /* LOG_MONITOR */

#if defined(USE_HAL_DRIVER)
int main_application(void){
#else
int main(void){
#endif /* USE_HAL_DRIVER */
	system_init();
	rcc_init(&rcc);

#if CONFIG_USE_SDRAM && ENABLE_FMC
	fmc_sdram_init(&sdram_conf);
	sdram_init();
#endif /* CONFIG_USE_SDRAM && ENABLE_FMC */

#if CONFIG_USE_IWDG && ENABLE_IWDG
	iwdg_init(CONFIG_IWDG_PRESCALER, CONFIG_IWDG_AUTORELOAD);
	iwdg_disable_in_debugmode();
#endif /* CONFIG_USE_IWDG && ENABLE_IWDG */

#if	ENABLE_RNG && defined(RNG)
	rng_init();
#endif /* ENABLE_RNG && defined(RNG) */

#if CONFIG_USE_LOG_MONITOR
#if ENABLE_USART && CONFIG_LOG_OVER_UART
	uart_log_init();
	log_init(uart_log);
#endif
#if ENABLE_USB && CONFIG_LOG_OVER_USB
	MX_USB_DEVICE_Init();
	stm_log_init(usb_log);
#endif
	LOG_INFO(TAG, "Target        : %s",     mkstr(STM32F103xx));
	LOG_INFO(TAG, "Revision ID   : 0x%04x", get_revid());
	LOG_INFO(TAG, "Device ID     : 0x%04x", get_devid());
	LOG_INFO(TAG, "Flash size    : %dKb",   get_flashsize());
	LOG_INFO(TAG, "Ram size      : %dKb",   CONFIG_TOTAL_HEAP_SIZE/1024);
	LOG_INFO(TAG, "SDK version   : %s",     SDK_VERSION);
	LOG_INFO(TAG, "Core frequency: %luHz",  rcc_get_bus_frequency(SYSCLK));
	LOG_INFO(TAG, "AHB frequency : %luHz",  rcc_get_bus_frequency(AHB));
	LOG_INFO(TAG, "APB1 frequency: %luHz",  rcc_get_bus_frequency(APB1));
	LOG_INFO(TAG, "APB2 frequency: %luHz",  rcc_get_bus_frequency(APB2));

#endif
	LOG_INFO(TAG, "Calling app_main().");
	extern void app_main(void);
	app_main();
	LOG_INFO(TAG, "Returned from app_main().");

	return 0;
}


#if CONFIG_USE_LOG_MONITOR
#if ENABLE_USART && CONFIG_LOG_OVER_UART
static void uart_log_init(void){
	__IO uint32_t usart_bus_frequency = 0UL;

	gpio_port_clock_enable(CONFIG_LOG_UART_TXP);
	gpio_port_clock_enable(CONFIG_LOG_UART_RXP);
#if defined(STM32F1)
	gpio_set_alternatefunction(CONFIG_LOG_UART_TXP, CONFIG_LOG_UART_TX, GPIO_ALTERNATE_PUSHPULL);
	gpio_set_mode(CONFIG_LOG_UART_RXP, CONFIG_LOG_UART_RX, GPIO_INPUT);
#elif defined(STM32F4)
	if(log_uart == USART1 || log_uart == USART2 || log_uart == USART3){
		gpio_set_alternatefunction(CONFIG_LOG_UART_TXP, CONFIG_LOG_UART_TX, AF7_USART1_3);
		gpio_set_alternatefunction(CONFIG_LOG_UART_RXP, CONFIG_LOG_UART_RX, AF7_USART1_3);
	}
	else{
		gpio_set_alternatefunction(CONFIG_LOG_UART_TXP, CONFIG_LOG_UART_TX, AF8_USART4_8);
		gpio_set_alternatefunction(CONFIG_LOG_UART_RXP, CONFIG_LOG_UART_RX, AF8_USART4_8);
	}
#endif /* STM32F4 */

#if defined(USART1)
	if(log_uart == USART1){
		RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB2);
	}
#endif /* defined(USART1) */
#if defined(USART2)
	if(log_uart == USART2){
		RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB1);
	}
#endif /* defined(USART2) */
#if defined(USART3)
	if(log_uart == USART3){
		RCC -> APB1ENR |= RCC_APB1ENR_USART3EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB1);
	}
#endif /* defined(USART3) */
#if defined(UART4)
	if(log_uart == UART4){
		RCC -> APB1ENR |= RCC_APB1ENR_UART4EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB1);
	}
#endif /* defined(UART4) */
#if defined(UART5)
	if(log_uart == UART5){
		RCC -> APB1ENR |= RCC_APB1ENR_UART5EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB1);
	}
#endif /* defined(UART5) */
#if defined(USART6)
	if(log_uart == USART6){
		RCC -> APB2ENR |= RCC_APB2ENR_USART6EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB2);
	}
#endif /* defined(USART6) */
#if defined(UART7)
	if(log_uart == UART7){
		RCC -> APB1ENR |= RCC_APB1ENR_UART7EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB1);
	}
#endif /* defined(UART7) */
#if defined(UART8)
	if(log_uart == UART8){
		RCC -> APB1ENR |= RCC_APB1ENR_UART8EN;
		usart_bus_frequency = rcc_get_bus_frequency(APB1);
	}
#endif /* defined(UART8) */

	log_uart -> CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	float USARTDIV = (float)(usart_bus_frequency/(CONFIG_LOG_UART_BAUDRATE * 16.0));
	uint16_t DIV_Fraction = 0x00UL;
	uint16_t DIV_Mantissa = (uint16_t)USARTDIV;

	float Fraction = (float)(((float)(((uint16_t)(USARTDIV * 100.0) - (uint16_t)(DIV_Mantissa * 100.0)) / 100.0)) * 16.0);
	DIV_Fraction = ceil(Fraction);
	log_uart -> BRR = (DIV_Mantissa << 4) | (DIV_Fraction << 0);

	uart_log((char *)"\r\n\r\n*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*Target starting*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*\r\n");
}

static void uart_log(char *log){
	while(*log) {
		log_uart -> DR = *log++;
		while(!(log_uart -> SR & USART_SR_TC));
	}
}
#endif /* ENABLE_USART */

#if ENABLE_USB && CONFIG_LOG_OVER_USB
static void usb_log(char *log){
	CDC_Transmit_FS((uint8_t *)log, strlen(log));
}
#endif /* ENABLE_USB && LOG_USB */

#endif /* LOG_MONITOR */













