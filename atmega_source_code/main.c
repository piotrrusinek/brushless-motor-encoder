/*
 * test.c
 *
 * Created: 6/17/2022 1:04:53 AM
 * Author : piotr
 */ 

#define F_CPU 1000000UL  // avoid warning

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


//D0-D7 -> PD0-PD7
//RS,RW,E -> PB0,PB1,PB2
//LED ->PB3

#define LCD_Data_Dir DDRD		/* Define LCD data port direction */
#define LCD_Command_Dir DDRB		/* Define LCD command port direction register */
#define LCD_Data_Port PORTD		/* Define LCD data port */
#define LCD_Command_Port PORTB		/* Define LCD data port */
#define RS PB0				/* Define Register Select (data/command reg.)pin */
#define RW PB1				/* Define Read/Write signal pin */
#define EN PB2
#define ADMUXP0 0b01100000
#define ADMUXP1 0b01100001
#define ADMUXP2 0b01100010
#define LED 0b00001000 // PC3


void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port = cmnd;
	LCD_Command_Port &= ~(1<<RS);	/* RS=0 command reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 Write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN);   /*enable pulse end*/
	_delay_ms(3);
}

void LCD_Char (unsigned char char_data)	/* LCD data write function */
{
	LCD_Data_Port= char_data;
	LCD_Command_Port |= (1<<RS);	/* RS=1 Data reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable Pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN);   /* Enable Pulse end*/
	_delay_ms(1);
}

void LCD_Char_pos(unsigned char char_data,char row, char pos)
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	
	LCD_Data_Port= char_data;
	LCD_Command_Port |= (1<<RS);	/* RS=1 Data reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable Pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN);   /* Enable Pulse end*/
	_delay_ms(1);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Command_Dir = 0xFF;		/* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;		/* Make LCD data port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_Command (0x38);		/* Initialization of 16X2 LCD in 8bit mode */
	LCD_Command (0x0C);		/* Display ON Cursor OFF */
	LCD_Command (0x06);		/* Auto Increment cursor */
	LCD_Command (0x01);		/* Clear display */
	LCD_Command (0x80);		/* Cursor at home position */
}

void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)/* Send string to LCD with xy position */
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	LCD_String(str);		/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* clear display */
	LCD_Command (0x80);		/* cursor at home position */
}


int main() {
	
	LCD_Init();			/* Initialize LCD */

	// Configure PORT C bit 0,1,2 to an input and bit 3 to output
	DDRC = 0b00001000;

	// Configure ADC to be left justified, use AVCC as reference, and select ADC0 as ADC input
	ADMUX = 0b01100000;

	// Enable the ADC and set the prescaler to max value (128)
	ADCSRA = 0b10000111;
	
	PORTC |= LED;

	int count = 1;
	int frame_count = 1;
	

	while(1)
	{
		
		
		ADMUX = ADMUXP0;
		// Start an ADC conversion by setting ADSC bit (bit 6)
		ADCSRA = ADCSRA | (1 << ADSC);
		// Wait until the ADSC bit has been cleared
		while(ADCSRA & (1 << ADSC));
		int reading1 = ADCH;
		
		
		ADMUX = ADMUXP1;
		// Start an ADC conversion by setting ADSC bit (bit 6)
		ADCSRA = ADCSRA | (1 << ADSC);
		// Wait until the ADSC bit has been cleared
		while(ADCSRA & (1 << ADSC));
		int reading2 = ADCH;
		
		
		ADMUX = ADMUXP2;
		// Start an ADC conversion by setting ADSC bit (bit 6)
		ADCSRA = ADCSRA | (1 << ADSC);
		// Wait until the ADSC bit has been cleared
		while(ADCSRA & (1 << ADSC));
		int reading3 = ADCH;
		
		
		if(reading3>5)
		{
			
			if(reading2>reading1)
			{
				 //LCD_Clear();
				 //LCD_String("w prawo");
				 count++;
				 if(count>16) count = 16;
				 
				 LCD_Clear();
				 for(int i = 0; i<count; i++)
				 {
					 LCD_Char((char)43);
				 }
			}
			else if(reading1>reading2)
			{
				//LCD_Clear();
				//LCD_String("w lewo");
				count--;
				if(count<1) count = 1;
				
				LCD_Clear();
				for(int i = 0; i<count; i++)
				{
					LCD_Char((char)43);
				}
			}
			
			
			
		}
		
		
		//PWM LED control
		//TDO: LED constantly blinking due to too long cycle
		if(frame_count>16) frame_count=1;
		if(frame_count<=count) PORTC |= LED;
		else PORTC &= ~LED;
		frame_count++;
		
		
		//char str[12];
		//sprintf(str, "%d", reading1);
		//LCD_String(str);
	}
	
	
	return 0;
	
	
}