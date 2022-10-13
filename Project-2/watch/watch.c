#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define INT_PER_MIN 60
#define INT_PER_HR  60

unsigned char g_tick_min;
unsigned char g_tick_hr;
unsigned char g_sec1;
unsigned char g_sec2;
unsigned char g_min1;
unsigned char g_min2;
unsigned char g_hour1;
unsigned char g_hour2;

void reset(void) {
	g_sec1 = 0;
	g_sec2 = 0;
	g_min1 = 0;
	g_min2 = 0;
	g_hour1 = 0;
	g_hour2 = 0;
	g_tick_min = 0;
	g_tick_hr = 0;
}
void display_num(unsigned char num) {
	PORTC = (PORTC & 0xF0) | (num & 0x0F);
}
void external_interrupt_0(void) {
	DDRD &= ~(1 << 2);
	PORTD |= (1 << 2);
	MCUCR &= ~(1 << ISC00);
	MCUCR |= (1 << ISC01);
	GICR |= (1 << INT0);
}

void external_interrupt_1(void) {
	DDRD &= ~(1 << 3);
	MCUCR |= (1 << ISC10) | (1 << ISC11);
	GICR |= (1 << INT1);
}

void external_interrupt_2(void) {
	DDRB &= ~(1 << 2);
	PORTB |= (1 << 2);
	MCUCSR &= ~(1 << ISC2);
	GICR |= (1 << INT2);
}

void timer_1(void) {
	TCCR1A = (1 << FOC1A);
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	TCNT1 = 0;
	TIMSK = (1 << OCIE1A);
	OCR1A = 1000;
}
ISR(TIMER1_COMPA_vect) {
	if (g_sec1 == 9 && g_sec2 == 5 && g_min1 == 9 && g_min2 == 5 && g_hour1 == 9
			&& g_hour2 == 9) {
		reset();
	} else {
		if (g_sec1 == 9) {
			g_sec1 = 0;
			if (g_sec2 == 5) {
				g_sec2 = 0;
			} else {
				g_sec2++;
			}
		} else {
			g_sec1++;
		}
		g_tick_min++;
		if (g_tick_min == INT_PER_MIN) {
			if (g_min1 == 9) {
				g_min1 = 0;
				if (g_min2 == 5) {
					g_min2 = 0;
				} else {
					g_min2++;
				}
			} else {
				g_min1++;
			}
			g_tick_min = 0;
			g_tick_hr++;
		}
		if (g_tick_hr == INT_PER_HR) {
			if (g_hour1 == 9) {
				g_hour1 = 0;
				if (g_hour2 == 5) {
					g_hour2 = 0;
				} else {
					g_hour2++;
				}
			} else {
				g_hour1++;
			}
			g_tick_hr = 0;
		}
	}
}

ISR(INT0_vect) {
	reset();
}
ISR(INT1_vect) {
	TCCR1B = 0;
}
ISR(INT2_vect) {
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
}
int main(void) {
	DDRC |= 0x0F;
	DDRA |= 0x3F;
	PORTA |= 0x3F;
	display_num(0);
	reset();
	external_interrupt_0();
	external_interrupt_1();
	external_interrupt_2();
	timer_1();
	SREG |= (1 << 7);
	for (;;) {
		display_num(g_sec1);
		PORTA |= (1 << 0);
		_delay_ms(2);
		PORTA &= 0xC0;
		display_num(g_sec2);
		PORTA |= (1 << 1);
		_delay_ms(2);
		PORTA &= 0xC0;
		display_num(g_min1);
		PORTA |= (1 << 2);
		_delay_ms(2);
		PORTA &= 0xC0;
		display_num(g_min2);
		PORTA |= (1 << 3);
		_delay_ms(2);
		PORTA &= 0xC0;
		display_num(g_hour1);
		PORTA |= (1 << 4);
		_delay_ms(2);
		PORTA &= 0xC0;
		display_num(g_hour2);
		PORTA |= (1 << 5);
		_delay_ms(2);
		PORTA &= 0xC0;
	}
	return 0;
}
