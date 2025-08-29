#include <reg52.h>
#include <board.h>
/**************************************************/
//Revision：1.3.4.0.0824	
/**************************************************/

/***************参数定义***************/
#define STATEconfirm		0x6E			//状态机确认 回读STATEconfirm的寄存值确认IC正常工作状态
#define NORMAL_I2S			0x00
#define NORMAL_LJ			0x01
#define NORMAL_DSPA			0x03
#define NORMAL_DSPB			0x23
#define Format_Len24		0x00
#define Format_Len20		0x01
#define Format_Len18		0x02
#define Format_Len16		0x03
#define Format_Len32		0x04

/***************参数定义***************/

/***************参数选择***************/

#define MSMode_MasterSelOn	0				//产品主从模式选择:默认选择0为SlaveMode,打开为1选择MasterMode
#define Ratio 				256				//实际Ratio=MCLK/LRCK比率，需要和实际时钟比例匹配
#define Format 				NORMAL_I2S		//数据格式选择,需要和实际时序匹配
#define Format_Len			Format_Len16	//数据长度选择,需要和实际时序匹配
#define SCLK_DIV			4				//SCLK分频选择:(选择范围1~18),SCLK=MCLK/SCLK_DIV，超过后非等比增加具体对应关系见相应DS说明
#define SCLK_INV			0				//默认对齐方式为下降沿,1为上升沿对齐,需要和实际时序匹配

#define ADCChannelSel		1				//单声道ADC输入通道选择是CH1(MIC1P/1N)还是CH2(MIC2P/2N)，
#define DACChannelSel		0				//单声道DAC输出通道选择:默认选择0:L声道,1:R声道	
#define ADC_PGA_DF2SE_15DB	0				//ADC模拟固定15dB增益:默认选择关闭0,打开为1
#define ADC_PGA_GAIN		0				//ADC模拟增益:(选择范围0~7),具体对应关系见相应DS说明
#define ADC_Volume			0				//ADC数字增益:(选择范围0~192),0:0DB,-0.5dB/Step
#define DAC_Volume			0				//DAC数字增益:(选择范围0~192),0:0DB,-0.5dB/Step
#define Dmic_Selon 			0				//DMIC选择:默认选择关闭0,立体声打开为1,单声道需要选择2为H,3为L
#define Dmic_GAIN 			0				//DMIC增益:(选择范围0~1),6dB/Step
#define	MICBIASoff			1				//内部MICBIAS偏置:默认选择关闭1,开启配置为0
#define PLLSet12288			0				//PLL选择输入频率,单位为KHZ:外部输入XXXXKHZ内部PLL锁成12M288
/***************参数选择***************/

