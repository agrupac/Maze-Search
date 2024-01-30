#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//used to locate jerry's starting position in a maze
void find_jerry(char ** maze, int * x, int * y, int rows, int cols){
	
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if(maze[i][j] == 'M'){
				*x = j + 1;
				*y = i + 1;
				return; 
			}
		}
	}

}

//used to print results of searches to a file
void write_output(FILE * output, char ** steps, int step_num, char ** maze, int rows, int cols){
	
	for(int i = rows - 1; i >= 0; i--){
		for(int j = 0; j < cols; j++){
			putc(maze[i][j], output);
		}
		putc('\n', output);
	}
	for(int i = 0; i < step_num; i++){
		fprintf(output, "%s\n", steps[i]);
	}

}

void iterative_search(char ** maze, int rows, int cols, char * directions, int dir_length, FILE * output){

	int jerry_x, jerry_y, new_x, new_y;
	int valid_steps = 0;
		
	//create array of strings for steps taken by jerry
	char ** steps_taken = malloc((dir_length + 1) * sizeof(char *));
	for(int i = 0; i < dir_length + 1; i++){
		steps_taken[i] = malloc(40 * sizeof(char));
		steps_taken[i][1] = '\0';
	}

	//locate jerry's starting position
	find_jerry(maze, &jerry_x, &jerry_y, rows, cols);

	//loop through directions
	for(int i = 0; i < dir_length; i++){
		
		//determine new position	
		switch(directions[i]){
			case 'N':
				new_x = jerry_x;
				new_y = jerry_y + 1;
				break;
			case 'S':
				new_x = jerry_x;
				new_y = jerry_y - 1;
				break;
			case 'W':
				new_x = jerry_x - 1;
				new_y = jerry_y;
				break;
			case 'E':
				new_x = jerry_x + 1;
				new_y = jerry_y;
				break;
		}

		//if new position is within bounds
		if(maze[new_y - 1][new_x - 1] != '|' && ( (new_x <= cols && new_x > 0) && (new_y <= rows && new_y > 0) ) ){		

			char step[40];

			switch(directions[i]){
				case 'N':
					sprintf(step, "Jerry moved North (%d, %d)", new_x, new_y);
					break;
				case 'S':
					sprintf(step, "Jerry moved South (%d, %d)", new_x, new_y);
					break;
				case 'W':
					sprintf(step, "Jerry moved West (%d, %d)", new_x, new_y);
					break;
				case 'E':
					sprintf(step, "Jerry moved East (%d, %d)", new_x, new_y);
					break;
			}
	
			strcpy(steps_taken[valid_steps], step);
			valid_steps++;

			//update coordinates
			maze[jerry_y - 1][jerry_x - 1] = '.';	
			jerry_x = new_x;
			jerry_y = new_y;
			
			//if cheese found
			if(maze[new_y - 1][new_x - 1] == 'C'){
				strcpy(steps_taken[valid_steps], "Jerry found the cheese!");
				valid_steps++;
				maze[new_y - 1][new_x - 1] = 'M';
				break;
			}
			maze[new_y - 1][new_x - 1] = 'M';
		}
	}

	write_output(output, steps_taken, valid_steps, maze, rows, cols);

	free(steps_taken);

}

//used to determine the steps taken after a path is found using recursive search
void determine_steps(int start_x, int start_y, char ** maze, int rows, int cols, char ** steps_taken, int num_steps){

	int x = start_x;
	int y = start_y;
	char prev_direction = '\0';

	for(int i = 0; i < num_steps; i++){
		char step[40];
		
		//check north
		if((maze[y][x - 1] == '.' || maze[y][x - 1] == 'M') && prev_direction != 'S'){
			y++;
			prev_direction = 'N';
			sprintf(step, "Jerry moved North (%d, %d)", x, y);
			strcpy(steps_taken[i], step);
			//if current position is jerry
			if(maze[y - 1][x - 1] == 'M'){
				sprintf(step, "Jerry found the cheese!");
				strcpy(steps_taken[++i], step);
				break;	
			}	
		}
		//check south
		else if((maze[y - 2][x - 1] == '.' || maze[y - 2][x - 1] == 'M') && prev_direction != 'N'){
			y--;
			prev_direction = 'S';
			sprintf(step, "Jerry moved South (%d, %d)", x, y);
			strcpy(steps_taken[i], step);
			//if current position is jerry
			if(maze[y - 1][x - 1] == 'M'){
				sprintf(step, "Jerry found the cheese!");
				strcpy(steps_taken[++i], step);
				break;	
			}	
		}
		//check east
		else if((maze[y - 1][x] == '.' || maze[y - 1][x] == 'M') && prev_direction != 'W'){
			x++;
			prev_direction = 'E';
			sprintf(step, "Jerry moved East (%d, %d)", x, y);
			strcpy(steps_taken[i], step);
			//if current position is jerry
			if(maze[y - 1][x - 1] == 'M'){
				sprintf(step, "Jerry found the cheese!");
				strcpy(steps_taken[++i], step);
				break;	
			}	
		}
		//check west
		else if((maze[y - 1][x - 2] == '.'|| maze[y - 1][x - 2] == 'M') && prev_direction != 'E'){
			x--;
			prev_direction = 'W';
			sprintf(step, "Jerry moved West (%d, %d)", x, y);
			strcpy(steps_taken[i], step);
			//if current position is jerry
			if(maze[y - 1][x - 1] == 'M'){
				sprintf(step, "Jerry found the cheese!");
				strcpy(steps_taken[++i], step);
				break;	
			}	
		}
	}

}

