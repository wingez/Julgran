
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

//const unsigned char gammaValues[256] PROGMEM =
//{
//	0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,
//	2,   2,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6,
//	7,   7,   7,   8,   8,   8,   9,   9,   10,  10,  10,  11,  11,  12,  12,  13,
//	13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  21,  21,
//	22,  22,  23,  23,  24,  25,  25,  26,  27,  27,  28,  29,  29,  30,  31,  31,
//	32,  33,  34,  34,  35,  36,  37,  37,  38,  39,  40,  40,  41,  42,  43,  44,
//	44,  45,  46,  47,  48,  49,  50,  50,  51,  52,  53,  54,  55,  56,  57,  58,
//	58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,
//	74,  75,  76,  77,  78,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  91,
//	92,  93,  94,  95,  96,  97,  99,  100, 101, 102, 103, 105, 106, 107, 108, 109,
//	111, 112, 113, 114, 116, 117, 118, 120, 121, 122, 123, 125, 126, 127, 129, 130,
//	131, 133, 134, 135, 137, 138, 139, 141, 142, 144, 145, 146, 148, 149, 151, 152,
//	154, 155, 156, 158, 159, 161, 162, 164, 165, 167, 168, 170, 171, 173, 174, 176,
//	177, 179, 180, 182, 183, 185, 186, 188, 190, 191, 193, 194, 196, 198, 199, 201,
//	202, 204, 206, 207, 209, 211, 212, 214, 216, 217, 219, 221, 222, 224, 226, 227,
//	229, 231, 233, 234, 236, 238, 240, 241, 243, 245, 247, 248, 250, 252, 254, 255
//};




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


void setHSV(hue)
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

	/*r = pgm_read_byte(&(gammaValues[r]));
	g = pgm_read_byte(&(gammaValues[g]));
	b = pgm_read_byte(&(gammaValues[b]));
	*/
	red = r;
	green = g;
	blue = b;


}


int main(void) {

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

