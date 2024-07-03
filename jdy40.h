#ifndef JDY40_H
#define JDY40_H

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define JDY40UartTimeout  10
#define BUFFER_SIZE 100

typedef enum
{
	Nothing = 0,
	Recieve
}Port_State;

typedef struct {
	Port_State State;
	uint8_t Rcv_Data[BUFFER_SIZE];
	uint32_t Len;
	uint16_t Timeout;
}JDY40UartData_t;

class JDY40
{
	private:
		UART_HandleTypeDef *huart;
		JDY40UartData_t Uart_Struct;
		uint8_t BaudRate;//1:1200 2:2400 3:4800 *4:9600 5:14400 6:19200
		uint16_t WirelessID;// 0000 - FFFF *8899
		uint16_t DeviceID;// 0000 - FFFF *1122
		uint8_t Channel;//001 - 128 *001;
		uint8_t POWE;//0:-25db 1:-15db 2:-5db 3:0db 4:+3db 5:+6db 6:+9db 7:+10db *8:+10db 9:+12db
		uint8_t CLSS[2];//*"A0"
		GPIO_TypeDef *SETPinGPIOx,*CSPinGPIOx;
		uint16_t SETPin,CSPin;
		void configUart(void);
		uint32_t getBaudRate(void);
		void set_gpio_output(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin);
		void sleepMode(void);
		void awakeMode(void);
		void atCommandMode(void);
		void transparentMode(void);
	public:
		void init(UART_HandleTypeDef *_huart,uint8_t _BaudRate=4,uint16_t _WirelessID=8899,uint16_t _DeviceID=1122,uint8_t _Channel=1,uint8_t _POWE=8,uint8_t *_CLSS=((uint8_t *)"A0"));
		void configSetPin(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin);
		void configCSPin(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin);
		HAL_StatusTypeDef sendData(uint8_t *_Data,uint32_t _Len);
		void updateState(void);//check this function every one milliseconds
		void retryTimeout(USART_TypeDef *_Instance);//add this to "static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)"
		Port_State checkState(void);
		uint8_t *getData(void);
		uint32_t getDataLen(void);
};

#endif
