#include <stdio.h>
#include <postgres.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "utils/builtins.h"
#include "libpq/pqformat.h"
// #include "smallchesslib.h"

PG_MODULE_MAGIC;

typedef struct
{
    // Length field including the size of the length field itself
    int32 length;
    // FEN string data
    char fen[FLEXIBLE_ARRAY_MEMBER];
} chessboard;

// register the function as a PostgreSQL function that follows
// version 1 calling conventions
PG_FUNCTION_INFO_V1(chessboard_in);
PG_FUNCTION_INFO_V1(chessboard_out);
PG_FUNCTION_INFO_V1(chessboard_recv);
PG_FUNCTION_INFO_V1(chessboard_send);
PG_FUNCTION_INFO_V1(chessboard_constructor);
PG_FUNCTION_INFO_V1(chessboard_eq);
PG_FUNCTION_INFO_V1(chessboard_lt);
PG_FUNCTION_INFO_V1(chessboard_le);
PG_FUNCTION_INFO_V1(chessboard_gt);
PG_FUNCTION_INFO_V1(chessboard_ge);
PG_FUNCTION_INFO_V1(chessboard_neq);

Datum chessboard_in(PG_FUNCTION_ARGS)
{
    char *fenStr = PG_GETARG_CSTRING(0);
    // Calculate total length
    int len = strlen(fenStr) + VARHDRSZ;
    chessboard *result = (chessboard *)palloc(len);
    SET_VARSIZE(result, len);
    memcpy(result->fen, fenStr, len - VARHDRSZ);

    PG_RETURN_POINTER(result);
}

Datum chessboard_out(PG_FUNCTION_ARGS)
{
    chessboard *board = (chessboard *)PG_GETARG_POINTER(0);
    char *fenStr = palloc(VARSIZE(board) - VARHDRSZ + 1); // +1 for null terminator
    memcpy(fenStr, board->fen, VARSIZE(board) - VARHDRSZ);
    fenStr[VARSIZE(board) - VARHDRSZ] = '\0';
    PG_RETURN_CSTRING(fenStr);
}

Datum chessboard_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    const char *str = pq_getmsgstring(buf);
    int len = strlen(str) + VARHDRSZ;
    chessboard *result = (chessboard *)palloc(len);

    SET_VARSIZE(result, len);
    memcpy(result->fen, str, len - VARHDRSZ);

    PG_RETURN_POINTER(result);
}

Datum chessboard_send(PG_FUNCTION_ARGS)
{
    chessboard *board = (chessboard *)PG_GETARG_POINTER(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendstring(&buf, board->fen);

    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum chessboard_constructor(PG_FUNCTION_ARGS)
{
    // Get the text input (FEN string)
    text *fenText = PG_GETARG_TEXT_PP(0);
    // Convert the text input to a C string
    char *fenStr = text_to_cstring(fenText);
    // Calculate the total length of the chessboard structure
    int len = strlen(fenStr) + VARHDRSZ;
    // Allocate memory for the chessboard structure
    chessboard *result = (chessboard *)palloc(len);
    // Set the size of the chessboard structure
    SET_VARSIZE(result, len);
    // Copy the FEN string into the structure
    memcpy(result->fen, fenStr, len - VARHDRSZ);
    // Return the new chessboard structure
    PG_RETURN_POINTER(result);
}

Datum chessboard_eq(PG_FUNCTION_ARGS)
{
    // Get the two chessboard structures
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    // Compare the two FEN strings
    PG_RETURN_BOOL(strcmp(board1->fen, board2->fen) == 0);
}

Datum chessboard_neq(PG_FUNCTION_ARGS)
{
    // Get the two chessboard structures
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    // Compare the two FEN strings
    PG_RETURN_BOOL(strcmp(board1->fen, board2->fen) != 0);
}

Datum chessboard_lt(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(strcmp(board1->fen, board2->fen) < 0);
}

Datum chessboard_le(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(strcmp(board1->fen, board2->fen) <= 0);
}

Datum chessboard_gt(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(strcmp(board1->fen, board2->fen) > 0);
}

Datum chessboard_ge(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);

    PG_RETURN_BOOL(strcmp(board1->fen, board2->fen) >= 0);
}