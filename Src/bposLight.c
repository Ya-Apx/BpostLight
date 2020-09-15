#include "main.h"
#include "bposLight.h"
#include "stdio.h"
//#include "stdlib.h"
#include "string.h"

//#define debug_parcer_handler
//#define debug_handler
//#define debug_resp
//#define debug_con
//#define debug_pur
//#define debug_ack

uart_req bposLightReq = {NOP, NOTRECIVED, {0}, 0, DENIED, 0, 0}; //флаг ответа, принят\не принят, буфер данных, счетчик байт, разрешение на выдачу, количество пульсов, сумма 
purRespBody recivedResp = {{0}, 0, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 0, 0, 0, {0}};
uint32_t INVOICE=0;
uint8_t pulseCost=5;

/***********************************************************/
/**************передача запроса по USART1*******************/
/***********************************************************/
void usart_TX (uint8_t* message, uint8_t size)
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
/*************************DEBUG*****************************/
/***********************************************************/

void usart_DEBUG (uint8_t* message, uint8_t size)
{
	uint8_t ind = 0;
  while (ind<size)
  {
		while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
    LL_USART_TransmitData8(USART3,*(uint8_t*)(message+ind));
    ind++;
  }
}
/***********************************************************/

/***********************************************************/
/**************передача запроса по USART2*******************/
/**************UINT32_T**TU**ASCII**DEBUG*******************/
/***********************************************************/
void usart_DEBUG_ASCII(uint32_t value)
{
			//uint8_t* buffer;
	
		char buffer[12]={0,0,0,0,0,0,0,0,0,0,0,0};
		
		sprintf(buffer, "%u", value );
	
	uint8_t ind = 0;
  while (ind<sizeof(buffer)&&(*(uint8_t*)(buffer+ind)))
  {
		while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
    LL_USART_TransmitData8(USART3,*(uint8_t*)(buffer+ind));
    ind++;
  }
	while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
  LL_USART_TransmitData8(USART3, 0x0a);
}
/***********************************************************/

/***********************************************************/
/***********подсчет контрольной суммы LRC*******************/
/***********************************************************/
uint8_t calculate_lrc(uint8_t *message, uint32_t messageSize)
{
	uint8_t lrc=0x00;
	for (uint32_t i=0; i<messageSize; i++)
	{
		lrc^=message[i];
	}
	return lrc;
}
/***********************************************************/

/***********************************************************/
/**********************запрос PUR***************************/
/*********принимаем сумму как масив ASCII символов**********/
/*******формируем запрос на продажу и отправляем его********/ //мрачняк на скорую руку
void PURreq_ASCII(uint8_t * number,uint8_t size)
{
	//записуем сумму 
	bposLightReq.sum=(number[1]-0x30)*10 + (number[0]-0x30);    // number[1] и number[0] перепутаны местами
	
	if(*(number+1)!=0x30)
	{
		uint8_t sizeOfmessage[]={0,44}; 
		uint8_t message[44]={	 
												0x52, 0x45, 0x51, 0x3d, 0x50, 0x55, 0x52, 0x3b,                                  //REQ=PUR;          0-7
												0x41, 0x4d, 0x4f, 0x55, 0x4e, 0x54, 0x3d, 0x30, 0x30, 0x30, 0x30, 0x3b,          //AMOUNT=0000;      8-19                (15.16.17.18 byts)
												0x4d, 0x49, 0x44, 0x3d, 0x53, 0x31, 0x32, 0x31, 0x30, 0x38, 0x35, 0x30, 0x3b,    //MID=S1210850;     17-29
												0x53, 0x45, 0x52, 0x56, 0x49, 0x44, 0x3d, 0x30, 0x3b								             //SERVID=0;       	 30-38
												};
		//пишем сумму (без копеек)
		message[15]=* (number+1);
		message[16]=*	number;
											
		//шлем стартовый байт
		LL_USART_TransmitData8(USART1, STX);
		//шлем длину посылки
		usart_TX(sizeOfmessage, 2);											
		//шлем запрос
		usart_TX(message,44);	
		//шлем контрольную сумму
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
		LL_USART_TransmitData8(USART1, calculate_lrc(message, 44));	 											
	}
	else
	{
		uint8_t sizeOfmessage[]={0,43}; 
		uint8_t message[43]={	 
												0x52, 0x45, 0x51, 0x3d, 0x50, 0x55, 0x52, 0x3b,                                  //REQ=PUR;          0-7
												0x41, 0x4d, 0x4f, 0x55, 0x4e, 0x54, 0x3d, 0x30, 0x30, 0x30, 0x3b,          			 //AMOUNT=000;       8-19                (15.16.17 byts)
												0x4d, 0x49, 0x44, 0x3d, 0x53, 0x31, 0x32, 0x31, 0x30, 0x38, 0x35, 0x30, 0x3b,    //MID=S1210850;     17-29
												0x53, 0x45, 0x52, 0x56, 0x49, 0x44, 0x3d, 0x30, 0x3b								             //SERVID=0;       	 30-38
												};
		//пишем сумму (без копеек)
		message[15]=* number;
											
		//шлем стартовый байт
		LL_USART_TransmitData8(USART1, STX);
		//шлем длину посылки
		usart_TX(sizeOfmessage, 2);											
		//шлем запрос
		usart_TX(message,43);	
		//шлем контрольную сумму
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
		LL_USART_TransmitData8(USART1, calculate_lrc(message, 43));	 			
	}
}
/***********************************************************/

