#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int calculateHalfMoves(const char *san)
{
    int length = strlen(san);
    int isCompleteMove = 0;
    char numberStack[3]; // Stack for digits, assuming no chess game exceeds 999 moves
    int stackTop = -1;
    int i;
    // Step 1 & 2: Iterate backwards to find the period and check for complete move
    for (i = length - 1; i >= 0; i--)
    {
        if (san[i] == ' ')
        {
            isCompleteMove = 1;
        }
        else if (san[i] == '.')
        {
            break;
        }
    }

    // Step 3: Collect digits before the period
    for (; i >= 0; i--)
    {
        if (isdigit(san[i]))
        {
            numberStack[++stackTop] = san[i];
        }
        else
        {
            break;
        }
    }

    // Convert the collected digits to a number
    int moveNumber = 0;
    for (int i = 0; i <= stackTop; i++)
    {
        moveNumber = moveNumber * 10 + (numberStack[stackTop - i] - '0');
    }

    // Step 4: Calculate the number of half moves
    return isCompleteMove ? moveNumber * 2 : moveNumber * 2 - 1;
}

int main()
{
    const char *SAN = "1.e4 c5 2.Nf3 Nc6 3.d4 cxd4 4.Nxd4";

    int halfMoves = calculateHalfMoves(SAN);
    printf("Number of half moves: %d\n", halfMoves);

    return 0;
}
