#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <Windows.h>

#define SCREEN_WIDTH	50
#define	SCREEN_HEIGHT	26

#define LARGE_STRING_ERR	1

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

static COORD SetInitialScreen(void)
{
	COORD padding = SetNewBuffer();
	padding.X = (padding.X / 2) - (SCREEN_WIDTH / 2);
	padding.Y = (padding.Y / 2) - (SCREEN_HEIGHT / 2);

	// hide cursor
	printf("\x1b[?25l");

	// entering into drawing mode
	printf("\x1b(0");
	
	//move cursor
	printf("\x1b[%dC", padding.X - 1);
	printf("\x1b[%dB", padding.Y - 1);

	// prints upper part of box
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
		printf("\x1b[%dC", padding.X - 1);
		printf("\x78");
		for (int i = 0; i < SCREEN_WIDTH; i++)
		{
			printf(" ");
		}
		printf("\x78");
		printf("\n");
	}

	//prints the lower part of the box
	printf("\x1b[%dC", padding.X - 1);
	printf("\x6d");
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		printf("\x71");
	}
	printf("\x6a");

	// return to ascii mode
	printf("\x1b(B");

	return padding;
}

// makes the screen buffer empty
void SetEmptyBuffer(void)
{
	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		for (int j = 0; i < SCREEN_WIDTH; j++)
		{
			screen_buff[j][i] = ' ';
		}
	}
}

int CountStringLen(char* string)
{
	int i = 0;
	for (; i < SCREEN_WIDTH | string[i] != '\0'; i++)
		;
	return i;
}

void WriteOnBuffer(char string[SCREEN_WIDTH], COORD position)
{
	int string_len = CountStringLen(string);
	// validating the size of string
	if (string_len > (SCREEN_WIDTH - position.X - 1) | position.Y > (SCREEN_WIDTH - 1))
	{
		fprintf(stderr, "WriteOnBuffer: The string is too large.");
		exit(LARGE_STRING_ERR);
	}
}

int main(void)
{
	SetWindowsTitle("Tetris!");
	COORD padding = SetInitialScreen();
	SetEmptyBuffer();

	char c = getch();

	// restores the main buffer
	printf("\x1b[?1049l");
	// show cursor
	printf("\x1b[?25l");

	return 0;
}