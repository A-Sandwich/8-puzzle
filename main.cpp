/*
*   By Kyle Burkholder
*   CSCI-487
*   Programming Assignment 1
*   Start-date: 02/15/14
*   Due date: 03/03/14
*
*
* Some notes:
* I only commented on the beginning of each search function as there is some repitition, if given more time I would have put more of the code into functions
* When I generate a new node that is unique, I immediately add it to the fringe list and the closed list. This way I only need to search one list to ensure that
* a new board is unique. I felt this was okay since as soon as the I attach the node to the fringe, I know that it has been created and that the board will either
* be solved before the node is expanded or the node will be selected and expanded eventually. This comes at a slightly higher memory cost, but it is managable.
*
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
    int rating;
};

int** Generate_Board();
void Print_Board(int **);
bool Solution_Found(int **);
void Breadth_First_Search(int **, int **, int **);
Node* Create_Node(int **);
void Find_Blank(int **);
int** Swap_Tile(int **, int , int , int , int );
bool Valid_Move(int , int ,int , int);
void A_Star(int **, int **, int**);
void Delete_List(Node);
int Heuristic(int **);
Node* Find_Best_Node(Node *);

using namespace std;



int main()
{
    srand(time(NULL));//Seed psuedo random number generator.
    cout << "A* search: " << endl << endl;
    A_Star(Generate_Board(), Generate_Board(), Generate_Board());
    cout << endl << endl << "Breadth First Search: " << endl << endl;
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
                if(used_numbers[new_number] != new_number+1){
                    used_numbers[new_number] = new_number+1;
                    board[i][j] = new_number+1;
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


//Find the blank tile which is a 9 in this case.
void Find_Blank(int &row, int &column, int **board){
    for(int i=0; i < ROW_SIZE; i++){
        for(int j = 0; j < COLUMN_SIZE; j++){
            if(board[i][j] == 9){
                column = j;
                row = i;
            }//end if
        }//end for
    }//for
}//end find_blank

//check solution function
//Returns 0 for false and 1 for true
bool Solution_Found(int **board){
    bool goal_achieved = true;
    int itteration = 1;
    for(int i=0; i < ROW_SIZE; i++){
        for(int j = 0; j < COLUMN_SIZE; j++){
            if(board[i][j] != itteration)
                goal_achieved = false;
            itteration++;
        }//end for
    }//end for

    return goal_achieved;
}//end Solution_Found();

Node* Create_Node(int **passed_board){
    Node *temp;

    temp = new Node;//create a new node.
    temp->board = passed_board;//set node's board to the board that was passed.
    temp->next = 0;//set next pointer to 0 as a signal it points to nothing
    temp->rating = 0;
    temp->rating = Heuristic(passed_board);//generate rating for node
    return temp;//return new node
}//end create node

int** Swap_Tile(int **board, int row, int column, int dx, int dy){
    int** temp_board;
    int temp;

    temp_board = new int*[ROW_SIZE];

    //copy board
    for(int i=0;i<ROW_SIZE;i++){
        temp_board[i] = new int[COLUMN_SIZE];
        for(int j=0; j<COLUMN_SIZE;j++){
            temp_board[i][j] = board[i][j];//copy board to a new memory location
        }//end for
    }//end for

    //swap tile in new board
    temp = temp_board[row][column];//store old value
    temp_board[row][column] = temp_board[row + dx][column + dy];//switch one tile
    temp_board[row + dx][column + dy] = temp;//switch the other tile
    return temp_board;//return the board.
}

//checks if a board is in the list supplied as an argument.
bool In_Closed_List(Node *head, int **board){
    bool in_list = true;
    Node *temp;
    temp = head;

    if(head == NULL || head == 0){//check if the head is NULL or 0
        return false;
    }else if(head->next == NULL || head->next == 0){//check if the list is one node large
        for(int i=0;i<ROW_SIZE;i++){//if there is only one node, check it and return the appropriate result
            for(int j=0;j<COLUMN_SIZE;j++){
                if(head->board[i][j] != board[i][j])
                    in_list = false;
            }//end for
        }//end for
        return in_list;
    }//end if else

    //iterate through list and check all nodes
    while(temp->next != 0){
        in_list = true;
        for(int i=0;i<ROW_SIZE;i++){
            for(int j=0;j<COLUMN_SIZE;j++){
                if(temp->board[i][j] != board[i][j])
                    in_list = false;
            }//end for
        }//end for

        if(in_list)
            return in_list;
        temp = temp->next;//traverse to next node.
    }//end while

    return in_list;
}

//check for a valid move.
bool Valid_Move(int row, int column,int dx, int dy){
    bool valid_move = false;
    if(row + dx >= 0 && row + dx <= 2 && column + dy >= 0 && column + dy <= 2)//Checks bounds assuming the board is a 3x3 matrix.
        valid_move = true;
    return valid_move;
}

//Heuristic used for rating each board's optimality.
int Heuristic(int **board){
    int rating = 0, iteration = 1;
    for(int i; i < BOARD_SIZE; i++){
        for(int j; j < BOARD_SIZE; j++){
            if(board[i][j] != iteration)
                rating++;//rating goes up based on how many tiles are out of place
            iteration++;
        }//end for j
    }//end for i
    return rating;
}//end heuristic

//Delete an entire list. Argument is the head of some list.
void Delete_List(Node *head){
    Node *temp_node;
    while(head->next != 0){
        temp_node = head;
        head=head->next;//Make head point to next node
        delete temp_node;//delete node removed from list.
    }//end while

    if(head != 0)//In case the list passed is only one node large
        delete head;
}//end delete list

//Find the highest rated node in a list.
Node* Find_Best_Node(Node *head){
    Node *previous, *best_node, *current_node;
    //Set all nodes equal to the head node.
    best_node = head;
    current_node = head;
    previous = head;

    if(head!=0){//ensures the list isn't empty
        if(head->next == 0){//ensures that the list isn't one node large
            return current_node;
        }

        do{
            if(best_node->rating > current_node->rating){//compares the best node's rating to the current node's rating
                best_node = current_node;//switches best node to the current node.
                previous->next = current_node->next;//Take the previous node's next pointer and points it at the current node's next pointer.(This removes the best node from the list)
                current_node = current_node->next;//Traverses to the next point in the list
                best_node->next = head;//attaches the best node to the front of the list.
                head = best_node;//Changes the head pointer back to the front of the list
            }else{//Moves forward in the list since the best node is better than the current node
                previous = current_node;
                current_node = current_node->next;
            }//end if else
        }while(current_node->next != 0);

    }//end if
    return head;//returns the head of the list, also the first element of that list is the highest rated node.
};

void A_Star(int **board1, int **board2, int **board3){
    //Define variables
    bool keepgoing = true, board_1_solved = false, board_2_solved = false, board_3_solved = false, board_1_failed = false, board_2_failed= false, board_3_failed= false;
    Node *fringe_1, *fringe_2, *fringe_3, *tail_1, *tail_2, *tail_3, *closed_1, *closed_2, *closed_3, *temp_node;
    int row, column;
    int **temp_board;
    int fringe_1_size = 0, fringe_2_size = 0, fringe_3_size = 0, closed_1_size = 0, closed_2_size = 0, closed_3_size = 0;
    unsigned int counter = 0;

    //Assign lists and create nodes
    fringe_1 = Create_Node(board1);
    tail_1 = fringe_1;

    fringe_2 = Create_Node(board2);
    tail_2 = fringe_2;

    fringe_3 = Create_Node(board3);
    tail_3 = fringe_3;

    closed_1 = Create_Node(board1);

    closed_2 = Create_Node(board2);

    closed_3 = Create_Node(board3);

    //Start main loop
    while(keepgoing){
        counter++;
        //Display the search's current stats every 10,000 iterations
        if(counter%10000 == 0){
            cout << endl;
            cout << "While loop iterations: " << counter << endl;
            if(!board_1_solved && !board_1_failed){
                cout << "Fringe 1 size: " << fringe_1_size << endl;
                cout << "Closed 1 list size: " << closed_1_size << endl;
            }//end if
            if(board_1_failed)
                    cout << "Board 1 Failed" << endl;
            if(!board_2_solved && !board_2_failed){
                cout << "Fringe 2 size: " << fringe_2_size << endl;
                cout << "Closed 2 list size: " << closed_2_size << endl;
            }//end if
            if(board_2_failed)
                    cout << "Board 2 Failed" << endl;
            if(!board_3_solved && !board_3_failed){
                cout << "Fringe 3 size: " << fringe_3_size << endl;
                cout << "Closed 3 list size: " << closed_3_size << endl;
            }//end if
            if(board_3_failed)
                cout << "Board 3 Failed" << endl;
        }//end if

        //small indication that processing is still occuring every 1,000 iterations
        if(counter%1000 == 0)
            clog << '.' << '.' << '.';
        //sets conditional for main while loop. If the two boards are solved or if two board fail to be solved the loop will exit.
        //One of these sequences will always occur since there are only 181,440 possible distinct nodes and in the worst case, all of these will get added to the closed list causing the program to terminate.
        if((board_1_solved&&board_2_solved) || (board_1_solved&&board_3_solved) || (board_2_solved&&board_3_solved) || (board_1_failed&&board_3_failed) || (board_1_failed&&board_2_failed) || (board_2_failed&&board_3_failed))
            keepgoing = false;

        if(fringe_1 != 0){
            if(!board_1_solved && !board_1_failed){
                fringe_1 = Find_Best_Node(fringe_1);//Find and append the best node to the front of the list
                if(fringe_1->next == 0)//check to see if the tail needs to be reassigned.
                    tail_1 = fringe_1;

                Find_Blank(row, column, fringe_1->board);//Find where the blank tile is (In this case it is a 9)

                if(fringe_1 != 0){
                    //Check moving blank up
                    if(Valid_Move(row, column, 0, 1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_1->board, row, column, 0, 1);
                        if(!In_Closed_List(closed_1, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_1_solved = true;//solution found, set marker variable
                                cout << endl << "Board 1 has been solved!" << endl; //display to user
                                Print_Board(temp_board);//print out solved board
                                Delete_List(fringe_1);//delete fringe list to free up memory
                                Delete_List(closed_1);//delete closed list to free up memory
                                fringe_1 = 0;//set fringe to a null value
                                closed_1_size = 0;//set list size to 0
                                fringe_1_size = 0;//set fringe list size to 0
                            }else{
                                tail_1->next = Create_Node(temp_board);//append node to the end of the fringe
                                tail_1 = tail_1->next;//point tail to the end of the list
                                tail_1->rating = fringe_1->rating+tail_1->rating;
                                temp_node = Create_Node(temp_board);//create a node to put on the end of the closed list
                                temp_node->next = closed_1;//append closed list to the node
                                closed_1 = temp_node;//set the pointer to the front of the closed list to the newly created node
                                fringe_1_size++;//increment fringe size tracker
                                closed_1_size++;//increment closed size tracker
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_1 != 0){
                    //Check moving blank down
                    if(Valid_Move(row, column, 0, -1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_1->board, row, column, 0, -1);
                        if(!In_Closed_List(closed_1, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_1_solved = true;
                                cout << endl << "Board 1 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_1);
                                Delete_List(closed_1);
                                fringe_1 = 0;
                                fringe_1_size = 0;
                                closed_1_size = 0;
                            }else{
                                tail_1->next = Create_Node(temp_board);
                                tail_1 = tail_1->next;
                                tail_1->rating = fringe_1->rating+tail_1->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_1;
                                closed_1 = temp_node;
                                fringe_1_size++;
                                closed_1_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_1 != 0){
                    //Check moving blank right
                    if(Valid_Move(row, column, 1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_1->board, row, column, 1, 0);
                        if(!In_Closed_List(closed_1, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_1_solved = true;
                                cout << endl << "Board 1 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_1);
                                Delete_List(closed_1);
                                fringe_1 = 0;
                                fringe_1_size = 0;
                                closed_1_size = 0;
                            }else{
                                tail_1->next = Create_Node(temp_board);
                                tail_1 = tail_1->next;
                                tail_1->rating = fringe_1->rating+tail_1->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_1;
                                closed_1 = temp_node;
                                fringe_1_size++;
                                closed_1_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_1 != 0){
                    //Check moving blank left
                    if(Valid_Move(row, column, -1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_1->board, row, column, -1, 0);
                        if(!In_Closed_List(closed_1, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_1_solved = true;
                                cout << endl << "Board 1 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_1);
                                Delete_List(closed_1);
                                fringe_1 = 0;
                                fringe_1_size = 0;
                                closed_1_size = 0;
                            }else{
                                tail_1->next = Create_Node(temp_board);
                                tail_1 = tail_1->next;
                                tail_1->rating = fringe_1->rating+tail_1->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_1;
                                closed_1 = temp_node;
                                fringe_1_size++;
                                closed_1_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe


            }//end if
            if(fringe_1 != 0){//if fringe is not empty.
                fringe_1_size--;//decrement fringe size
                temp_node = fringe_1;// set a temp node equal to the head of the fringe
                fringe_1 = fringe_1->next;// move fringe head pointer to the next node in the list
                delete temp_node;//delete the old fringe head node because it has been fully expanded and can now be destroyed.
            }
        }else{
            if(!board_1_solved && !board_1_failed){
                board_1_failed = true;//set board to failed
                cout << "board 1 failed." << endl;//inform the user.
            }
        }//end if fringe_1

        if(fringe_2 != 0){
            if(!board_2_solved && !board_2_failed){
                fringe_2 = Find_Best_Node(fringe_2);
                if(fringe_2->next == 0)
                    tail_2 = fringe_2;

                Find_Blank(row, column, fringe_2->board);//Find where the blank tile is.

                if(fringe_2 != 0){
                    //Check moving blank up
                    if(Valid_Move(row, column, 0, 1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_2->board, row, column, 0, 1);
                        if(!In_Closed_List(closed_2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_2_solved = true;
                                cout << endl << "Board 2 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_2);
                                Delete_List(closed_2);
                                fringe_2 = 0;
                                fringe_2_size = 0;
                                closed_2_size = 0;
                            }else{
                                tail_2->next = Create_Node(temp_board);
                                tail_2 = tail_2->next;
                                tail_2->rating = fringe_2->rating+tail_2->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_2;
                                closed_2 = temp_node;
                                fringe_2_size++;
                                closed_2_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_2 != 0){
                    //Check moving blank down
                    if(Valid_Move(row, column, 0, -1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_2->board, row, column, 0, -1);
                        if(!In_Closed_List(closed_2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_2_solved = true;
                                cout << endl << "Board 1 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_2);
                                Delete_List(closed_2);
                                fringe_2 = 0;
                                fringe_2_size = 0;
                                closed_2_size = 0;
                            }else{
                                tail_2->next = Create_Node(temp_board);
                                tail_2 = tail_2->next;
                                tail_2->rating = fringe_2->rating+tail_2->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_2;
                                closed_2 = temp_node;
                                fringe_2_size++;
                                closed_2_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_2 != 0){
                    //Check moving blank right
                    if(Valid_Move(row, column, 1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_2->board, row, column, 1, 0);
                        if(!In_Closed_List(closed_2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_2_solved = true;
                                cout << endl << "Board 2 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_2);
                                Delete_List(closed_2);
                                fringe_2 = 0;
                                fringe_2_size = 0;
                                closed_2_size = 0;
                            }else{
                                tail_2->next = Create_Node(temp_board);
                                tail_2 = tail_2->next;
                                tail_2->rating = fringe_2->rating+tail_2->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_2;
                                closed_2 = temp_node;
                                fringe_2_size++;
                                closed_2_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_2 != 0){
                    //Check moving blank left
                    if(Valid_Move(row, column, -1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_2->board, row, column, -1, 0);
                        if(!In_Closed_List(closed_2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_2_solved = true;
                                cout << endl << "Board 2 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_2);
                                Delete_List(closed_2);
                                fringe_2 = 0;
                                fringe_2_size = 0;
                                closed_2_size = 0;
                            }else{
                                tail_2->next = Create_Node(temp_board);
                                tail_2 = tail_2->next;
                                tail_2->rating = fringe_2->rating+tail_2->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_2;
                                closed_2 = temp_node;
                                fringe_2_size++;
                                closed_2_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe
            }//end if
            if(fringe_2 != 0){
                fringe_2_size--;
                temp_node = fringe_2;
                fringe_2 = fringe_2->next;
                delete temp_node;
            }
        }else{
            if(!board_2_solved && !board_2_failed){
                board_2_failed = true;
                cout << "Board 2 Failed" << endl;
            }
        }//end if fringe_2

        if(fringe_3 != 0){
            if(!board_3_solved && !board_3_failed){
                fringe_3 = Find_Best_Node(fringe_3);
                if(fringe_3->next == 0)
                    tail_3 = fringe_3;

                Find_Blank(row, column, fringe_3->board);//Find where the blank tile is.

                if(fringe_3 != 0){
                    //Check moving blank up
                    if(Valid_Move(row, column, 0, 1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_3->board, row, column, 0, 1);
                        if(!In_Closed_List(closed_3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_3_solved = true;
                                cout << endl << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_3);
                                Delete_List(closed_3);
                                fringe_3 = 0;
                                fringe_3_size = 0;
                                closed_3_size = 0;
                            }else{
                                tail_3->next = Create_Node(temp_board);
                                tail_3 = tail_3->next;
                                tail_3->rating = fringe_3->rating+tail_3->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_3;
                                closed_3 = temp_node;
                                fringe_3_size++;
                                closed_3_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_3 != 0){
                    //Check moving blank down
                    if(Valid_Move(row, column, 0, -1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_3->board, row, column, 0, -1);
                        if(!In_Closed_List(closed_3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_3_solved = true;
                                cout << endl << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_3);
                                Delete_List(closed_3);
                                fringe_3 = 0;
                                fringe_3_size = 0;
                                closed_3_size = 0;
                            }else{
                                tail_3->next = Create_Node(temp_board);
                                tail_3 = tail_3->next;
                                tail_3->rating = fringe_3->rating+tail_3->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_3;
                                closed_3 = temp_node;
                                fringe_3_size++;
                                closed_3_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_3 != 0){
                    //Check moving blank right
                    if(Valid_Move(row, column, 1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_3->board, row, column, 1, 0);
                        if(!In_Closed_List(closed_3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_3_solved = true;
                                cout << endl << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_3);
                                Delete_List(closed_3);
                                fringe_3 = 0;
                                fringe_3_size = 0;
                                closed_3_size = 0;
                            }else{
                                tail_3->next = Create_Node(temp_board);
                                tail_3 = tail_3->next;
                                tail_3->rating = fringe_3->rating+tail_3->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_3;
                                closed_3 = temp_node;
                                fringe_3_size++;
                                closed_3_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe

                if(fringe_3 != 0){
                    //Check moving blank left
                    if(Valid_Move(row, column, -1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe_3->board, row, column, -1, 0);
                        if(!In_Closed_List(closed_3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board_3_solved = true;
                                cout << endl << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe_3);
                                Delete_List(closed_3);
                                fringe_3 = 0;
                                fringe_3_size = 0;
                                closed_3_size = 0;
                            }else{
                                tail_3->next = Create_Node(temp_board);
                                tail_3 = tail_3->next;
                                tail_3->rating = fringe_3->rating+tail_3->rating;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed_3;
                                closed_3 = temp_node;
                                fringe_3_size++;
                                closed_3_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if fringe


            }//end if
            if(fringe_3 != 0){
                fringe_3_size--;
                temp_node = fringe_3;
                fringe_3 = fringe_3->next;
                delete temp_node;
            }
        }else{
            if(!board_3_solved && !board_3_failed){
                board_3_failed = true;
                cout << "board 3 failed." << endl;
            }
        }//end if fringe_1

    }//end while
}//end A_Star

//function that loops through the three boards looking for a solution.
//Note: Each board must do one move and the proceed, since not all
//board will have a solution. Thus we cannot simply solve one and move
//to the next.
void Breadth_First_Search(int **board1, int **board2, int **board3){
    //Define variables
    bool keepgoing = true, board1_solved = false, board2_solved = false, board3_solved = false, board1_failed = false, board2_failed = false, board3_failed = false;
    int column, row;
    int  fringe1_size = 1, fringe2_size = 1, fringe3_size = 1, closed1_size = 1, closed2_size = 1, closed3_size = 1;
    int **temp_board;
    unsigned int counter = 0;
    Node *tail1, *tail2, *tail3, *fringe1, *fringe2, *fringe3, *closed1, *closed2, *closed3, *temp_node;

    //Start lists and create new nodes from boards
    fringe1 = Create_Node(board1);
    fringe2 = Create_Node(board2);
    fringe3 = Create_Node(board3);

    closed1 = Create_Node(board1);
    closed2 = Create_Node(board2);
    closed3 = Create_Node(board3);

    tail1 = fringe1;
    tail2 = fringe2;
    tail3 = fringe3;

    //Main search loop
    while(keepgoing){
        counter++;
        //Update to the user when the program is computing.
        if(counter%10000 == 0){
            cout << endl;
            cout << "While loop itterations: " << counter << endl;
            if(!board1_solved && !board1_failed){
                cout << "Fringe 1 size: " << fringe1_size << endl;
                cout << "Closed 1 list size: " << closed1_size << endl;
            }//end if
            if(!board2_solved && !board2_failed){
                cout << "Fringe 2 size: " << fringe2_size << endl;
                cout << "Closed 2 list size: " << closed2_size << endl;
            }//end if
            if(!board3_solved && !board3_failed){
                cout << "Fringe 3 size: " << fringe3_size << endl;
                cout << "Closed 3 list size: " << closed3_size << endl;
            }//end if
        }

        //Allows for a more subtle indication to the user that progress is being made.
        if(counter%1000 == 0)
            clog << '.' << '.' << '.';

        //maintains main switch for if the search should continue or not.
        if((board1_solved&&board2_solved) || (board2_solved&&board3_solved) || (board1_solved&&board3_solved) || (board1_failed&&board3_failed) || (board1_failed&&board2_failed) || (board2_failed&&board3_failed))
            keepgoing = false;

        //Checks to make sure that the fringe is not empty, that the board has not been solved yet and that the board has not failed yet.
        if(fringe1 != 0 && !board1_solved && !board1_failed){//Check if the fringe is null
            if(!board1_solved&&!board1_failed){
                 Find_Blank(row, column, fringe1->board);//Find where the blank tile is.
                if(fringe1 != 0 && !board1_solved && !board1_failed){
                    //Check moving blank up
                    if(Valid_Move(row, column, 0, 1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe1->board, row, column, 0, 1);//Swaps tile and gets the new board
                        if(!In_Closed_List(closed1, temp_board)){//checks if the board is already part of a node that is in the closed list.
                            if(Solution_Found(temp_board)){//Checks to see if the board is a solution
                                board1_solved = true;//sets solution variable to true
                                cout << "Board 1 has been solved!" << endl;//inform the user
                                Print_Board(temp_board);//print the solved board
                                Delete_List(fringe1);//delete the fringe list to free memory
                                Delete_List(closed1);//delete the closed list to free memory
                                fringe1 = Create_Node(temp_board);//set the fringe equal to the one solved node
                                fringe1_size = 1;//set fringe to the 1 solved node
                                closed1_size = 0;//set closed list to zero
                            }else{//This is the case that the board has not been solved
                                tail1->next = Create_Node(temp_board);//append node to the end of the tail list.
                                tail1 = tail1->next;//move pointer to the end of the list.
                                temp_node = Create_Node(temp_board);//create a new node with the new board
                                temp_node->next = closed1;//put the new node at the front of the closed lsit
                                closed1 = temp_node;//change where the head of closed 1 points
                                fringe1_size++;//increase fringe size
                                closed1_size++;//increase closed list size
                            }
                        }//in closed list
                    }//valid move
                }

                if(fringe1 != 0 && !board1_solved && !board1_failed){
                    //Check moving blank down
                    if(Valid_Move(row, column, 0, -1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe1->board, row, column, 0, -1);
                        if(!In_Closed_List(closed1, temp_board)){
                            if(Solution_Found(temp_board)){
                                board1_solved = true;
                                cout << "Board 1 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe1);
                                Delete_List(closed1);
                                fringe1 = Create_Node(temp_board);;
                                fringe1_size = 0;
                                closed1_size = 0;
                            }else{
                                tail1->next = Create_Node(temp_board);
                                tail1 = tail1->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed1;
                                closed1 = temp_node;
                                fringe1_size++;
                                closed1_size++;
                            }
                        }//in closed list
                    }//valid move
                }

                if(fringe1 != 0 && !board1_solved && !board1_failed){
                    //Check moving blank right
                    if(Valid_Move(row, column, 1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe1->board, row, column, 1, 0);
                        if(!In_Closed_List(closed1, temp_board)){
                            if(Solution_Found(temp_board)){
                                board1_solved = true;
                                cout << "Board 1 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe1);
                                Delete_List(closed1);
                                fringe1 = Create_Node(temp_board);
                                fringe1_size = 0;
                                closed1_size = 0;
                            }else{
                                tail1->next = Create_Node(temp_board);
                                tail1 = tail1->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed1;
                                closed1 = temp_node;
                                fringe1_size++;
                                closed1_size++;
                            }
                        }//in closed list
                    }//valid move
                }
                if(fringe1 != 0 && !board1_solved && !board1_failed){
                    //Check moving blank left
                    if(Valid_Move(row, column, -1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe1->board, row, column, -1, 0);
                        if(!In_Closed_List(closed1, temp_board)){
                            if(Solution_Found(temp_board)){
                                board1_solved = true;
                                cout << "Board 1 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe1);
                                Delete_List(closed1);
                                fringe1 = Create_Node(temp_board);
                                fringe1_size = 0;
                                closed1_size = 0;
                            }else{
                                tail1->next = Create_Node(temp_board);
                                tail1 = tail1->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed1;
                                closed1 = temp_node;
                                fringe1_size++;
                                closed1_size++;
                            }
                        }//in closed list
                    }//valid move
                }
            }

            if(fringe1 != 0 && !board1_solved && !board1_failed){
                fringe1_size--;//decrease fringe size tracker
                temp_node = fringe1;//set temp pointer to the head of fringe
                fringe1 = fringe1->next;//move the pointer for the head of fringe to the second node in the list
                delete temp_node;//delete the old head of fringe since it has been fully expanded and is no longer useful.
            }

        }else{
            if(!board1_solved && !board1_failed){//ensures the following actiosn only occur once.
                board1_failed = true;//sets board to failed
                cout << "Board 1 failed" << endl;//informs the user
            }
        }


        if(fringe2 != 0 && !board2_solved && !board2_failed){//Check if the fringe is null
            if(!board2_solved&&!board2_failed){
                 Find_Blank(row, column, fringe2->board);//Find where the blank tile is.

                if(fringe2 != 0 && !board2_solved && !board2_failed){
                    //Check moving blank up
                    if(Valid_Move(row, column, 0, 1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe2->board, row, column, 0, 1);
                        if(!In_Closed_List(closed2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board2_solved = true;
                                cout << "Board 2 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe2);
                                Delete_List(closed2);
                                fringe2 = Create_Node(temp_board);
                                fringe2_size = 0;
                                closed2_size = 0;
                            }else{
                                tail2->next = Create_Node(temp_board);
                                tail2 = tail2->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed2;
                                closed2 = temp_node;
                                fringe2_size++;
                                closed2_size++;
                            }
                        }//in closed list
                    }//valid move
                }

                if(fringe2 != 0 && !board2_solved && !board2_failed){
                    //Check moving blank down
                    if(Valid_Move(row, column, 0, -1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe2->board, row, column, 0, -1);
                        if(!In_Closed_List(closed2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board1_solved = true;
                                cout << "Board 2 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe2);
                                Delete_List(closed2);
                                fringe2 = Create_Node(temp_board);
                                fringe2_size = 0;
                                closed2_size = 0;
                            }else{
                                tail2->next = Create_Node(temp_board);
                                tail2 = tail2->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed2;
                                closed2 = temp_node;
                                fringe2_size++;
                                closed2_size++;
                            }
                        }//in closed list
                    }//valid move
                }

                if(fringe2 != 0 && !board2_solved && !board2_failed){
                    //Check moving blank right
                    if(Valid_Move(row, column, 1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe2->board, row, column, 1, 0);
                        if(!In_Closed_List(closed2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board2_solved = true;
                                cout << "Board 2 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe2);
                                Delete_List(closed2);
                                fringe2 = Create_Node(temp_board);
                                fringe2_size = 0;
                                closed2_size = 0;
                            }else{
                                tail2->next = Create_Node(temp_board);
                                tail2 = tail2->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed2;
                                closed2 = temp_node;
                                fringe2_size++;
                                closed2_size++;
                            }
                        }//in closed list
                    }//valid move
                }
                if(fringe2 != 0 && !board2_solved && !board2_failed){
                    //Check moving blank left
                    if(Valid_Move(row, column, -1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe2->board, row, column, -1, 0);
                        if(!In_Closed_List(closed2, temp_board)){
                            if(Solution_Found(temp_board)){
                                board2_solved = true;
                                cout << "Board 2 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe2);
                                Delete_List(closed2);
                                fringe2 = Create_Node(temp_board);
                                fringe2_size = 0;
                                closed2_size = 0;
                            }else{
                                tail2->next = Create_Node(temp_board);
                                tail2 = tail2->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed2;
                                closed2 = temp_node;
                                fringe2_size++;
                                closed2_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if
            }



            if(fringe2 != 0 && !board2_solved && !board2_failed){
                temp_node = fringe2;
                fringe2 = fringe2->next;
                delete temp_node;
                fringe2_size--;
            }

        }else{
            if(!board2_solved && !board2_failed){
                board2_failed = true;
                cout << "Board 2 failed" << endl;
            }
        }

        if(fringe3 != 0 && !board3_solved && !board3_failed){//Check if the fringe is null
            if(!board3_solved&&!board3_failed){
                 Find_Blank(row, column, fringe3->board);//Find where the blank tile is.

                if(fringe3 != 0 && !board3_solved && !board3_failed){
                    //Check moving blank up
                    if(Valid_Move(row, column, 0, 1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe3->board, row, column, 0, 1);
                        if(!In_Closed_List(closed3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board3_solved = true;
                                cout << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe3);
                                Delete_List(closed3);
                                fringe3 = Create_Node(temp_board);
                                fringe3_size = 0;
                                closed3_size = 0;
                            }else{
                                tail3->next = Create_Node(temp_board);
                                tail3 = tail3->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed3;
                                closed3 = temp_node;
                                fringe3_size++;
                                closed3_size++;
                            }
                        }//in closed list
                    }//valid move
                }

                if(fringe3 != 0 && !board3_solved && !board3_failed){
                    //Check moving blank down
                    if(Valid_Move(row, column, 0, -1)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe3->board, row, column, 0, -1);
                        if(!In_Closed_List(closed3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board3_solved = true;
                                cout << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe3);
                                Delete_List(closed3);
                                fringe3 = Create_Node(temp_board);
                                fringe3_size = 0;
                                closed3_size = 0;
                            }else{
                                tail3->next = Create_Node(temp_board);
                                tail3 = tail3->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed3;
                                closed3 = temp_node;
                                fringe3_size++;
                                closed3_size++;
                            }
                        }//in closed list
                    }//valid move
                }

                if(fringe3 != 0 && !board3_solved && !board3_failed){
                    //Check moving blank right
                    if(Valid_Move(row, column, 1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe3->board, row, column, 1, 0);
                        if(!In_Closed_List(closed3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board3_solved = true;
                                cout << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe3);
                                Delete_List(closed3);
                                fringe3 = Create_Node(temp_board);
                                fringe3_size = 0;
                                closed3_size = 0;
                            }else{
                                tail3->next = Create_Node(temp_board);
                                tail3 = tail3->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed3;
                                closed3 = temp_node;
                                fringe3_size++;
                                closed3_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if

                if(fringe3 != 0 && !board3_solved && !board3_failed){
                    //Check moving blank left
                    if(Valid_Move(row, column, -1, 0)){//check if up is a valid move.
                        temp_board = Swap_Tile(fringe3->board, row, column, -1, 0);
                        if(!In_Closed_List(closed3, temp_board)){
                            if(Solution_Found(temp_board)){
                                board3_solved = true;
                                cout << "Board 3 has been solved!" << endl;
                                Print_Board(temp_board);
                                Delete_List(fringe3);
                                Delete_List(closed3);
                                fringe3 = Create_Node(temp_board);
                                fringe3_size = 0;
                                closed3_size = 0;
                            }else{
                                tail3->next = Create_Node(temp_board);
                                tail3 = tail3->next;
                                temp_node = Create_Node(temp_board);
                                temp_node->next = closed3;
                                closed3 = temp_node;
                                fringe3_size++;
                                closed3_size++;
                            }
                        }//in closed list
                    }//valid move
                }//end if
            }//end if

            if(fringe3 != 0 && !board3_solved && !board3_failed){
                temp_node = fringe3;
                fringe3 = fringe3->next;
                delete temp_node;
                fringe3_size--;
            }//end if

        }else{
            if(!board3_solved && !board3_failed){
                board3_failed = true;
                cout << "Board 3 failed" << endl;
            }
        }

    }//end while
}//end breadth first search
