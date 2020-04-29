#include <Arduino.h>
#include <QueueArray.h>
#include "map.h"

#define SERIAL Serial2

using namespace std;

/* 5 x 9 
 *  _ _ _ _ _ _ _ _ _ 
 * |S|    _ _ _ _  | |
 * | |_| |_ _ _ _ _| |
 * |  _ _   #  |     |
 * |_|  _ _ _  |     |
 * |_ _ _ _ _|_|_ _ _|
 */

void rotate_until_left_wall_and_no_front_wall();
void rotate_to_side(Side next_side);
void drive_forward();
bool wall_exists(int side);
void print_node(Node node);
//std::ostream& operator << (std::ostream& o, const Node& a);
//std::ostream& operator << (std::ostream& o, const Direction& dir);

void print_pos(Coord pos) {
//    SERIAL.print("AT (");
//    SERIAL.print(pos.row);
//    SERIAL.print(", ");
//    SERIAL.print(pos.col);
//    SERIAL.println(");");
}

Map::Map() {
    for (int i = 0; i < MAX_DIM; i++) {
        for(int j = 0; j < MAX_DIM; j++) {
            map[i][j].value = UNVISITED;
            map[i][j].row = i;
            map[i][j].col = j;
            for(int k = 0; k < NUM_SIDES; k++) {
                map[i][j].edges[k].dest.row = -1;
                map[i][j].edges[k].dest.col = -1;
                map[i][j].edges[k].visited = false;
            }
        }
    }
    SERIAL.println("MAP init complete");
}

void Map::set_func(void(*init_pos)(void), void(*rotate_to_side)(enum Side), void(*drive_forward)(void), bool (*wall_exists)(int, int), enum Side (*get_priority_side)(int)) {
    this->init_pos = init_pos;
    this->rotate_to_side = rotate_to_side;
    this->drive_forward = drive_forward;
    this->wall_exists = wall_exists;
    this->get_priority_side = get_priority_side;
}

void Map::explore() {
    SERIAL.println("STARTING EXPLORATION");
    init_pos();
    add_node(pos, direction);
    while(!is_centre_found(orientation)) { // Exploration
        Node *curr_node = get_node(pos);
        Node next_node;
        Side next_side = Side::NONE;
        bool seen = false;
        num shortest_dist = 100;
        for (int i = 0; i < NUM_SIDES; i++) { 
            // Explores sides near current position.
            // Gets FRONT, RIGHT, LEFT then BACK. 
            // Chooses the first node that's "SEEN", otherwise will go to a node that is visited but has potential paths
            // Otherwise chooses a node where the edge hasn't been traversed (one way).
            // Will never go to a node with value "VISITED" since no potential paths exist.
            // If orientation is determined prefers points with a closer distance to the centre.
            Side potential_side;
            potential_side = get_priority_side(i);
            int next_direction = (int)NEW_DIRECTION(direction, potential_side);
            if (curr_node->edges[next_direction].dest.col != -1 && !curr_node->edges[next_direction].visited) {
                next_node = *get_node(curr_node->edges[next_direction].dest);
                if (orientation == UNDETERMINED) {
                    if (next_node.value == SEEN) {
                        next_side = potential_side;
                        break;
                    } else if (next_node.value != VISITED) {
                        if (next_side == Side::NONE) {
                            next_side = potential_side;
                        }
                   }
                } else {
                    if (next_node.value == SEEN) {
                        if (!seen) {
                            shortest_dist = get_dist_from_centre(orientation, curr_node->edges[next_direction].dest);
//                            cout << "SHORT: " << shortest_dist << (int)potential_side << " FROM: " << curr_node->edges[next_direction].dest << endl;
                            next_side = potential_side;
                            seen = true;
                        } else {
                            num curr_dist = get_dist_from_centre(orientation, curr_node->edges[next_direction].dest);
                            next_side = (curr_dist < shortest_dist) ? potential_side : next_side;
                            shortest_dist = (curr_dist < shortest_dist) ? curr_dist : shortest_dist;
                        }
                    } else if (!seen && next_node.value != VISITED) {
                        if (next_side == Side::NONE) {
                            next_side = potential_side;
                        }
                    }
                }
            }
        }
        if (next_side == Side::NONE) {
            Node *curr = get_node(pos);
            if (orientation == UNDETERMINED) {
                go_to_nearest_unvisited();
            } else {
                break;
            }
        }

        rotate_to_side(next_side); // TODO: Change this!
        direction = NEW_DIRECTION(direction, next_side);
        
        drive_forward();
        move_forward_once();
        
//        SERIAL.println("########################\n"); 
//            << "END OF ITERATION " << move_counter << "\nOrientation: " << orientation << endl
//            << "Direction: " << direction << " at (" << pos.row << ", " << pos.col << ")\n"
//            << "########################\n"; 
    }
}

