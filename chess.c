#include <stdio.h>
#include <postgres.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "lib/stringinfo.h"

#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "smallchesslib.h"

PG_MODULE_MAGIC;
/************************************************************************************************************
                                          Chessboard Structure
************************************************************************************************************/

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

chessboard *create_chessboard_from_fen(const char *fenStr)
{
    int len = strlen(fenStr) + VARHDRSZ;
    chessboard *result = (chessboard *)palloc(len);
    SET_VARSIZE(result, len);
    memcpy(result->fen, fenStr, len - VARHDRSZ);
    return result;
}

Datum chessboard_in(PG_FUNCTION_ARGS)
{
    char *fenStr = PG_GETARG_CSTRING(0);
    chessboard *result = create_chessboard_from_fen(fenStr);
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

/************************************************************************************************************
                                          Chessgame structure
************************************************************************************************************/

// Helper function to create a string builder
SCL_StringBuilder *SCL_createStringBuilder()
{
    SCL_StringBuilder *sb = palloc0(sizeof(SCL_StringBuilder));
    if (!sb)
        return NULL;

    // Initial capacity
    sb->capacity = 256;
    sb->length = 0;
    sb->buffer = palloc0(sb->capacity * sizeof(char));

    if (!sb->buffer)
    {
        free(sb);
        return NULL;
    }

    sb->buffer[0] = '\0';
    return sb;
}

// Function to append a character to the string builder
void SCL_appendChar(SCL_StringBuilder *sb, char c)
{
    if (sb->length + 1 >= sb->capacity)
    {
        size_t newCapacity = sb->capacity * 2;
        char *newBuffer = repalloc(sb->buffer, newCapacity);
        if (!newBuffer)
            return; // Handle reallocation failure

        sb->buffer = newBuffer;
        sb->capacity = newCapacity;
    }

    sb->buffer[sb->length] = c;
    sb->length++;
    sb->buffer[sb->length] = '\0';
}

// Function to finalize the string builder and get the final string
char *SCL_finalizeStringBuilder(SCL_StringBuilder *sb)
{
    char *finalString = repalloc(sb->buffer, sb->length + 1); // Resize to actual size
    pfree(sb);
    return finalString;
}

typedef struct
{
    SCL_Record record;
} chessgame;

int32 getChessgameNumHalfMoves(chessgame *game)
{
    return SCL_recordLength(game->record);
}

char *getChessgameSanMoves(chessgame *game)
{
    SCL_Record r;
    memcpy(r, game->record, sizeof(SCL_Record));
    char *pgnString = SCL_printPGN(r);
    return pgnString;
}

char *getChessgameBoard(chessgame *game, int halfMoveIndex)
{
    // Handle invalid index
    if (halfMoveIndex < 0)
        return NULL;
    // maximum length of fen can be 136
    char *fen = palloc(137 * sizeof(char));
    SCL_Board board = SCL_BOARD_START_STATE;
    // Copy the record
    SCL_Record r;
    memcpy(r, game->record, sizeof(SCL_Record));
    // Apply the required number of half moves from the record to the board
    SCL_recordApply(r, board, halfMoveIndex);
    // Convert the board to its FEN representation
    SCL_boardToFEN(board, fen);
    return fen;
}

PG_FUNCTION_INFO_V1(chessgame_in);
PG_FUNCTION_INFO_V1(chessgame_out);
PG_FUNCTION_INFO_V1(chessgame_recv);
PG_FUNCTION_INFO_V1(chessgame_send);
PG_FUNCTION_INFO_V1(text_to_chessgame);
PG_FUNCTION_INFO_V1(getBoard);
PG_FUNCTION_INFO_V1(getFirstMoves);
PG_FUNCTION_INFO_V1(hasOpening);
PG_FUNCTION_INFO_V1(hasBoard);
PG_FUNCTION_INFO_V1(chessgame_lt);
PG_FUNCTION_INFO_V1(chessgame_lte);
PG_FUNCTION_INFO_V1(chessgame_eq);
PG_FUNCTION_INFO_V1(chessgame_neq);
PG_FUNCTION_INFO_V1(chessgame_gte);
PG_FUNCTION_INFO_V1(chessgame_gt);
PG_FUNCTION_INFO_V1(chessgame_cmp);

// Helper function to check if 'pre' is a prefix of 'str'
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

chessgame *create_chessgame(const char *SAN)
{
    // Allocate memory for the chessgame
    chessgame *game = palloc(sizeof(chessgame));
    // create the record
    SCL_Record r;
    // apply the SAN to this record
    SCL_recordFromPGN(r, SAN);
    // put the record in the game
    memcpy(game->record, r, sizeof(SCL_Record));
    // sanity check
    // char *pgnString = getChessgameSanMoves(game);

    return game;
}

Datum chessgame_in(PG_FUNCTION_ARGS)
{
    char *sanMovesStr;
    chessgame *game;

    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("input string must not be null")));

    sanMovesStr = PG_GETARG_CSTRING(0);
    game = create_chessgame(sanMovesStr);
    PG_FREE_IF_COPY(sanMovesStr, 0);
    PG_RETURN_POINTER(game);
}

Datum chessgame_out(PG_FUNCTION_ARGS)
{
    chessgame *game;
    char *shortSANStr;
    char *sanMovesStr;

    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame must not be null")));

    game = (chessgame *)PG_GETARG_POINTER(0);
    char *SAN = getChessgameSanMoves(game);
    PG_FREE_IF_COPY(game, 0);
    PG_RETURN_CSTRING(SAN);
}

