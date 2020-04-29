#include <arduino.h>

#define MAZE_ROWS 5
#define MAZE_COLS 9
#define NUM_DRAW_ROWS 11
#define NUM_DRAW_COLS 19
#define VERT_WALLS 8
#define HORZ_WALLS 9
#define CELL_MAX 99
#define VIST_IN_PROGRESS 100
#define TURN_MAX 99
#define TOTAL_HORZ_WALLS 36
#define TOTAL_VERT_WALLS 40


#define VERT_ROW_LEN  5
#define VERT_COL_LEN  8
#define HORZ_ROW_LEN  4
#define HORZ_COL_LEN  9

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define FORWARD 0
#define RIGHT_SIDE 1
#define FULLROTATE 2
#define LEFT_SIDE 3


struct __attribute__ ((packed)) robot_path
{
    unsigned char* turn_list = nullptr;
    char turn_count = 0;
    char step_count = 0;
};



class Maze {
private:
    unsigned char vert_walls[5][8] = {{0}};
    unsigned char horz_walls[4][9] = {{0}};
    unsigned char iorientation;
    unsigned char curr_orientation;
    unsigned char start_loc[2];
    unsigned char dest_loc[2] = {2, 4};
    unsigned char flood_filled_open[5][9];
    unsigned char flood_filled_closed[5][9];
    unsigned char robot_path[5][9];
    struct robot_path open_path;
    struct robot_path temp_path;
    struct robot_path closed_path;
    Stream* output_stream;

    void calc_closed_path(unsigned char orientation);
    void calc_open_path(unsigned char orientation);
    void depth_first_search_closed(unsigned char curr_row, unsigned char curr_col);
    void depth_first_search_start_closed();
    void depth_first_search_open(unsigned char curr_row, unsigned char curr_col);
    void depth_first_search_start_open();
    void update_best_path_closed();
    void update_best_path_open();
    unsigned char get_next_row(unsigned char row,unsigned char direction);
    unsigned char get_next_col(unsigned char col,unsigned char direction);

    
public:
    Maze(char _vert_walls[][8],char _horz_walls[][9], unsigned char _orientation, unsigned char _start_loc[2], unsigned char _dest_loc[2] );
    Maze(char _vert_walls[][8],char _horz_walls[][9], unsigned char _orientation, unsigned char _start_loc[2]);
    ~Maze();
    void show_maze();
    void show_maze_with_path();
    void show_open_directions();
    void show_closed_directions();
    void show_flood_fill_open();
    void show_flood_fill_closed();

    void set_vert_walls(char _vert_walls[][8]);
    void set_horz_walls(char _horz_walls[][9]);
    void set_orientation(unsigned char _orientation);
    void set_location(unsigned char _start_loc[2]);
    void set_serial(Stream* _output_stream);
   
    struct robot_path get_turn_list(bool get_open = true);
    void flood_fill_open();
    void flood_fill_closed();
    void create_path_open();
    void create_path_closed();
    bool is_explored();
    
};

unsigned char required_turns(unsigned char prev_orientation, unsigned char curr_orientation);
unsigned char turn_direction(unsigned char prev_orientation, unsigned char curr_orientation);
