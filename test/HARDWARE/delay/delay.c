/**
   ******************************************************************************
   * @file    delay.c 
   * @author  
   * @version v1
   * @date    2024/10/30
   * @brief   实现微秒和毫秒的延时
   ******************************************************************************
**/


#include "delay.h"
#include "sys.h"

uint32_t fac_us = 0;

#if SYSTEM_SUPPORT_OS 

#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler(void);

/**
   ******************************************************************************
   * @brief    嘀嗒定时器中断服务函数，使用操作系统时用到 
   *   
   * @note 	   FreeRTOS的心跳就是由嘀嗒定时器产生的，根据FreeRTOS的系统时钟节拍设
   *           置好嘀嗒定时的周期，这样就会周期触发嘀嗒定时器中断了，在嘀嗒定时器
   *           中断服务函数中调用FreeRTOS的API函数xPortSysTickHandler()
   *      
   * @param    None
   *
   * @retval   None
   ******************************************************************************
**/
void SysTickHandler(void)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)   //OS开始跑了，才开始调度处理
	{
		xPortSysTickHandler();
	}
}

#endif


/**
   ******************************************************************************
   * @brief    初始化延时函数 
   *   
   * @note 	   FreeRTOS的心跳就是由嘀嗒定时器产生的，根据FreeRTOS的系统时钟节拍设
   *           置好嘀嗒定时的周期，这样就会周期触发嘀嗒定时器中断了，在嘀嗒定时器
   *           中断服务函数中调用FreeRTOS的API函数xPortSysTickHandler()
   *      
   * @param    sysclk,系统时钟频率
   *
   * @retval   None
   ******************************************************************************
**/
void delay_init(uint16_t sysclk)
{
#if SYSTEM_SUPPORT_OS 
	uint32_t reload;
#endif
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	fac_us = sysclk;

#if SYSTEM_SUPPORT_OS 
	reload = sysclk;
	reload *= 1000000 / configTICK_RATE_HZ;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->LOAD = reload;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#endif	
}


#if SYSTEM_SUPPORT_OS

/**
   ******************************************************************************
   * @brief    微秒级延时函数 
   *   
   * @note 	   Systick定时器属于Cortex-M4内核中的一款定时器，具有2个时钟源，内部时钟
   *           (168MHZ)和参考时钟(168MHZ/8=21MHZ), 但是FreeRTOS使用Systick定时器进行
   *		   时基生成是使用内核时钟(168MHZ)
   *		
   *		   由于时钟源是168MHZ，所以Systick计数周期是1/168us，换句话说，1us可以计
   *		   数168次
   *		
   *		   之前裸机开发时是直接对Systick定时器的寄存器进行控制，Systick定时器的
   *		   寄存器的数量有4个，其中3个必须使用(CTRL、LOAD、VAL)
   *		
   *		   注意：MCU搭载了FreeRTOS之后，Systick定时器需要提供给FreeRTOS内核来生成
   *		   时基，所以用户就不应该再对Systick定时器的寄存器进行修改，但是用户可以
   *		   读取Systick寄存器的值，完成接口的优化
   *		
   *		   定时的原理：通过计数的方式达到定时的目的，由于Syctick是倒计时的定时器，
   *		   所以Systick会从一个初值不断递减，当值为0时则表示时间到达，用户可以利用
   *		   计数器的差值来换算时间
   *		
   *		   注意：这两个延时函数的是非阻塞型的延时函数，所以在调用时是不会导致任务
   *		   阻塞的。
   *
   *           nus的值不能大于93206us
   *      
   * @param    需要延时的时间，单位是微秒
   *
   * @retval   None
   ******************************************************************************
**/
void delay_us(uint32_t nus)
{
	uint32_t ticks = 0;
	uint32_t told = 0;
	uint32_t tnew = 0;
	uint32_t load = SysTick->LOAD;
	
	told = SysTick->VAL;
	
	while(1)
	{
		tnew = SysTick->VAL;
		
		if(told != tnew)
		{
			if(told > tnew)
			{
				ticks += told + told;
			}
			else
			{
				ticks += load - tnew + told; 
			}
			told = tnew;
			
			if(ticks > nus)
			{
				break;
			}
		}
	}
} 


/**
   ******************************************************************************
   * @brief    毫秒级延时函数 
   *   
   * @note 	   使用SysTick定时器的寄存器达到延时的目的，利用循环，实现了可以不受
   *           最大时间限制
   *           计数时间 = 计数的次数 * 计数周期(21MHz，1us可以生成21个脉冲)
   *      
   * @param    需要延时的时间，单位是ms
   *
   * @retval   None
   ******************************************************************************
**/
void delay_ms(uint32_t nms)
{
	while(nms--)
	{
		delay_us(1000);
	}
} 






#else

/**
   ******************************************************************************
   * @brief    微秒级延时函数 
   *   
   * @note 	   使用SysTick定时器的寄存器达到延时的目的,Systick是24bit的递减计数器,
   *           约等于798915us,所以参数不可以超过这个数
   *           计数时间 = 计数的次数 * 计数周期(21MHz，1us可以生成21个脉冲)
   *      
   * @param    需要延时的时间，单位是微秒
   *
   * @retval   None
   ******************************************************************************
**/
void delay_us(uint32_t us)
{
	SysTick->CTRL = 0;                          //向SysTick控制状态寄存器写0，关闭定时器,并且选用了外部时钟(21MHz)作为时钟源
	SysTick->LOAD = (21 * us) - 1;              //当递减至0的时候，只要定时器没关，就会重新从这个值继续递减计数
	SysTick->VAL  = 0;                          //清除数值寄存器的值
	SysTick->CTRL = 1;                          //向SysTick控制状态寄存器写1，打开定时器
	
	while((SysTick->CTRL & 0x00010000) == 0);   //控制寄存器的第16位为1时，说明已经计数到0了
	
	SysTick->CTRL = 0;                          //向SysTick控制状态寄存器写0，关闭定时器
} 


/**
   ******************************************************************************
   * @brief    毫秒级延时函数 
   *   
   * @note 	   使用SysTick定时器的寄存器达到延时的目的，利用循环，实现了可以不受
   *           最大时间限制
   *           计数时间 = 计数的次数 * 计数周期(21MHz，1us可以生成21个脉冲)
   *      
   * @param    需要延时的时间，单位是ms
   *
   * @retval   None
   ******************************************************************************
**/
void delay_ms(uint32_t ms)
{
	while(ms--)
	{
		SysTick->CTRL = 0;                          //向SysTick控制状态寄存器写0，关闭定时器
		SysTick->LOAD = (21 * 1000) - 1;            //当递减至0的时候，重新从这个值继续递减
		SysTick->VAL  = 0;                          //清除数值寄存器的值
		SysTick->CTRL = 1;                          //向SysTick控制状态寄存器写1，打开定时器
		
		while((SysTick->CTRL & 0x00010000) == 0);   //控制寄存器的第16位为1时，说明已经计数到0了
		
		SysTick->CTRL = 0;                          //向SysTick控制状态寄存器写0，关闭定时器
	}
} 

#endif
