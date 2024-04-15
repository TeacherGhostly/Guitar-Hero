/*koooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols, Jarrod Bennett, Cody Burnett
 * Modified by Ryan Chen
 */

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <string.h>

#define F_CPU 8000000UL
#define CHAR_WIDTH 3
#define CHAR_HEIGHT 5
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

bool game_paused = false;
bool manual_mode = false;
uint16_t game_speed;
uint8_t game_speed_tier = 1;
uint8_t trackNumber = 1;


/////////////////////////////// main //////////////////////////////////
int main(void)
{
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete.
	start_screen();
	
	// Loop forever and continuously play the game.
	while(1)
	{
		new_game();
		play_game();
		handle_game_over();
	}
}

static int one[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,0,0,0},
	{0,0,1,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0},
};
static int two[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,1,0,0,1,0,0},
	{0,0,0,0,1,1,0,0},
	{0,0,0,1,1,0,0,0},
	{0,0,1,1,1,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0},
};
static int three[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,1,1,1,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,0,0,1,1,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,0,0,1,1,0,0},
	{0,0,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0},
};

static int go[10][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,1,1,0,0,1,1,0},
	{1,0,0,0,1,0,0,1},
	{1,0,0,0,1,0,0,1},
	{1,0,0,0,1,0,0,1},
	{1,0,1,0,1,0,0,1},
	{0,1,1,0,0,1,1,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};



void print_game_speed(void) {
	switch(game_speed_tier) {
		case 1:
		printf("Current Game Speed: Normal\n");
		break;
		case 2:
		printf("Current Game Speed: Fast\n");
		break;
		case 3:
		printf("Current Game Speed: Extreme\n");
		break;
		default:
		printf("Error: Invalid Game Speed tier\n");
	}
}

void initialise_hardware(void)
{
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200, 0);
	
	init_timer0();
	init_timer1();
	init_timer2();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void)
{
	// Clear terminal screen and output a message
	clear_terminal();
	show_cursor();
	clear_terminal();
	hide_cursor();
	set_display_attribute(FG_WHITE);
	move_terminal_cursor(10,4);
	printf_P(PSTR("  ______   __     __  _______         __    __"));
	move_terminal_cursor(10,5);
	printf_P(PSTR(" /      \\ |  \\   |  \\|       \\       |  \\  |  \\"));
	move_terminal_cursor(10,6);
	printf_P(PSTR("|  $$$$$$\\| $$   | $$| $$$$$$$\\      | $$  | $$  ______    ______    ______"));
	move_terminal_cursor(10,7);
	printf_P(PSTR("| $$__| $$| $$   | $$| $$__| $$      | $$__| $$ /      \\  /      \\  /      \\"));
	move_terminal_cursor(10,8);
	printf_P(PSTR("| $$    $$ \\$$\\ /  $$| $$    $$      | $$    $$|  $$$$$$\\|  $$$$$$\\|  $$$$$$\\"));
	move_terminal_cursor(10,9);
	printf_P(PSTR("| $$$$$$$$  \\$$\\  $$ | $$$$$$$\\      | $$$$$$$$| $$    $$| $$   \\$$| $$  | $$"));
	move_terminal_cursor(10,10);
	printf_P(PSTR("| $$  | $$   \\$$ $$  | $$  | $$      | $$  | $$| $$$$$$$$| $$      | $$__/ $$"));
	move_terminal_cursor(10,11);
	printf_P(PSTR("| $$  | $$    \\$$$   | $$  | $$      | $$  | $$ \\$$     \\| $$       \\$$    $$"));
	move_terminal_cursor(10,12);
	printf_P(PSTR(" \\$$   \\$$     \\$     \\$$   \\$$       \\$$   \\$$  \\$$$$$$$ \\$$        \\$$$$$$"));
	move_terminal_cursor(10,14);
	// change this to your name and student number; remove the chevrons <>
	printf_P(PSTR("CSSE2010/7201 A2 by Ryan Chen - 48026383"));
	
	move_terminal_cursor(0,22); // move cursor to appropriate position
	clear_to_end_of_line(); // clear the line
	printf("Selected Track: Fire song\n");
	
	move_terminal_cursor(0,20);
	clear_to_end_of_line();
	print_game_speed();
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	show_start_screen();

	uint32_t last_screen_update, current_time;
	last_screen_update = get_current_time();
	
	uint8_t frame_number = 0;
	game_speed = 1000;

	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1)
	{
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
			
			if(serial_input == 't' || serial_input == 'T') {
				trackNumber = (trackNumber + 1); 
				if (trackNumber > 3) {
					trackNumber = 1;
				}
				// You may want to add code here to display the newly selected track
				track_change(trackNumber);
				if (trackNumber == 1) {
					move_terminal_cursor(0,22); // move cursor to appropriate position
					clear_to_end_of_line(); // clear the line
					printf("Selected Track: Fire song\n");
				} else if (trackNumber == 2){
					move_terminal_cursor(0,22); // move cursor to appropriate position
					clear_to_end_of_line(); // clear the line
					printf("Selected Track: Ice song\n");
				} else if (trackNumber == 3){
				move_terminal_cursor(0,22); // move cursor to appropriate position
				clear_to_end_of_line(); // clear the line
				printf("Selected Track: Water song\n");
			}
			}
			
			  if(serial_input == '1') {
				  game_speed_tier = 1;
				  game_speed = 1000;
				  move_terminal_cursor(0,20);
				  clear_to_end_of_line();
				  print_game_speed();
			  }
			  else if(serial_input == '2') {
				  game_speed_tier = 2;
				  game_speed = 500;
				  move_terminal_cursor(0,20);
				  clear_to_end_of_line();
				  print_game_speed();
			  }
			  else if(serial_input == '3') {
				  game_speed_tier = 3;
				  game_speed = 250;
				  move_terminal_cursor(0,20);
				  clear_to_end_of_line();
				  print_game_speed();
			  }
			  
			if(serial_input == 'm' || serial_input == 'M') {
				manual_mode = !manual_mode; // toggle manual mode
				if(manual_mode) {
					move_terminal_cursor(0,17);
					clear_to_end_of_line();
					printf("Manual mode activated.\n");
					} else {
					move_terminal_cursor(0,17);
					clear_to_end_of_line();	
				}
			}
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S')
		{
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED)
		{
			break;
		}

		// every 200 ms, update the animation
		current_time = get_current_time();
		if (current_time - last_screen_update > game_speed/5)
		{
			update_start_screen(frame_number);
			frame_number = (frame_number + 1) % 32;
			last_screen_update = current_time;
		}
	}
}