void Map::move_forward_once() {
    Node *curr_node = get_node(pos);
    Coord next_pos = {get_next_row(pos.row, direction, (int)Side::FRONT), get_next_col(pos.col, direction, (int)Side::FRONT)};
    Node *next_node = get_node(next_pos);
//    cout << "Now at: " << *next_node << endl;
    decrement_value(pos); // We are going to explore one of its points.
    mark_edge_visited(curr_node, (int)direction);

    if (is_neg == -1 && next_node->row > 0) {
        is_neg = 0;
    } else if (is_neg == -1 && next_node->row < 0) {
        is_neg = 1;
    }

    Coord last_coord = pos;
    pos.row = next_node->row; 
    pos.col = next_node->col;

    if (next_node->value == UNVISITED || next_node->value == SEEN) {
        add_node(pos, direction);
        if (get_node(last_coord)->value != VISITED) {
            increment_value(pos);
            set_back_edge(pos, last_coord, direction);
        }
    }
    // print_node(*get_node(pos));
    
    if (orientation == UNDETERMINED && is_neg != -1 && 
        ((pos.row >= NUM_ROWS || pos.row <= -NUM_ROWS) || 
            (pos.col >= NUM_ROWS || pos.col <= -NUM_ROWS))) {
        if (pos.row >= NUM_ROWS || pos.row <= -NUM_ROWS) {
            orientation = is_neg ?  WRONG_NEG : WRONG;
        } else {
            orientation = is_neg ? CORRECT_NEG : CORRECT;
        }
    }
}

void Map::go_to_centre() {
    Coord centre; 
    go_to_pos({NUM_ROWS/2, NUM_COLS/2});
}

void Map::go_to_nearest_unvisited() {
    Node *dest_node;
    for (int i = 0; i < NUM_ROWS; i++) { // Only calling this when a 4x4 or less is explored
        for (int j = 0; j < NUM_ROWS; j++) {
               if (map[i][j].value == SEEN) {
                    dest_node = get_node({i, j});
                    break;
               }
        }
    }
    go_to_pos({dest_node->row, dest_node->col});
}

void Map::go_to_pos(Coord dest) {

    Node *dest_node = (orientation == WRONG || orientation == WRONG_NEG) ? get_node({dest.col, dest.row}) : get_node(dest); // Undetermined goes to normal dest.
    Coord path[NUM_ROWS*NUM_COLS];
    if (pos.row == dest_node->row && pos.col == dest_node->col) {
        SERIAL.println("DONE!!!");
        return;
    }
     
    int steps = get_path(pos, {dest_node->row, dest_node->col}, path);
    for (int i = steps -2; i >= 0; i--) {
        Direction next_dir;
        Node *curr_node = get_node(pos);
        for(int k = 0; k < NUM_SIDES; k++) {
            if (curr_node->edges[k].dest.row == path[i].row && curr_node->edges[k].dest.col == path[i].col) {
                next_dir = (Direction)k;
                break;
            }
        }
        int next_side = (NUM_SIDES + (int)next_dir - (int)direction) % NUM_SIDES;
        rotate_to_side((Side)next_side);
        direction = next_dir;
        drive_forward();
        pos = path[i];
    }
    Node *node = get_node(pos);
    if (node->value == SEEN) {
        add_node(pos, direction);
    }
    if (pos.row != dest.row && pos.col != dest.col) {
        go_to_pos(dest);
    }
    SERIAL.print("Reached centre!!! Direction: "); SERIAL.println((int)direction);
    
}