void ES8374_Codec(void)//上电后执行一次启动，之后电不掉的情况下 走Standby/Powerdown关闭+resume开启
{
	I2CWRNBYTE_CODEC(0x00,0x3F);	//IC Rst start          
	I2CWRNBYTE_CODEC(0x00,0x03);	//IC Rst stop

	if(Ratio == 1536)//Ratio=MCLK/LRCK=1536：12M288-8K；
	{
		I2CWRNBYTE_CODEC(0x01,0x7F);	//IC clk on
		I2CWRNBYTE_CODEC(0x02,0x00);	
		I2CWRNBYTE_CODEC(0x03,0x20);	
		I2CWRNBYTE_CODEC(0x04,0x00);			
		I2CWRNBYTE_CODEC(0x05,0x66);	//clk div set		
		I2CWRNBYTE_CODEC(0x06,0x06);			
		I2CWRNBYTE_CODEC(0x07,0x00);	//LRCK=MCLK/6/256	
	}
	if(Ratio == 768)//Ratio=MCLK/LRCK=768：12M288-16K；24M576-32K；6M144-8K;
	{
		I2CWRNBYTE_CODEC(0x01,0x7F);	//IC clk on
		I2CWRNBYTE_CODEC(0x02,0x00);	
		I2CWRNBYTE_CODEC(0x03,0x20);	
		I2CWRNBYTE_CODEC(0x04,0x00);			
		I2CWRNBYTE_CODEC(0x05,0x33);	//clk div set		
		I2CWRNBYTE_CODEC(0x06,0x03);			
		I2CWRNBYTE_CODEC(0x00,0x00);	//LRCK=MCLK/768		
	}
	if(Ratio == 512)//Ratio=MCLK/LRCK=512：12M288-24K；24M576-48K；8M192-16K
	{
		I2CWRNBYTE_CODEC(0x01,0x7F);	//IC clk on
		I2CWRNBYTE_CODEC(0x02,0x00);	
		I2CWRNBYTE_CODEC(0x03,0x20);	
		I2CWRNBYTE_CODEC(0x04,0x00);			
		I2CWRNBYTE_CODEC(0x05,0x22);	//clk div set		
		I2CWRNBYTE_CODEC(0x06,0x02);			
		I2CWRNBYTE_CODEC(0x00,0x00);	//LRCK=MCLK/2/256			
	}
	if(Ratio == 256)//Ratio=MCLK/LRCK=256：12M288-48K；4M096-16K; 2M048-8K
	{
		I2CWRNBYTE_CODEC(0x01,0x7F);	//IC clk on
		I2CWRNBYTE_CODEC(0x02,0x00);	
		I2CWRNBYTE_CODEC(0x03,0x20);	
		I2CWRNBYTE_CODEC(0x04,0x00);			
		I2CWRNBYTE_CODEC(0x05,0x11);	//clk div set		
		I2CWRNBYTE_CODEC(0x06,0x01);			
		I2CWRNBYTE_CODEC(0x00,0x00);	//LRCK=MCLK/256		
	}
	if(Ratio == 128)//Ratio=MCLK/LRCK=128：6M144-48K；2M048-16K;//MCLK必须大于2M
	{//PLL 2倍频座256Ratio
		I2CWRNBYTE_CODEC(0x6F,0xA0);	//pll set:mode enable
		I2CWRNBYTE_CODEC(0x72,0x41);	//pll set:mode set
		I2CWRNBYTE_CODEC(0x09,0x02);	//pll set:reset on ,set start
		I2CWRNBYTE_CODEC(0x0C,0x00);	//pll set:k
		I2CWRNBYTE_CODEC(0x0D,0x00);	//pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x00);	//pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x88);	//pll set:
		I2CWRNBYTE_CODEC(0x0B,0x08);	//pll set:n
		I2CWRNBYTE_CODEC(0x09,0x42);	//pll set:reset off ,set stop
		I2CWRNBYTE_CODEC(0x02,0x08);	//pll set:use pll

		I2CWRNBYTE_CODEC(0x01,0x7F);	//IC clk on
		I2CWRNBYTE_CODEC(0x02,0x00);	
		I2CWRNBYTE_CODEC(0x03,0x20);	
		I2CWRNBYTE_CODEC(0x04,0x00);			
		I2CWRNBYTE_CODEC(0x05,0x11);	//clk div set		
		I2CWRNBYTE_CODEC(0x06,0x01);			
		I2CWRNBYTE_CODEC(0x00,0x00);	//LRCK=MCLK/256		
	}
	if(Ratio == 64)//Ratio=MCLK/LRCK=64：3M073-48K；2M048-32K;//MCLK必须大于2M
	{//PLL 4倍频座256Ratio
		I2CWRNBYTE_CODEC(0x6F,0xA0);	//pll set:mode enable
		I2CWRNBYTE_CODEC(0x72,0x41);	//pll set:mode set
		I2CWRNBYTE_CODEC(0x09,0x02);	//pll set:reset on ,set start
		I2CWRNBYTE_CODEC(0x0C,0x00);	//pll set:k
		I2CWRNBYTE_CODEC(0x0D,0x00);	//pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x00);	//pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x88);	//pll set:
		I2CWRNBYTE_CODEC(0x0B,0x0A);	//pll set:n
		I2CWRNBYTE_CODEC(0x09,0x42);	//pll set:reset off ,set stop
		I2CWRNBYTE_CODEC(0x02,0x08);	//pll set:use pll

		I2CWRNBYTE_CODEC(0x01,0x7F);	//IC clk on
		I2CWRNBYTE_CODEC(0x02,0x00);	
		I2CWRNBYTE_CODEC(0x03,0x20);	
		I2CWRNBYTE_CODEC(0x04,0x00);			
		I2CWRNBYTE_CODEC(0x05,0x11);	//clk div set		
		I2CWRNBYTE_CODEC(0x06,0x01);			
		I2CWRNBYTE_CODEC(0x00,0x00);	//LRCK=MCLK/256		
	}

	I2CWRNBYTE_CODEC(0x10,Format + (Format_Len<<2));
	I2CWRNBYTE_CODEC(0x11,Format + (Format_Len<<2));

	if(PLLSet12288 == 26000)//13M锁12M288 Ratio按照锁后的12M288/LRCK来选择,通过GPIO2输出PLL锁存后MCLK确认波形
	{
		I2CWRNBYTE_CODEC(0x01,0xFF);	//26M内部DIV2变成13000
		I2CWRNBYTE_CODEC(0x6F,0xA0);	//pll set:mode enable
		I2CWRNBYTE_CODEC(0x72,0x41);	//pll set:mode set
		I2CWRNBYTE_CODEC(0x09,0x01);	//pll set:reset on ,set start
		I2CWRNBYTE_CODEC(0x0C,0x17);	//pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xA3);	//pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2F);	//pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x88);	//pll set:
		I2CWRNBYTE_CODEC(0x0B,0x07);	//pll set:n
		I2CWRNBYTE_CODEC(0x09,0x41);	//pll set:reset off ,set stop
		I2CWRNBYTE_CODEC(0x02,0x08);	//pll set:use pll
	}
	if(PLLSet12288 == 13000)//13M锁12M288 Ratio按照锁后的12M288/LRCK来选择,通过GPIO2输出PLL锁存后MCLK确认波形
	{
		I2CWRNBYTE_CODEC(0x6F,0xA0);	//pll set:mode enable
		I2CWRNBYTE_CODEC(0x72,0x41);	//pll set:mode set
		I2CWRNBYTE_CODEC(0x09,0x01);	//pll set:reset on ,set start
		I2CWRNBYTE_CODEC(0x0C,0x17);	//pll set:k
		I2CWRNBYTE_CODEC(0x0D,0xA3);	//pll set:k
		I2CWRNBYTE_CODEC(0x0E,0x2F);	//pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x88);	//pll set:
		I2CWRNBYTE_CODEC(0x0B,0x07);	//pll set:n
		I2CWRNBYTE_CODEC(0x09,0x41);	//pll set:reset off ,set stop
		I2CWRNBYTE_CODEC(0x02,0x08);	//pll set:use pll
	}
	if(PLLSet12288 == 12000)//12M锁12M288 Ratio按照锁后的12M288/LRCK来选择,通过GPIO2输出PLL锁存后MCLK确认波形
	{
		I2CWRNBYTE_CODEC(0x6F,0xA0);	//pll set:mode enable
		I2CWRNBYTE_CODEC(0x72,0x41);	//pll set:mode set
		I2CWRNBYTE_CODEC(0x09,0x01);	//pll set:reset on ,set start
		I2CWRNBYTE_CODEC(0x0C,0x08);	//pll set:k
		I2CWRNBYTE_CODEC(0x0D,0x13);	//pll set:k
		I2CWRNBYTE_CODEC(0x0E,0xE0);	//pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x88);	//pll set:
		I2CWRNBYTE_CODEC(0x0B,0x08);	//pll set:n
		I2CWRNBYTE_CODEC(0x09,0x41);	//pll set:reset off ,set stop
		I2CWRNBYTE_CODEC(0x02,0x08);	//pll set:use pll
	}
	if(PLLSet12288 == 4000)//4M锁12M288 Ratio按照锁后的12M288/LRCK来选择,通过GPIO2输出PLL锁存后MCLK确认波形
	{
		I2CWRNBYTE_CODEC(0x6F,0xA0);	//pll set:mode enable
		I2CWRNBYTE_CODEC(0x72,0x41);	//pll set:mode set
		I2CWRNBYTE_CODEC(0x09,0x01);	//pll set:reset on ,set start
		I2CWRNBYTE_CODEC(0x0C,0x0C);	//pll set:k
		I2CWRNBYTE_CODEC(0x0D,0x1D);	//pll set:k
		I2CWRNBYTE_CODEC(0x0E,0xD0);	//pll set:k
		I2CWRNBYTE_CODEC(0x0A,0x88);	//pll set:
		I2CWRNBYTE_CODEC(0x0B,0x0C);	//pll set:n
		I2CWRNBYTE_CODEC(0x09,0x41);	//pll set:reset off ,set stop
		I2CWRNBYTE_CODEC(0x02,0x08);	//pll set:use pll
	}
	I2CWRNBYTE_CODEC(0x0F,(MSMode_MasterSelOn<<7) + (SCLK_INV<<5) + SCLK_DIV);

	I2CWRNBYTE_CODEC(0x24,0x08 + (Dmic_GAIN<<7) + Dmic_Selon);	//adc set
	I2CWRNBYTE_CODEC(0x36,(DACChannelSel<<6));	//dac set
	I2CWRNBYTE_CODEC(0x12,0x30);	//timming set
	I2CWRNBYTE_CODEC(0x13,0x20);	//timming set
	I2CWRNBYTE_CODEC(0x18,0xFF);
	I2CWRNBYTE_CODEC(0x21,0x50);
	I2CWRNBYTE_CODEC(0x22,ADC_PGA_GAIN + (ADC_PGA_GAIN<<4));	//adc set:	PGA GAIN=0DB
	I2CWRNBYTE_CODEC(0x21,(ADCChannelSel<<4) + (ADC_PGA_DF2SE_15DB<<2));
	I2CWRNBYTE_CODEC(0x00,0x80);	// IC START	
	DELAY_MS(30);  					//DELAY_MS                
	I2CWRNBYTE_CODEC(0x14,0x8A + (MICBIASoff<<4));	
	I2CWRNBYTE_CODEC(0x15,0x40);	
	I2CWRNBYTE_CODEC(0x1A,0xA0);	// monoout set
	I2CWRNBYTE_CODEC(0x1B,0x19);//5V 0DB 
	I2CWRNBYTE_CODEC(0x1C,0x90);
	I2CWRNBYTE_CODEC(0x1D,0x2B);//5V 0DB 8R=>1W
	I2CWRNBYTE_CODEC(0x1F,0x00);	// spk set
	I2CWRNBYTE_CODEC(0x20,0x00);	// spk set
	I2CWRNBYTE_CODEC(0x1E,0xA0);	// spk on
	I2CWRNBYTE_CODEC(0x28,0x00);	// alc set
	I2CWRNBYTE_CODEC(0x25,ADC_Volume);	// ADCVOLUME  on
	I2CWRNBYTE_CODEC(0x38,DAC_Volume);	// DACVOLUMEL on
	I2CWRNBYTE_CODEC(0x37,0x00);	// dac set
	I2CWRNBYTE_CODEC(0x6D,0x60);	//SEL:GPIO1=DMIC CLK OUT+SEL:GPIO2=PLL CLK OUT
}

