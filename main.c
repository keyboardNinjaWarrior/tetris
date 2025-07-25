#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

#define SCREEN_WIDTH	51
#define	SCREEN_HEIGHT	22
#define	GAME_WIDTH		20
#define GAME_HEIGHT		30
#define TET_HEIGHT		2
#define TET_WIDTH		8

#define LARGE_STRING_ERR	1

char screen_buff[SCREEN_HEIGHT][SCREEN_WIDTH];
char game_screen_buff[GAME_HEIGHT][GAME_WIDTH];
char tetromino[TET_HEIGHT][TET_WIDTH];
char next_tetromino[TET_HEIGHT][TET_WIDTH];
COORD screen_padding;

enum {I, J, L, O, S, T, Z, EMPTY};

struct _tetromino_properties{
	int tetromino_type;
	COORD dimensions;
	COORD index;
} tetromino_properties, next_termino_property;

static void GetAnyInput(void)
{
	char c = _getch();
}

static void SetWindowsTitle(char *title)
{
	printf("\x1B]0;%s\x1B\x5c", title);
}

static void SetNewScreenBuffer(void)
{
	// new screen buffer
	printf("\x1b[?1049h");
	// moves the curser to (0,0)
	printf("\x1b[0;0f");
}

static COORD GetConsoleDimensions(void)
{
	CONSOLE_SCREEN_BUFFER_INFO screen;
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	// gets console properties and stores them in screen
	if (!GetConsoleScreenBufferInfo(console_handle, &screen))
	{
		fprintf(stderr, "\nSetNewBuffer: Unable to get information of buffer.\n" "Exited with GetLastError.\n");
		exit(GetLastError());
	}
	return screen.dwSize;
}

