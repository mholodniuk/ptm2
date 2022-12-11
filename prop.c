/**************************************/
/*              ARE 2008              */
/*      e-mail: biuro@are.net.pl      */
/*      www   : are.net.pl            */
/**************************************/
/* Marcin Wawszczak 235274
/* Jerzy Lorek 241575
/* Micha� �akomski 241570   

25.11.2019*/
/**************************************/

#define F_CPU 8000000UL  // 1 MHz
//#define F_CPU 14.7456E6
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>

void delay_ms(int ms)
	{
	volatile long unsigned int i;
	for(i=0;i<ms;i++)
		_delay_ms(1);
	}
	
void delay_us(int us)
	{
	volatile long unsigned int i;
	for(i=0;i<us;i++)
		_delay_us(1);
	}

//RS PA0
//RW PA1
//E  PA2
//DATA PD

#define RS 0
#define RW 1
#define E  2

void LCD2x16_init(void)
{
PORTC &= ~(1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
PORTD = 0x38;   // dwie linie, 5x7 punktow
PORTC &=~(1<<E);
_delay_us(120);

PORTC |= (1<<E);
PORTD = 0x0e;   // wlacz wyswietlacz, kursor, miganie
PORTC &=~(1<<E);
_delay_us(120);

PORTC |= (1<<E);
PORTD = 0x06;
PORTC &=~(1<<E);
_delay_us(120);
}

void LCD2x16_clear(void){
PORTC &= ~(1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
PORTD = 0x01;
PORTC &=~(1<<E);
delay_ms(120);
}

void LCD2x16_putchar(int data)
{
PORTC |= (1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
PORTD = data;
PORTC &=~(1<<E);
_delay_us(120);
}

void LCD2x16_pos(int wiersz, int kolumna)
{
PORTC &= ~(1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
delay_ms(1);
PORTD = 0x80+(wiersz-1)*0x40+(kolumna-1);  
PORTC &=~(1<<E);
_delay_us(120);
}



int main(void) {
  char www[16] = "www-> are.net.pl";
  char email[16] = "biuro@are.net.pl";
  char tmp[17];


  int i;
  int j=4;

  DDRA = 0x00;
  PORTA = 0x00;
  DDRD = 0xff;
  PORTD = 0x00;
  DDRC = 0xff;
  PORTC = 0x00;


  _delay_ms(200);

  LCD2x16_init();
  LCD2x16_clear();


  for(i=0;i < 16;i++)
      LCD2x16_putchar(www[i]);

  LCD2x16_pos(2,1);

  for(i=0;i < 16;i++)
      LCD2x16_putchar(email[i]);


  LCD2x16_clear();
  for(i=0;i < 16;i++)
    LCD2x16_putchar(www[i]);

  ADMUX = 0x40;
  ADCSRA = 0xe0;
  float _pv = 0;
  int _cv = 0;
  int _sp = 40;
  int _h = 8;
  int _e = 0;
  int _ipv;
  int _decpv;
  int _xp = 40;
  int q2 = 0;
  int _T=4000;
  int _tH=0;
  int iter=0;
  LCD2x16_clear();
  delay_ms(3000);


  while(1)
  {
    _pv = ADC;
    _ipv =  _pv / 10;
    _e = _sp - _ipv;
    _decpv = (_pv - _ipv * 10);


  if(_e< -_xp/2) { 
    _cv=0;
  }
  else if(_e> _xp/2) {
    _cv=100;
  }
  else { 
    _cv= (((_e+_xp/2)*20/_xp))*5;
  }

  //dioda

  if(iter<_T/200*_cv/100){
    PORTC &= ~(1 << PC3);
  } 
  else{
    PORTC |= (1 << PC3);
  }



  if(!(PINA &(1 << PINA1))){
    _sp = 60;
  }

  if(!(PINA &(1 << PINA2))){
    _sp = 50;
  }

  if(!(PINA &(1 << PINA3))){
    _xp = 30;
  }

  if(!(PINA &(1 << PINA4))){
    _xp = 40;
  }

  LCD2x16_pos(1,1);
  sprintf(tmp,"CV=%3d PV=%3d.%1d%%",_cv, _ipv, _decpv);

  for(i=0;i < 16;i++)
    LCD2x16_putchar(tmp[i]);

  LCD2x16_pos(2,1);
  sprintf(tmp,"Xp=%2d   E=%3d.%1d%%", _xp, _e, _decpv);

  for(i=0;i < 16;i++)
    LCD2x16_putchar(tmp[i]);

  delay_ms(1000);   
  j++;
  iter++;
  if(iter>=_T/200) iter=0;
}

  return 0;
}
