// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

@SCREEN
D = A 
@i
M = D // i = SCREEN_START_ADDRESS;
(LOOP)
	@KBD
	D = A 
	@i 
	D = M - D
	@START
	D; JGE // if i - SCREEN_END_ADDRESS >= 0 goto START
	@KBD
	D = M  
	@CLEAR
	D; JEQ // if !PRESS goto CLEAR	
	@i
	A = M 
	M = -1 
	@i
	M = M + 1 // fill pixel and loop 
	@LOOP 
	0; JMP 
(CLEAR)
	@i
	A = M 
	M = 0 
	@i
	M = M + 1 // clear pixel and loop
	@LOOP 
	0; JMP
(START)
	@SCREEN
	D = A 
	@i
	M = D // i = SCREEN_START_ADDRESS;
	@LOOP
	0; JMP

