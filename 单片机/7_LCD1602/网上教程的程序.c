//----------------------------------------LCD1602.C----------------------------
//液晶控制与显示程序
//------------------------------------------------------------------------------
#include <reg51.h>
#include <intrins.h>
#define u8 unsigned char
#define u16 unsigned int

sbit RS=P2^0;
sbit RW=P2^1;
sbit EN=P2^2;
//-------------------------------------
//延时
//-------------------------------------
void DelayMS(u16 ms)
{
 u8 i;
 while(ms--) for(i=0;i<120;i++);
}
//------------------------------------
//忙检查
//------------------------------------
u8 Busy_Check()
{
 u8 LCD_Status;
 RS=0;    //RS为寄存器选择，高电平1时选择数据寄存器、低电平0时选择指令寄存器。
 RW=1;    //R/W为读写信号线，高电平(1)时进行读操作，低电平(0)时进行写操作
 EN=1;  //E(或EN)端为使能(enable)端，下降沿使能，下跳变时液晶屏开始执行命令
 DelayMS(1);
 LCD_Status=P0;    //DB9为高4位三态、 双向数据总线 7位（最高位）（也是busy flag）
 EN=0;
 return LCD_Status;
}
//-----------------------------
//写LCD命令
//---------------------------------
void Write_LCD_Command(u8 cmd)
{
 while((Busy_Check()&0x80)==0x80); //忙等待
 RS=0;  //低电平0时选择指令寄存器
 RW=0;  //低电平(0)时进行写操作
 EN=0;  //写指令时，E为高脉冲，即让E从0到1发生正跳变，应先置0
 P0=cmd;
 EN=1;       //产生正跳变
 DelayMS(1);
 EN=0; 
}
//-----------------------------
//设置LCD显示位置
//---------------------------------
void Set_Disp_Pos(u8 pos)
{
 Write_LCD_Command(pos|0x80);
}
//-----------------------------
//发送数据
//---------------------------------
void Write_LCD_Data(u8 dat)
{
 while((Busy_Check()&0x80)==0x80); //忙等待
 RS=1;    //高电平1时选择数据寄存器
 RW=0;  //低电平(0)时进行写操作
 EN=0;
 P0=dat;
 EN=1;
 DelayMS(1);
 EN=0;
}
//-----------------------------
//LCD初始化
//---------------------------------
void Initialize_LCD()
{
 Write_LCD_Command(0x38);    //设置16×2显示，5×7点阵，8位数据接口
 DelayMS(1);
 Write_LCD_Command(0x01);    //显示清屏，数据指针=0，所有显示=0
 DelayMS(1);
 Write_LCD_Command(0x06); //写指令06H  置字符输入模式：屏幕不动，字符后移
 DelayMS(1);
 Write_LCD_Command(0x0c);    //显示开，关光标
 DelayMS(1);
}

void Display_LCD_String(u8 p,u8 *s)
{
 u8 i; 
 Set_Disp_Pos(p);
 for(i=0;i<16;i++)                     //输出字符串
 {
     Write_LCD_Data(s[i]);
    }
}
/*
//----------------------------------
//显示字符串
//---------------------------------
void ShowString(u8 x,u8 y,u8 *str)
{
 u8 i=0; 
 if (y==0) Write_LCD_Command(0x80|x);    //以下设置显示起始位置
 if (y==1) Write_LCD_Command(0xc0|x); //1602型LCD字符显示位置的确定方法规定为：80H+地址码，注意可以设置显示位置
           //第二行行首地址：80H+40H=0CH  教材P287
 for(i=0;i<16;i++)                     //输出字符串
 {
     Write_LCD_Data(str[i]);
    }
}
*/
//-----------------------------------------------------------------------
//1602型LCD内部地址参照教材P287，1602型字符显示位置的确定方法规定为80H+地址码（00-0FH 40-4FH）
//如要将字符显示在第2行第6列，则确定地址的指令代码应为80H+45H=C5H
//--------------------------------------------------------------------------