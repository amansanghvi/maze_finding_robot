#include "maze.h"
#define VISITED 0
#define UNVISITED -1
#define SEEN -2
#define ALTERNATIVE 1

#define NUM_SIDES 4

#define NUM_ROWS 5
#define NUM_COLS 9
#define MAX_DIM NUM_COLS
#define NUM_NODES NUM_COLS*NUM_COLS

#define CORRECT 1
#define CORRECT_NEG 2
#define WRONG 0
#define WRONG_NEG 3
#define UNDETERMINED -1

// (Direction + side) % 4 = new Direction
#define NEW_DIRECTION(direction, side) (enum Direction)(((int)direction + (int)side) % NUM_SIDES)
#define HASH_COORD(coord) (coord.row >= 0 ? coord.row*NUM_COLS + coord.col : -coord.row*NUM_COLS + coord.col)

typedef int16_t num;

struct __attribute__ ((packed))  Coord {
  num row;
  num col;
};

struct __attribute__ ((packed)) Edge {
  Coord dest;
  bool visited;
};

struct __attribute__ ((packed)) Node {
  num value;
  num row;
  num col;
  Edge edges[NUM_SIDES];
};

// Clockwise ordering
enum class Direction {
  RIGHT = 0,
  DOWN  = 1,
  LEFT  = 2,
  UP    = 3,
  NONE  = -1,
};

// # of clockwise rotations
enum class Side {
  FRONT = 0,
  RIGHT = 1,
  BACK  = 2,
  LEFT  = 3,
  NONE  = -1,
};

class Map {
  private:
    Coord pos = {0, 0};
    enum Direction direction = Direction::RIGHT;
    num orientation = UNDETERMINED;
    int is_neg = UNDETERMINED;
    int move_counter = 0;
    // Size = 26*9*9 = 2106kB
    void (*init_pos)(void);
    void (*rotate_to_side)(Side);
    void (*drive_forward)(void);
    bool (*wall_exists)(int, int);
    enum Side (*get_priority_side)(int);
    Node map[NUM_COLS][NUM_COLS];
  public:
    Map();
    void set_func(void(*init_pos)(void), void(*rotate_to_side)(enum Side), void(*drive_forward)(void), bool (*wall_exists)(int, int), enum Side (*get_priority_side)(int));
    void explore();
    void go_to_nearest_unvisited();
    void go_to_centre();
    void go_to_pos(Coord dest);
    void go_to_pos_and_explore(Coord dest);
    void move_forward_once();

    bool is_centre_found(int orientation);
    void print();
    void print_details();
    Maze *create_maze(Coord dest);

    void add_node(Coord pos, enum Direction direction);
    void set_back_edge(Coord pos, Coord val, enum Direction direction);
    void mark_edge_visited(Node *node, int edge);
    void decrement_value(Coord pos);
    void increment_value(Coord pos);
    void update_value(Coord pos, int new_value);

    Node *get_node(Coord pos);
    int get_path(Coord start, Coord end, Coord *path);
    num get_next_row(num row, enum Direction curr_dir, int side);
    num get_next_col(num row, enum Direction curr_dir, int side);
    num get_dist_from_centre(int orientation, Coord from);
};
