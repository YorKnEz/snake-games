#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define WINDOW_W 16
#define WINDOW_H 16

enum EndGame { RUNNING, OUT_OF_BOUNDS, OVERLAP, WIN };

EndGame game_state = RUNNING;
int fps = 1000.0 / 20;

struct Point {
	int x, y;
};

char** board;

int snake_length = 0;
Point *snake;

char c = 'w';

// allocates memory for the board and initializes its chars
void init_board();

// allocates memory for the snake and initializes its body on the board
void init_snake();

// prints the board on the screen
void print_board();

// calls the Sleep function using the fps parameter
void call_sleep();

// moves the snake in the (x, y) direction
// (x, y) = (0, -1), (1, 0), (0, 1), (-1, 0)
void move_snake(int x, int y);

// gets the currently pressed key from the keyboard and does that move, otherwise keeps the old one
void handle_input();

// generates a fruit randomly on the board
void generate_fruit();

int main() {
	const char* cls = "cls";

	const char* out_of_bounds = "Your snake got outside of the board!\n";
	const char* overlap = "Your snake ate itself!\n";
	const char* win = "You won! Damn!\n";

	_asm {
		pushad
		call init_board
		popad

		pushad
		call init_snake
		popad

		pushad
		call generate_fruit
		popad

	game_loop:
		cmp game_state, RUNNING
		jnz game_loop_end

		pushad
		push cls
		call system
		add esp, 4
		popad

		pushad
		call handle_input
		popad

		pushad
		call print_board
		popad

		pushad
		call call_sleep
		popad

		jmp game_loop
	game_loop_end:

	// determine in which state the game ended and print a message accordingly
	switch_start:
		pushad
		cmp game_state, OUT_OF_BOUNDS
		je case_out_of_bounds
		cmp game_state, OVERLAP
		je case_overlap
		cmp game_state, WIN
		je case_win

		jmp switch_end

	case_out_of_bounds:
		push out_of_bounds
		call printf
		add esp, 4

		jmp switch_end
	case_overlap:
		push overlap
		call printf
		add esp, 4

		jmp switch_end
	case_win:
		push win
		call printf
		add esp, 4

		jmp switch_end
	switch_end:
		popad
	}

	return 0;
}

void init_board() {
	// allocate memory for the board
	board = (char**)malloc(WINDOW_H * sizeof(char*));
	
	for (int i = 0; i < WINDOW_H; i++) {
		board[i] = (char*)malloc((WINDOW_W + 1) * sizeof(char));
		board[i][WINDOW_W] = '\0';
	}

	// go through the matrix and put ' '
	_asm {
		mov esi, board

		xor ebx, ebx // ebx = 0

	first_loop:
		cmp ebx, WINDOW_H
		jge first_loop_end

		mov edi, [esi + 4 * ebx] // edi = board[i]

		xor ecx, ecx // ecx = 0

	second_loop:
		cmp ecx, WINDOW_W
		jge second_loop_end

		mov byte ptr [edi + ecx], ' ' // board[i][j] = ' '

		inc ecx
		jmp second_loop
	second_loop_end:

		inc ebx
		jmp first_loop
	first_loop_end:
	}
}

void init_snake() {
	// allocate memory for the snake
	snake = (Point*)malloc((WINDOW_W * WINDOW_H + 1) * sizeof(Point));

	Point mid;

	int p_size = sizeof(Point);
	snake_length = 4;

	_asm {
		mov dword ptr mid, WINDOW_W
		shr dword ptr mid, 1				// mid.x = WINDOW_W / 2
		mov dword ptr mid + 4, WINDOW_H
		shr dword ptr mid + 4, 1			// mid.y = WINDOW_H / 2

		mov edi, board
		mov esi, snake

		// snake head coords
		mov ebx, dword ptr mid
		mov ecx, dword ptr mid + 4 

		xor edx, edx // edx = 0

	first_loop:
		cmp edx, snake_length
		jge first_loop_end

		mov eax, [edi + 4 * ecx]		// eax = board[i]
		mov byte ptr [eax + ebx], 's'	// board[i][j] = 's'

		mov [esi + 8 * edx], ebx		// snake[i].x = ebx
		mov [esi + 8 * edx + 4], ecx	// snake[i].y = ecx

		inc ecx
		inc edx
		jmp first_loop
	first_loop_end:
	}

	for (int i = 0; i < 4; i++) {
		printf("%d %d\n", snake[i].x, snake[i].y);
	}
}