void Map::go_to_pos_and_explore(Coord dest) {
    Node *dest_node = (orientation == CORRECT || orientation == CORRECT_NEG) ? get_node(dest) : get_node({dest.col, dest.row});
    while (!(pos.row == dest_node->row && pos.col == dest_node->col)) {
        SERIAL.println("AAAAAAAAAA");
        Maze *maze = create_maze({dest_node->row, dest_node->col});
        struct robot_path path = maze->get_turn_list();
        maze->show_maze_with_path();
        SERIAL.print("Created path with len: "); SERIAL.println((int)path.step_count);
        for (int i = 0; i < path.step_count; i++) {
            Side new_side = (Side)path.turn_list[i];
            Coord new_pos = {get_next_row(pos.row, direction, (int)new_side), get_next_col(pos.col, direction, (int)new_side)};
            
            rotate_to_side(new_side);
            direction = NEW_DIRECTION(direction, new_side);
            drive_forward();
            pos = new_pos;
            
            if (get_node(pos)->value < 0) { // Reset everything if SEEN or UNVISITED
                add_node(pos, direction);
                SERIAL.println("RESETTING 2"); SERIAL.print(pos.row); SERIAL.print(", "); SERIAL.println(pos.col);
                SERIAL.println("New destination 2"); SERIAL.print(dest_node->row); SERIAL.print(", "); SERIAL.println(dest_node->col);
                break;
            }
        }
        delete maze;
        SERIAL.println("RESETTING "); SERIAL.print(pos.row); SERIAL.print(", "); SERIAL.println(pos.col);
        SERIAL.println("New destination "); SERIAL.print(dest_node->row); SERIAL.print(", "); SERIAL.println(dest_node->col);
    }
   
}

void Map::add_node(Coord node_pos, enum Direction direction) {
    SERIAL.print("Adding node "); SERIAL.println(move_counter);
    int num_paths = 0;
    Node *curr_node = get_node(node_pos);

//    SERIAL.print(curr_node->row); SERIAL.print(','); SERIAL.println(curr_node->col);
    
    for (int i = 0; i < NUM_SIDES; i++) { // Should not read Side::BACK
        Coord dest = {get_next_row(node_pos.row, direction, i), get_next_col(node_pos.col, direction, i)};
        Node *dest_node = get_node(dest);
        if (!wall_exists(i, move_counter)) {
            if (is_neg == -1 && dest.row != 0) {
                if (dest.row < 0) {
                    is_neg = 1;
                } else {
                    is_neg = 0;
                }
//                cout << "IS NEG: " << is_neg << endl;
            }
            if (orientation == UNDETERMINED && is_neg != -1) {
                if (dest.row >= NUM_ROWS || dest.row <= - NUM_ROWS) {
                    orientation = is_neg ? WRONG_NEG : WRONG;
                } else if (dest.col >= NUM_ROWS || dest.col <= -NUM_ROWS) {
                    orientation = is_neg ? CORRECT_NEG : CORRECT;
                }
            }
            
            if ((dest_node->value != VISITED)) {
                dest_node->row = dest.row;
                dest_node->col = dest.col;
                if (i == (int)Side::BACK) {
                    continue; // For back, wall never exists except for start.
                }
                num_paths++;
                int edge_dir = (int)NEW_DIRECTION(direction, i);
//                SERIAL.print("Dest node: "); SERIAL.println(dest_node->row); SERIAL.print(','); SERIAL.println(dest_node->col);
//                SERIAL.print("Dest node direction: "); SERIAL.println((edge_dir + NUM_SIDES/2) % NUM_SIDES);
                curr_node->edges[edge_dir].dest.row = dest.row;
                curr_node->edges[edge_dir].dest.col = dest.col;
                dest_node->edges[(edge_dir + NUM_SIDES/2) % NUM_SIDES].dest.row = node_pos.row;
                dest_node->edges[(edge_dir + NUM_SIDES/2) % NUM_SIDES].dest.col = node_pos.col;
                if (dest_node->value == UNVISITED) {
                    decrement_value(dest); // It is SEEN now
                }
            }
        }    
    }
    curr_node->row = pos.row;
    curr_node->col = pos.col;
    curr_node->value = num_paths;
    move_counter++;
}

