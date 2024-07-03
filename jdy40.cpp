#include "jdy40.h"

void JDY40::configUart(void)
{
  huart->Init.BaudRate = getBaudRate();
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

uint32_t JDY40::getBaudRate(void)
{
	uint32_t _BaudRate = 9600;
	switch(BaudRate)
	{
		case 1:
			_BaudRate = 1200;
			break;
		case 2:
			_BaudRate = 2400;
			break;
		case 3:
			_BaudRate = 4800;
			break;
		case 4:
			_BaudRate = 9600;
			break;
		case 5:
			_BaudRate = 14400;
			break;
		case 6:
			_BaudRate = 19200;
			break;
		default:
			_BaudRate = 9600;
			break;
	}
	return _BaudRate;
}

void JDY40::init(UART_HandleTypeDef *_huart,uint8_t _BaudRate,uint16_t _WirelessID,uint16_t _DeviceID,uint8_t _Channel,uint8_t _POWE,uint8_t *_CLSS)
{
	huart = _huart;
	
	configUart();	
	HAL_UART_Receive_IT(huart,Uart_Struct.Rcv_Data,BUFFER_SIZE);
	
	BaudRate = _BaudRate;
	WirelessID = _WirelessID;
	DeviceID = _DeviceID;
	Channel = _Channel;
	POWE = _POWE;
	CLSS[0] = _CLSS[0];
	CLSS[1] = _CLSS[1];
	
	awakeMode();
	transparentMode();
}

void JDY40::set_gpio_output(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = _GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(_GPIOx,&GPIO_InitStruct);
}

void JDY40::configSetPin(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin)
{
	SETPin = _GPIO_Pin;
	SETPinGPIOx = _GPIOx;
	set_gpio_output(SETPinGPIOx,SETPin);
}

void JDY40::configCSPin(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin)
{
	CSPin = _GPIO_Pin;
	CSPinGPIOx = _GPIOx;
	set_gpio_output(CSPinGPIOx,CSPin);
}

HAL_StatusTypeDef JDY40::sendData(uint8_t *_Data,uint32_t _Len)
{
	if(_Len == 0)
	{
		while(_Data[_Len] != 0)_Len++;
	}
	return HAL_UART_Transmit_DMA(huart,_Data,_Len);
}

void JDY40::updateState(void)
{
	if(Uart_Struct.Timeout > 0)
	{
		Uart_Struct.Timeout--;
		if(Uart_Struct.Timeout == 0)
		{
			__HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
			/* Check if a transmit process is ongoing or not */
      if(huart->RxState == HAL_UART_STATE_BUSY_TX_RX) {
        huart->RxState = HAL_UART_STATE_BUSY_TX;
      }
      else{
        /* Disable the UART Parity Error Interrupt */
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
        huart->RxState = HAL_UART_STATE_READY;
      }
			Uart_Struct.Len = huart->RxXferSize - huart->RxXferCount;
			Uart_Struct.Rcv_Data[Uart_Struct.Len] = 0x00;
			Uart_Struct.State = Recieve;
		}
	}
}

void JDY40::retryTimeout(USART_TypeDef *_Instance)
{
	if(_Instance == huart->Instance)
	{
		Uart_Struct.Timeout = JDY40UartTimeout;
	}
}

Port_State JDY40::checkState(void)
{
	return Uart_Struct.State;
}

void JDY40::awakeMode(void)
{
	HAL_GPIO_WritePin(CSPinGPIOx,CSPin,GPIO_PIN_RESET);
}

void JDY40::sleepMode(void)
{
	HAL_GPIO_WritePin(CSPinGPIOx,CSPin,GPIO_PIN_SET);
}

void JDY40::atCommandMode(void)
{
	HAL_GPIO_WritePin(SETPinGPIOx,SETPin,GPIO_PIN_RESET);
}

void JDY40::transparentMode(void)
{
	HAL_GPIO_WritePin(SETPinGPIOx,SETPin,GPIO_PIN_SET);
}

uint8_t *JDY40::getData(void)
{
	return Uart_Struct.Rcv_Data;
}

uint32_t JDY40::getDataLen(void)
{
	return Uart_Struct.Len;
}