void print_board() {
	const char* score = "Score: %d\n";
	const char* format = "%c";
	const char* newline = "\n";
	const char* border_char = "#";

	_asm {
		mov esi, board

		pushad
		push snake_length
		push score
		call printf
		add esp, 8
		popad

		// draw the upper border
		xor ebx, ebx

		mov ecx, WINDOW_W
		add ecx, 2
		
	upper_border_start:
		cmp ebx, ecx
		jge upper_border_end

		pushad
		push border_char
		call printf
		add esp, 4
		popad

		inc ebx
		jmp upper_border_start
	upper_border_end:

		pushad
		push newline
		call printf
		add esp, 4
		popad

		xor ebx, ebx

	first_loop:
		cmp ebx, WINDOW_H
		jge first_loop_end

		// print the left border
		pushad
		push border_char
		call printf
		add esp, 4
		popad

		mov edi, [esi + 4 * ebx] // edi = board[i]
		xor ecx, ecx

	second_loop:
		cmp ecx, WINDOW_W
		jge second_loop_end

		pushad
		push dword ptr [edi + ecx]
		push format
		call printf
		add esp, 8
		popad

		inc ecx
		jmp second_loop
	second_loop_end:

		// print the right border
		pushad
		push border_char
		call printf
		add esp, 4
		popad

		pushad
		push newline
		call printf
		add esp, 4
		popad

		inc ebx
		jmp first_loop
	first_loop_end:

		// print the bottom border
		xor ebx, ebx
		mov ecx, WINDOW_W
		add ecx, 2
		
	bottom_border_start:
		cmp ebx, ecx
		jge bottom_border_end

		pushad
		push border_char
		call printf
		add esp, 4
		popad

		inc ebx
		jmp bottom_border_start
	bottom_border_end:

		pushad
		push newline
		call printf
		add esp, 4
		popad
	}
}

void call_sleep() {
	Sleep(fps);
}

bool is_valid_move(int x, int y) {
	_asm {
		// check if 0 <= x < WINDOW_W
		cmp [ebp + 8], 0
		jl out_of_bounds
		cmp [ebp + 8], WINDOW_W
		jge out_of_bounds

		// check if 0 <= y < WINDOW_H
		cmp [ebp + 12], 0
		jl out_of_bounds
		cmp [ebp + 12], WINDOW_H
		jge out_of_bounds

		mov edi, board					// edi = board

		mov eax, [ebp + 12]				// eax = y
		mov edi, [edi + 4 * eax]		// edi = board[i]

		mov eax, [ebp + 8]				// eax = x
		cmp byte ptr [edi + eax], 'o'	// compares board[i][j] with 's' to check for overlapping
		je overlap

		jmp check_end

	// end the game if needed
	out_of_bounds:
		mov game_state, OUT_OF_BOUNDS
		jmp check_end
	overlap:
		mov game_state, OVERLAP

	check_end:
	}
}

void move_snake(int x, int y) {
	_asm {
		mov esi, snake
		mov edi, board

		// check if current move is valid
		pushad
		mov eax, [esi + 4]	// eax = snake[0].y
		add eax, y			// eax += y
		push eax

		mov eax, [esi]		// eax = snake[0].x
		add eax, x			// eax += x
		push eax

		call is_valid_move
		add esp, 8
		popad

		// end game if user made an invalid move
		cmp game_state, RUNNING
		jne move_end

		// get index of snake tail
		mov eax, snake_length	// eax = snake_length
		dec eax					// eax--
		mov ecx, 8				// ecx = 8
		mul ecx					// eax *= 8
		mov ebx, eax			// ebx = eax

		// remove the tail from the board
		mov ecx, [esi + ebx + 4]		// ecx = snake[snake_length - 1].y
		mov edi, [edi + 4 * ecx]		// edi = board[ecx]

		mov ecx, [esi + ebx]			// ecx = snake[snake_length - 1].x
		mov byte ptr [edi + ecx], ' '	// edi = board[snake[snake_lenght - 1].y][snake[snake_lenght - 1].x]

		// loop through snake and update it's coords
		mov ebx, snake_length

	first_loop:
		cmp ebx, 0
		jle first_loop_end

		mov ecx, [esi + 8 * ebx - 8]	// ecx = snake[ebx - 1].y
		mov [esi + 8 * ebx], ecx		// snake[ebx].y = ecx

		mov ecx, [esi + 8 * ebx - 4]	// ecx = snake[ebx - 1].x
		mov [esi + 8 * ebx + 4], ecx	// snake[ebx].x = ecx

		dec ebx
		jmp first_loop
	first_loop_end:

		// update head
		mov ebx, x					// ebx = x
		add [esi], ebx				// snake[0].x += ebx

		mov ebx, y					// ebx = y
		add [esi + 4], ebx			// snake[0].y += ebx
		
		// update board
		mov edi, board				// edi = board
		mov ebx, [esi + 4]			// ebx = snake[0].y
		mov edi, [edi + 4 * ebx]	// edi = board[ebx]
		mov ebx, [esi]				// ebx = snake[0].x

		// cmp board[snake[0].y][snake[0].x] with '@'
		cmp byte ptr [edi + ebx], '@'
		jne eat_fruit_end

	eat_fruit:
		// get index of the new node to add
		mov eax, snake_length
		mov ebx, 8
		mul ebx
		mov ebx, eax

		mov edi, board					// edi = board

		mov eax, [esi + ebx + 4]		// eax = snake[0].y
		mov edi, [edi + 4 * eax]		// edi = board[eax]

		mov eax, [esi + ebx]			// eax = snake[0].x
		mov byte ptr [edi + eax], 'o'	// board[snake[0].y][snake[0].x] = 's'

		inc snake_length				// snake_length++

		mov eax, WINDOW_W
		mov edx, WINDOW_H
		mul edx							// eax = WINDOW_W * WINDOW_H

		cmp snake_length, eax			// check if the player won
		jne game_continues

		mov game_state, WIN				// end game
		jmp move_end
	game_continues:

		pushad
		call generate_fruit				// generate a new fruit
		popad
	eat_fruit_end:
		// redraw the old head
		mov edi, board					// edi = board

		mov ebx, [esi + 12]				// ebx = snake[0].y
		mov edi, [edi + 4 * ebx]		// edi = board[ebx]

		mov ebx, [esi + 8]				// ebx = snake[0].x
		mov byte ptr [edi + ebx], 'o'


		// draw the new head
		mov edi, board					// edi = board

		mov ebx, [esi + 4]				// ebx = snake[0].y
		mov edi, [edi + 4 * ebx]		// edi = board[ebx]

		mov ebx, [esi]					// ebx = snake[0].x
		mov byte ptr [edi + ebx], 'O'
	move_end:
	}
}