static void SetInitialScreen(void)
{
	screen_padding = GetConsoleDimensions();
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
static void SetEmptyBuffer(void)
{
	for (int i = 0; i < SCREEN_HEIGHT; i++)
	{
		for (int j = 0; j < SCREEN_WIDTH; j++)
		{
			screen_buff[i][j] = ' ';
		}
	}
}

static int CountStringLen(char * string)
{
	int i = 0;
	for (; i < SCREEN_WIDTH && string[i] != '\0'; i++)
		;
	return i;
}

static void WriteOnScreenBuffer(char string[SCREEN_WIDTH], int x, int y)
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

static void FlushScreenBuffer(void)
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

static void SetGameScreen(void)
{
	// the borders of the game
	for (int i = 0; i < SCREEN_HEIGHT - 1; i++)
	{
		WriteOnScreenBuffer("<", 0, i);
		WriteOnScreenBuffer("!", 1, i);
	}

	for (int i = 0; i < SCREEN_HEIGHT - 1; i++)
	{
		WriteOnScreenBuffer("!", GAME_WIDTH + 2, i);
		WriteOnScreenBuffer(">", GAME_WIDTH + 3, i);
	}

	for (int i = 2; (i - 2) < GAME_WIDTH; i++)
	{
		WriteOnScreenBuffer("=", i, SCREEN_HEIGHT - 2);
	}

	for (int i = 2; (i - 2) < GAME_WIDTH; i = i + 2)
	{
		WriteOnScreenBuffer("\\", i, SCREEN_HEIGHT - 1);
	}

	for (int i = 3; (i - 3) < GAME_WIDTH; i = i + 2)
	{
		WriteOnScreenBuffer("/", i, SCREEN_HEIGHT - 1);
	}

	FlushScreenBuffer();

	// the dividing line
	printf("\x1b(0");
	
	printf("\x1b[%d;%df", screen_padding.Y, screen_padding.X + GAME_WIDTH + 5);
	printf("\x77");
	for (int i = 1; i <= GAME_HEIGHT - 8; i++)
	{
		printf("\x1b[%d;%df", screen_padding.Y + i, screen_padding.X + GAME_WIDTH + 5);
		printf("\x78");
	}
	printf("\x1b[%d;%df", screen_padding.Y + GAME_HEIGHT - 7, screen_padding.X + GAME_WIDTH + 5);
	printf("\x76");

	// the score heading
	for (int i = 1; i <= 2; i++)
	{
		for (int j = GAME_WIDTH + 6; j <= SCREEN_WIDTH; j++)
		{
			printf("\x1b[%d;%df", screen_padding.Y + i, screen_padding.X + j);
			printf("\x61");
		}
	}

	printf("\x1b(B");
	printf("\x1b[%d;%df", screen_padding.Y + 3, screen_padding.X + GAME_WIDTH + 6);
	printf("Score:");
	printf("\x1b(0");

	for (int i = GAME_WIDTH + 6; i <= SCREEN_WIDTH; i++)
	{
		printf("\x1b[%d;%df", screen_padding.Y + 4, screen_padding.X + i);
		printf("\x61");
	}

	for (int i = 6; i <= 7; i++)
	{
		for (int j = GAME_WIDTH + 6; j <= SCREEN_WIDTH; j++)
		{
			printf("\x1b[%d;%df", screen_padding.Y + i, screen_padding.X + j);
			printf("\x61");
		}
	}

	// next block division
	printf("\x1b[%d;%df", screen_padding.Y + 8, screen_padding.X + GAME_WIDTH + 5);
	printf("\x74");
	for (int i = GAME_WIDTH + 6; i < SCREEN_WIDTH + 1; i++)
	{
		printf("\x1b[%d;%df", screen_padding.Y + 8, screen_padding.X + i);
		printf("\x71");
	}
	printf("\x1b[%d;%df", screen_padding.Y + 8, screen_padding.X + SCREEN_WIDTH + 1);
	printf("\x75");

	for (int i = GAME_WIDTH + 6; i <= SCREEN_WIDTH; i++)
	{
		printf("\x1b[%d;%df", screen_padding.Y + 9, screen_padding.X + i);
		printf("\x61");
	}


	for (int i = GAME_WIDTH + 6; i <= SCREEN_WIDTH; i++)
	{
		printf("\x1b[%d;%df", screen_padding.Y + 9, screen_padding.X + i);
		printf("\x61");
	}
	
	printf("\x1b[%d;%df", screen_padding.Y + 10, screen_padding.X + GAME_WIDTH + 6);
	printf("\x1b(B");
	printf("Next Block:");
	printf("\x1b(0");

	for (int i = GAME_WIDTH + 6; i <= SCREEN_WIDTH; i++)
	{
		printf("\x1b[%d;%df", screen_padding.Y + 11, screen_padding.X + i);
		printf("\x61");
	}

	printf("\x1b(B");
}

static void SetTetrominoI(char *p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	for (int i = 0; i < 8; i = i + 2)
	{
		*(p_tetromino + (0 * 8) + i) = '[';
	}

	for (int i = 1; i < 8; i = i + 2)
	{
		*(p_tetromino + (0 * 8) + i) = ']';
	}

	p_tetromino_properties->tetromino_type = I;
	p_tetromino_properties->dimensions.X = 4;
	p_tetromino_properties->dimensions.Y = 1;

}

static void SetTetrominoT(char *p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	*(p_tetromino + (1 * 8) + 0) = '[';
	*(p_tetromino + (1 * 8) + 1) = ']';

	*(p_tetromino + (1 * 8) + 2) = '[';
	*(p_tetromino + (1 * 8) + 3) = ']';

	*(p_tetromino + (1 * 8) + 4) = '[';
	*(p_tetromino + (1 * 8) + 5) = ']';

	*(p_tetromino + (0 * 8) + 2) = '[';
	*(p_tetromino + (0 * 8) + 3) = ']';

	p_tetromino_properties->tetromino_type = T;
	p_tetromino_properties->dimensions.X = 3;
	p_tetromino_properties->dimensions.Y = 2;
}

static void SetTetrominoL(char *p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	*(p_tetromino + (1 * 8) + 0) = '[';
	*(p_tetromino + (1 * 8) + 1) = ']';

	*(p_tetromino + (1 * 8) + 2) = '[';
	*(p_tetromino + (1 * 8) + 3) = ']';

	*(p_tetromino + (1 * 8) + 4) = '[';
	*(p_tetromino + (1 * 8) + 5) = ']';

	*(p_tetromino + (0 * 8) + 4) = '[';
	*(p_tetromino + (0 * 8) + 5) = ']';

	p_tetromino_properties->tetromino_type = L;
	p_tetromino_properties->dimensions.X = 3;
	p_tetromino_properties->dimensions.Y = 2;
}

static void SetTetrominoJ(char *p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	*(p_tetromino + (1 * 8) + 0) = '[';
	*(p_tetromino + (1 * 8) + 1) = ']';

	*(p_tetromino + (1 * 8) + 2) = '[';
	*(p_tetromino + (1 * 8) + 3) = ']';

	*(p_tetromino + (1 * 8) + 4) = '[';
	*(p_tetromino + (1 * 8) + 5) = ']';

	*(p_tetromino + (0 * 8) + 0) = '[';
	*(p_tetromino + (0 * 8) + 1) = ']';

	p_tetromino_properties->tetromino_type = J;
	p_tetromino_properties->dimensions.X = 3;
	p_tetromino_properties->dimensions.Y = 2;
}

static void SetTetrominoZ(char *p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	*(p_tetromino + (0 * 8) + 0) = '[';
	*(p_tetromino + (0 * 8) + 1) = ']';

	*(p_tetromino + (0 * 8) + 2) = '[';
	*(p_tetromino + (0 * 8) + 3) = ']';

	*(p_tetromino + (1 * 8) + 2) = '[';
	*(p_tetromino + (1 * 8) + 3) = ']';

	*(p_tetromino + (1 * 8) + 4) = '[';
	*(p_tetromino + (1 * 8) + 5) = ']';

	p_tetromino_properties->tetromino_type = Z;
	p_tetromino_properties->dimensions.X = 3;
	p_tetromino_properties->dimensions.Y = 2;
}

static void SetTetrominoS(char *p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	*(p_tetromino + (1 * 8) + 0) = '[';
	*(p_tetromino + (1 * 8) + 1) = ']';

	*(p_tetromino + (1 * 8) + 2) = '[';
	*(p_tetromino + (1 * 8) + 3) = ']';

	*(p_tetromino + (0 * 8) + 2) = '[';
	*(p_tetromino + (0 * 8) + 3) = ']';

	*(p_tetromino + (0 * 8) + 4) = '[';
	*(p_tetromino + (0 * 8) + 5) = ']';

	p_tetromino_properties->tetromino_type = S;
	p_tetromino_properties->dimensions.X = 3;
	p_tetromino_properties->dimensions.Y = 2;
}

static void SetTetrominoO(char *p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	*(p_tetromino + (0 * 8) + 0) = '[';
	*(p_tetromino + (0 * 8) + 1) = ']';

	*(p_tetromino + (0 * 8) + 2) = '[';
	*(p_tetromino + (0 * 8) + 3) = ']';

	*(p_tetromino + (1 * 8) + 0) = '[';
	*(p_tetromino + (1 * 8) + 1) = ']';

	*(p_tetromino + (1 * 8) + 2) = '[';
	*(p_tetromino + (1 * 8) + 3) = ']';

	p_tetromino_properties->tetromino_type = O;
	p_tetromino_properties->dimensions.X = 2;
	p_tetromino_properties->dimensions.Y = 2;
}

static void SetTetrominoNull(char* p_tetromino, struct _tetromino_properties *p_tetromino_properties)
{
	for (int i = 0; i < TET_HEIGHT; i++)
	{
		for (int j = 0; j < TET_WIDTH; j++)
		{
			*(p_tetromino + (j * TET_WIDTH) + i) = ' ';
		}
	}

	p_tetromino_properties->tetromino_type = EMPTY;
}

static unsigned short int RandomIndex(void)
{
	srand(time(NULL));
	unsigned short int random_tetromino_index;

	do
	{
		random_tetromino_index = rand() % 10;
	} while (random_tetromino_index >= 6 && random_tetromino_index <= 0);

	return random_tetromino_index;
}

static void PrintTetromino(void)
{
	for (int i = 0; i < tetromino_properties.dimensions.Y; i++)
	{
		printf("\x1b[%d;%df", screen_padding.Y + tetromino_properties.index.Y + i + 1, screen_padding.X + tetromino_properties.index.X + 3);
		
		for (int j = 0; j < tetromino_properties.dimensions.X * 2; j++)
		{
			printf("%c", tetromino[i][j]);
		}

	}
}

static void Game(void)
{
	SetEmptyBuffer();
	SetGameScreen();

	void (*SetTetromino[7])(char *, struct _tetromino_properties *) =	{
																			&SetTetrominoI,	\
																			&SetTetrominoJ,	\
																			&SetTetrominoL,	\
																			&SetTetrominoO,	\
																			&SetTetrominoS,	\
																			&SetTetrominoT,	\
																			&SetTetrominoZ	\
																		};

	SetTetrominoNull(&tetromino[0][0], &tetromino_properties);
	SetTetromino[RandomIndex()](&tetromino[0][0], &tetromino_properties);
	tetromino_properties.index.X = 6;
	tetromino_properties.index.Y = 0;
	PrintTetromino();
	GetAnyInput();
}

int main(void)
{
	// meta
	SetNewScreenBuffer();
	SetWindowsTitle("Tetris!");
	SetInitialScreen();

	// start menu
	SetEmptyBuffer();
	WriteOnScreenBuffer(" _____    _        _     "		, 13, 4);
	WriteOnScreenBuffer("|_   _|__| |_ _ __(_)___ "		, 13, 5);
	WriteOnScreenBuffer("  | |/ _ \\ __| '__| / __|"	, 13, 6);
	WriteOnScreenBuffer("  | |  __/ |_| |  | \\__ \\"	, 13, 7);
	WriteOnScreenBuffer("  |_|\\___|\\__|_|  |_|___/"	, 13, 8);
	
	WriteOnScreenBuffer("Press any key to play", 15, SCREEN_HEIGHT - 1);
	FlushScreenBuffer();
	GetAnyInput();
	
	// the main game
	Game();

	// restores the main buffer
	printf("\x1b[?1049l");
	// show cursor
	printf("\x1b[?25l");

	return 0;
}