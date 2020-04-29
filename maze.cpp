#include "maze.h"
Maze::Maze(char _vert_walls[][8],char _horz_walls[][9], unsigned char _orientation, unsigned char _start_loc[2])
{
    set_vert_walls(_vert_walls);
    set_horz_walls(_horz_walls);
    iorientation = _orientation;
    curr_orientation = _orientation;
    start_loc[0] = _start_loc[0];
    start_loc[1] = _start_loc[1];
    flood_fill_closed();
    flood_fill_open();

    // open_path = new struct path;
    // closed_path =  new struct path;
    // if(open_path == nullptr || closed_path == nullptr){
    //     output_stream->println(" WARNINING MAZE NOT INSTANTIATED CORRECTLY");
    // }
}

Maze::Maze(char _vert_walls[][8],char _horz_walls[][9], unsigned char _orientation, unsigned char _start_loc[2], unsigned char _dest_loc[2])
{
    dest_loc[0] = _dest_loc[0];
    dest_loc[1] = _dest_loc[1];
    set_vert_walls(_vert_walls);
    set_horz_walls(_horz_walls);
    iorientation = _orientation;
    curr_orientation = _orientation;
    start_loc[0] = _start_loc[0];
    start_loc[1] = _start_loc[1];
    flood_fill_closed();
    flood_fill_open();
}
    
Maze::~Maze()
{
    if(open_path.turn_list != NULL){
        delete [] open_path.turn_list;
        open_path.turn_list = NULL;
    }

    if(closed_path.turn_list != NULL){
        delete [] closed_path.turn_list;
        closed_path.turn_list = NULL;
    }
}

void Maze::show_maze()
{
    
    int vert_wall_row = 0;
    int vert_wall_col = 0;
    int horz_wall_row = 0;
    int horz_wall_col = 0;

    for (int row = 1; row <= NUM_DRAW_ROWS; row++){
        for(int col = 1; col <= NUM_DRAW_COLS; col++){
            
            // if row is odd it is time to print a horizontal wall
            if (row%2 == 1){
                //if col is odd its a vertical wall space
                //if col is even it is a horizontal wall space

                if(col%2 == 1){

                    output_stream->print(" ");

                    
                } else {

                    //Print the outside horizontal walls
                    if(row == 1 || row == NUM_DRAW_ROWS){
                        output_stream->print("---");
                    } else {
                        if(horz_walls[horz_wall_row][horz_wall_col] == 1){
                            output_stream->print("---");
                        } else if (horz_walls[horz_wall_row][horz_wall_col] == 0){
                            output_stream->print("   ");
                        } else if (horz_walls[horz_wall_row][horz_wall_col] == 2){
                            output_stream->print("***");
                        } else {
                            output_stream->println("ERROR");
                        }
                        horz_wall_col++;
                    }

                    

                }
                
            }

            if (row%2 == 0){
                //if col is odd its a vertical wall space
                //if col is even it is a horizontal wall space
                if(col%2 == 1){

                    //print the verticle outside walls
                    if(col == 1 || col == NUM_DRAW_COLS){
                        output_stream->print("|");
                    } else {
                        if(vert_walls[vert_wall_row][vert_wall_col] == 1){
                            output_stream->print("|");
                        } else if (vert_walls[vert_wall_row][vert_wall_col] == 0){
                            output_stream->print(" ");
                        } else if (vert_walls[vert_wall_row][vert_wall_col] == 2){
                            output_stream->print("*");
                        } else {
                            output_stream->println("ERROR");
                        }
                        vert_wall_col++;
                    }
                    
                } else {
                    if(col != 1 || col == NUM_DRAW_COLS){
                        output_stream->print("   ");
                    }
                }
            }
            
        }
        vert_wall_col = 0;
        horz_wall_col = 0;
        output_stream->println("");
        if(row == 1 || row == NUM_DRAW_ROWS){
            continue;
        }
        if(row%2 == 0){
            vert_wall_row++;
        } else {
            horz_wall_row++;
        }
        // if row is even it is time to print a vertical wall
    }
    // output_stream->println(vert_wall_row);
    // output_stream->println(vert_wall_col);
    // output_stream->println(horz_wall_row);
    // output_stream->println(horz_wall_col);
}