void handle_input() {
	const char* format = "%c";

	if (_kbhit()) {
		char new_c = _getch();

		_asm {
		dir_switch_start:
			cmp new_c, 'w'
			je dir_w
			cmp new_c, 'a'
			je dir_a
			cmp new_c, 's'
			je dir_s
			cmp new_c, 'd'
			je dir_d

		dir_w:
			cmp c, 's'
			je dir_switch_end
			jmp update_c
		dir_a:
			cmp c, 'd'
			je dir_switch_end
			jmp update_c
		dir_s:
			cmp c, 'w'
			je dir_switch_end
			jmp update_c
		dir_d:
			cmp c, 'a'
			je dir_switch_end
			jmp update_c

		update_c:
			mov al, new_c
			mov c, al

		dir_switch_end:
		}
	}

	_asm {
		pushad
	switch_start:
		cmp c, 'w'
		je case_w
		cmp c, 'a'
		je case_a
		cmp c, 's'
		je case_s
		cmp c, 'd'
		je case_d

		jmp switch_end

	case_w:
		push dword ptr -1
		push dword ptr 0
		call move_snake
		add esp, 8

		jmp switch_end

	case_a:
		push dword ptr 0
		push dword ptr -1
		call move_snake
		add esp, 8

		jmp switch_end

	case_s:
		push dword ptr 1
		push dword ptr 0
		call move_snake
		add esp, 8

		jmp switch_end

	case_d:
		push dword ptr 0
		push dword ptr 1
		call move_snake
		add esp, 8

		jmp switch_end

	switch_end:
		popad
	}

}

void generate_fruit() {
	srand(time(NULL));

	Point p;

	_asm {
		lea esi, p
		mov edi, board

	generate_start:
		call rand
		xor edx, edx					// edx = 0
		mov ebx, WINDOW_W				// ebx = WINDOW_W
		div ebx							// eax = rand() / WINDOW_W, edx = rand() % WINDOW_W

		mov [esi], edx					// p.x = rand() % WINDOW_W

		call rand
		xor edx, edx					// edx = 0
		mov ebx, WINDOW_W				// ebx = WINDOW_W
		div ebx							// eax = rand() / WINDOW_W, edx = rand() % WINDOW_W

		mov [esi + 4], edx				// p.x = rand() % WINDOW_W

		mov eax, [esi + 4]				// eax = p.y
		mov edx, [edi + 4 * eax]		// edi = board[p.y]

		mov eax, [esi]					// eax = p.x
		mov bl, [edx + eax]				// bl = board[p.y][p.x]

		cmp bl, ' '						// cmp al with ' '
		jne generate_start				// if there was no space to place the fruit, retry generating

	 	mov byte ptr [edx + eax],  '@'	// board[p.y][p.x] = '@'
	generate_end:
	}
}
