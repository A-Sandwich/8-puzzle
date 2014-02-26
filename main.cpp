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
    return temp;
}

//function that loops through the three boards looking for a solution.
//Note: Each board must do one move and the proceed, since not all
//board will have a solution. Thus we cannot simply solve one and move
//to the next.
void Breadth_First_Search(int **board1, int **board2, int **board3){
    Node *root, *middle, *tail;
    root = Create_Node(board1);
    middle = Create_Node(board2);
    tail = Create_Node(board3);
    root->next = middle;
    middle->next = tail;
    Print_Board(root->board);
}
