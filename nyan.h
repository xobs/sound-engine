static const uint16_t nyan_intro[] = {
	NN(-12, N_HALF, 0),
	NN(-1, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(2, N_HALF, 0),
	NN(7, N_HALF, 0),
	NN(-1, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(2, N_QUARTER, 0),
	NN(7, N_QUARTER, 0),
	NN(9, N_QUARTER, 0),
	NN(11, N_QUARTER, 0),
	NN(9, N_QUARTER, 0),
	NN(6, N_QUARTER, 0),
	NN(7, N_HALF, 0),
	NN(2, N_HALF, 0),
	NN(-1, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(2, N_HALF, 0),
	//NN(7, N_HALF, 0),
	NN(9, N_QUARTER, 0),
	NN(6, N_QUARTER, 0),
	NN(7, N_QUARTER, 0),
	NN(9, N_QUARTER, 0),
	NN(12, N_QUARTER, 0),
	NN(11, N_QUARTER, 0),
	NN(12, N_QUARTER, 0),
	NN(9, N_QUARTER, 0),
	NE(PATTERN_JUMP_REL, 1),
};

static const uint16_t nyan_loop1[] = {
	NN(2, N_HALF, 0),
	NN(4, N_QUARTER, N_QUARTER),
	NN(-3, N_QUARTER, 0),
	NN(-1, N_HALF, 0),
	NN(-5, N_EIGHTH, N_EIGHTH),
	NN(-2, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-3, N_HALF, 0),
	NN(-2, N_QUARTER, N_QUARTER),
	NN(-2, N_EIGHTH, N_EIGHTH),
	NN(-3, N_EIGHTH, N_EIGHTH),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(2, N_QUARTER, 0),
	NN(4, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(2, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-2, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-1, N_HALF, 0),
	NN(2, N_QUARTER, N_QUARTER),
	NN(4, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(2, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-2, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(-2, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-2, N_QUARTER, N_QUARTER),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-2, N_QUARTER, 0),
	NN(2, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-2, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, N_QUARTER),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-3, N_QUARTER, N_QUARTER),
	NE(PATTERN_REPEAT_COUNT, 2),
	NE(PATTERN_JUMP_REL, 1),
};

static const uint16_t nyan_loop2[] = {
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-10, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-10, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(2, N_QUARTER, 0),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-10, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-10, N_QUARTER, 0),
	NN(-13, N_QUARTER, 0),
	NN(-12, N_QUARTER, 0),
	NN(-10, N_QUARTER, 0),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-10, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-10, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-3, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-10, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(-10, N_QUARTER, 0),
	NN(-5, N_HALF, 0),
	NN(-5, N_QUARTER, 0),
	NN(-6, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(-10, N_QUARTER, 0),
	NN(-8, N_QUARTER, 0),
	NN(-5, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(-1, N_QUARTER, 0),
	NN(0, N_QUARTER, 0),
	NN(2, N_QUARTER, 0),
	NN(-5, N_QUARTER, N_QUARTER),
	NN(-6, N_QUARTER, N_QUARTER),
	NE(PATTERN_REPEAT_COUNT, 2),
	NE(PATTERN_JUMP_REL, -1),
};

#define SONG_PATTERNS nyan_intro, nyan_loop1, nyan_loop2
