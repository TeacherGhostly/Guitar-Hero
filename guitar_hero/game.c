/*
 * game.c
 *
 * Functionality related to the game state and features.
 *
 * Author: Jarrod Bennett, Cody Burnett
 */ 

#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "display.h"
#include "ledmatrix.h"
#include "terminalio.h"
#define F_CPU 8000000UL	// 8MHz
#include <util/delay.h>
#include <stdint.h>
#include <avr/io.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

uint8_t track[TRACK_LENGTH] = {0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x80, 0x00, 0x00, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x12, 0x20, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x01, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00};

static const uint8_t track1[TRACK_LENGTH] = {0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x80, 0x00, 0x00, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x12, 0x20, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x01, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00};

static const uint8_t track2[TRACK_LENGTH] = {0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x20, 0x40, 0x00,
	0x02, 0x04, 0x00, 0x10, 0x20, 0x00, 0x80, 0x01,
	0x00, 0x04, 0x08, 0x10, 0x00, 0x40, 0x80, 0x00,
	0x02, 0x00, 0x08, 0x10, 0x20, 0x40, 0x00, 0x01,
	0x01, 0x00, 0x04, 0x08, 0x00, 0x20, 0x40, 0x00,
	0x02, 0x04, 0x00, 0x10, 0x20, 0x00, 0x80, 0x01,
	0x00, 0x04, 0x08, 0x10, 0x00, 0x40, 0x80, 0x00,
	0x02, 0x00, 0x08, 0x10, 0x20, 0x40, 0x00, 0x01,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00
};




static const uint8_t track3[TRACK_LENGTH] = {0x00,
	0x00, 0x02, 0x10, 0x02, 0x10, 0x02, 0x10, 0x02,
	0x02, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08, 0x04,
	0x04, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08,
	0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x08, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08,
	0x08, 0x04, 0x10, 0x04, 0x10, 0x04, 0x10, 0x04,
	0x04, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02,
	0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
0x02, 0x04, 0x00, 0x10, 0x20, 0x00, 0x80, 0x01,
0x00, 0x04, 0x08, 0x10, 0x00, 0x40, 0x80, 0x00,
0x02, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00};


uint16_t beat;
uint8_t note_index; 
uint8_t note_lane;
uint8_t column;
uint8_t color[TRACK_LENGTH];
int score = 0;
int combo_count = 0;
uint16_t freq;
float dutycycle;
uint8_t seven_segment[10] = {63,6,91,79,102,109,125,7,127,111};


volatile uint8_t display = 1;
volatile uint8_t ss_cc = 0;


void track_change(uint8_t trackNum) {
	
	for (uint8_t i = 0; i<TRACK_LENGTH; i++) {
		if(trackNum == 1) {
			track[i] = track1[i];
		}
		if(trackNum == 2) {
			track[i] = track2[i];
		}
		if(trackNum == 3){
			track[i] = track3[i];
		}
	}
}

void display_seven_segment(void)
{
	DDRC = 0xFF; 
	DDRA = 0x80; 

	uint8_t first = abs(score % 10);
	uint8_t second = abs(score/10) % 10;

	ss_cc = 1 - ss_cc;

	if(display) {
		if (score >= 0)
		{
			if(ss_cc == 0) {
				PORTC = seven_segment[first];
				} else {
				PORTC = seven_segment[second];
			}
		}
		else if (score > -10)
		{
			if(ss_cc == 0) {
				PORTC = seven_segment[first];
				} else {
				PORTC = 0b1000000; // Display "-" on the left digit
			}
		}
		else
		{
			PORTC = 0b1000000; // Display "-" on both digits
		}

		if (ss_cc == 1)
		{
			PORTA ^= (1 << 7);
			} else {
			PORTA &= !(1<<7);
		}
		} else {
		PORTC = 0;
	}
}

uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);	

}


uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

void play_buzzer(uint8_t lane, uint8_t column) {
	if (lane == 0) {
		freq = 784;
	}
	else if (lane == 1) {
		freq = 698;
	}
	else if (lane == 2) {
		freq = 622;
	}
	else if (lane == 3) {
		freq = 523;
	}
	if (column == 11) {
		dutycycle = 2.0;
	}
	else if (column == 12) {
		dutycycle = 10.0;
	}
	else if (column == 13) {
		dutycycle = 50.0;
	}
	else if (column == 14) {
		dutycycle =90.0;
	}
	else if (column == 15) {
		dutycycle = 98.0;
	}
	uint16_t clockperiod = freq_to_clock_period(freq);
	uint16_t pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
	
	
	DDRD = (1<<4);
	

	OCR1A = clockperiod - 1;
	

	if(pulsewidth == 0) {
		OCR1B = 0;
		} else {
		OCR1B = pulsewidth - 1;
	}
	
	
	TCCR1A = (1 << COM1B1) | (0 <<COM1B0) | (1 <<WGM11) | (1 << WGM10);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10);
}

void display_score(void)
{	
	move_terminal_cursor(0,18);
	printf("Game Score: %4d", score); 
}

void display_combo_count(void)
{
	move_terminal_cursor(0,19);
	printf("Combo Count: %d", combo_count); 
}

