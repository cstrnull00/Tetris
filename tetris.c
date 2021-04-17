#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);
	
	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	for(i = 0; i < BLOCK_NUM; i++)
	 	nextBlock[i] = rand()%7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	if(BLOCK_NUM > 1)
	 	printw("NEXT BLOCK");
	for(i = 0; i < BLOCK_NUM - 1; i++)
		DrawBox(3 + i*6,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(3 + (BLOCK_NUM - 1)*6,WIDTH+10);
	printw("SCORE");
	DrawBox(4 + (BLOCK_NUM - 1)*6,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		command = ' ';
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	case ' ':
		blockY = ProjectY(blockY, blockX, nextBlock[0], blockRotate);
		drawFlag = 1;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(5 + (BLOCK_NUM - 1)*6, WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j, k;
	for( k = 1; k < BLOCK_NUM; k++) {
		for( i = 0; i < 4; i++ ){
			move(4 + i +(k - 1)*6,WIDTH + 13);
			for( j = 0; j < 4; j++ ){
				if( block[nextBlock[k]][0][i][j] == 1 ){
					attron(A_REVERSE);
					printw(" ");
					attroff(A_REVERSE);
				}	
				else printw(" ");
			}
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void EraseBlock(int y, int x, int blockID, int blockRotate) {
	int i, j;
	char tile = '.';
	for(i = 0; i < 4; i++)
	 	for(j = 0; j < 4; j++)
		 	if(block[blockID][blockRotate][i][j] == 1 && i + y >= 0) {
				move(i + y + 1, j + x + 1);
				printw("%c", tile);
			}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = ProcessCommand(GetCommand());
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

bool CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				if(!(0 <= blockX + j && blockX + j < WIDTH && 0 <= blockY + i && blockY + i < HEIGHT))
					return false;

				if(f[blockY + i][blockX + j] == 1) return false;;
			}
		}
	}

	return true;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
		
 	int projY;
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	switch(command) {
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
		case KEY_UP:
		 	projY = ProjectY(blockY, blockX, currentBlock, (blockRotate+3)%4);
		 	EraseBlock(projY, blockX, currentBlock, (blockRotate + 3) % 4);
			EraseBlock(blockY, blockX, currentBlock, (blockRotate + 3) % 4);
			break;
		case KEY_DOWN:
			EraseBlock(blockY - 1, blockX, currentBlock, blockRotate);
			break;
		case KEY_LEFT:
			projY = ProjectY(blockY, blockX + 1, currentBlock, blockRotate);
			EraseBlock(projY, blockX + 1, currentBlock, blockRotate);
			EraseBlock(blockY, blockX + 1, currentBlock, blockRotate);
			break;
		case KEY_RIGHT:
			projY = ProjectY(blockY, blockX - 1, currentBlock, blockRotate);
			EraseBlock(projY, blockX - 1, currentBlock, blockRotate);
			EraseBlock(blockY, blockX - 1, currentBlock, blockRotate);
			break;
	}

	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	//3. 새로운 블록 정보를 그린다. 
}

void BlockDown(int sig){
	int i;
	
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
	 	score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);

		if(blockY == -1)
		 	gameOver = 1;
		else {
			score += DeleteLine(field);

			for(i = 0; i < BLOCK_NUM -1; i++)
			 	nextBlock[i] = nextBlock[i + 1];

			nextBlock[BLOCK_NUM - 1] = rand() % 7;
			DrawNextBlock(nextBlock);
			blockY = -1;
			blockX = WIDTH/2 - 2;
			blockRotate = 0;
			PrintScore(score);
		}
		DrawField();
	}

	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j, touched = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				if(0 <= blockY + i && blockY + i < HEIGHT && 0 <= blockX + j && blockX + j < WIDTH)
				 	f[blockY + i][blockX + j] = 1;
				if(blockY == HEIGHT - 1) touched++;
				else if(f[blockY + i + 1][blockX + j] == 1) touched++;
			}
		}
	}
	//Block이 추가된 영역의 필드값을 바꾼다.

	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int cnt = 0, i, j, k;
	bool full;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	for(i = 0; i < HEIGHT; i++) {
		full = true;
		for (j = 0; j < WIDTH; j++) {
			if(f[i][j] == 0) {
				full = false;
				break;
			}
		}
		//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
		if(full) {
			cnt++;
			for(k = i - 1; k >= 0; k--) {
				for(j = WIDTH; j > 0; j--)
					f[k + 1][j] = f[k][j];
			}
		}
	}

	return cnt*cnt*100;
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int projY;
	projY = ProjectY(y, x, blockID, blockRotate);	
	DrawBlock(projY, x, blockID, blockRotate, '/');
}

int ProjectY(int y, int x, int blockID, int blockRotate) {
	int projY = y;
	while(CheckToMove(field, nextBlock[0], blockRotate, projY + 1, x))
		projY++;
	return projY;
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