void Map::set_back_edge(Coord pos, Coord val, enum Direction direction) {
    int edge_dir = (int)NEW_DIRECTION(direction, Side::BACK);
    Node *curr_node = get_node(pos);
    curr_node->edges[edge_dir].dest.row = val.row;
    curr_node->edges[edge_dir].dest.col = val.col; 
}

num Map::get_dist_from_centre(int orientation, Coord from) {
    if (orientation == CORRECT) {
        return (from.row - NUM_ROWS/2)*(from.row- NUM_ROWS/2) + (from.col- NUM_COLS/2)*(from.col- NUM_COLS/2);
    } else if (orientation == CORRECT_NEG) {
        return (from.row + NUM_ROWS/2)*(from.row + NUM_ROWS/2) + (from.col - NUM_COLS/2)*(from.col - NUM_COLS/2);
    } else if (orientation == WRONG) {
        return (from.row - NUM_COLS/2)*(from.row- NUM_COLS/2) + (from.col- NUM_ROWS/2)*(from.col- NUM_ROWS/2);
    } else if (orientation == WRONG_NEG) {
        return (from.row + NUM_COLS/2)*(from.row + NUM_COLS/2) + (from.col- NUM_ROWS/2)*(from.col- NUM_ROWS/2);
    }
    return 100;
}

void Map::decrement_value(Coord pos) {
    get_node(pos)->value--;
}

void Map::increment_value(Coord pos) {
    get_node(pos)->value++;
}

Node *Map::get_node(Coord pos) {
    if (pos.row >= 0) {
        return &map[pos.row][pos.col];
    } else {
        return &map[-pos.row][pos.col];
    }
}

void Map::update_value(Coord pos, int new_value) {
    get_node(pos)->value = new_value;
}

void Map::mark_edge_visited(Node *node, int edge) {
    node->edges[edge].visited = true;
}

bool Map::is_centre_found(int orientation) {
    return ((orientation == CORRECT || orientation == CORRECT_NEG) && map[NUM_ROWS/2][NUM_COLS/2].value != UNVISITED) ||
        ((orientation == WRONG || orientation == WRONG_NEG) && map[NUM_COLS/2][NUM_ROWS/2].value != UNVISITED);
}

num Map::get_next_row(num row, enum Direction curr_dir, int side) {
    enum Direction next_direction = NEW_DIRECTION(curr_dir, side);
    switch (next_direction) {
    case Direction::UP:
        return row - 1;
        break;
    case Direction::DOWN:
        return row + 1;
    default:
        return row;
    }
}

num Map::get_next_col(num col, enum Direction curr_dir, int side) {
    enum Direction next_direction = NEW_DIRECTION(curr_dir, side);
    switch (next_direction) {
    case Direction::LEFT:
        return col - 1;
        break;
    case Direction::RIGHT:
        return col + 1;
    default:
        return col;
    }
}

