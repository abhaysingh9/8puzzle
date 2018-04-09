#include <bits/stdc++.h>
using namespace std;
//Verison Tries to detect and elliminate duplicate nodes to optimize serach memorywise.
void checkValid(int temp[]);
void print(int temp[3][3]);
void solve(int initial[3][3], int fin[3][3], int x, int y,int choice);
int computeMisplaced(int initial[3][3], int fin[3][3]);
int computeManhattan(int initial[3][3], int fin[3][3]);
long long int computeuni(int man, int dis, int initial[3][3]);
long long int numberOfNodes=0;
vector < long long int > a;
int islegal(int x, int y)
{   // To check if the move is legal or not. (otherwise we may get segmentation fault.)
    return ((x >= 0 && x < 3) && (y >= 0 && y < 3));
}
int isNonDuplicate(int x, int y, int initial[3][3])
{   // To check if the move has been seen already. (otherwise we may run outta mem.)
    long long int temp = computeuni(x,y,initial);
    if(std::find(a.begin(), a.end(), temp) != a.end())
      return 0;  //elem exists in the vector
    a.push_back(temp);
    return 1;
}

struct Node
{
    //state at the node
    int state[3][3];
    //the number of misplaced tiles Heuristic
    int misplaced;
    //store manhattan Heuristic
    int manhattan;
    //cost to reach assumed to be 1 for ever parent child edge.
    int uniformCost;
    // position of blank in state
    int x, y;
    int Astar;
};
struct comparator{
    bool operator()(const Node* lhs, const Node* rhs) const{
        return (lhs->Astar) > (rhs->Astar);
    }
};

Node* expand(int state[3][3], int x, int y, int newX,
              int newY, int Cost){
    Node* node = new Node;
    numberOfNodes++;
    // copy data from parent node to current node
    memcpy(node->state, state, sizeof node->state);
    // move tile by 1 postion
    swap(node->state[x][y], node->state[newX][newY]);
    // set no. misplaced tiles
    node->misplaced = INT_MAX;
    // set no. manhattan Heuristic
    node->manhattan = INT_MAX;
    // set cost to reach
    node->uniformCost = Cost;
    node->Astar = INT_MAX;
    // update new blank tile cordinates
    node->x = newX;
    node->y = newY;
    return node;
}

int main()
{

    int initial[3][3] = //if default  chosen this will  be initial state
    {   {8, 6, 7},
        {2, 5, 4},
        {3, 0, 1}
    };
    int Choice;
    // Input for matrix
    cout<<"Welcome to the 8-puzzle solver."<<endl<<
      "Type 1 to use a default puzzle, or 2 to enter your own puzzle"
      <<endl;
    cin>>Choice;
    if(Choice == 2){
      cout<<"Please enter the numbers for each row and press enter"<<endl;
      cout<<"Note use 0 for blank."<<endl;
      for(int i = 0; i < 3 ; i++){
        cout<<"Enter Space seprated elements of the row "<<i+1<<endl;
        for(int j = 0; j < 3 ; j++){
          cin>>initial[i][j];
        }
      }
    }
    cout<<"The Entered Matrix is..."<<endl;
    print(initial);
    cout<<endl;
    checkValid((int *)initial); //To check if input matrix is valid or not
    if((Choice != 1) && (Choice != 2)){
      cout<<"Wrong Choice... Exiting"; //incase of erroneous choice entry
      exit(0);
    }

    // Input for Algorithm
    cout<<"choose the algorithm"<<endl;
    cout<<"1. Uniform Cost Search"<<endl;
    cout<<"2. A* with misplaced tile"<<endl;
    cout<<"3. A* with Manhattan distance"<<endl;
    cin>>Choice;
    cout<<endl;
    Choice = Choice-1;

    // Defining goal state
    int fin[3][3] =
    {   {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };

    //find the postion of blank in initial
    int x,y;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if(initial[i][j]==0){
          x = i; y = j;
        }
      }
    }
    //Calculating the time
    clock_t start = clock();
    solve(initial,fin,x,y,Choice);
    clock_t end = clock();
    if((double(end - start) / CLOCKS_PER_SEC)==0)
      cout<<"total clocks taken by solve function: "<< (end - start) ;
    else cout<<"total time taken by solve function: "<< double(end - start) / CLOCKS_PER_SEC;

    return 0;
}
void checkValid(int temp[]){
  //check if the given matrix has a solution before building the tree.
  // This is done by counting number of inversions in the matrix if even solvable
  // if odd it's impossible.
  //inspired by https://www.cs.bham.ac.uk/~mdr/teaching/modules04/java2/TilesSolvability.html

  int inverted = 0;
  for (int i = 0; i < 8; i++)
      for (int j = i+1; j < 9; j++)
           if (temp[j] && temp[i] &&  temp[i] > temp[j])
                inverted++;
  /*Commented out to check the robustness of solve function
  if(inv % 2 != 0){
    cout<<"The Matrix entered is impossible to solve";
    exit(0);
  }*/
  // check if entries are among 0-8
  for (int i = 0; i < 9 ; i++) {
    if(temp[i] < 0 && temp[i] > 8 ){
      cout<<"Invalid entry"<< temp[i]<<" in the matrix";
      exit(0);
    }
  }
}

