#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>

using namespace std;
//Deniz Gurleyen
struct cell {
    int x;
    int y;
    int l;
    int r;
    int u;
    int d;
    int is_corner;
    int visited; //0 if not visited yet
    int visited_for_path; //0 if not visited yet
};

//the stack class have been taken from the course slides

template <class Object>
class Stack {

public:
    Stack( );
    Stack( const Stack & rhs );
    ~Stack( );

    [[nodiscard]] bool isEmpty( ) const;

    void makeEmpty( );

    bool pop( ) ;
    void push( const Object & x );
    Object & top( ) ;

    const Stack & operator=( const Stack & rhs );


private:
    struct ListNode
    {
        Object   element;
        ListNode *next;

        ListNode( const Object & theElement, ListNode * n = nullptr )
                : element( theElement ), next( n ) { }
    };

    ListNode *topOfStack;  // list itself is the stack

};


template <class Object>
Stack<Object>::Stack() : topOfStack(nullptr) { }

template <class Object>
bool Stack<Object>::isEmpty() const
{
    return topOfStack == nullptr;
}

template <class Object>
Object & Stack<Object>::top()  {
    //if (isEmpty()) {
      //  return 0; // might cause errors needs to be remembered
    //}
    return topOfStack->element; // Return the element at the top of the stack
}

template <class Object>
bool Stack<Object>::pop()
{
    if ( isEmpty( ) )
        return false;
    ListNode *oldTop = topOfStack;
    topOfStack = topOfStack->next;
    delete oldTop;
    return true; //pop was successful
}

template <class Object>
void Stack<Object>::push( const Object & x )
{
    topOfStack = new ListNode( x, topOfStack );
}

template <class Object>
void Stack<Object>::makeEmpty( )
{
    while ( ! isEmpty( ) )
        pop( );
}

template <class Object> //deep copy
const Stack<Object> & Stack<Object>::
operator=( const Stack<Object> & rhs )
{
    if ( this != &rhs )
    {
        makeEmpty( );
        if ( rhs.isEmpty( ) )
            return *this;

        ListNode *rptr = rhs.topOfStack;
        ListNode *ptr  = new ListNode( rptr->element );
        topOfStack = ptr;

        for ( rptr = rptr->next; rptr != nullptr; rptr = rptr->next )
            ptr = ptr->next = new ListNode( rptr->element );
    }
    return *this;
}

template <class Object>
Stack<Object>::Stack( const Stack<Object> & rhs )
{
    topOfStack = nullptr;
    *this = rhs; // deep copy
}
template <class Object>
Stack<Object>::~Stack( )
{
    makeEmpty( );
}

class Maze {
private:
    int row;
    int col;
    vector<vector<cell>> maze_grid;
    int maze_id;
public:
    Maze(int m, int n, int k); //constructor
    void maze_generator();
    bool availability_checker(int random_number, cell &current_top); //false if not available
    void wall_breaker(cell &current_top, cell &next_top, int random_number);
    cell& next_cell_finder(cell &current_top, int random_number);
    void file_writing_phase1();
    void path_finder(int entry_x, int entry_y, int exit_x, int exit_y);
    bool availability_checker_path(int random_number, cell &current_top); //false if not available
    void file_writing_phase2(int entry_x, int entry_y, int exit_x, int exit_y, Stack<cell*> &path_stack) const;
};
Maze::Maze(int m, int n,int k) {
    row = m;
    col = n;
    maze_id = k;
    maze_grid.resize(m,vector<cell>(n));


    int i;
    int j;

    for(i = 0; i < m; i++)
    {
        for(j=0;j<n;j++)
        {
            maze_grid[i][j].l = 1;
            maze_grid[i][j].r = 1;
            maze_grid[i][j].u = 1;
            maze_grid[i][j].d = 1;
            maze_grid[i][j].x = j;
            maze_grid[i][j].y = i;
            maze_grid[i][j].visited = 0;
            maze_grid[i][j].visited_for_path= 0;

            if( maze_grid[i][j].y == 0 && maze_grid[i][j].x != 0 && maze_grid[i][j].x != n-1){
                maze_grid[i][j].is_corner = 1; //down corner
            }
            else if( maze_grid[i][j].y != m-1 && maze_grid[i][j].y != 0 && maze_grid[i][j].x == n-1){
                maze_grid[i][j].is_corner = 4; //right corner
            }
            else if( maze_grid[i][j].x == 0 && maze_grid[i][j].y != m-1 && maze_grid[i][j].y != 0){
                maze_grid[i][j].is_corner = 2; //left corner
            }
            else if( maze_grid[i][j].y == m-1 && maze_grid[i][j].x != 0 && maze_grid[i][j].x != n-1){
                maze_grid[i][j].is_corner = 3; //up corner
            }
            else if( maze_grid[i][j].y == m-1 && maze_grid[i][j].x == 0){
                maze_grid[i][j].is_corner = 6; //up-left corner
            }
            else if( maze_grid[i][j].y == m-1 && maze_grid[i][j].x == n-1){
                maze_grid[i][j].is_corner = 7; //up-right corner
            }
            else if( maze_grid[i][j].y == 0 && maze_grid[i][j].x == n-1){
                maze_grid[i][j].is_corner = 8; //down-right corner
            }
            else if( maze_grid[i][j].y == 0 && maze_grid[i][j].x == 0){
                maze_grid[i][j].is_corner = 5; //down-left corner (beginning corner of the maze)
            }
            else{
                maze_grid[i][j].is_corner = 0;//not a corner spot
            }
        }
    }
}