void print_node(Node node) {
    SERIAL.print("PRINTING NODE ("); SERIAL.print(node.row); SERIAL.print(", "); SERIAL.print(node.col); SERIAL.println(")");
    SERIAL.print("VALUE: "); SERIAL.println(node.value);
    SERIAL.println("EDGES: ");
    for (int i = 0; i < NUM_SIDES; i++) {
        SERIAL.print("EDGE "); SERIAL.print(i); SERIAL.print(" with status: "); 
        SERIAL.print(node.edges[i].visited); SERIAL.print(" has coord: ("); SERIAL.print(node.edges[i].dest.row); SERIAL.print(", "); SERIAL.print(node.edges[i].dest.col); SERIAL.print(")\n");
    } 
    SERIAL.println("END node");
}

int Map::get_path(Coord start, Coord end, Coord *path) {
    QueueArray<Coord> q;
    Node *dest = get_node(end);
    q.push(start);
    Coord visited[NUM_NODES];
    for (int i = 0; i < NUM_NODES; i++) {
        visited[i].col = -1;
    }
    visited[HASH_COORD(start)].col = -2;

    while (!q.isEmpty() && visited[HASH_COORD(end)].col == -1) {
        Node *curr_node = get_node(q.pop());
        for (int i = 0; i < NUM_SIDES; i++) {
            if (curr_node->edges[i].dest.col != -1 && visited[HASH_COORD(curr_node->edges[i].dest)].col == -1) {
                visited[HASH_COORD(curr_node->edges[i].dest)].row = curr_node->row;
                visited[HASH_COORD(curr_node->edges[i].dest)].col = curr_node->col;
                q.push(curr_node->edges[i].dest);
                if (dest->row == curr_node->edges[i].dest.row && dest->col == curr_node->edges[i].dest.col) {
                    break;
                }
            }
        }
    }

    if (visited[HASH_COORD(end)].col == -1) {
        return -1;
    }

    int steps = 0;
    path[0].row = dest->row;
    path[0].col = dest->col;

    for (steps = 1; path[steps-1].col != -2; steps++) {
        path[steps].row = visited[HASH_COORD(path[steps - 1])].row;
        path[steps].col = visited[HASH_COORD(path[steps - 1])].col;
    }

    return steps -1;
}

void Map::print() {
    int row_lim = (orientation == CORRECT || orientation == CORRECT_NEG) ? NUM_ROWS : NUM_COLS;
    int col_lim = (orientation == CORRECT || orientation == CORRECT_NEG) ? NUM_COLS : NUM_ROWS;

    bool is_negative = false;
    for (int j = 0; j < col_lim; j++) {
        if (map[0][j].edges[(int)Direction::UP].dest.col != -1) {
            is_negative = true;
            break;
        }
        if (map[0][j].edges[(int)Direction::DOWN].dest.col != -1) {
            is_negative = false;
            break;
        }
    }

    SERIAL.println();
    int col_start = is_negative ? col_lim -1 : 0;
    col_lim = is_negative ? 0 : col_lim -1;
    int col_inc = is_negative ? -1 : 1;
    for (int i = 0; i < row_lim*2; i++) {
        if (i == 0) {
            for (int k = 0; k <= col_lim + col_start; k++) {
                SERIAL.print(" ---");
            }
            SERIAL.println();
        }
        for (int j = col_start; 0 <= j && j <= col_lim + col_start; j += col_inc) {
            if (i%2 == 0) {
                
                if ((!is_negative && map[i/2][j].edges[(int)Direction::LEFT].dest.col == -1) ||
                    (is_negative && map[i/2][j].edges[(int)Direction::RIGHT].dest.col == -1)) {
                    if (j == col_start || map[i/2][j].value >= 0 || map[i/2][j-col_inc].value >= 0) {
                        SERIAL.print("|");
                    } else {
                        SERIAL.print("*");
                    }
                } else {
                    SERIAL.print(" ");
                }
                
                if (i/2 == 0 && j == 0) {
                    SERIAL.print(is_negative ? " W " : " E ");
                } else if (i/2 == row_lim/2 && j == (col_lim + col_start)/2) {
                    SERIAL.print(" X ");
                } else {
                    SERIAL.print("   ");
                }
                
                if (j == col_lim) {
                    SERIAL.print("|");
                }
            } else {
                if ((!is_negative && map[i/2][j].edges[(int)Direction::DOWN].dest.col == -1) ||
                    (is_negative && map[i/2][j].edges[(int)Direction::UP].dest.col == -1)) {
                    if (i/2 == row_lim -1 || map[i/2][j].value >= 0 || map[i/2 + 1][j].value >= 0) {
                        SERIAL.print(" ---");
                    } else {
                        SERIAL.print(" ***");
                    }
                } else {
                    SERIAL.print("    ");
                }
            }
        }
        SERIAL.println();
    }
}

