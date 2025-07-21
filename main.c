#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <Windows.h>

#define SCREEN_WIDTH	50
#define	SCREEN_HEIGHT	50

static void SetWidnowsTitle(char *title)
{
	printf("\x1B]0;%s\x1B\x5c", title);
}
static void SetNewBuffer(void)
{
	CONSOLE_SCREEN_BUFFER_INFO screen;
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	// new screen buffer
	printf("\x1b[?1049h");
	if (!GetConsoleScreenBufferInfo(console_handle, &screen))
	{
		fprintf(stderr, "SetNewBuffer: Unable to get information of buffer.\n" "Exited with GetLastError.\n");
		exit(GetLastError());
	}
}

int main(void)
{
	SetWidnowsTitle("Tetris!");
	SetNewBuffer();

	char c = getch();
	return 0;
}