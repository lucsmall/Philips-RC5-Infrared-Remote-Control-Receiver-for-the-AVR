
//#define UART_BAUD_RATE 4800
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_CPU)/((UART_BAUD_RATE)*16l)-1)

void serial_init(unsigned int baud);
void serial_putc(unsigned char c);
void serial_puts (char *s) ;