Coord ideal_location_to_map(int orientation, Coord dest) {
    Node *map_dest = (orientation == CORRECT || orientation == WRONG) ? get_node(dest) ? get_node({dest.col, dest.row});
    if (orientation == CORRECT) {
        return dest;
    } else if (orientation == CORRECT_NEG) {
        return 
    } else if (orientation == WRONG) {
        
    } else if (orientation == WRONG_NEG) {
        return 
    }
}
Maze *Map::create_maze(Coord dest) {
    SERIAL.println("Creating maze");
    int row_lim, col_lim = 0;
    char vert_walls[VERT_ROW_LEN][VERT_COL_LEN] = {{0}};
    char horz_walls[HORZ_ROW_LEN][HORZ_COL_LEN] = {{0}};

    if (orientation == CORRECT || orientation == CORRECT_NEG) {
        row_lim = NUM_ROWS;
        col_lim = NUM_COLS;
    } else {
        row_lim = NUM_COLS;
        col_lim = NUM_ROWS;
    }
    
    bool is_negative = (orientation == CORRECT || orientation == WRONG) ? false : true;
    int horz_wall_count = 0;
    int vert_wall_count = 0;
    Coord start = {0, 0};
    Coord end = dest;
    unsigned char maze_dir = ((int)direction + 1)%NUM_SIDES;
    SERIAL.print("Entering loop w orientation: "); SERIAL.println(orientation);
    if (orientation == CORRECT) {
        for (int i = 0; i < NUM_ROWS; i++) {
            for (int j = 0; j < NUM_COLS; j++) {
                if (j != NUM_COLS-1) {
                    if (map[i][j].edges[(int)Direction::RIGHT].dest.col != -1) { // It has a connection to another node
                        vert_walls[vert_wall_count/VERT_COL_LEN][vert_wall_count%VERT_COL_LEN] = 0;
                    } else {
                        if (map[i][j].value >= 0 || map[i][j+1].value >= 0) {
                            vert_walls[vert_wall_count/VERT_COL_LEN][vert_wall_count%VERT_COL_LEN] = 1;
                        } else {
                            vert_walls[vert_wall_count/VERT_COL_LEN][vert_wall_count%VERT_COL_LEN] = 2;
                        }
                    }
                    vert_wall_count++;
                }
                if (i != NUM_ROWS-1) {
                    if (map[i][j].edges[(int)Direction::DOWN].dest.col != -1) {
                        horz_walls[horz_wall_count/HORZ_COL_LEN][horz_wall_count%HORZ_COL_LEN] = 0;
                    } else {
                        if (map[i][j].value >= 0 || map[i+1][j].value >= 0) {
                            horz_walls[horz_wall_count/HORZ_COL_LEN][horz_wall_count%HORZ_COL_LEN] = 1;
                        } else {
                            horz_walls[horz_wall_count/HORZ_COL_LEN][horz_wall_count%HORZ_COL_LEN] = 2;
                        }
                    }
                    horz_wall_count++;
                } 
            }
        }
        start = pos;
        end = dest;
    } else if (orientation == WRONG) {
        for (int i = 0; i < NUM_COLS; i++) {
            for (int j = 0; j < NUM_ROWS; j++) {
                if (j != NUM_ROWS-1) {
                    if (map[i][j].edges[(int)Direction::RIGHT].dest.col != -1) { // It has a connection to another node
                        horz_walls[HORZ_ROW_LEN-1 - horz_wall_count%HORZ_ROW_LEN][horz_wall_count/HORZ_ROW_LEN] = 0;    
                    } else {
                        if (map[i][j].value >= 0 || map[i][j+1].value >= 0) {
                            horz_walls[HORZ_ROW_LEN-1 - horz_wall_count%HORZ_ROW_LEN][horz_wall_count/HORZ_ROW_LEN] = 1;
                        } else {
                            horz_walls[HORZ_ROW_LEN-1 - horz_wall_count%HORZ_ROW_LEN][horz_wall_count/HORZ_ROW_LEN] = 2;
                        }
                    }
                    horz_wall_count++;
                }
                if (i != NUM_COLS-1) {
                    if (map[i][j].edges[(int)Direction::DOWN].dest.col != -1) {
                        vert_walls[VERT_ROW_LEN-1 - vert_wall_count%VERT_ROW_LEN][vert_wall_count/VERT_ROW_LEN] = 0;
                    } else {
                        if (map[i][j].value >= 0 || map[i+1][j].value >= 0) {
                            vert_walls[VERT_ROW_LEN-1 - vert_wall_count%VERT_ROW_LEN][vert_wall_count/VERT_ROW_LEN] = 1;    
                        } else {
                            vert_walls[VERT_ROW_LEN-1 - vert_wall_count%VERT_ROW_LEN][vert_wall_count/VERT_ROW_LEN] = 2;    
                        }
                    }
                    vert_wall_count++;
                } 
            }
        }
        maze_dir = ((int)direction)%NUM_SIDES;
        start = {NUM_ROWS-1-pos.col, pos.row};
        end = {NUM_ROWS-1-dest.col, dest.row};    
    } else if (orientation == CORRECT_NEG) {
        for (int i = -(NUM_ROWS-1); i <= 0; i++) {
            for (int j = 0; j < NUM_COLS; j++) {
                Coord curr = {(num)i, (num)j};
                Coord next_row = {(num)(i+1), (num)j};
                Coord next_col = {(num)i, (num)(j+1)};
                if (j != NUM_COLS-1) {
                    if (get_node(curr)->edges[(int)Direction::RIGHT].dest.col != -1) { // It has a connection to another node
                        vert_walls[vert_wall_count/VERT_COL_LEN][vert_wall_count%VERT_COL_LEN] = 0;
                    } else {
                        if (get_node(curr)->value >= 0 || get_node(next_col)->value >= 0) {
                            vert_walls[vert_wall_count/VERT_COL_LEN][vert_wall_count%VERT_COL_LEN] = 1;
                        } else {
                            vert_walls[vert_wall_count/VERT_COL_LEN][vert_wall_count%VERT_COL_LEN] = 2;
                        }
                    }
                    vert_wall_count++;
                }
                if (i != 0) {
                    if (get_node(curr)->edges[(int)Direction::DOWN].dest.col != -1) {
                        horz_walls[horz_wall_count/HORZ_COL_LEN][horz_wall_count%HORZ_COL_LEN] = 0;
                    } else {
                        if (get_node(curr)->value >= 0 || get_node(next_row)->value >= 0) {
                            horz_walls[horz_wall_count/HORZ_COL_LEN][horz_wall_count%HORZ_COL_LEN] = 1;
                        } else {
                            horz_walls[horz_wall_count/HORZ_COL_LEN][horz_wall_count%HORZ_COL_LEN] = 2;
                        }
                    }
                    horz_wall_count++;
                } 
            }
        }
        maze_dir = ((int)direction + 1)%NUM_SIDES;
        start = {NUM_ROWS+pos.row -1, pos.col};
        end = {NUM_ROWS+dest.row-1, dest.col};
    } else if (orientation == WRONG_NEG) {
        for (int i = -(NUM_COLS-1); i <= 0; i++) {
            for (int j = 0; j < NUM_ROWS; j++) {
                Coord curr = {(num)i, (num)j};
                Coord next_row = {(num)(i+1), (num)j};
                Coord next_col = {(num)i, (num)(j+1)};
                if (j != NUM_ROWS-1) {
                    if (get_node(curr)->edges[(int)Direction::RIGHT].dest.col != -1) { // It has a connection to another node
                        horz_walls[HORZ_ROW_LEN-1 - horz_wall_count%HORZ_ROW_LEN][horz_wall_count/HORZ_ROW_LEN] = 0;    
                    } else {
                        if (get_node(curr)->value >= 0 || get_node(next_col)->value >= 0) {
                            horz_walls[HORZ_ROW_LEN-1 - horz_wall_count%HORZ_ROW_LEN][horz_wall_count/HORZ_ROW_LEN] = 1;
                        } else {
                            horz_walls[HORZ_ROW_LEN-1 - horz_wall_count%HORZ_ROW_LEN][horz_wall_count/HORZ_ROW_LEN] = 2;
                        }
                    }
                    horz_wall_count++;
                }
                if (i != 0) {
                    if (get_node(curr)->edges[(int)Direction::DOWN].dest.col != -1) {
                        vert_walls[VERT_ROW_LEN-1 - vert_wall_count%VERT_ROW_LEN][vert_wall_count/VERT_ROW_LEN] = 0;
                    } else {
                        if (get_node(curr)->value >= 0 || get_node(next_row)->value >= 0) {
                            vert_walls[VERT_ROW_LEN-1 - vert_wall_count%VERT_ROW_LEN][vert_wall_count/VERT_ROW_LEN] = 1;    
                        } else {
                            vert_walls[VERT_ROW_LEN-1 - vert_wall_count%VERT_ROW_LEN][vert_wall_count/VERT_ROW_LEN] = 2;
                        }
                    }
                    vert_wall_count++;
                } 
            }
        }
        maze_dir = ((int)direction)%NUM_SIDES;
        start = {NUM_ROWS -1 - pos.col,NUM_COLS-1+pos.row };
        end = {NUM_ROWS - 1 - dest.col, NUM_COLS-1+dest.row};
    }
    SERIAL.print("Start: "); SERIAL.print(start.row); SERIAL.print(", "); SERIAL.println(start.col);
    SERIAL.print("Dest: "); SERIAL.print(end.row); SERIAL.print(", "); SERIAL.println(end.col);
    unsigned char maze_loc[2] = {start.row, start.col};
    unsigned char maze_dest[2] = {end.row, end.col};
    Maze *maze = new Maze(vert_walls, horz_walls, maze_dir, maze_loc, maze_dest);
    maze->set_serial(&SERIAL);
    maze->create_path_open();
    return maze;
//    show_maze(vert_walls, horz_walls);
}

void Map::print_details() {
    SERIAL.println("VISITED");
    for (int i = 0; i < NUM_COLS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            if (map[i][j].value == VISITED) {
                SERIAL.print(map[i][j].row); SERIAL.print("\t");
            }
        }
    }
    SERIAL.println("\nSEEN");;
    for (int i = 0; i < NUM_COLS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            if (map[i][j].value == SEEN) {
                SERIAL.print(map[i][j].row); SERIAL.print("\t");
            }
        }
    }
    SERIAL.println("\nOther");;
    for (int i = 0; i < NUM_COLS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            if (map[i][j].value > VISITED) {
                SERIAL.print(map[i][j].row); SERIAL.print(" With val: "); SERIAL.print(map[i][j].value); SERIAL.print("\t");
            }
        }
    }
    SERIAL.println();
}
