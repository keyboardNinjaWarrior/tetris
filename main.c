#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <Windows.h>

#define SCREEN_WIDTH	50
#define	SCREEN_HEIGHT	28

#define LARGE_STRING_ERR	1

char screen_buff[SCREEN_HEIGHT][SCREEN_WIDTH];
COORD screen_padding;

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
		fprintf(stderr, "\nSetNewBuffer: Unable to get information of buffer.\n" "Exited with GetLastError.\n");
		exit(GetLastError());
	}

	// moves the curser to (0,0)
	printf("\x1b[0;0f");
	return screen.dwSize;
}

static void SetInitialScreen(void)
{
	screen_padding = SetNewBuffer();
	screen_padding.X = (screen_padding.X / 2) - (SCREEN_WIDTH / 2);
	screen_padding.Y = (screen_padding.Y / 2) - (SCREEN_HEIGHT / 2);

	// hide cursor
	printf("\x1b[?25l");

	// entering into drawing mode
	printf("\x1b(0");
	
	//move cursor
	printf("\x1b[%dC", screen_padding.X - 1);
	printf("\x1b[%dB", screen_padding.Y - 1);

	// prints upper part of box
	printf("\x6c");
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		printf("\x71");
	}
	printf("\x6b");
	printf("\n");

	//prints middle part of box
	for(int i = 0; i < SCREEN_HEIGHT; i++)
	{
		printf("\x1b[%dC", screen_padding.X - 1);
		printf("\x78");
		for (int j = 0; j < SCREEN_WIDTH; j++)
		{
			printf(" ");
		}
		printf("\x78");
		printf("\n");
	}

	//prints the lower part of the box
	printf("\x1b[%dC", screen_padding.X - 1);
	printf("\x6d");
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		printf("\x71");
	}
	printf("\x6a");

	// return to ascii mode
	printf("\x1b(B");
}

// makes the screen buffer empty
void SetEmptyBuffer(void)
{
	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH; j++)
		{
			screen_buff[i][j] = ' ';
		}
	}
}

int CountStringLen(char* string)
{
	int i = 0;
	for (; i < SCREEN_WIDTH && string[i] != '\0'; i++)
		;
	return i;
}

void WriteOnBuffer(char string[SCREEN_WIDTH], int x, int y)
{
	int string_len = CountStringLen(string);
	// validating the size of string
	if (string_len > (SCREEN_WIDTH - x) || y >= SCREEN_HEIGHT)
	{
		fprintf(stderr, "\nWriteOnBuffer: The string doesn't fit or either is too large.\n");
		exit(LARGE_STRING_ERR);
	}

	for (int i = x; (i - x) < string_len; i++)
	{
		screen_buff[y][i] = string[i - x];
	}

}

void FlushScreenBuffer(void)
{
	// move cursor to initial position
	printf("\x1b[%d;%df", screen_padding.Y + 1, screen_padding.X + 1);

	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH; j++)
		{
			printf("%c", screen_buff[i][j]);
		}
		printf("\n");
		printf("\x1b[%dC", screen_padding.X);
	}
}

int main(void)
{
	// meta
	SetWindowsTitle("Tetris!");
	SetInitialScreen();
	SetEmptyBuffer();

	// start menu
	WriteOnBuffer(" _____    _        _     "	, 12, 11);
	WriteOnBuffer("|_   _|__| |_ _ __(_)___ "	, 12, 12);
	WriteOnBuffer("  | |/ _ \\ __| '__| / __|"	, 12, 13);
	WriteOnBuffer("  | |  __/ |_| |  | \\__ \\"	, 12, 14);
	WriteOnBuffer("  |_|\\___|\\__|_|  |_|___/"	, 12, 15);

	WriteOnBuffer("Press any key to continue", 12, SCREEN_HEIGHT - 1);
	FlushScreenBuffer();
	char c = getch();

	// restores the main buffer
	printf("\x1b[?1049l");
	// show cursor
	printf("\x1b[?25l");

	return 0;
}