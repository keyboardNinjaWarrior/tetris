#include <stdio.h>
#include <conio.h>
#include <Windows.h>

#define SCREEN_WIDTH	30
#define	SCREEN_HEIGHT	100

#define SCREEN_ERR		100

static void SetWidnowsTitle(char *title)
{
	printf("\x1B]0;%s\x1B\x5c", title);
}
static void SetNewBuffer(void)
{
	CONSOLE_SCREEN_BUFFER_INFO screen;
	
	// new screen buffer
	printf("\x1b[?1049h");
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &screen))
	{
		// checks if the buffer dimensions are large enough
		if (!(screen.dwSize.X < SCREEN_WIDTH | screen.dwSize.Y < SCREEN_HEIGHT))
		{
			fprintf(stderr, "SetNewBuffer: Screen is too small.\n");
			exit(SCREEN_ERR);
		}
	}
	else
	{
		fprintf(stderr, "SetNewBuffer: Unable to get information of buffer.\n" "Exited with GetLastError.\n");
		exit(GetLastError());
	}
}

int main(void)
{
	SetWidnowsTitle("Tetris!");
	SetNewBuffer();

	printf(" _____    _        _     \n");
	printf("|_   _|__| |_ _ __(_)___ \n");
	printf("  | |/ _ \\ __| '__| / __|\n");
	printf("  | |  __/ |_| |  | \\__ \\\n");
	printf("  |_|\\___|\\__|_|  |_|___/\n");

	char c = getch();
	return 0;
}