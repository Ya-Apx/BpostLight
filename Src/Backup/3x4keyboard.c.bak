#include "bposLight.h"
#include "3x4keyboard.h"
#include "math.h"
#include "stdio.h"
//#include "stdlib.h"

//#define debug_keyRead_keys
//#define debug_keyChangeColumn

uint8_t columnsCounter=0;
//uint8_t numCounter=0;

uint32_t sysTick =0;
uint32_t savedTick =0;

uint8_t indexCounter=0;

uint8_t firstNum=0;
uint8_t secondNum=0;

uint8_t number[]={0x30, 0x30};

//extern uart_req bposLightReq;

/***********************************************************/
/**************передача запроса по USART1*******************/
/***********************************************************/
void usart_TX_3x4 (uint8_t* message, uint8_t size)
{
	uint8_t ind = 0;
  while (ind<size)
  {
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
    LL_USART_TransmitData8(USART1,*(uint8_t*)(message+ind));
    ind++;
  }
}
/***********************************************************/

/***********************************************************/
/**************передача запроса по USART2*******************/
/**************UINT32_T**TU**ASCII**DEBUG*******************/
/***********************************************************/
void usart_DEBUG_ASCII_3x4(uint32_t value)
{
			//uint8_t* buffer;
	
		char buffer[12]={0,0,0,0,0,0,0,0,0,0,0,0};
		
		sprintf(buffer, "%u", value );
	
	uint8_t ind = 0;
  while (ind<sizeof(buffer)&&(*(uint8_t*)(buffer+ind)))
  {
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
    LL_USART_TransmitData8(USART1,*(uint8_t*)(buffer+ind));
    ind++;
  }
	while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
  LL_USART_TransmitData8(USART1, 0x0a);
}
/***********************************************************/

