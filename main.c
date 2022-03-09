#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#define ASCII_ESC 27						// Escape character, decimal value, octal value = 33, hex = 1B
#define VT100_HOME "\033[H"			// move cursor home (upper left hand corner)
#define VT100_LINE "\033[%d;1H" // move to line, use with sprintf/prinf functions
#define VT100_CLR_TO_END_LINE "\033[K" // Clears from cursor to end of line
#define VT100_HIDE_CURSOR "\033[?25l"	 // Not always supported on Vt100's
#define VT100_SHOW_CURSOR "\033[?25h"
#define VT100_CLEAR_SCREEN "\033[2J"
#define BCK_SPC 8 // Backspace character/key
#define DEL 127		// Delete character/key

#define UI_LINE_SIZE 80

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

void clear()
{
	snprintf(line, UI_LINE_SIZE, "%s%s", VT100_HOME, VT100_CLEAR_SCREEN);
	uart_print(line);
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

int main()
{
	for (int i = 0; i < 5; i++)
	{
		clear();
		display(131072 >> (i * 4));
		sleep(2);
	}

	return 0;
}