void print(int temp[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            cout<< temp[i][j]<<" ";
        cout<<endl;
    }
}

// Main solving subroutine
void solve(int initial[3][3], int fin[3][3], int x, int y, int Choice){
  int k1,k2,k3,numberOfNodesExpanded;
  numberOfNodesExpanded = 0;
  if(Choice==0){
    k1 = 1; k2 = 0; k3 = 0;
  }
  else if(Choice==1){
    k1 = 1; k2 = 1; k3 = 0;
  }
  else{
      k1 = 1; k2 = 0; k3 = 1;
  }
  //1.make priority queue
  priority_queue <Node*, vector<Node*>, comparator> frontier;
  //2.start building tree with initial state
  Node* initnode = expand(initial,x,y,x,y,0);
  //enter heuristic numbers
  initnode->uniformCost = 0;
  initnode->manhattan = computeManhattan(initial,fin);
  initnode->misplaced = computeMisplaced(initial,fin);
  initnode->Astar = (k1*(initnode->uniformCost)) + (k2*(initnode->misplaced)) + (k3*(initnode->manhattan));
  //3.put the only node in pri queue
  frontier.push(initnode);
  //4. take it out from pri queue
  //5. check if this is goal state
  //6. expand the node and put them in pq
  //repeat.3-6 on algorithm given.
  int rowop[] = {-1, 0, 1, 0 };
  int colop[] = { 0, 1, 0, -1 };
  int maxQueue = 0;
  // above are possible operations
  while (!frontier.empty())
  {
      // Find least estimated cost node
       Node* min = frontier.top();
       if(frontier.size() > maxQueue) maxQueue = frontier.size();
       cout<<"About to expand the following state"<<endl;
       cout<<endl;
       print(min->state);
       cout<<endl;
       cout<<"Cost associated with this state is: "<<min->Astar<<endl<<endl;
      // delete the node from queue
      frontier.pop();

      // check if popped element is the goal state
      if (min->misplaced == 0)
      {
          cout<<"The goal state reached"<<endl;
          print(min->state);
          cout<<endl;
          cout<<"Number of states created: "<<numberOfNodes<<endl;
          cout<<"Number of states expanded: "<<numberOfNodesExpanded<<endl;
          cout<<"Max entries in the queue: "<<maxQueue<<endl;
          cout<<"The solution was at level: "<<min->uniformCost<<endl;
          return;
      }
      numberOfNodesExpanded++;
      // do for each child of min
      // max 4 children for a node
      for (int i = 0; i < 4; i++)
      {
          if (islegal(min->x + rowop[i], min->y + colop[i]))
          {
              // create a child node and calculate costs
              Node* child = expand(min->state, min->x,
                            min->y, min->x + rowop[i],
                            min->y + colop[i],
                            ((min->uniformCost) + 1));
              child->misplaced = computeMisplaced(child->state, fin);
              child->manhattan = computeManhattan(child->state, fin);
              child->Astar = k1*child->uniformCost + k2*child->misplaced + k3*child->manhattan;
              // Add child to priority_queue
              if(isNonDuplicate(child->misplaced,child->manhattan,child->state))
                frontier.push(child);
          }
      }
  }
  cout<<"No Solution "<<endl;
  cout<<"Number of states created: "<<numberOfNodes<<endl;
  cout<<"Number of states expanded: "<<numberOfNodesExpanded<<endl;
  cout<<"Max entries in the queue: "<<maxQueue<<endl;
}
int computeMisplaced(int initial[3][3], int fin[3][3])
{
    //compute how many tiles are in wrong places
    int count = 0;
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        if (initial[i][j] && initial[i][j] != fin[i][j])// if initial state not equal to final and not equal to 0
           count++;
    return count;
}
int computeManhattan(int initial[3][3], int fin[3][3])
{
    //compute how far is each tile from it's destination
    int count = 0;
    int tileVal=0, finx=0, finy=0;
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++){
          tileVal = initial[i][j];
          if(tileVal == 0) continue;
          finx = (tileVal-1) / 3;
          finy = (tileVal-1) % 3;
          count = count + abs(finx - i) + abs(finy - j); //if tile at final position,
          // count=count
      }
    return count;
}
long long int computeuni(int man, int dis, int initial[3][3]){ //Modified from stack exchange
  int sum=0;
  for (int i = 0; i < 3; i++) {
    for(int j=0;j<3;j++)
    sum = sum + 300*pow(i+1,3)+10*pow(j+1,3)*(initial[i][j]) ;
  }
  return sum*man*dis;
}