Datum chessgame_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    chessgame *game;

    game = (chessgame *)palloc(sizeof(chessgame));
    if (game == NULL)
        ereport(ERROR,
                (errcode(ERRCODE_OUT_OF_MEMORY),
                 errmsg("out of memory")));

    // Read the record data from the buffer
    for (int i = 0; i < SCL_RECORD_MAX_SIZE; i++)
    {
        game->record[i] = pq_getmsgint(buf, sizeof(int));
    }

    PG_RETURN_POINTER(game);
}

Datum chessgame_send(PG_FUNCTION_ARGS)
{
    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    StringInfoData buf;

    // Initialize a StringInfo buffer
    pq_begintypsend(&buf);

    // Send the record data
    for (int i = 0; i < SCL_RECORD_MAX_SIZE; i++)
    {
        pq_sendint(&buf, game->record[i], sizeof(int));
    }

    // Return the buffer
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum text_to_chessgame(PG_FUNCTION_ARGS)
{
    text *inputText;
    char *sanMovesStr;
    chessgame *game;

    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("input string must not be null")));

    inputText = PG_GETARG_TEXT_PP(0);
    sanMovesStr = text_to_cstring(inputText);
    game = create_chessgame(sanMovesStr);

    PG_FREE_IF_COPY(inputText, 0);
    PG_RETURN_POINTER(game);
}

/************************************************************************************************************
                                          Chessgame functions
************************************************************************************************************/

bool lessThan(chessgame *game1, chessgame *game2)
{
    /*
     * game1 is 'less than' game2 if game1 is a prefix of game2
     */
    const char *SAN1 = getChessgameSanMoves(game1);
    const char *SAN2 = getChessgameSanMoves(game2);
    return prefix(SAN1, SAN2);
}

Datum getBoard(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame must not be null")));

    if (PG_ARGISNULL(1))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("half-move index must not be null")));

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    int halfMoveIndex = PG_GETARG_INT32(1);
    // Max number of half moves is equivalent to number of half moves in game
    if (halfMoveIndex > getChessgameNumHalfMoves(game))
        halfMoveIndex = getChessgameNumHalfMoves(game);
    // return the FEN of chessboard at index i
    char *fen = getChessgameBoard(game, halfMoveIndex);

    if (fen == NULL)
        PG_RETURN_NULL();

    text *result = cstring_to_text(fen);
    PG_RETURN_TEXT_P(result);
}

char *getFirstNHalfMoves(const char *inputSAN, int N)
{
    if (N <= 0 || inputSAN == NULL)
    {
        // Return an empty string for invalid input.
        return pstrdup("");
    }

    int halfMoveCount = 0;
    int i = 0;
    int length = strlen(inputSAN);

    // Traverse the string to find the Nth half-move.
    while (i < length && halfMoveCount < N)
    {
        // Increment halfMoveCount on encountering a space,
        // but ensure it's not a space following a move number.
        if (inputSAN[i] == ' ' && (i == 0 || inputSAN[i - 1] != '.'))
        {
            halfMoveCount++;
        }
        i++;
    }

    // If the string ends with a space, move back to exclude it.
    if (i > 0 && inputSAN[i - 1] == ' ')
    {
        i--;
    }

    // Allocate memory for the output string and copy the substring.
    char *output = (char *)palloc(sizeof(char) * (i + 1));
    if (output == NULL)
    {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }
    strncpy(output, inputSAN, i);
    output[i] = '\0'; // Null-terminate the string.
    return output;
}

Datum getFirstMoves(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame must not be null")));

    if (PG_ARGISNULL(1))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("half-move count must not be null")));

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    int n = PG_GETARG_INT32(1);

    if (n <= 0)
    {
        text *empty_text = cstring_to_text("");
        PG_RETURN_TEXT_P(empty_text);
    }

    char *SAN = getChessgameSanMoves(game);
    char *trimmedSAN = getFirstNHalfMoves(SAN, n);
    PG_RETURN_TEXT_P(cstring_to_text(trimmedSAN));
}

Datum hasOpening(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame arguments must not be null")));

    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    // game1 has the opening of game2 if game2 < game1
    PG_RETURN_BOOL(lessThan(game2, game1));
}

Datum hasBoard(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1) || PG_ARGISNULL(2))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("Arguments must not be null")));

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    chessboard *board = (chessboard *)PG_GETARG_POINTER(1);
    int nHalfMoves = PG_GETARG_INT32(2);

    char *boardFen = board->fen;
    bool found = false;

    for (int i = 0; i <= nHalfMoves && !found; ++i)
    {
        char *currentFen = getChessgameBoard(game, i);
        if (currentFen != NULL && strcmp(boardFen, currentFen) == 0)
            found = true;
    }

    PG_RETURN_BOOL(found);
}

/**
 * B Tree Index Operators
 * for chessgame - chessgame comparison
 * less than
 * less than or equal
 * equal
 * greater than or equal
 * greater than
 * comparison (support function)
 */

Datum chessgame_lt(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);

    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(lessThan(game1, game2));
}

Datum chessgame_lte(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(lessThan(game1, game2) || strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2)) == 0);
}

Datum chessgame_eq(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2)) == 0);
}

Datum chessgame_neq(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2)) != 0);
}

Datum chessgame_gt(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(lessThan(game2, game1));
}

Datum chessgame_gte(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(lessThan(game2, game1) || strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2)) == 0);
}

Datum chessgame_cmp(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    int result = strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2));
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_INT32(result);
}