void game_countdown(uint16_t game_speed) {
	// Arrays for the numbers are already defined
	int (*numbers[4])[10][8] = {&three, &two, &one, &go};
	for (int num = 0; num < 4; num++) {
		uint32_t last_time = get_current_time();
		uint32_t current_time = get_current_time();
		ledmatrix_clear();
		while (current_time < last_time + game_speed * 2) {
			// Draw a number
			for(int i=0; i<10; i++){
				for(int j=0; j<8; j++){
					if((*numbers[num])[i][j] == 1){
						ledmatrix_update_pixel(i, j, COLOUR_YELLOW);
					}
				}
			}
			current_time = get_current_time();
		}
	}
	// Clear the LED matrix after the countdown
	ledmatrix_clear();
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	// Countdown before the game starts
	game_countdown(game_speed);
	
	// Initialise the game and display
	initialise_game();
	
	move_terminal_cursor(0,20);
	print_game_speed();

	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}


void play_game(void)
{
	
	uint32_t last_advance_time, current_time;
	int8_t btn; // The button pushed
	DDRA = (3);
	
	last_advance_time = get_current_time();
	
	// We play the game until it's over
	while (!is_game_over())
	{	
		if (combo_count == 1) {
			PORTA |= (0b00000100);
		} 
		else if  (combo_count == 2) {
			PORTA |= (0b00001100);
		}
		else if  (combo_count >= 3) {
			PORTA |= (0b00011100);
		}
		else if  (combo_count == 0) {
			PORTA &=  ~(0b00011100);
		}
		display_seven_segment();
		// Check for terminal input (serial input)
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
			 if(serial_input == 'p' || serial_input == 'P') {
				 game_paused = !game_paused; // toggle pause state
				 if(game_paused) {
					 move_terminal_cursor(0,16);
					 printf("Game Paused\n");
					 PORTA |= (0b00000010);
					 } else {
					 move_terminal_cursor(0,16);
					 clear_to_end_of_line(); // This will clear the current line in the terminal
					 PORTA &= ~(0b00000010);
					 }
				 }
			if(serial_input == 'm' || serial_input == 'M') {
				manual_mode = !manual_mode; // toggle manual mode
				if(manual_mode) {
					move_terminal_cursor(0,17);
					clear_to_end_of_line();
					printf("Manual mode activated.\n");
					} else {
					move_terminal_cursor(0,17);
					clear_to_end_of_line();
				}
			}
			else if(manual_mode && (serial_input == 'n' || serial_input == 'N')) {
				advance_note(); // manually advance note in manual mode
			}
		}
		if(game_paused) {
		   continue;
		}
		   	
		// Check for terminal input and play notes accordingly
		if (serial_input == 'a' || serial_input == 'A')
		{	
			play_note(3); // Leftmost lane
		}
		else if (serial_input == 's' || serial_input == 'S')
		{
			play_note(2); // Next lane
		}
		else if (serial_input == 'd' || serial_input == 'D')
		{
			play_note(1); // Next lane
		}
		else if (serial_input == 'f' || serial_input == 'F')
		{
			play_note(0); // Rightmost lane
		}
		
		btn = button_pushed();
		
		if (btn == BUTTON0_PUSHED)
		{
			// If button 0 play the lowest note (right lane)
			play_note(0);
		}
		if (btn == BUTTON1_PUSHED)
		{
			// If button 0 play the lowest note (right lane)
			play_note(1);
		}
		
		if (btn == BUTTON2_PUSHED)
		{
			// If button 0 play the lowest note (right lane)
			play_note(2);
		}
		
		if (btn == BUTTON3_PUSHED)
		{
			// If button 0 play the lowest note (right lane)
			play_note(3);
		}
		
		current_time = get_current_time();
		if (!manual_mode && current_time >= last_advance_time + game_speed/5)
		{
			// 200ms (0.2 second) has passed since the last time we advance the
			// notes here, so update the advance the notes
			advance_note();
			
			// Update the most recent time the notes were advance
			last_advance_time = current_time;
		}
	}
	// We get here if the game is over.
}

void handle_game_over()
{
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button or 's'/'S' to start a new game"));
	
	// Do nothing until a button is pushed. Hint: 's'/'S' should also start a
	// new game
	while (1)
	{
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
			if(serial_input == 's' || serial_input == 'S') {
				break;
			}
		}

		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED)
		{
			break;
		}
	}

	// Return to the splash screen
	start_screen();
}
