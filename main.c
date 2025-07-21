#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <Windows.h>

#define SCREEN_WIDTH	50
#define	SCREEN_HEIGHT	26

char screen_buff[SCREEN_WIDTH][SCREEN_HEIGHT];

static void SetWindowsTitle(char *title)
{
	printf("\x1B]0;%s\x1B\x5c", title);
}
static COORD SetNewBuffer(void)
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

	// moves the curser to (0,0)
	printf("\x1b[0;0f");
	return screen.dwSize;
}

static int SetInitialScreen(void)
{
	COORD dimensions = SetNewBuffer();
	int pad = (dimensions.X / 2) - (SCREEN_WIDTH / 2);
	
	// hide cursor
	printf("\x1b[?25l");

	// entering into drawing mode
	printf("\x1b(0");

	// prints upper part of box
	printf("\x1b[%dC", pad - 1);
	printf("\x6c");
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		printf("\x71");
	}
	printf("\x6b");
	printf("\n");

	//prints middle part of box
	for(int j = 0; j < SCREEN_HEIGHT; j++)
	{
		printf("\x1b[%dC", pad - 1);
		printf("\x78");
		for (int i = 0; i < SCREEN_WIDTH; i++)
		{
			printf(" ");
		}
		printf("\x78");
		printf("\n");
	}

	//prints the lower part of the box
	printf("\x1b[%dC", pad - 1);
	printf("\x6d");
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		printf("\x71");
	}
	printf("\x6a");

	// return to ascii mode
	printf("\x1b(B");

	return pad;
}

int main(void)
{
	SetWindowsTitle("Tetris!");
	SetInitialScreen();
	char c = getch();

	// restores the main buffer
	printf("\x1b[?1049l");
	// show cursor
	printf("\x1b[?25l");

	return 0;
}