void Maze::show_maze_with_path()
{
    int vert_wall_row = 0;
    int vert_wall_col = 0;
    int horz_wall_row = 0;
    int horz_wall_col = 0;
    for (int row = 1; row <= NUM_DRAW_ROWS; row++){
        for(int col = 1; col <= NUM_DRAW_COLS; col++){
            
            // if row is odd it is time to print a horizontal wall
            if (row%2 == 1){
                //if col is odd its a vertical wall space
                //if col is even it is a horizontal wall space

                if(col%2 == 1){

                    output_stream->print(" ");

                    
                } else {

                    //Print the outside horizontal walls
                    if(row == 1 || row == NUM_DRAW_ROWS){
                        output_stream->print("---");
                    } else {
                        if(horz_walls[horz_wall_row][horz_wall_col] == 1){
                            output_stream->print("---");
                        } else if (horz_walls[horz_wall_row][horz_wall_col] == 0){
                            output_stream->print("   ");
                        } else if (horz_walls[horz_wall_row][horz_wall_col] == 2){
                            output_stream->print("***");
                        } else {
                            output_stream->println("ERROR");
                        }
                        horz_wall_col++;
                    }

                    

                }
                
            }

            if (row%2 == 0){
                //if col is odd its a vertical wall space
                //if col is even it is a horizontal wall space
                if(col%2 == 1){

                    //print the verticle outside walls
                    if(col == 1 || col == NUM_DRAW_COLS){
                        output_stream->print("|");
                    } else {
                        if(vert_walls[vert_wall_row][vert_wall_col] == 1){
                            output_stream->print("|");
                        } else if (vert_walls[vert_wall_row][vert_wall_col] == 0){
                            output_stream->print(" ");
                        } else if (vert_walls[vert_wall_row][vert_wall_col] == 2){
                            output_stream->print("*");
                        } else {
                            output_stream->println("ERROR");
                        }
                        vert_wall_col++;
                    }
                    
                } else {
                    output_stream->print(" ");

                    unsigned char path_row = row/2-1;
                    unsigned char path_col = col/2-1;
                    bool skip = false;
                    if(robot_path[path_row][path_col] != CELL_MAX){
                        if(path_row == dest_loc[0] && path_col == dest_loc[1]){
                            output_stream->print("X");
                        } else if(path_row == start_loc[0] && path_col == start_loc[1]){
                            switch (iorientation)
                            {
                            case NORTH:
                                output_stream->print("N");
                                break;
                            case EAST:
                                output_stream->print("E");
                                break;
                            case SOUTH:
                                output_stream->print("S");
                                break;
                            case WEST:
                                output_stream->print("W");
                                break;
                            default:
                                break;
                            }

                        } else {
                            output_stream->print(robot_path[path_row][path_col]);
                            if(robot_path[path_row][path_col] > 9)
                                skip = true;
                        }
                    } else {
                        output_stream->print(" ");
                    }

                    if(!skip){
                        output_stream->print(" ");
                    }

                }
            }
            
        }
        vert_wall_col = 0;
        horz_wall_col = 0;
        output_stream->println("");
        if(row == 1 || row == NUM_DRAW_ROWS){
            continue;
        }
        if(row%2 == 0){
            vert_wall_row++;
        } else {
            horz_wall_row++;
        }
        // if row is even it is time to print a vertical wall
    }
    // output_stream->println(vert_wall_row);
    // output_stream->println(vert_wall_col);
    // output_stream->println(horz_wall_row);
    // output_stream->println(horz_wall_col);
}