void display_combo_ascii(void)
{
	move_terminal_cursor(10,4); // Move the cursor to a new line
	printf_P(PSTR("···················································\n"));
	move_terminal_cursor(10,5); // Move the cursor to the next line
	printf_P(PSTR(":                                                   :\n"));
	move_terminal_cursor(10,6); // Move the cursor to the next line
	printf_P(PSTR(":                                                   :\n"));
	move_terminal_cursor(10,7); // Move the cursor to the next line
	printf_P(PSTR(":    ____                       _               _   :\n"));
	move_terminal_cursor(10,8); // Move the cursor to the next line
	printf_P(PSTR(":   / ___|   ___    _ __ ___   | |__     ___   | |  :\n"));
	move_terminal_cursor(10,9); // Move the cursor to the next line
	printf_P(PSTR(":  | |      / _ \\  | '_ ` _ \\  | '_ \\   / _ \\  | |  :\n"));
	move_terminal_cursor(10,10); // Move the cursor to the next line
	printf_P(PSTR(":  | |___  | (_) | | | | | | | | |_) | | (_) | |_|  :\n"));
	move_terminal_cursor(10,11); // Move the cursor to the next line
	printf_P(PSTR(":   \\____|  \\___/  |_| |_| |_| |_.__/   \\___/  (_)  :\n"));
	move_terminal_cursor(10,12); // Move the cursor to the next line
	printf_P(PSTR(":                                                   :\n"));
	move_terminal_cursor(10,13); // Move the cursor to the next line
	printf_P(PSTR(":                                                   :\n"));
	move_terminal_cursor(10,14); // Move the cursor to the next line
	printf_P(PSTR("·······································\n"));
}


void reset_combo_ascii(void)
{
	for (int i = 4; i <= 14; i++) {
		move_terminal_cursor(10,i); // Move the cursor to the line
		clear_to_end_of_line(); // Clear the line
	}
}

// Initialise the game by resetting the grid and beat
void initialise_game(void)
{
	// initialise the display we are using.
	
	default_grid();
	beat = 0;
	
	for(uint8_t i = 0; i<TRACK_LENGTH; i++){
		color[i] = COLOUR_RED;
	}
	score = 0;
	display_score();
	display_combo_count();
	DDRC = 0xFF;
	DDRA = 0x80;
}

// Play a note in the given lane
void play_note(uint8_t lane)
{
	for(uint8_t i = 0; i<TRACK_LENGTH; i++){
		if(note_index!=i) {
			color[i] = COLOUR_RED; 
		}
	}
	
	if(column<=15 && column >= 11) {
		if(lane == 0 && note_lane == 3) {
			if(color[note_index] == COLOUR_GREEN) {
				score -= 1; // Subtract point if note is already green
				} else {
			ledmatrix_update_pixel(column, 2*note_lane, COLOUR_GREEN);
			ledmatrix_update_pixel(column, 2*note_lane+1, COLOUR_GREEN);
			color[note_index] = COLOUR_GREEN;
			
			play_buzzer(lane, column);
			
			// Update score based on note position
			if (column == 13) {
				combo_count++; 
				 if (combo_count >= 3) {
					 score += 4; 
					 display_combo_ascii();
					} else {
					 score += 3; 
				 } 
			} else if (column == 12 || column == 14) {
				score += 2; // Second or fourth row
				combo_count = 0;
				reset_combo_ascii();
			} else if (column == 11 || column == 15) {
				score += 1; // Outside rows
				combo_count = 0;
				reset_combo_ascii();
			} 
		} 
		}
		
		if(lane == 1 && note_lane == 2) {
			if(color[note_index] == COLOUR_GREEN) {
				score -= 1; // Subtract point if note is already green
			} else {
			ledmatrix_update_pixel(column, 2*note_lane, COLOUR_GREEN);
			ledmatrix_update_pixel(column, 2*note_lane+1, COLOUR_GREEN);
			color[note_index] = COLOUR_GREEN;
			play_buzzer(lane, column);
			
			// Update score based on note position
			if (column == 13) {
				if (combo_count >= 3) {
					score += 4;
					display_combo_ascii();
					} else {
					score += 3;
				} combo_count++;
				} else if (column == 12 || column == 14) {
				score += 2; // Second or fourth row
				combo_count = 0;
				reset_combo_ascii();
				} else if (column == 11 || column == 15) {
				score += 1; // Outside rows
				combo_count = 0;
				reset_combo_ascii();
			}
			}
			} 
		if(lane == 2 && note_lane == 1) {
			if(color[note_index] == COLOUR_GREEN) {
				score -= 1; // Subtract point if note is already green
				} else {
			ledmatrix_update_pixel(column, 2*note_lane, COLOUR_GREEN);
			ledmatrix_update_pixel(column, 2*note_lane+1, COLOUR_GREEN);
			color[note_index] = COLOUR_GREEN;
			play_buzzer(lane, column);
			
			// Update score based on note position
			if (column == 13) {
				if (combo_count >= 3) {
					score += 4;
					display_combo_ascii();
					} else {
					score += 3;
				} combo_count++;
				} else if (column == 12 || column == 14) {
				score += 2; // Second or fourth row
				combo_count = 0;
				reset_combo_ascii();
				} else if (column == 11 || column == 15) {
				score += 1; // Outside rows
				combo_count = 0;
				reset_combo_ascii();
			}
			}
			} 
		if(lane == 3 && note_lane == 0) {
			if(color[note_index] == COLOUR_GREEN) {
				score -= 1; // Subtract point if note is already green
				} else {
			ledmatrix_update_pixel(column, 2*note_lane, COLOUR_GREEN);
			ledmatrix_update_pixel(column, 2*note_lane+1, COLOUR_GREEN);
			color[note_index] = COLOUR_GREEN;
			play_buzzer(lane, column);
			
			// Update score based on note position
			if (column == 13) {
				if (combo_count >= 3) {
					score += 4;
					display_combo_ascii();
					} else {
					score += 3;
				} combo_count++;
				} else if (column == 12 || column == 14) {
				score += 2; // Second or fourth row
				combo_count = 0;
				reset_combo_ascii();
				} else if (column == 11 || column == 15) {
				score += 1; // Outside rows
				combo_count = 0;
				reset_combo_ascii();
			}
			}
		} 
	 } else {
		 score -= 1;
		 combo_count = 0;
	 }
	display_score();
	display_combo_count();
}

