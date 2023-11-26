#include <stdlib.h>
#include <stdio.h>
#include "smallchesslib.h"

char fen[4096];

void myPutChar(char c)
{
    putchar(c); // Print the character to the console
}

int main()
{
    const char *SAN = "1.c4 c5 2.Nf3 Nf6 3.d4 cxd4 4.Nxd4 e6 5.g3 d5 6.Bg2 e5 7.Nc2 d4 8.O-O Nc6 9.Nd2 Bg4 10.Nf3 a5 11.Bg5 Bc5 12.e4 h6 13.Bxf6 Qxf6 14.Nce1 Qe6 15.Nd3 Be7 16.h3 Bxf3 17.Qxf3 Qxc4 18.Rfc1 Qe6 19.Qf5 Bd6 20.Qxe6+ fxe6 21.a3 Kd7 22.Bf1 a4 23.Ne1 Ra5 24.Rc2 Rc8 25.Rac1 Na7 26.Rxc8 Nxc8 27.Bc4 Be7 28.Nd3 Nd6 29.f3 Nxc4 30.Rxc4 Bd6 31.Kf2 Ra6 32.Ke2 g5 33.Kd1 Ra8 34.Ke2 h5 35.Rc1 Rh8 36.Rh1 h4 37.g4 b5 38.Rc1 Rb8 39.Kd1 Rb6 40.Rc2 Rb8 41.Rc1 Rc8";

    SCL_Record r;
    // SCL_recordInit(r);
    SCL_recordFromPGN(r, SAN);
    uint16_t l = SCL_recordLength(r);
    printf("Num Half Moves: %d\n", l);
    SCL_Board initialState = SCL_BOARD_START_STATE; // Assume this is the initial state of the board
    // Call the function
    SCL_printPGN(r, myPutChar, initialState);

    // for (int i = 1; i <= 7; i++)
    // {
    //     SCL_Board board = SCL_BOARD_START_STATE;
    //     SCL_recordApply(r, board, i);
    //     SCL_boardToFEN(board, fen);
    //     printf("FEN: %s\n", fen);
    // }
    // SCL_Board board = SCL_BOARD_START_STATE;
    // SCL_recordApply(r, board, 1000);
    // int half_moves = board[65];
    // printf("Half moves: %d\n", half_moves);
    // SCL_boardToFEN(board, fen);

    return 0;
}
