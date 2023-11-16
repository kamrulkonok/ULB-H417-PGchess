#include <stdlib.h>
#include <stdio.h>
#include "smallchesslib.h"

char fen[4096];

int main()
{
    const char *sanMoves = "1.e4 c5 2.Nf3 Nc6 3.d4 cxd4 4.Nxd4";
    SCL_Record r;
    SCL_recordInit(r);
    SCL_recordFromPGN(r, sanMoves);

    for (int i = 1; i <= 7; i++)
    {
        SCL_Board board = SCL_BOARD_START_STATE;
        SCL_recordApply(r, board, i);
        SCL_boardToFEN(board, fen);
        printf("FEN: %s\n", fen);
    }

    return 0;
}