void Maze::show_flood_fill_open()
{
    for (int row = 0; row < MAZE_ROWS; row++){
        for(int col = 0; col < MAZE_COLS; col++){
            
            output_stream->print(" "); 
            
            if (flood_filled_open[row][col] < 10){
                output_stream->print("0");
            }
            output_stream->print((int)flood_filled_open[row][col]); 
            output_stream->print(" ");
        }
        output_stream->println("");
    }
}

void Maze::show_flood_fill_closed()
{
    for (int row = 0; row < MAZE_ROWS; row++){
        for(int col = 0; col < MAZE_COLS; col++){
            output_stream->print(" "); 
           
            if (flood_filled_closed[row][col] < 10){
                output_stream->print("0");
            }
            output_stream->print((int)flood_filled_closed[row][col]); 
            output_stream->print(" ");
        }
        output_stream->println("");
    }
}
void Maze::show_open_directions()
{
    if(open_path.turn_list == NULL){
        output_stream->println("OPEN PATH DOESN'T EXIST");
        return;
    }

    for (int i = 0; i < flood_filled_open[start_loc[0]][start_loc[1]]; i++){
        output_stream->print(i+1);
        output_stream->print(": ");
        switch (open_path.turn_list[i])
        {
        case FORWARD:
            output_stream->println("DRIVE FORWARD");
            break;
        
        case LEFT_SIDE:
            output_stream->println("TURN LEFT & DRIVE FORWARD");
            break;

        case RIGHT_SIDE:
            output_stream->println("TURN RIGHT & DRIVE FORWARD");
            break;

        case FULLROTATE:
            output_stream->println("TURN 180 & DRIVE FORWARD");
            break;        
        default:
            output_stream->println("ERROR");
            break;
        }
    }
}

void Maze::show_closed_directions()
{
    if(closed_path.turn_list == NULL){
        output_stream->println("CLOSED PATH DOESN'T EXIST");
        return;
    }

    for (int i = 0; i < flood_filled_closed[start_loc[0]][start_loc[1]]; i++){
        output_stream->print(i+1);
        output_stream->print(": ");
        switch (closed_path.turn_list[i])
        {
        case FORWARD:
            output_stream->println("DRIVE FORWARD");
            break;
        
        case LEFT_SIDE:
            output_stream->println("TURN LEFT & DRIVE FORWARD");
            break;

        case RIGHT_SIDE:
            output_stream->println("TURN RIGHT & DRIVE FORWARD");
            break;

        case FULLROTATE:
            output_stream->println("TURN 180 & DRIVE FORWARD");
            break;        
        default:
            output_stream->println("ERROR");
            break;
        }
    }
    //output_stream->println((int)closed_path.turn_count);
}

void Maze::set_vert_walls(char _vert_walls[][8])
{
    for (int row = 0; row < MAZE_ROWS; row++){
        for(int col = 0; col < VERT_WALLS; col++){
            vert_walls[row][col] = _vert_walls[row][col];
        }
    }
    flood_fill_closed();
    flood_fill_open();
}

void Maze::set_horz_walls(char _horz_walls[][9])
{
    for (int row = 0; row < MAZE_ROWS-1; row++){
        for(int col = 0; col < HORZ_WALLS; col++){
            horz_walls[row][col] = _horz_walls[row][col];
        }
    }
    flood_fill_closed();
    flood_fill_open();
}

void Maze::set_orientation(unsigned char _orientation)
{
    iorientation = _orientation;
}

void Maze::set_location(unsigned char _start_loc[2])
{
    start_loc[0] = _start_loc[0];
    start_loc[1] = _start_loc[1];
}

void Maze::set_serial(Stream* _output_stream)
{
    output_stream = _output_stream;
}

struct robot_path Maze::get_turn_list(bool open) {
    return open ? open_path : closed_path;
}

