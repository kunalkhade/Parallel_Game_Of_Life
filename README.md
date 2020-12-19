# Parallel_Game_Of_Life

Its a infinite, two dimensional orthogonal grid of square cells, each of which is
in one of two possible states, Live or Dead. Every cells interacts with its eight
neighbours at each time. (I designed this program only with 2 processors. I
used predefined matrix)

1) Any live cell with fewer than two live neighbours dies, as if by
under-population.
2) Any live cell with two or three live neighbours lives on to the next
generation.
3) Any live cell with more than three live neighbours dies, as if by
overpopulation.
4) Any dead cell with exactly three live neighbours becomes a live cell, as if by
reproduction.
There are different type of pattern with its own characteristic
1) Still life’s - These pattern do not change from current state to updated
state.
2) Oscillators - These pattern return to their initial state after a finite number
of generations.
3) Spaceships - These pattern translate theme selves across the grid.

# Algorithm and libraries
My approach for this problem statement will be same as classical image pro-
cessing. I used 3x3 mask in which i scan from X+1,Y+1 array point and scan
throughout the array till Xn-1, Xn-1. There will be 2 states of program one is
current and second is update state so every-time screen gets clear and update
next state.
Parallel method is used in the middle of the program when i divide entire ar-
ray into two different sections. Upper and Lower are assign to two different
processes. So rank 0 assign to upper and rank 1 assign to lower.

