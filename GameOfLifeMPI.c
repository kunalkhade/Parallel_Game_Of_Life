/*-----------------------------------------------------------------
 * File:    GameOfLifeMPI2.c
 * 
 * Purpose: Game of Life method using MPI. 
 *
 * Compile: mpicc -o GameOfLifeMPI GameOfLifeMPI.c
 *   
 * Run:     mpirun -np 2 ./GameOfLifeMPI2
 *
 * Input:   Default 2 processes in Blocking Mode (Send and Recv)
 * Output:  Parameters: Latency and Bandwidth between 2 processes
 *-----------------------------------------------------------------*/
// mpicc -o GameOfLifeMPI2 GameOfLifeMPI2.c
// mpirun -np 2 ./GameOfLifeMPI2

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>

#define NUMBERROWS 28
#define esc 27
#define cls() printf("%c[2J",esc)
#define pos(row,col) printf("%c[%d;%dH",esc,row,col)

//Initialize constants and initial grid of 28x28
char  *DISH0[ NUMBERROWS ];
char  *DISH1[ NUMBERROWS ];
char  *PATTERN[NUMBERROWS] = {
  "                                                                                  ",
  "   #                                                                              ",
  " # #                                            ###                               ",
  "  ##                                                                              ",
  "                                                                                  ",
  "                                                      #                           ",
  "                                                    # #                           ",
  "                                                     ##                           ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "             #                                                                    ",
  "           # #                                                                    ",
  "            ##                                                                    ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  ",
  "                                                                                  " 
};
int ROWSIZE = strlen( "                                                                                  ") + 1;
//Functional prototypes
void life( char**, char**, int );
void initDishes( int  );
void print( char **, int );

//Initialize dishes present and future
void initDishes( int rank ) {
  int i;
  for (i = 0; i< NUMBERROWS; i++ )  {
    DISH0[i] = (char *) malloc( ( strlen( PATTERN[0] ) + 1 ) * sizeof( char ) );
    strcpy( DISH0[i], PATTERN[i] );

    DISH1[i] = (char *) malloc( (strlen( DISH0[0] )+1) * sizeof( char )  );
    strcpy( DISH1[i], PATTERN[i] );
  }
}

//Print Everything on screen
//Cut entire grid into 2 segments upper and lower 
//Provide information to mpi so two different processes run different segment. 
void print( char* dish[], int rank ) {
  int i;

  if ( rank == 0 ) {
    for (i=0; i<NUMBERROWS/2; i++ ) {
      if ( dish[i] == NULL ) continue;
      pos( i, 0 );
      printf( "%s\n", dish[i] );
    }
  }

  if ( rank == 1 ) {
    for (i=NUMBERROWS/2; i<NUMBERROWS; i++ ) {
      if ( dish[i] == NULL ) continue;
      pos( i, 0 );
      printf( "%s\n", dish[i] );
    }

  }
}

  //input array processes using 3x3 game of life mask
  //Scan complete array and place # or ' ' in updated array as per the game of life rules
  //Return updated array string
void  life( char** dish, char** newGen, int rank ) {


  int i, j, row;
  int rowLength = strlen( dish[0] );
  int dishLength = NUMBERROWS;
  
  int lowerRow, upperRow;

  //slice array into two                               
  if ( rank == 0 ) {
    lowerRow = 0;
    upperRow = NUMBERROWS/2;
  }
  if ( rank == 1 ) {
    lowerRow = NUMBERROWS/2;
    upperRow = NUMBERROWS;
  }
  //Check neighbors and update 
  for (row = lowerRow; row < upperRow; row++) {
    
    if ( dish[row] == NULL )
      continue;

    for ( i = 0; i < rowLength; i++) { 
                                       

      int r, j, neighbors = 0;
      char current = dish[row][i];

      for ( r = row - 1; r <= row + 1; r++) {

        int realr = r;
        if (r == -1)
          realr = dishLength - 1;
        if (r == dishLength)
          realr = 0;

        for (int j = i - 1; j <= i + 1; j++) {

          int realj = j;
          if (j == -1)
            realj = rowLength - 1;
          if (j == rowLength)
            realj = 0;

          if (r == row && j == i)
            continue; 
          if (dish[realr][realj] == '#')
            neighbors++;
        }
      }

      if (current == '#') {
        if (neighbors < 2 || neighbors > 3)
          newGen[row][i] =  ' ';
        else
          newGen[row][i] = '#';
      }

      if (current == ' ') {
        if (neighbors == 3)
          newGen[row][i] = '#';
        else
          newGen[row][i] = ' ';
      }
    }
  }
}

int main( int argc, char* argv[] ) {
  int gens = 3000;      // # of generations
  int i;
  char **dish, **future, **temp;

  //MPI Initialize
  int noTasks = 0;
  int rank = 0;
  MPI_Status status;   

  MPI_Init( &argc, &argv );

  //Boundary Condition 
  MPI_Comm_size( MPI_COMM_WORLD, &noTasks );
  if ( noTasks != 2 ) {
    printf( "Number of Processes/Tasks must be 2.  Number = %d\n\n", noTasks );
    MPI_Finalize();
    return 1;
  }

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

  initDishes( rank );

  dish   = DISH0;
  future = DISH1;

  //clear screen
  cls();

  print( dish, rank );    

  //Iteration
  for ( i = 0; i < gens; i++) {
    
    pos( 33+rank, 0 );
    printf( "Rank %d: Generation %d\n", rank, i );

    // apply the rules 
    // generate the next generation.
    life( dish, future, rank );

    // display the new generation
    print( dish, rank );

    // add a bit of a delay to better see the visualization
    // remove this part to get full timing.
    if (rank == 0 ) sleep( 1 );

    if (rank==0 ) {
      //          buffer                #items   item-size src/dest tag   world  
      MPI_Send( future[    0         ], ROWSIZE, MPI_CHAR,    1,     0,  MPI_COMM_WORLD );
      MPI_Send( future[NUMBERROWS/2-1], ROWSIZE, MPI_CHAR,    1,     0,  MPI_COMM_WORLD );
      MPI_Recv( future[NUMBERROWS-1],   ROWSIZE, MPI_CHAR,    1,     0,  MPI_COMM_WORLD, &status );
      MPI_Recv( future[NUMBERROWS/2],   ROWSIZE, MPI_CHAR,    1,     0,  MPI_COMM_WORLD, &status );
    }
    if (rank==1 ) {
      MPI_Recv( future[    0         ], ROWSIZE, MPI_CHAR,    0,     0,  MPI_COMM_WORLD, &status );
      MPI_Recv( future[NUMBERROWS/2-1], ROWSIZE, MPI_CHAR,    0,     0,  MPI_COMM_WORLD, &status );
      MPI_Send( future[NUMBERROWS-1],   ROWSIZE, MPI_CHAR,    0,     0,  MPI_COMM_WORLD );
      MPI_Send( future[NUMBERROWS/2],   ROWSIZE, MPI_CHAR,    0,     0,  MPI_COMM_WORLD );
    }
    // copy future to dish
    temp = dish;
    dish = future;
    future = temp;
  }

  //display the last generation
  print(dish, rank);

  //close MPI 
  pos( 30+rank, 0 );
  printf( "Process %d done.  Exiting\n\n", rank );
  MPI_Finalize();

  return 0;
}