/***********************************************************/
/**********************запрос PUR***************************/
/***********************************************************/
/*******формируем запрос на продажу и отправляем его********/
void PURreq (uint32_t amount) 																//сейчас не используется
{	
	uint8_t sizeOfmessage[2]={0x00,0x00}; 
	
	//шлем стартовый байт
	LL_USART_TransmitData8(USART1, STX);

	//в зависимости от длины суммы (1,2,3 или 4 байта)	формируем сообщение
	//нужно что то с этим мрачняком делать, ато как то не серъЁзно
	if (amount<10)//(0-9)
	{	
		uint8_t message1[39]={											 
													0x52, 0x45, 0x51, 0x3d, 0x50, 0x55, 0x52, 0x3b,                                  	//REQ=PUR;          0-7
													0x41, 0x4d, 0x4f, 0x55, 0x4e, 0x54, 0x3d, 0x30, 0x3b,          										//AMOUNT=0;      		8-16                (15 byt)
													0x4d, 0x49, 0x44, 0x3d, 0x53, 0x31, 0x32, 0x31, 0x30, 0x38, 0x35, 0x30, 0x3b,    	//MID=S1210850;     17-29
													0x53, 0x45, 0x52, 0x56, 0x49, 0x44, 0x3d, 0x30, 0x3b								              //SERVID=0;       	30-38
													};
		//записываем сумму 
		message1[15]=(uint8_t)amount;
											
		//шлем длину посылки
		sizeOfmessage[1]=41;
		usart_TX(sizeOfmessage, 2);
													
		//шлем запрос
		usart_TX(message1,41);
		
		//шлем контрольную сумму
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
		LL_USART_TransmitData8(USART1, calculate_lrc(message1, 41));	
	}
	
	if (amount>9&&amount<100)//(10-99)
	{
		uint8_t message2[42]={											 
												 0x52, 0x45, 0x51, 0x3d, 0x50, 0x55, 0x52, 0x3b,                                  //REQ=PUR;          0-7
												 0x41, 0x4d, 0x4f, 0x55, 0x4e, 0x54, 0x3d, 0x30, 0x30,  0x3b,          						//AMOUNT=00;      	8-17                (16.17 byts)
												 0x4d, 0x49, 0x44, 0x3d, 0x53, 0x31, 0x32, 0x31, 0x30, 0x38, 0x35, 0x30, 0x3b,    //MID=S1210850;     17-29
												 0x53, 0x45, 0x52, 0x56, 0x49, 0x44, 0x3d, 0x30, 0x3b								              //SERVID=0;       	30-38
												 };
		char arr2[2]={0};
		sprintf(&arr2[0],"%02i", amount );
		message2[15]=arr2[0];
		message2[16]=arr2[1];
		
		//шлем длину посылки
		sizeOfmessage[1]=42;
		usart_TX(sizeOfmessage, 2);
													
		//шлем запрос
		usart_TX(message2,42);
		
		//шлем контрольную сумму
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
		LL_USART_TransmitData8(USART1, calculate_lrc(message2, 42));			
		
	}
	
	if (amount>99&&amount<1000)//(100-999)
	{
		uint8_t message3[43]={
											 
											 0x52, 0x45, 0x51, 0x3d, 0x50, 0x55, 0x52, 0x3b,                                  //REQ=PUR;          0-7
										 	 0x41, 0x4d, 0x4f, 0x55, 0x4e, 0x54, 0x3d, 0x30, 0x30, 0x30,  0x3b,         			//AMOUNT=000;       8-17                (16.17.18 byts)
											 0x4d, 0x49, 0x44, 0x3d, 0x53, 0x31, 0x32, 0x31, 0x30, 0x38, 0x35, 0x30, 0x3b,    //MID=S1210850;     17-29
											 0x53, 0x45, 0x52, 0x56, 0x49, 0x44, 0x3d, 0x30, 0x3b								              //SERVID=0;       	30-38
											};
		char arr3[3]={0};
		sprintf(&arr3[0],"%03i", amount );
		message3[15]=arr3[0];
		message3[16]=arr3[1];
		message3[17]=arr3[2];
		
		//шлем длину посылки
		sizeOfmessage[1]=43;
		usart_TX(sizeOfmessage, 2);
													
		//шлем запрос
		usart_TX(message3,43);
		
		//шлем контрольную сумму
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
		LL_USART_TransmitData8(USART1, calculate_lrc(message3, 43));	
	}
	
	if (amount>999) // (1000-9999)
	{
		uint8_t message4[44]={
											 
											 0x52, 0x45, 0x51, 0x3d, 0x50, 0x55, 0x52, 0x3b,                                  //REQ=PUR;          0-7
										 	 0x41, 0x4d, 0x4f, 0x55, 0x4e, 0x54, 0x3d, 0x30, 0x30, 0x30, 0x30, 0x3b,          //AMOUNT=0000;      8-19                (16.17.18.19 byts)
											 0x4d, 0x49, 0x44, 0x3d, 0x53, 0x31, 0x32, 0x31, 0x30, 0x38, 0x35, 0x30, 0x3b,    //MID=S1210850;     17-29
											 0x53, 0x45, 0x52, 0x56, 0x49, 0x44, 0x3d, 0x30, 0x3b								              //SERVID=0;       	30-38
											};
		char arr4[4]={0};
		sprintf(&arr4[0],"%04i", amount ); //почему то не хочет с string.h, а с stdio.h норм
		message4[15]=arr4[0];
		message4[16]=arr4[1];
		message4[17]=arr4[2];
		message4[18]=arr4[3];								
	
		//шлем длину посылки
		sizeOfmessage[1]=44;
		usart_TX(sizeOfmessage, 2);
													
		//шлем запрос
		usart_TX(message4,44);
		
		//шлем контрольную сумму
		while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
		LL_USART_TransmitData8(USART1, calculate_lrc(message4, 44));	
	}
}
/***********************************************************/