void Maze::flood_fill_open()
{
    for (int row = 0; row < MAZE_ROWS; row++){
        for(int col = 0; col < HORZ_WALLS; col++){
            flood_filled_open[row][col] = CELL_MAX;
        }
    }

    flood_filled_open[dest_loc[0]][dest_loc[1]] = 0;
    int curr_val = 0;
    bool maze_change = true;
    while(maze_change == true){
        maze_change = false;
        for(int row = 0; row < MAZE_ROWS; row++){
            for(int col = 0; col < MAZE_COLS; col++){
                if(flood_filled_open[row][col] == curr_val){
                   //row = 0 or 4 = outside horizontal wall
                   //col = 0 or 8 = outiside vert wall

                   //Check North
                   if (row == 0)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (horz_walls[row-1][col] != 1){
                           if(flood_filled_open[row-1][col] == CELL_MAX){
                               flood_filled_open[row-1][col] = flood_filled_open[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }
                   

                   //Check East
                   if (col == MAZE_COLS-1)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (vert_walls[row][col] != 1){
                           if(flood_filled_open[row][col+1] == CELL_MAX){
                               flood_filled_open[row][col+1] = flood_filled_open[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }

                   //Check South
                   if (row == MAZE_ROWS-1)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (horz_walls[row][col] != 1){
                           if(flood_filled_open[row+1][col] == CELL_MAX){
                               flood_filled_open[row+1][col] = flood_filled_open[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }

                   //Check West
                   if (col == 0)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (vert_walls[row][col-1] != 1){
                           if(flood_filled_open[row][col-1] == CELL_MAX){
                               flood_filled_open[row][col-1] = flood_filled_open[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }
                }
            }
        }
        curr_val++;
    }

}

void Maze::flood_fill_closed()
{
    for (int row = 0; row < MAZE_ROWS; row++){
        for(int col = 0; col < HORZ_WALLS; col++){
            flood_filled_closed[row][col] = CELL_MAX;
        }
    }

    flood_filled_closed[dest_loc[0]][dest_loc[1]] = 0;
    int curr_val = 0;
    bool maze_change = true;
    while(maze_change == true){
        maze_change = false;
        for(int row = 0; row < MAZE_ROWS; row++){
            for(int col = 0; col < MAZE_COLS; col++){
                if(flood_filled_closed[row][col] == curr_val){
                   //row = 0 or 4 = outside horizontal wall
                   //col = 0 or 8 = outiside vert wall

                   //Check North
                   if (row == 0)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (horz_walls[row-1][col] == 0){
                           if(flood_filled_closed[row-1][col] == CELL_MAX){
                               flood_filled_closed[row-1][col] = flood_filled_closed[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }
                   

                   //Check East
                   if (col == MAZE_COLS-1)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (vert_walls[row][col] == 0){
                           if(flood_filled_closed[row][col+1] == CELL_MAX){
                               flood_filled_closed[row][col+1] = flood_filled_closed[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }

                   //Check South
                   if (row == MAZE_ROWS-1)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (horz_walls[row][col] == 0){
                           if(flood_filled_closed[row+1][col] == CELL_MAX){
                               flood_filled_closed[row+1][col] = flood_filled_closed[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }

                   //Check West
                   if (col == 0)
                   {
                       // Don't to anything 
                   } else {
                       // check if horz wall is not there
                       // != 1 means both 0 and 2 are open
                       if (vert_walls[row][col-1] == 0){
                           if(flood_filled_closed[row][col-1] == CELL_MAX){
                               flood_filled_closed[row][col-1] = flood_filled_closed[row][col]+1;
                               maze_change = true;
                           }
                       }
                   }
                }
            }
        }
        curr_val++;
    }
}

void Maze::create_path_open()
{
    //search for the shortest path to the centre 
    unsigned char path_length = flood_filled_open[start_loc[0]][start_loc[1]];
    unsigned char curr_row = start_loc[0];
    unsigned char curr_col = start_loc[1];
    

    if(open_path.turn_list != NULL){
        delete [] open_path.turn_list;
        open_path.turn_list = NULL;
    }

    if(path_length == CELL_MAX){
        output_stream->println("WARNING Path to Center for OPEN is Unavailable");
        open_path.turn_count  = TURN_MAX;
        open_path.step_count = CELL_MAX;
        return;
    }

    open_path.turn_list = new unsigned char[path_length];
    open_path.turn_count = TURN_MAX;
    open_path.step_count = path_length;

    if(open_path.turn_list == nullptr){
        output_stream->println("WARNING OPEN PATH WASN'T INSTANTIATED");
        return;
    }
    
    depth_first_search_start_open();
    
    //Fill Robot Path
    for (int row = 0; row < MAZE_ROWS; row++){
        for(int col = 0; col < HORZ_WALLS; col++){
            robot_path[row][col] = CELL_MAX;
        }
    }

    unsigned char orientation = iorientation;
    curr_row = start_loc[0];
    curr_col = start_loc[1];
    robot_path[curr_row][curr_col] = 0;
    for (int i = 0; i < open_path.step_count; i++){
        unsigned char new_orientation = (orientation+open_path.turn_list[i])%4;
        curr_col = get_next_col(curr_col,new_orientation);
        curr_row = get_next_row(curr_row,new_orientation);
        // show_maze_with_path();
        orientation = new_orientation;
        robot_path[curr_row][curr_col] = i+1;
        // output_stream->println(orientation);
        // output_stream->println(open_path.turn_list[i]);

    }
    
}

void Maze::create_path_closed()
{
    //search for the shortest path to the centre 
    unsigned char path_length = flood_filled_closed[start_loc[0]][start_loc[1]];
    unsigned char curr_row = start_loc[0];
    unsigned char curr_col = start_loc[1];
    

    if(closed_path.turn_list != NULL){
        delete [] closed_path.turn_list;
        closed_path.turn_list = NULL;
    }

    if(path_length == CELL_MAX){
        output_stream->println("WARNING Path to Center for Closed is Unavailable");
        closed_path.turn_count  = TURN_MAX;
        closed_path.step_count = CELL_MAX;
        return;
    }

    

    closed_path.turn_list = new unsigned char[path_length];
    closed_path.turn_count = TURN_MAX;
    closed_path.step_count = path_length;

    if(closed_path.turn_list == nullptr){
        output_stream->println("WARNING CLOSED PATH WASN'T INSTANTIATED");
        return;
    }
    
    depth_first_search_start_closed();
    
    //Fill Robot Path
    for (int row = 0; row < MAZE_ROWS; row++){
        for(int col = 0; col < HORZ_WALLS; col++){
            robot_path[row][col] = CELL_MAX;
        }
    }

    unsigned char orientation = iorientation;
    curr_row = start_loc[0];
    curr_col = start_loc[1];
    robot_path[curr_row][curr_col] = 0;
    for (int i = 0; i < closed_path.step_count; i++){
        unsigned char new_orientation = (orientation+closed_path.turn_list[i])%4;
        curr_col = get_next_col(curr_col,new_orientation);
        curr_row = get_next_row(curr_row,new_orientation);
        // show_maze_with_path();
        orientation = new_orientation;
        robot_path[curr_row][curr_col] = i+1;
        // output_stream->println(orientation);
        // output_stream->println(closed_path.turn_list[i]);

    }

}

bool Maze::is_explored()
{
    if(flood_filled_open[start_loc[0]][start_loc[1]] < flood_filled_closed[start_loc[0]][start_loc[1]])
    {
        return false;
    }

    if(flood_filled_open[start_loc[0]][start_loc[1]] == flood_filled_closed[start_loc[0]][start_loc[1]] && open_path.turn_count < closed_path.turn_count){
        return false;
    }

    return true;
}

void Maze::depth_first_search_start_closed()
{
    if(temp_path.turn_list != NULL){
        delete [] temp_path.turn_list;
        temp_path.turn_list = NULL;
    }

    unsigned char path_length = flood_filled_closed[start_loc[0]][start_loc[1]];

    temp_path.turn_list = new unsigned char[path_length];
    temp_path.turn_count = 0;
    temp_path.step_count = path_length;

    if(temp_path.turn_list == NULL){
        output_stream->println("WARNING TEMP PATH WASN'T INSTANTIATED");
        return;
    }
    curr_orientation = iorientation;
    depth_first_search_closed(start_loc[0],start_loc[1]);
}

void Maze::depth_first_search_closed(unsigned char curr_row, unsigned char curr_col)
{
    if(curr_row == dest_loc[0] && curr_col == dest_loc[1]){
        update_best_path_closed();
        return;  
    }
    
    unsigned char steps_away = flood_filled_closed[curr_row][curr_col];
    unsigned char prev_orientation = curr_orientation;
    unsigned char path_pos = flood_filled_closed[start_loc[0]][start_loc[1]] - steps_away;
    unsigned char prev_turn_count = temp_path.turn_count;
    // output_stream->print("*** ");
    // output_stream->println(path_pos);
    

    //Check North
    if (curr_row != 0 && flood_filled_closed[curr_row-1][curr_col] == steps_away -1){
        //check if there is a wall
        if(horz_walls[curr_row-1][curr_col] == 0){
            curr_orientation = NORTH;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_closed(curr_row-1,curr_col);
        }
    }

    //Check East
    if (curr_col != MAZE_COLS  && flood_filled_closed[curr_row][curr_col+1] == steps_away -1){
        //check if there is a wall
        if(vert_walls[curr_row][curr_col] == 0){
            curr_orientation = EAST;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_closed(curr_row,curr_col+1);
        }
    }
    //Check South
    if (curr_row != MAZE_ROWS  && flood_filled_closed[curr_row+1][curr_col] == steps_away -1){
        //check if there is a wall
        if(horz_walls[curr_row][curr_col] == 0){
            curr_orientation = SOUTH;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_closed(curr_row+1,curr_col);
        }
    }

    //Check West
    if (curr_col != 0  && flood_filled_closed[curr_row][curr_col-1] == steps_away -1){
        //check if there is a wall
        if(vert_walls[curr_row][curr_col-1] == 0){
            curr_orientation = WEST;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_closed(curr_row,curr_col-1);

        }
    }
    return;
                
}

void Maze::depth_first_search_start_open()
{
    if(temp_path.turn_list != NULL){
        delete [] temp_path.turn_list;
        temp_path.turn_list = NULL;
    }

    unsigned char path_length = flood_filled_open[start_loc[0]][start_loc[1]];

    temp_path.turn_list = new unsigned char[path_length];
    temp_path.turn_count = 0;
    temp_path.step_count = path_length;

    if(temp_path.turn_list == NULL){
        output_stream->println("WARNING TEMP PATH WASN'T INSTANTIATED");
        return;
    }
    curr_orientation = iorientation;
    depth_first_search_open(start_loc[0],start_loc[1]);
}

void Maze::depth_first_search_open(unsigned char curr_row, unsigned char curr_col)
{
    if(curr_row == dest_loc[0] && curr_col == dest_loc[1]){
        update_best_path_open();
        return;  
    }
    
    unsigned char steps_away = flood_filled_open[curr_row][curr_col];
    unsigned char prev_orientation = curr_orientation;
    unsigned char path_pos = flood_filled_open[start_loc[0]][start_loc[1]] - steps_away;
    unsigned char prev_turn_count = temp_path.turn_count;
    // output_stream->print("*** ");
    // output_stream->println(path_pos);
    

    //Check North
    if (curr_row != 0 && flood_filled_open[curr_row-1][curr_col] == steps_away -1){
        //check if there is a wall
        if(horz_walls[curr_row-1][curr_col] != 1){
            curr_orientation = NORTH;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_open(curr_row-1,curr_col);
        }
    }

    //Check East
    if (curr_col != MAZE_COLS  && flood_filled_open[curr_row][curr_col+1] == steps_away -1){
        //check if there is a wall
        if(vert_walls[curr_row][curr_col] != 1){
            curr_orientation = EAST;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_open(curr_row,curr_col+1);
        }
    }
    //Check South
    if (curr_row != MAZE_ROWS  && flood_filled_open[curr_row+1][curr_col] == steps_away -1){
        //check if there is a wall
        if(horz_walls[curr_row][curr_col] != 1){
            curr_orientation = SOUTH;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_open(curr_row+1,curr_col);
        }
    }

    //Check West
    if (curr_col != 0  && flood_filled_open[curr_row][curr_col-1] == steps_away -1){
        //check if there is a wall
        if(vert_walls[curr_row][curr_col-1] != 1){
            curr_orientation = WEST;
            unsigned char turns = required_turns(prev_orientation,curr_orientation);
            unsigned char turn_dir = turn_direction(prev_orientation,curr_orientation);
            temp_path.turn_count = prev_turn_count+turns;
            temp_path.turn_list[path_pos] = turn_dir;
            depth_first_search_open(curr_row,curr_col-1);

        }
    }
    return;
                
}

unsigned char required_turns(unsigned char prev_orientation, unsigned char curr_orientation)
{
    unsigned char turns = 0;
    if(prev_orientation == curr_orientation){
        turns = 0;
    } else if ((prev_orientation+1)%4 == curr_orientation){
        turns = 1;
    } else if ((prev_orientation+3)%4 == curr_orientation){
        turns = 1;
    } else {
        turns = 2;
    }
    return turns;
}

unsigned char turn_direction(unsigned char prev_orientation, unsigned char curr_orientation)
{
    unsigned char turn = 0;
    if(prev_orientation == curr_orientation){
        turn = FORWARD;
    } else if ((prev_orientation+1)%4 == curr_orientation){
        turn = RIGHT_SIDE;
    } else if ((prev_orientation+3)%4 == curr_orientation){
        turn = LEFT_SIDE;
    } else {
        turn = FULLROTATE;
    }
    return turn;
}

unsigned char Maze::get_next_row(unsigned char row, unsigned char orientaiton)
{
    switch (orientaiton)
    {
    case NORTH:
        return row-1;
        break;
    case SOUTH:
        return row+1;
        break;
    default:
        return row;
        break;
    }
}

unsigned char Maze::get_next_col(unsigned char col, unsigned char orientaiton)
{
    
    switch (orientaiton)
    {
    case EAST:
        return col+1;
        break;
    case WEST:
        return col-1;
        break;
    default:
        return col;
        break;
    }
}

void Maze::update_best_path_closed()
{
    if(temp_path.turn_count < closed_path.turn_count){
        for(int i = 0; i < closed_path.step_count; i++){
            closed_path.turn_list[i] = temp_path.turn_list[i];
            closed_path.turn_count = temp_path.turn_count;
        }
    }
    //output_stream->println("CLOSED ROUTE TAKEN");

}

void Maze::update_best_path_open()
{
    if(temp_path.turn_count < open_path.turn_count){
        for(int i = 0; i < open_path.step_count; i++){
            open_path.turn_list[i] = temp_path.turn_list[i];
            open_path.turn_count = temp_path.turn_count;
            //output_stream->println(int(closed_path.turn_list[i]))   ;
        }
    }
}



//Idea
/*
    Recurs through all the possible paths 
    store this path as you go 
    when centre reached call a funciton to check if this is the best path 

    PseudoCode:
        if Center
            see if current built path is best 
            return out 
        
        //take all possible directions 
        if North is posssible
            current orientaiton = North
            get how many turns from current orientation to new orientation
            path turn count = prev path turn count + number of turns 
            path position i = right, forward or left turn 


 */
