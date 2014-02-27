/*
*   By Kyle Burkholder
*   CSCI-487
*   Programming Assignment 1
*   Start-date: 02/15/14
*   Due date: 03/03/14
*/
#include <iostream>
#include <time.h> /*for time*/
#include <stdlib.h> /*for srand and rand*/

#define BOARD_SIZE 9
#define ROW_SIZE 3
#define COLUMN_SIZE 3
#define NUMBER_OF_BOARDS 3

struct Node{
    int **board;
    Node *next;
};

int** Generate_Board();
void Print_Board(int **);
bool Solution_Found(int **);
void Breadth_First_Search(int **, int **, int **);
Node* Create_Node(int **);
void Find_Blank(int **);
int** Swap_Tile(int **, int , int , int , int );

using namespace std;



int main()
{
    srand(time(NULL));//Seed psuedo random number generator.
    Breadth_First_Search(Generate_Board(), Generate_Board(), Generate_Board());
    return 0;
}//end main


//Create board generation function
int** Generate_Board(){
    //Define Variables
    int **board;
    int new_number;
    int *used_numbers;
    bool keepgoing;

    //create arrays.
    board = new int*[ROW_SIZE];
    used_numbers = new int[BOARD_SIZE];

    for(int i = 0; i < BOARD_SIZE; i++){
        board[i] = new int[COLUMN_SIZE];
        used_numbers[i] = BOARD_SIZE+1;
    }


    for(int i = 0; i < ROW_SIZE; i++){
        for(int j = 0; j < COLUMN_SIZE; j++){
            keepgoing = true;
            while(keepgoing){
                new_number = rand() % BOARD_SIZE;
                if(used_numbers[new_number] != new_number){
                    used_numbers[new_number] = new_number;
                    board[i][j] = new_number;
                    keepgoing = false;
                }//end if
            }//end while
        }//end for j
    }//end for i

    Print_Board(board);

    return board;
}//end Generate_Board()

//Print Board
void Print_Board(int **board){
    for(int i=0; i < ROW_SIZE; i++){
        for(int j = 0; j < COLUMN_SIZE; j++){
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}//end Print_Board()

void Find_Blank(int &row, int &column, int **board){
    for(int i=0; i < ROW_SIZE; i++){
        for(int j = 0; j < COLUMN_SIZE; j++){
            if(board[i][j] == 0){
                column = j;
                row = i;
            }
        }
    }
}


//A* sort function

//Breadth first search function

//check solution function
//Returns 0 for false and 1 for true
bool Solution_Found(int **board){
    bool goal_achieved = true;
    int itteration = 0;
    for(int i=0; i < ROW_SIZE; i++){
        for(int j = 0; j < COLUMN_SIZE; j++){
            if(board[i][j] != itteration)
                goal_achieved = false;
            itteration++;
        }
    }

    return goal_achieved;
}//end Solution_Found();

Node* Create_Node(int **passed_board){
    int **board;
    Node *temp;
    temp = new Node;
    temp->board = passed_board;//set node's
    temp->next = NULL;
    return temp;
}

int** Swap_Tile(int **board, int row, int column, int dx, int dy){
    int** temp_board;
    int temp;

    temp_board = new int*[ROW_SIZE];

    //copy board
    for(int i=0;i<ROW_SIZE;i++){
        temp_board[i] = new int[COLUMN_SIZE];
        for(int j=0; j<COLUMN_SIZE;j++){
            temp_board[i][j] = board[i][j];
        }
    }

    //make switch
    cout << "In Function: " << row << ", " << column << endl;
    cout << dx << ", " << dy << endl;
    temp = temp_board[row][column];
    temp_board[row][column] = temp_board[row + dx][column + dy];
    temp_board[row + dx][column + dy] = temp;

    return temp_board;
}

bool In_Closed_List(Node *head, int **board){
    bool equivalent = true;
    Node *temp;
    temp = head;
    if(head == NULL)
        return false;
    if(temp != NULL){
        while(temp->next != NULL){
            for(int i=0; i<ROW_SIZE;i++){
                for(int j=0;j<COLUMN_SIZE;j++){
                    if(board[i][j] != temp->board[i][j])
                        equivalent = false;
                }
            }
            if(equivalent == true)
                return equivalent;
            temp = temp->next;
        }
    }

    return equivalent;

}

//function that loops through the three boards looking for a solution.
//Note: Each board must do one move and the proceed, since not all
//board will have a solution. Thus we cannot simply solve one and move
//to the next.
void Breadth_First_Search(int **board1, int **board2, int **board3){
    bool keepgoing = true, board1_solved = false, board2_solved = false, board3_solved = false;
    int row;
    int column, temp;
    int **temp_board;
    Node *tail1, *tail2, *tail3, *fringe1, *fringe2, *fringe3, *closed1, *closed2, *closed3, *null, *temp_node;
    fringe1 = Create_Node(board1);
    fringe2 = Create_Node(board2);
    fringe3 = Create_Node(board3);

    null = NULL;
    closed1 = NULL;
    closed2 = NULL;
    closed3 = NULL;

    tail1 = fringe1;
    tail2 = fringe2;
    tail3 = fringe3;

    while(keepgoing){
        //check for solutions:
        if((board1_solved&&board2_solved) || (board2_solved&&board3_solved) || (board1_solved&&board3_solved))
            keepgoing = false;
        Find_Blank(row, column, fringe1->board);
        if(!board1_solved && row - 1 >=0){
            temp_board = Swap_Tile(fringe1->board, row, column, -1, 0);
            if(In_Closed_List(closed1, temp_board) == false){
                if(Solution_Found(temp_board)){
                    board1_solved = true;
                }else{

                }

                tail1->next = Create_Node(temp_board);
                tail1 = tail1->next;
            }//end if
        }

        if(!board1_solved && row + 1 <=2){
            temp_board = Swap_Tile(fringe1->board, row, column, 1, 0);
            if(In_Closed_List(closed1, temp_board) == false){
                if(Solution_Found(temp_board)){
                    board1_solved = true;
                }else{

                }
                tail1->next = Create_Node(temp_board);
                tail1 = tail1->next;
            }//end if
        }

        if(!board1_solved && column - 1 >=0){
            temp_board = Swap_Tile(fringe1->board, row, column, 0, -1);
            if(In_Closed_List(closed1, temp_board) == false){
                if(Solution_Found(temp_board)){
                    board1_solved = true;
                }else{

                }
                tail1->next = Create_Node(temp_board);
                tail1 = tail1->next;
            }//end if
        }

        if(!board1_solved && column + 1 <=2){
            temp_board = Swap_Tile(fringe1->board, row, column, 0, 1);
            if(In_Closed_List(closed1, temp_board) == false){
                if(Solution_Found(temp_board)){
                    board1_solved = true;
                }else{

                }
                tail1->next = Create_Node(temp_board);
                tail1 = tail1->next;
            }//end if
        }
        if(!board1_solved){
            temp_node = fringe1;
            fringe1 = fringe1->next;
            delete temp_node;
        }

        //cout << row << column << endl;
        //Print_Board(Swap_Tile(fringe1->board, row, column, -1, 0));
        //Print_Board(fringe1->board);
        //keepgoing = false;

    }//end while
}//end breadth first search