/***********************************************************/
/********************отправляем ACK*************************/
/***********************************************************/
void ACKsend(void)
{
	#ifdef debug_ack
	/*************************DEBUG****************************/
	/*********выводим в UART3 сообщение "ACK" *****************/
	uint8_t message[]={0x41, 0x43, 0x4b, 0x0a};
	usart_DEBUG(message, sizeof(message));
	/**********************************************************/
	#endif
	
	LL_USART_TransmitData8(USART1, ACK);
}
/***********************************************************/

/***********************************************************/
/**********************запрос EOT***************************/
/***********************************************************/
/**Формируем запрос отмены ожидания карты и отправляем его**/
void EOTreq(void)
{
	LL_USART_TransmitData8(USART1, EOT);
}
/***********************************************************/

/***********************************************************/
/********************отправляем NAK*************************/
/***********************************************************/
/****запрос не совпала контрольная сумма и отправляем его***/
void NAKsend(void)
{
	LL_USART_TransmitData8(USART1, NAK);
}
/***********************************************************/

/***********************************************************/
/***********************запрос CAN**************************/
/***********************************************************/
/****Формируем запрос отмены транзакции и отправляем его****/ //пока что не реализовано
void CANreq(uint32_t invoice)
{
	
}
/***********************************************************/

/***********************************************************/
/***********************запрос CON**************************/
/***********************************************************/
/***********Формируем подтверждение снятия денег************/
/*******после получения ACK разрешаем выдачу товара*********/ //ХЗ ПОКА КАК
void CONreq(uint32_t invoice)
{			
	
	//char *invNum; 					  //указатель на масив с номером инвойса
	char invNumMas[]={0};
	uint8_t lenOfInvNum=0;		//количество байт занятых номером инвойса	
	
	uint8_t sizeOfMessage[2]={0x00,0x00};																			//sizeof
	uint8_t messageHat[16]={0x52, 0x45, 0x51, 0x3d, 0x43, 0x4f, 0x4e, 0x3b, 	//REQ=CON;
													0x49, 0x4e, 0x56, 0x4f, 0x49, 0x43, 0x45, 0x3d}; 	//INVOICE= 														
																																
	//определяем количество байт занимаемых номером инвойса										
	lenOfInvNum=sprintf(invNumMas, "%d", invoice);
													
	#ifdef debug_con												
	/*************************************DEBUG***************************************/
	/************выводим в UART3 сообщение "CON" (отвечаем на RESP=CON)***************/
	uint8_t message[]={0x43, 0x4f, 0x4e, 0x0a};
	usart_DEBUG(message, sizeof(message));
	/*********************************************************************************/						
	//проверяем номер инвойса
	usart_DEBUG((uint8_t*)invNumMas, lenOfInvNum);
	/*********************************************************************************/												
	#endif	
													
	uint8_t conReq[lenOfInvNum+16+1];					//массив под пакет для отправки
													
	//шлем стартовый байт
	LL_USART_TransmitData8(USART1, STX);												
													
	//определяем длину пакета и отправляем ее
	//"REQ=CON;INVOICE=" - 16 байт
	//lenOfInvNum - количество байт занятых инвойсом
	//";" - 1 байт, между номером инвойса и LRC												
	sizeOfMessage[1]=lenOfInvNum+16+1;
	usart_TX(sizeOfMessage, 2);
	
	//собираем
	memcpy(conReq, messageHat, 16);
	memcpy(&conReq[16], invNumMas, lenOfInvNum);
	conReq[lenOfInvNum+16]=0x3b;		//";" между номером инвойса и LRC	
													
	//отправляем
	usart_TX(conReq, lenOfInvNum+16+1);
	
	//ждем пока отправятся данные
	//считаем и шлем контрольную сумму
	while (!LL_USART_IsActiveFlag_TXE(USART1)) {}
	LL_USART_TransmitData8(USART1, calculate_lrc(conReq, lenOfInvNum+16+1));
		
	//считаем количество пульсов
	bposLightReq.pulses=((bposLightReq.sum*100)/pulseCost)*2;
		
	#ifdef debug_pulsWend	
	/*********************************************************/
	/*****выводим в UART3 сумму и количество пульсов**********/
	/*********************************************************/
	usart_DEBUG_ASCII (bposLightReq.sum);
	usart_DEBUG_ASCII (bposLightReq.pulses);
	/*********************************************************/
	#endif
		
		
		
}
/***********************************************************/