int recursive_search(int x, int y, char ** maze, int rows, int cols, int * num_steps){

	//base case: cheese found
	if(maze[y - 1][x - 1] == 'C'){
		*num_steps = *num_steps + 1;
		maze[y - 1][x - 1] = 'M';
		return 1;
	}
	//base case: cannot move to position
	if(maze[y - 1][x - 1] == '|' || maze[y - 1][x - 1] == '.'){
		return 0;
	}
	//recursion
	maze[y - 1][x - 1] = '.';
	//go east
	if(recursive_search(x + 1, y, maze, rows, cols, num_steps)){
		*num_steps = *num_steps + 1;
		return 1;
	}
	//go west
	if(recursive_search(x - 1, y, maze, rows, cols, num_steps)){
		*num_steps = *num_steps + 1;
		return 1;
	}
	//go north
	if(recursive_search(x, y + 1, maze, rows, cols, num_steps)){
		*num_steps = *num_steps + 1;
		return 1;
	}
	//go south
	if(recursive_search(x, y - 1, maze, rows, cols, num_steps)){
		*num_steps = *num_steps + 1;
		return 1;
	}
	//back up if path failed
	maze[y - 1][x - 1] = ' ';
	return 0;

}

int main(int argc, char ** argv){

	if(argc != 3){
		printf("Two input files are required.\n");
		exit(1);
	}

	//determine output filenames
	char i_filename[30] = "iterative_";
	char r_filename[30] = "recursive_";
	char * buffer = malloc(20);
	sscanf(argv[1], "%[^.]", buffer);
	strcat(buffer, ".out");
	strcat(i_filename, buffer);
       	strcat(r_filename, buffer);	
	free(buffer);

	//open files
	FILE * maze_file = fopen(argv[1], "r");
	FILE * dir_file = fopen(argv[2], "r");
	
	//check if files could be opened
	if(!maze_file || !dir_file){
		printf("Could not open file.\n");
		exit(1);
	}

	//create output files
	FILE * i_out = fopen(i_filename, "w");
	FILE * r_out = fopen(r_filename, "w");

	//find dimensions of maze file
	int rows = 0;
	int cols = 0;
	buffer = malloc(100);
	while(fgets(buffer, 100, maze_file) != NULL){
		rows++;
	}
	cols = strlen(buffer) - 1;
	free(buffer);
	rewind(maze_file);

	//allocate two maze matricies
	char ** i_maze = malloc(rows * sizeof(char *));
	char ** r_maze = malloc(rows * sizeof(char *));
	for(int i = 0; i < rows; i++){
		i_maze[i] = malloc(cols * sizeof(char));
		r_maze[i] = malloc(cols * sizeof(char));
	}
	//populate maze matrix using maze file
	for(int i = rows - 1; i >= 0; i--){
		for(int j = 0; j < cols; j++){
			i_maze[i][j] = fgetc(maze_file);
			r_maze[i][j] = i_maze[i][j];
		}
		fgetc(maze_file);
	}
	fclose(maze_file);

	//find length of directions file
	int dir_length = 0;
	char c;
	while((c = fgetc(dir_file)) != EOF){
		fgetc(dir_file);
		dir_length++;
	}
	rewind(dir_file);

	//create directions array from directions file	
	char directions[dir_length];
	for(int i = 0; i < dir_length; i++){
		directions[i] = fgetc(dir_file);
		fgetc(dir_file);	
	}
	fclose(dir_file);
	
	//perform iterative search and write to file
	iterative_search(i_maze, rows, cols, directions, dir_length, i_out);
	free(i_maze);
	fclose(i_out);

	//locate start for recursion	
	int start_x, start_y;
	int num_steps = 0;
	find_jerry(r_maze, &start_x, &start_y, rows, cols);

	//create array of directions strings
	char ** r_steps = malloc(100 * sizeof(char *));
	for(int i = 0; i < 100; i++){
		r_steps[i] = malloc(40 * sizeof(char));
		r_steps[i][1] = '\0';
	}

	//perform recursive search, determine directions to final location, and write to file
	recursive_search(start_x, start_y, r_maze, rows, cols, &num_steps);
	determine_steps(start_x, start_y, r_maze, rows, cols, r_steps, num_steps);
	write_output(r_out, r_steps, num_steps, r_maze, rows, cols);
	free(r_maze);
	fclose(r_out);	

	printf("%s and %s were successfully created.\n", i_filename, r_filename);

}