/***********************************************************/
/***************перебираем столбци клавиатуры****************/
/***********************************************************/
void ChangeColumn (void)
{
	columnsCounter=(columnsCounter+1)%3;
		switch (columnsCounter)
    {
        case 0:
        { 
				#ifdef debug_keyChangeColumn
				/**************************key1******************************/
				uint8_t messageKey[]={0x30, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif					
 					LL_GPIO_SetOutputPin(GPIOA, COLUMN0_Pin);
					LL_GPIO_ResetOutputPin(GPIOA, COLUMN1_Pin | COLUMN2_Pin);
          ReadKey(columnsCounter);
					//LL_mDelay(10);
					break;
        }
        case 1:
        { 
				#ifdef debug_keyChangeColumn
				/**************************key1******************************/
				uint8_t messageKey[]={0x31, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif										
					LL_GPIO_SetOutputPin(GPIOA, COLUMN1_Pin);
					LL_GPIO_ResetOutputPin(GPIOA, COLUMN0_Pin | COLUMN2_Pin);        
          ReadKey(columnsCounter);					
					//LL_mDelay(10);
					break;					
        }     
        case 2:
        { 
				#ifdef debug_keyChangeColumn
				/**************************key1******************************/
				uint8_t messageKey[]={0x32, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif										
					LL_GPIO_SetOutputPin(GPIOA, COLUMN2_Pin);
					LL_GPIO_ResetOutputPin(GPIOA, COLUMN1_Pin | COLUMN0_Pin);
          ReadKey(columnsCounter);
					//LL_mDelay(10);
					break;
        }
        default:
        {
            break;
        }
    }
}
/***********************************************************/

/***********************************************************/
/**************определяем какая кнопка нажата***************/
/************считывая состояние на пинах рядов**************/
/***********************************************************/
void ReadKey (uint8_t column)
{
	switch (column)
	{
		case 0:
		{
			//кнопка 1
			if (LL_GPIO_IsInputPinSet(GPIOA, LINE0_Pin))
			{	
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key1******************************/
				uint8_t messageKey[]={0x31, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x31;
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}	
			//кнопка 4
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE1_Pin))
			{
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key4******************************/
				uint8_t messageKey[]={0x34, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x34;	
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			//кнопка 7
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE2_Pin))
			{
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key7******************************/
				uint8_t messageKey[]={0x37, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x37;
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			//кнопка "*" (отмена), очищаем number и indexCounter
			//и шлем EOT
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE3_Pin))
			{
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
					#ifdef debug_keyRead_keys
					/**************************key"*"****************************/
					uint8_t messageKey[]={0x2a, 0x0a};
					usart_DEBUG(messageKey, sizeof(messageKey));
					/***********************************************************/
					#endif
					CanceleBeep();
					//шлем EOT, логика следующая
					//
					EOTreq();
					//сбрасываем буфер
					indexCounter=0;
					number[0]=0x30;	
					number[1]=0x30;
					//обнуляем время последнего срабатывания
					savedTick=0;
				}	
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			break;
		}	
		case 1:
		{
			//кнопка 2
			if (LL_GPIO_IsInputPinSet(GPIOA, LINE0_Pin))
			{	
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key2******************************/
				uint8_t messageKey[]={0x32, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x32;
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}	
			//кнопка 5
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE1_Pin))
			{
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key5******************************/
				uint8_t messageKey[]={0x35, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x35;
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			//кнопка 8
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE2_Pin))
			{
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key8******************************/
				uint8_t messageKey[]={0x38, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x38;	
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			//кнопка 0
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE3_Pin))
			{
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key0*****************************/
				uint8_t messageKey[]={0x30, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x30;				
					//обнуляем время последнего срабатывания
					savedTick=0;
				}	
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			break;
		}	
		case 2:
		{
			//кнопка 3
			if (LL_GPIO_IsInputPinSet(GPIOA, LINE0_Pin))
			{
				//если с момента последнего срабатывания прошло больше 50 секунд
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key3******************************/
				uint8_t messageKey[]={0x33, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x33;								
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}	
			//кнопка 6
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE1_Pin))
			{
				//если с момента последнего срабатывания прошло больше 50 секунд
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key6******************************/
				uint8_t messageKey[]={0x36, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x36;				
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			//кнопка 9
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE2_Pin))
			{
				//если с момента последнего срабатывания прошло больше 50 секунд
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
				#ifdef debug_keyRead_keys
				/**************************key9******************************/
				uint8_t messageKey[]={0x39, 0x0a};
				usart_DEBUG(messageKey, sizeof(messageKey));
				/***********************************************************/
				#endif
					Beep();
					//инкрементируем индекс в пределах 0-1 и 
					//записываем число соответствующее нажатой кнопке в буффер
					indexCounter=(indexCounter+1)%2;
					number[indexCounter]=0x39;				
					//обнуляем время последнего срабатывания
					savedTick=0;
				}
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			//кнопка "#" (подтвердить), шлем PUR_ASCII(number)  ---- нужно переписать PUR под прием ASCII
			//очищаем number и indexCounter
			else if (LL_GPIO_IsInputPinSet(GPIOA, LINE3_Pin))
			{
				//если с момента последнего срабатывания прошло больше 50 секунд
				if ((savedTick!=0)&&(sysTick-savedTick)>50) //
				{
					#ifdef debug_keyRead_keys
					/**************************key"#"***************************/
					uint8_t messageKey[]={0x23, 0x0a};
					usart_DEBUG(messageKey, sizeof(messageKey));
					/***********************************************************/
					#endif
					ConfBeep();
					//если в буфере не нули (0x30 ноль в ASCII) шлем запрос
					if (number[0]!=0x30||number[1]!=0x30) 
					{
						//шлем запрос
						PURreq_ASCII(number,sizeof(number)); // number[1] и number[0] перепутаны местами					
					}
					//#ifdef debug_keyRead_keys
					/**********************temp_debug***************************/
					else
					{
						uint8_t messageKey[]={0x4e, 0x4f, 0x50, 0x0a};
						usart_DEBUG(messageKey, sizeof(messageKey));
					}
					/***********************************************************/
					//#endif
					
					//сбрасываем буфер
					indexCounter=0;
					number[0]=0x30;	
					number[1]=0x30;					
					//обнуляем время последнего срабатывания
					savedTick=0;
				}	
				else
				{
					//сохраняем время срабатывания
					savedTick=sysTick;
				}
			}
			break;
		}
    default:
    {
			ErrorBeep();
			break;
    }		
	}
}
/***********************************************************/

void Beep(void)
{
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(10);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
}

void ErrorBeep(void)
{
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(100);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(100);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(100);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
}

void CanceleBeep(void)
{
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(10);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(80);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(10);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
}

void ConfBeep(void)
{
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);
	LL_mDelay(50);
	LL_GPIO_TogglePin(GPIOA, BEEP_Pin);	
}