/***********************************************************/
/**************responce function BPOSLIGHT******************/
/***********************************************************/
//в зависимости от пришедшего ответа отвечаем нужной командой
//или не отвечаем
void BPOS_cmd_resp(void)
{
		switch (bposLightReq.cmd)
		{

			case 0x55:	//U resp=pUr
			//Пришоло ответ на PUR запрс, АСК неотвечаем,
			//так как уже ответили после подсчета контрольной суммы,
			//нужно подтвердить с номером инвойса
			//req=pur-> 
			//<-RESP=PUR
			{
				#ifdef debug_resp
				/*************************************DEBUG***************************************/
				/************выводим в UART2 сообщение "PUR" (отвечаем на RESP=PUR)***************/
				uint8_t message[]={0x50, 0x55, 0x52, 0x0a};
				usart_DEBUG(message, sizeof(message));
				
				//проверяем номер инвойса
				//usart_DEBUG_ASCII(recivedResp.INVOICE);
				/*********************************************************************************/
				#endif
				
				CONreq(recivedResp.INVOICE);				
				
				bposLightReq.cmd=NOP;//сбрасываем флаг ответа
				break;
			}
			
			case 0x41:	//A resp=cAn
			//Пришоло подтверждение отмены транзакции, АСК неотвечаем,
			//так как уже ответили после подсчета контрольной суммы
			//req=pur-> 
			//<-RESP=PUR	
			//req=can->
			//<-RESP=CAN
			{
				#ifdef debug_resp
				/*************************************DEBUG***************************************/
				/************выводим в UART2 сообщение "САN" (отвечаем на RESP=CAN)***************/
				uint8_t message[]={0x43, 0x41, 0x4e, 0x0a};
				usart_DEBUG(message, sizeof(message));
				/*********************************************************************************/
				#endif
				bposLightReq.cmd=NOP;//сбрасываем флаг ответа
				break;
			}
	
			case 0x4F:	//O resp=cOn
			//Пришоло подтверждение оплаты, АСК неотвечаем,
			//так как уже ответили после подсчета контрольной суммы.
			//Можно отпускать товар
			//req=pur-> 
			//<-resp=pur
			//req=con->
			//<-RESP=CON
			{
				#ifdef debug_resp
				/*************************************DEBUG***************************************/
				/************выводим в UART2 сообщение "СON" (отвечаем на RESP=CON)***************/
				uint8_t message[]={0x43, 0x4f, 0x4e, 0x0a};
				usart_DEBUG(message, sizeof(message));
				/*********************************************************************************/
				#endif
				
				bposLightReq.cmd=NOP;//сбрасываем флаг ответа
				/************************************************/
				/*здесь должен быть какой то код продающий товар*/
				/*******************если RC==0*******************/
				/************************************************/
				
				break;	
			}
			
			case 0x45:	//E resp=sEt
			//Пришол отват на SET запрос (отправлять REQ=SET пока не планируется)
			//АСК неотвечаем, так как уже ответили после подсчета контрольной суммы.
			//req=set-> 
			//<-RESP->SET
			{
				#ifdef debug_resp
				/*************************************DEBUG***************************************/
				/************выводим в UART2 сообщение "SET" (отвечаем на RESP=SET)***************/
				uint8_t message[]={0x53, 0x45, 0x54, 0x0a};
				usart_DEBUG(message, sizeof(message));
				/*********************************************************************************/
				#endif
				
				bposLightReq.cmd=NOP;//сбрасываем флаг ответа
				break;
			}
			
			case 0x00:	//NOP
			//ничего отвечать не нужно
			{
				#ifdef debug_resp
				/*************************************DEBUG***************************************/
				/************выводим в UART2 сообщение "NOP" (отвечаем на RESP=SET)***************/
				//uint8_t message[]={0x4e, 0x4f, 0x50, 0x0a};
				//usart_DEBUG(message, sizeof(message));
				/*********************************************************************************/
				#endif
				
				bposLightReq.cmd=NOP;//сбрасываем флаг ответа
				break;
			}	
			default:	//что то другое Оо		
			{
				#ifdef debug_resp
				/*************************************DEBUG***************************************/
				/************выводим в UART2 сообщение "WTF" (отвечаем на RESP=SET)***************/
				uint8_t message[]={0x57, 0x54, 0x46, 0x0a};
				usart_DEBUG(message, sizeof(message));
				/*********************************************************************************/
				#endif
				
				bposLightReq.cmd=NOP;//сбрасываем флаг ответа
				break;
			}
			
		}
}
/***********************************************************/