// Advance the notes one row down the display
void advance_note(void)
{
	// remove all the current notes; reverse of below
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col;
		uint8_t index = (future + beat) / 5;
		if (index >= TRACK_LENGTH)
		{
			break;
		}
		if ((future+beat) % 5)
		{
			continue;
		}
		for (uint8_t lane = 0; lane < 4; lane++)
		{
			if (track[index] & (1<<lane))
			{
				PixelColour colour;

				// yellows in the scoring area
				if (col==11 || col == 15)
				{
					colour = COLOUR_QUART_YELLOW;
				}
				else if (col==12 || col == 14)
				{
					colour = COLOUR_HALF_YELLOW;
				}
				else if (col==13)
				{
					colour = COLOUR_YELLOW;
				}
				else
				{
					colour = COLOUR_BLACK;
				}
				if (color[index] == COLOUR_RED && col == 15) {
					combo_count = 0;
					score -= 1; // Deduct point
					display_score();
					display_combo_count();
					reset_combo_ascii();
				}
				ledmatrix_update_pixel(col, 2*lane, colour);
				ledmatrix_update_pixel(col, 2*lane+1, colour);
			}
			
		}
	}
	
	
	// increment the beat
	beat++;
	
	uint8_t future = 16;

	while (1)
	{
		uint8_t index = (future + beat) / 5;

		if (index > TRACK_LENGTH)
		{
			break;
		}

		if ((future + beat) % 5)
		{
			future++;
			continue;
		}

		uint8_t maskedTrackValue =  0x0F & track[index];
		
		if (maskedTrackValue)
		{
			for (uint8_t lane = 0; lane < 4; lane++)
			{
				if (maskedTrackValue & (1 << lane))
				{
				
					if (combo_count >= 3) {
						ledmatrix_update_pixel(0, 2 * lane, COLOUR_DARK_ORANGE);
						ledmatrix_update_pixel(0, 2 * lane + 1, COLOUR_DARK_ORANGE);
						} else {
						ledmatrix_update_pixel(0, 2 * lane, COLOUR_DARK_RED);
						ledmatrix_update_pixel(0, 2 * lane + 1, COLOUR_DARK_RED);
						}
				}
			}
			break; // Found a note, exit the loop
		}

		future++;
	}

	
	// draw the new notes
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		// col counts from one end, future from the other
		uint8_t future = MATRIX_NUM_COLUMNS-1-col;
		
		// notes are only drawn every five columns
		if ((future+beat)%5)
		{
			continue;
		}
		
		// index of which note in the track to play
		uint8_t index = (future+beat)/5;
		note_index = index;
		
		// if the index is beyond the end of the track,
		// no note can be drawn
		if (index >= TRACK_LENGTH)
		{
			continue;
		}
		
		// iterate over the four paths
		for (uint8_t lane=0; lane<4; lane++)
		{
			// check if there's a note in the specific path
			if (track[index] & (1<<lane))
			{	
				if (combo_count >= 3 && color[index] != COLOUR_GREEN) {
					ledmatrix_update_pixel(col, 2*lane, COLOUR_ORANGE);
					ledmatrix_update_pixel(col, 2*lane+1, COLOUR_ORANGE);
				} else {
					ledmatrix_update_pixel(col, 2*lane, color[index]);
					ledmatrix_update_pixel(col, 2*lane+1, color[index]);
				}
				note_lane = lane;
				column = col;
			}
		}
	}
	 display_score();
	 display_combo_count();
}

// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void)
{
	// Detect if the game is over i.e. if a player has won.
	if ((beat/5) >= TRACK_LENGTH){
		for (uint8_t row = 0; row < 8; row++)
		{
			ledmatrix_update_pixel(0, row, COLOUR_BLACK);
		}
		return 1;
	}
	else {
		return 0;	
	}
}