void ES8374_StandBy(void)//待机配置--搭配ES8374_Resume(void)//恢复配置
{
	I2CWRNBYTE_CODEC(0x38,0xC0);
	I2CWRNBYTE_CODEC(0x25,0xC0);
	I2CWRNBYTE_CODEC(0x15,0x00);
	I2CWRNBYTE_CODEC(0x28,0x1C);
	I2CWRNBYTE_CODEC(0x36,0x20);
	I2CWRNBYTE_CODEC(0x37,0x20);
	I2CWRNBYTE_CODEC(0x6D,0x00);	
	I2CWRNBYTE_CODEC(0x09,0x80);
	I2CWRNBYTE_CODEC(0x1A,0x08);
	I2CWRNBYTE_CODEC(0x1E,0x20);
	DELAY_MS(5);  	
	I2CWRNBYTE_CODEC(0x1C,0x10);
	I2CWRNBYTE_CODEC(0x1D,0x10);
	I2CWRNBYTE_CODEC(0x1E,0x40);
	I2CWRNBYTE_CODEC(0x24,0x20);
	I2CWRNBYTE_CODEC(0x22,0x00);
	I2CWRNBYTE_CODEC(0x21,0xC0);
	I2CWRNBYTE_CODEC(0x15,0xFF);
	I2CWRNBYTE_CODEC(0x14,0x16);
	I2CWRNBYTE_CODEC(0x01,0x03);
}

void ES8374_Resume(void)//恢复配置(未下电)--搭配ES8374_StandBy(void)
{
	I2CWRNBYTE_CODEC(0x01,0x7F);	
	I2CWRNBYTE_CODEC(0x14,0x8A);	
	I2CWRNBYTE_CODEC(0x15,0x40);	
	I2CWRNBYTE_CODEC(0x21,0x10);
	I2CWRNBYTE_CODEC(0x22,0xFF);	
	I2CWRNBYTE_CODEC(0x24,0x08);	
	I2CWRNBYTE_CODEC(0x24,0xA0);	
	I2CWRNBYTE_CODEC(0x1D,0x2B);	
	I2CWRNBYTE_CODEC(0x1C,0x90);
	I2CWRNBYTE_CODEC(0x1A,0xA0);	
	I2CWRNBYTE_CODEC(0x09,0x41);	
	I2CWRNBYTE_CODEC(0x6D,0x60);	
	I2CWRNBYTE_CODEC(0x37,0x00);			
	I2CWRNBYTE_CODEC(0x36,0x00);			
	I2CWRNBYTE_CODEC(0x28,0x00);				
	I2CWRNBYTE_CODEC(0x38,0x00);
	I2CWRNBYTE_CODEC(0x25,0x00);

}