/***********************************************************/
/****************main function BPOSLIGHT********************/
/***********************************************************/
void BPOS_cmd_handler(void)
{
		if(bposLightReq.state)
		{			
				#ifdef debug_handler
				/*************************************DEBUG***************************************/
				/**********выводим в UART3 сообщение "RCVD" (флаг по окончанию приема)************/
				//uint8_t message[]={0x52, 0x43, 0x56, 0x44};
				//usart_DEBUG(message, sizeof(message));
				usart_DEBUG_ASCII(bposLightReq.position);
				/*********************************************************************************/
				#endif	
			
				switch (bposLightReq.buffer[0])
				{
					case STX:
					//пришли данные
					{
						#ifdef debug_handler
						/*************************************DEBUG***************************************/
						/**********выводим в UART3 сообщение "STX" (стартовый STX байт принят)************/
						uint8_t message[]={0x53, 0x54, 0x58, 0x0a};
						usart_DEBUG(message, sizeof(message));
						/*********************************************************************************/
						#endif
						
						//проверка контрольной суммы
						//(bposLightReq.position) - количество принятых байт
						//(bposLightReq.position-1) - последний байт он же LRC 
						//bposLightReq.buffer[0] - STX
						//bposLightReq.buffer[1] - старший байт длины
						//bposLightReq.buffer[2] - младший байт длины
						//bposLightReq.buffer[3] - первый байт данных
						//(bposLightReq.position-4) - количество байт данных (без STX, длины и LRC)
						if(bposLightReq.buffer[bposLightReq.position-1]==calculate_lrc(&bposLightReq.buffer[3], bposLightReq.position-4))
						{	
							#ifdef debug_handler
							/*************************************DEBUG***************************************/
							/**********выводим в UART3 сообщение "LRCisOK" (контрольная сумма ОК)*************/
							uint8_t message[]={0x4c, 0x52, 0x43, 0x69, 0x73, 0x4f, 0x4b, 0x0a};
							usart_DEBUG(message, sizeof(message));
							/*********************************************************************************/		
							#endif
							
							//пишем данные из полей в структуру
							Parcer();
							
							#ifdef debug_parcer_handler
							/*************************************DEBUG***************************************/
							//RESP[3];
							usart_DEBUG(recivedResp.RESP, sizeof(recivedResp.RESP));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
								
							//RC;
							usart_DEBUG_ASCII(recivedResp.RC);
							
							//PAN[19];
							usart_DEBUG(recivedResp.PAN, sizeof(recivedResp.PAN));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
							
							//DATETIME[14];
							usart_DEBUG(recivedResp.DATETIME, sizeof(recivedResp.DATETIME));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
							
							//EAID[16];
							usart_DEBUG(recivedResp.EAID, sizeof(recivedResp.EAID));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
							
							//HOLDER[26];
							usart_DEBUG(recivedResp.HOLDER, sizeof(recivedResp.HOLDER));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
							
							//AUTH[8];
							usart_DEBUG(recivedResp.AUTH, sizeof(recivedResp.AUTH));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
								
							//MID[15];
							usart_DEBUG(recivedResp.MID, sizeof(recivedResp.MID));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
							
							//TID[8];
							usart_DEBUG(recivedResp.TID, sizeof(recivedResp.TID));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
								
							//SIGN;
							usart_DEBUG_ASCII(recivedResp.SIGN);
								
							//INVOICE;
							usart_DEBUG_ASCII(recivedResp.INVOICE);
								
							//BATCH;
							usart_DEBUG_ASCII(recivedResp.BATCH);
								
							//CARDTYPE[8];
							usart_DEBUG(recivedResp.CARDTYPE, sizeof(recivedResp.CARDTYPE));
							while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
							LL_USART_TransmitData8(USART3, 0x0a);
							/*********************************************************************************/
							#endif
							
							//подтверждаем получение корректных данных
							ACKsend();
					
							//сбрасываем флаг и счетчик полученых байт
							bposLightReq.position=0;									//счетчик полученых байт
							bposLightReq.state=NOTRECIVED;						//флаг получения ответа							
							bposLightReq.purchConf=DENIED;						//флаг разрешения выдачи товара
						}
						else
						{
							#ifdef debug_handler
							/*************************************DEBUG***************************************/
							/**********выводим в UART3 сообщение "LRC:" (контрольная сумма не ОК)*************/
							uint8_t message[]={0x4c, 0x52, 0x43, 0x3a, 0x0a};
							usart_DEBUG(message, sizeof(message));
							
							//шлем что пришло
							//while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
							//LL_USART_TransmitData8(USART2,bposLightReq.buffer[bposLightReq.position-1]);
							//":"
							//while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
							//LL_USART_TransmitData8(USART2, 0x3a);
								
							//шлем то что насчитали
							//while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
							//LL_USART_TransmitData8(USART2, calculate_lrc(&bposLightReq.buffer[3], bposLightReq.position-4));
							//":"
							//while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
							//LL_USART_TransmitData8(USART2, 0x3a);
								
							//шлем bposLightReq.position
							//usart_DEBUG_ASCII(bposLightReq.position);//214							
							/*********************************************************************************/
							#endif
							
							//шленм NAK, протоколу нужно слать трижды (три попытки получение данных) потом EOT
							NAKsend();
							
							// сбрасываем флаг и счетчик полученых байт
							bposLightReq.position=0;									//счетчик полученых байт
							bposLightReq.state=NOTRECIVED;						//флаг получения ответа							
							bposLightReq.purchConf=DENIED;						//флаг разрешения выдачи товара
						}
						break;
					}

					case NAK:
					//не совпала контрольная сумма, нужно повторить отправку запроса (не реализовано пока)
					//после трех раз шлем EOT								 												 (сейчас после первого же)
					//сбрасываем флаг что данные не получены 												 (не реализовано пока)
					//сбрасываем счетчик повторных отправок  												 (не реализовано пока)
					{ 
						#ifdef debug_handler
						/*************************************DEBUG***************************************/
						/*********выводим в UART3 сообщение "NAK" (не совпала контрольная сумма)**********/
						uint8_t message[]={0x4e, 0x41, 0x4b, 0x0a};
						usart_DEBUG(message, sizeof(message));
						/*********************************************************************************/
						#endif
						
						EOTreq();// не совпало, да и хуй с ним, попробуем потом еще раз
						
// сбрасываем флаг и счетчик полученых байт
						bposLightReq.position=0;									//счетчик полученых байт
						bposLightReq.state=NOTRECIVED;						//флаг получения ответа							
						bposLightReq.purchConf=DENIED;						//флаг разрешения выдачи товара
						break;
					}

					case ACK:
					//данные принял 
					//сбрасываем флаг что данные не получены (не реализовано пока)
					//сбрасываем счетчик повторных отправок  (не реализовано пока)
					{
						#ifdef debug_handler
						/*************************************DEBUG***************************************/
						/***********выводим в UART3 сообщение "ACK" (подтверждение получения)*************/
						uint8_t message[]={0x41, 0x43, 0x4b, 0x0a};
						usart_DEBUG(message, sizeof(message));
						/*********************************************************************************/
						#endif
						
						// сбрасываем флаг и счетчик полученых байт
						bposLightReq.position=0;									//счетчик полученых байт
						bposLightReq.state=NOTRECIVED;						//флаг получения ответа			
						
						//прлверяем флаг разрешения выдачи товара
						if(bposLightReq.purchConf)
						{
							//код выдающий товар 
						}
						break;
					}
					
					case EOT:
					//конец транзакции
					{
						#ifdef debug_handler
						/*************************************DEBUG***************************************/
						/***********выводим в UART3 сообщение "EOT" (подтверждение получения)*************/
						uint8_t message[]={0x45, 0x4f, 0x54, 0x0a};
						usart_DEBUG(message, sizeof(message));
						/*********************************************************************************/
						#endif
						
						// сбрасываем флаг и счетчик полученых байт
						bposLightReq.position=0;									//счетчик полученых байт
						bposLightReq.state=NOTRECIVED;						//флаг получения ответа							
						bposLightReq.purchConf=DENIED;						//флаг разрешения выдачи товара
						break;
					}
					default:
					{
						// сбрасываем флаг и счетчик полученых байт
						bposLightReq.position=0;									//счетчик полученых байт
						bposLightReq.state=NOTRECIVED;						//флаг получения ответа							
						bposLightReq.purchConf=DENIED;						//флаг разрешения выдачи товара
						break;					
					}
				}
		}
		
		//на всякий случай сбрасываем флаги
		bposLightReq.position=0;
		bposLightReq.state=NOTRECIVED;
}
/***********************************************************/