bool Maze::availability_checker(int random_number, cell& current_top ) {
    if (random_number == 1) { //we'll check the right sight
        if (current_top.is_corner == 4 || current_top.is_corner == 7 || current_top.is_corner == 8) {
            return false; // if the cell is not at right or right corner
        }
        else {
            if (maze_grid[current_top.y][current_top.x + 1].visited == 1) {
                return false;//if next cell is already visited
            }
        }
        return true; //next cell is not visited before and exists
    }
    else if (random_number == 3) { //we'll check the left sight
        if (current_top.is_corner == 2 || current_top.is_corner == 6 || current_top.is_corner == 5) {
            return false; // if the cell is not at left or left corner
        }
        else {
            if (maze_grid[current_top.y][current_top.x - 1].visited == 1) {
                return false;//if next cell is already visited
            }
        }
        return true; //next cell is not visited before and exists
    }
    else if (random_number == 2) { //we'll check the upside sight
        if (current_top.is_corner == 3 || current_top.is_corner == 6 || current_top.is_corner == 7) {
            return false; // if the cell is not at up or up corner
        }
        else {
            if (maze_grid[current_top.y + 1][current_top.x].visited == 1) {
                return false;//if next cell is already visited
            }
        }
        return true; //next cell is not visited before and exists
    }
    else { //random number is 4, we'll check downside
        if (current_top.is_corner == 1 || current_top.is_corner == 8 || current_top.is_corner == 5) {
            return false; // if the cell is not at down or down corner
        }
        else {
            if (maze_grid[current_top.y - 1][current_top.x].visited == 1) {
                return false;//if next cell is already visited
            }
        }
        return true;
    }
}

bool Maze::availability_checker_path(int random_number, cell &current_top) {
    if (random_number == 1) { //we'll check the right sight
        if (current_top.is_corner == 4 || current_top.is_corner == 7 || current_top.is_corner == 8) {
            return false; // if the cell is not at right or right corner
        } else {

            if (maze_grid[current_top.y][current_top.x + 1].visited_for_path == 1 ||
                maze_grid[current_top.y][current_top.x].r == 1) { //if the next cell is not visited already or have a wall between
                return false;
            }
        }
        return true; //next cell is not visited before and exists
    }
    else if (random_number == 2) { //we'll check the up sight
        if (current_top.is_corner == 3 || current_top.is_corner == 6 || current_top.is_corner == 7) {
            return false; //if the cell is not at up or up corner
        } else {

            if (maze_grid[current_top.y + 1][current_top.x].visited_for_path == 1 ||
                maze_grid[current_top.y][current_top.x].u == 1) {//if the next cell is not visited already or have a wall between
                return false;
            }
        }
        return true; //next cell is not visited before and exists
    }
    else if (random_number == 3) { //we'll check the left sight
        if (current_top.is_corner == 2 || current_top.is_corner == 6 || current_top.is_corner == 5) {
            return false; // if the current cell is not at left or left corner
        } else {

            if (maze_grid[current_top.y][current_top.x - 1].visited_for_path == 1 ||
                maze_grid[current_top.y][current_top.x].l == 1) { //if the next cell is not visited already or have a wall between
                return false;
            }
        }
        return true; //next cell is not visited before and exists
    }
    else { //random number is 4, we'll check downside
        if (current_top.is_corner == 1 || current_top.is_corner == 8 || current_top.is_corner == 5) {
            return false; //if the current cell is not at down or down corner
        } else {
            if (maze_grid[current_top.y - 1][current_top.x].visited_for_path == 1 ||
                maze_grid[current_top.y][current_top.x].d == 1) //if the next cell is not visited already or have a wall between
                return false;
        }

    return true;
    }
}

cell& Maze::next_cell_finder(cell &current_top, int random_number) {
    // Return a reference to the cell in the maze_grid so that we modify the cell in the maze_grid directly afterwards
    if (random_number == 1) {
        return maze_grid[current_top.y][current_top.x + 1];
    } else if (random_number == 2) {
        return maze_grid[current_top.y + 1][current_top.x];
    } else if (random_number == 3) {
        return maze_grid[current_top.y][current_top.x - 1];
    } else {
        return maze_grid[current_top.y - 1][current_top.x];
    }
}

void Maze::wall_breaker(cell &current_top, cell &next_top, int random_number) {
    if(random_number == 1) //if the wall to be broken is in the right direction
    {
        current_top.r = 0;
        next_top.l = 0;
    }
    else if(random_number == 2)//if the wall to be broken is in the up direction
    {
        current_top.u = 0;
        next_top.d = 0;
    }
    else if(random_number == 3)//if the wall to be broken is in the left direction
    {
        current_top.l = 0;
        next_top.r = 0;
    }
    else{//if the wall to be broken is in the down direction
        current_top.d = 0;
        next_top.u = 0;
    }

    maze_grid[current_top.y][current_top.x] = current_top;
    maze_grid[next_top.y][next_top.x] = next_top;
}

