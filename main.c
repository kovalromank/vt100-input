#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>

#define ASCII_ESC 27									 // Escape character, decimal value, octal value = 33, hex = 1B
#define VT100_HOME "\033[H"						 // move cursor home (upper left hand corner)
#define VT100_LINE "\033[%d;1H"				 // move to line, use with sprintf/prinf functions
#define VT100_CLR_TO_END_LINE "\033[K" // Clears from cursor to end of line
#define VT100_HIDE_CURSOR "\033[?25l"	 // Not always supported on Vt100's
#define VT100_SHOW_CURSOR "\033[?25h"
#define VT100_CLEAR_SCREEN "\033[2J"
#define LF 10 // Line feed character/key
#define CR 13 // Carriage return character/key
#define SPACE 32
#define BCK_SPC 8 // Backspace character/key
#define DEL 127		// Delete character/key

#define UI_LINE_SIZE 80

int set_interface_attribs(int fd, int speed, int parity);
void set_blocking(int fd, int should_block);

char line[UI_LINE_SIZE];

void uart_print(char *str)
{
	size_t len = strlen(str);
	fwrite(str, sizeof(char), len, stdout);
	fflush(stdout);
}

void moveCursorToLine(int i)
{
	snprintf(line, UI_LINE_SIZE, VT100_LINE, i);
	uart_print(line);

	snprintf(line, UI_LINE_SIZE, VT100_LINE, i);
	uart_print(line);
}

void clear(int fd)
{
	snprintf(line, UI_LINE_SIZE, "%s%s", VT100_HOME, VT100_CLEAR_SCREEN);
	write(fd, line, strlen(line) + 1);
}

void display(int i)
{
	snprintf(line, UI_LINE_SIZE, VT100_HOME);
	uart_print(line);

	moveCursorToLine(1);
	snprintf(line, UI_LINE_SIZE, "SOURCE P1");
	uart_print(line);

	moveCursorToLine(2);
	snprintf(line, UI_LINE_SIZE, "vrms %d", i);
	uart_print(line);

	moveCursorToLine(3);
	snprintf(line, UI_LINE_SIZE, "irms %d", i);
	uart_print(line);

	moveCursorToLine(4);
	snprintf(line, UI_LINE_SIZE, "freq %d", i);
	uart_print(line);

	moveCursorToLine(5);
}

int max(int x, int y)
{
	return x > y ? x : y;
}

void parseInput(char *input)
{
	char *enable = "enable";
	char *disable = "disable";
	char *open = "open";
	char *close = "close";
	char *source = "source";
	char *load = "load";
	char *p1 = "1";
	char *p2 = "2";
	char *p3 = "3";

	char *actions[] = {enable, disable, open, close};
	char *groups[] = {source, load};
	char *ports[] = {p1, p2, p3};

	printf("input: '%s'\n", input);

	char *split = strtok(input, " ");
	while (split != NULL)
	{
		printf("keyword: '%s'\n", split);
		split = strtok(NULL, " ");
	}
}

void run(int fd)
{
	static char buf[100];
	static char input[100];
	static int i = 0;

	input[i] = 0;

	int n = read(fd, buf, sizeof(buf));

	if (n > 0)
	{
		for (int j = 0; j < n; j++)
		{
			char c = buf[j];

			switch (c)
			{
			case DEL:
			case BCK_SPC:
				i = max(i - 1, 0);
				input[i] = 0;
				break;
			case LF:
			case CR:
				parseInput2(input);
				i = 0;
				input[i] = 0;
				break;
			default:
				if (i + 1 < sizeof(input))
				{
					input[i] = c;
					input[++i] = 0;
				}
				break;
			}
		}
	}

	clear(fd);
	if (i > 0)
	{
		write(fd, input, i);
	}
}

int main()
{
	char *name = "/dev/ptyp7";

	int fd = open(name, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		printf("error %d opening %s: %s\n", errno, name, strerror(errno));
		return 1;
	}

	printf("opened serial port\n");

	set_interface_attribs(fd, B115200, 0); // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking(fd, 0);									 // set no blocking

	printf("init serial port\n");

	while (1)
	{
		run(fd);
		usleep(240);
	}

	// for (int i = 0; i < 5; i++)
	// {
	// 	clear();
	// 	display(131072 >> (i * 4));
	// 	sleep(2);
	// }

	return 0;
}