/***********************************************************/
/*****************parcer of responses***********************/
/***********************************************************/
//парсим запрос, приводим типы и пишем в структуру purRespBody
void Parcer(void)
{	
	//(bposLightReq.position-4) - количество байт данных (без STX, длины и LRC)
	char tempStr[bposLightReq.position-4];
	char *token, *last, *token2, *last2;
	
	//копируем на всякий лучай масссив принятых давнных
	//bposLightReq.buffer[3] - первый байт данных
	memcpy(tempStr, &bposLightReq.buffer[3], bposLightReq.position-4);	
	
	//розбиваем массив на токены по ";", token содержить адрес 
	//первого "кортежа", last следующего
  token = strtok_r(tempStr, ";", &last);
	
	//розбиваем на токены полученый "кортежа" по "=", token2 
	//содержить адрес названия поля, last его значение
  token2= strtok_r(token, "=", &last2);
    		
	//повторяем то же для оставшейся части массива
  while (token != NULL) 
	{
		//в начале каждой следующей итерации распихиваем даннные
		//полученые в предидущей по соответствующим полям структуры
		
		#ifdef debug_parcer
		/*************************************DEBUG***************************************/
		/*********выводим в UART2 сообщение "RCVD" (стартовый STX байт принят)************/
		usart_DEBUG((uint8_t*)token2, sizeof(token2));
		/*********************************************************************************/
		#endif	
		
    switch (token2[0])
		{
			case 0x52:
      {
				switch (token2[1])
				{
			/******RSP*****************************************************/
			//Ответ на операцию, может быть PUR, CAN, CON, SET
					case 0x45:
					{		
						strcpy((char *)recivedResp.RESP, last2);						
						//записываем что нужно отвечать
						switch (recivedResp.RESP[1])
						{
							case 0x55:	//U resp=pUr
							{
								bposLightReq.cmd=PUR;
								break;
							}
							case 0x41:	//A resp=cAn
							{
								bposLightReq.cmd=CAN;
								break;
							}
							case 0x4F:	//O resp=cOn
							{
								bposLightReq.cmd=CON;
								break;
							}
							case 0x45:	//E resp=sEt
							{
								bposLightReq.cmd=SET_Z;
								break;
							}
							default:								
							{
								break;
							}
						}
						break;
					}
					
			/*****RC*******************************************************/
			//Код ответа 0-все прошло удачно
					case 0x43:
					{
						recivedResp.RC=atoi(last2); 
						break;
					}
				}
				break;
			}
			/*****PAN******************************************************/
			//PAN карты в виде 1234*5678
			case 0x50:
      {
				strcpy((char *)recivedResp.PAN, last2);
				break;
      }
			/*****DATETIME*************************************************/
			//Дата и время транзакции
      case 0x44:
      {
				strcpy((char *)recivedResp.DATETIME, last2);
				break;
      }
			/*****EAID*****************************************************/
      //MVA AID
			case 0x45:
      {
				strcpy((char *)recivedResp.EAID, last2);
				break;
      }
			/*****HOLDER***************************************************/
      //Holder name
			case 0x48:
      {
				strcpy((char *)recivedResp.HOLDER, last2);
				break;
      }
			/*****AUTH*****************************************************/
      //Код авторизации
			case 0x41:
      {
				strcpy((char *)recivedResp.AUTH, last2);
				break;
      }
			/*****MID******************************************************/
      //Merchant ID
			case 0x4d:
      {
				strcpy((char *)recivedResp.MID, last2);
				break;
      }
			/*****TID******************************************************/
      //Terminal ID
			case 0x54:
      {
				strcpy((char *)recivedResp.TID, last2);
				break;
      }
			/*****SIGN******************************************************/
			//Верификация по подписи: 1 - была, 0 нет
      case 0x53:
      {
				recivedResp.SIGN=atoi(last2);
				break;
      }
			/*****INVOIC****************************************************/
			//Invoice number
      case 0x49:
      {
				recivedResp.INVOICE=atoi(last2);
				break;
      }
			/*****BATCH*****************************************************/
			//Batch number
      case 0x42:
      {
				recivedResp.BATCH=atoi(last2);
				break;
      }
			/*****CARDTYPE**************************************************/
			//Тип карты, может быть: CLESS, CHIP MSTRIPE, MANUAL
      case 0x43:
      {
				strcpy((char *)recivedResp.CARDTYPE, last2);
				break;
      }
			
		}
				//розбиваем оставшуюся часть массива на токены по ";", 
				//token содержить адрес первого "кортежа", last следующего
        token = strtok_r(NULL, ";", &last);
        token2= strtok_r(token, "=", &last2);
  }	
}
/***********************************************************/

/***********************************************************/
/***********************sale the item***********************/
/***********************************************************/
//проверяем если bposLightReq.purchConf==APPROVED считаем и 
//отправляем импульсы
void SalePulse(void)
{
	if(bposLightReq.purchConf==APPROVED)
	{
			usart_DEBUG_ASCII (bposLightReq.sum);
	}
}
/***********************************************************/

/***********************************************************/
/**************************PULSE****************************/
/***********************************************************/
void Pulse(void)
{

}
/***********************************************************/