void Maze::maze_generator() {
    Stack<cell*> cell_stack;
    maze_grid[0][0].visited = 1;
    cell_stack.push(&maze_grid[0][0]);

    random_device rd;
    mt19937 gen(rd());
    vector<int> directions = {1, 2, 3, 4}; // Right, Up, Left, Down

    while (!cell_stack.isEmpty()) {

        cell *current_cell_ptr = cell_stack.top();

        shuffle(directions.begin(), directions.end(), gen); // for finding a random number between 1-4
        bool found_unvisited = false;

        for (int dir : directions) {
            if (availability_checker(dir, *current_cell_ptr)) {

                //if there is an available cell in at least 1 of the directions, we put the new cell in the stack
                cell* next_cell_ptr = &next_cell_finder(*current_cell_ptr, dir);
                wall_breaker(*current_cell_ptr, *next_cell_ptr, dir);
                next_cell_ptr->visited = 1;

                cell_stack.push(next_cell_ptr);
                found_unvisited = true;
                break;
            }
        }

        if (!found_unvisited) {
            //if there are no available cells in any directions, we do backtracking
            cell_stack.pop();

        }
    }
}

void Maze::file_writing_phase1() {

    ofstream file;
    file.open("maze_" + to_string(maze_id) + ".txt");

    // Write the size of the maze
    file << row << " " << col << endl;

    // Write each cell's data
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            file << "x=" << maze_grid[i][j].x << " ";
            file << "y=" << maze_grid[i][j].y << " ";
            file << "l=" << maze_grid[i][j].l << " ";
            file << "r=" << maze_grid[i][j].r << " ";
            file << "u=" << maze_grid[i][j].u << " ";
            file << "d=" << maze_grid[i][j].d << endl;
        }
    }

    file.close();
}

void Maze::file_writing_phase2(int entry_x, int entry_y, int exit_x, int exit_y, Stack<cell*> &path_stack) const {
    ofstream file;
    file.open("maze_" + to_string(maze_id) + "_path_" +
              to_string(entry_x) + "_" + to_string(entry_y) + "_" +
              to_string(exit_x) + "_" + to_string(exit_y) + ".txt");
    while (!path_stack.isEmpty()) {
        cell* currentCell = path_stack.top();
        path_stack.pop();
        file << currentCell->x << " " << currentCell->y << endl;
    }
    file.close();
}

void Maze::path_finder(int entry_x, int entry_y, int exit_x, int exit_y) {
    Stack<cell*> path_stack; //will be used for writing a file
    Stack<cell*> cell_stack;
    cell_stack.push(&maze_grid[entry_y][entry_x]);

    random_device rd;
    mt19937 gen(rd());
    vector<int> directions = {1, 2, 3, 4}; // Right, Up, Left, Down

    while(cell_stack.top() -> y != exit_y && cell_stack.top() -> x != exit_x) //while we are not at the exit
    {
        cell *current_cell_ptr = cell_stack.top();
        shuffle(directions.begin(), directions.end(), gen); //for random numbers

        bool found_unvisited = false;

        for (int dir : directions) { //looping in random number list
            if (availability_checker_path(dir, *current_cell_ptr)) {
                //if the cell in the given direction available, we add it to the stack
                cell* next_cell_ptr = &next_cell_finder(*current_cell_ptr, dir);
                next_cell_ptr->visited_for_path = 1;
                cell_stack.push(next_cell_ptr);
                found_unvisited = true;

                break;
            }
        }

        if (!found_unvisited) {
           //if no available cell in all directions we do backtracking
            cell_stack.pop();

        }
    }
    //file writing

    while (!cell_stack.isEmpty()) {
        //for inverting the stack
        path_stack.push(cell_stack.top());
        cell_stack.pop();
    }

    file_writing_phase2(entry_x, entry_y, exit_x, exit_y, path_stack);
}



int main() {

    int k,m,n;
    cout << "Enter the number of mazes : ";
    cin >> k;
    cout << "Enter the number of rows and columns(M and N) : ";
    cin >> m >> n;

    vector<Maze> maze_vec; //for storing the mazes

    for (int maze_id = 1; maze_id <= k; ++maze_id) {
        Maze maze(m, n, maze_id);
        maze.maze_generator();
        maze_vec.push_back(maze);
        maze.file_writing_phase1(); // Function to write the maze to a file
    }

    int entry_x, entry_y, exit_x, exit_y,path_id;
    cout<< "Enter a maze ID between 1 to "<<k <<" inclusive to find a path: ";
    cin >> path_id;
    cout << "Enter x and y coordinates of the entry points (x,y) or (column,row): ";
    cin >> entry_x >> entry_y;
    cout <<"Enter x and y coordinates of the exit points (x,y) or (column,row): ";
    cin >> exit_x >> exit_y;

    maze_vec[path_id-1].path_finder(entry_x,entry_y,exit_x,exit_y);


    return 0;
}