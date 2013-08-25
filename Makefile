CC=msp430-gcc
MCU=-mmcu=msp430g2452
MCU1=-mmcu=msp430g2553
#MCU=-mmcu=msp430f5438a

CFLAGS=$(MCU) -g -Wall
CFLAGS1=$(MCU1) -g -Wall

all: timer_intr blink_test msp430g2xx2_ta_uart9600 msp430g2xx2_ta_uart2400 back_channel_uart breadboard_led master_spi

timer_intr: timer_intr.o
	$(CC) $(CFLAGS) $^ -o $@

timer_intr.o: timer_intr.c
	$(CC) $(CFLAGS) -c $^

blink_test: blink_test.o
	$(CC) $(CFLAGS) $^ -o $@

blink_test.o: blink_test.c
	$(CC) $(CFLAGS) -c $^

msp430g2xx2_ta_uart9600: msp430g2xx2_ta_uart9600.o
	$(CC) $(CFLAGS) $^ -o $@

msp430g2xx2_ta_uart9600.o: msp430g2xx2_ta_uart9600.c
	$(CC) $(CFLAGS) -c $^

msp430g2xx2_ta_uart2400: msp430g2xx2_ta_uart2400.o
	$(CC) $(CFLAGS) $^ -o $@

msp430g2xx2_ta_uart2400.o: msp430g2xx2_ta_uart2400.c
	$(CC) $(CFLAGS) -c $^

back_channel_uart: back_channel_uart.o
	$(CC) $(CFLAGS) $^ -o $@

back_channel_uart.o: back_channel_uart.c
	$(CC) $(CFLAGS) -c $^

breadboard_led: breadboard_led.o
	$(CC) $(CFLAGS1) $^ -o $@

breadboard_led.o: breadboard_led.c
	$(CC) $(CFLAGS1) -c $^

master_spi: master_spi.o
	$(CC) $(CFLAGS1) $^ -o $@

master_spi.o: master_spi.c
	$(CC) $(CFLAGS1) -g -Wall -c $^

clean:
	rm -rvf timer_intr blink_test *.o msp430g2xx2_ta_uart9600 msp430g2xx2_ta_uart2400 back_channel_uart master_spi breadboard_led
