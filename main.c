#define F_CPU 1000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


#define RGBRED PB2
#define RGBGREEN PB1
#define RGBBLUE PB0
#define RED PB3
#define BLUE PB4

#define PORT PORTB
#define DDR DDRB

volatile unsigned char red, green, blue, R, B = 255;


volatile unsigned char pwmvalue = 1;

unsigned int hue = 0;


ISR(TIM0_COMPA_vect) {

	char port = 0;
	if (pwmvalue > red)
		port |= (1 << RGBRED);

	if (pwmvalue > green)
		port |= (1 << RGBGREEN);

	if (pwmvalue > blue)
		port |= (1 << RGBBLUE);

	if (pwmvalue > R)
		port |= (1 << RED);

	if (pwmvalue > B)
		port |= (1 << BLUE);


	PORT = port;

	pwmvalue += 2;
}


void setHSV(int hue)
{
	char r, g, b;
	unsigned int accent = hue / 60;
	unsigned char bottom = 0;
	unsigned char top = 255;
	unsigned char rising = (255 * (hue % 60)) / 60;
	unsigned char falling = (255 * (60 - hue % 60)) / 60;

	switch (accent)
	{
	case 0:
		r = top;
		g = rising;
		b = bottom;
		break;
	case 1:
		r = falling;
		g = top;
		b = bottom;
		break;
	case 2:
		r = bottom;
		g = top;
		b = rising;
		break;
	case 3:
		r = bottom;
		g = falling;
		b = top;
		break;
	case 4:
		r = rising;
		g = bottom;
		b = top;
		break;
	case 5:
		r = top;
		g = bottom;
		b = falling;
		break;
	}

	red = r;
	green = g;
	blue = b;


}


int main() {

	DDR = (1 << RGBRED) | (1 << RGBGREEN) | (1 << RGBBLUE) | (1 << RED) | (1 << BLUE);

	R = 0;
	B = 0;


	TCCR0A = (1 << WGM01);

	// prescale timer to 1/8th the clock rate
	TCCR0B = (1 << CS01);

	OCR0A = 13;

	// enable timer overflow interrupt
	TIMSK0 = (1 << OCIE0A);
	sei();


	setHSV();

	char lamp = 0;
	char falling = 0;

	unsigned char counter = 0;
	const int delay = 50;
	while (1)
	{
		char value;
		if (falling)
		{
			if (counter < delay)
				value = 0xff - delay;
			else
				value = 0xff - counter;
		}
		else
		{
			if (counter < delay)
				value = 0;
			else
				value = counter - delay;
		}

		if (lamp)
			R = value;
		else
			B = value;


		if (counter == 0xff)
		{
			if (falling)
				lamp ^= 0x01;

			falling ^= 0x01;
		}

		if (!(counter & 0x0f))
		{
			if (hue >= 359)
				hue = 0;
			else
				hue+=2;
			setHSV();
		}


		_delay_ms(2);

		counter++;

	}
}

