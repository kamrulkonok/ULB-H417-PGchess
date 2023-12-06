#include <stdio.h>
#include <postgres.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "lib/stringinfo.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#include "utils/lsyscache.h"

#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "smallchesslib.h"

PG_MODULE_MAGIC;
/************************************************************************************************************
                                          Chessboard Structure
************************************************************************************************************/

typedef SCL_Board chessboard;

// register the function as a PostgreSQL function that follows
// version 1 calling conventions
PG_FUNCTION_INFO_V1(chessboard_in);
PG_FUNCTION_INFO_V1(chessboard_out);
PG_FUNCTION_INFO_V1(chessboard_recv);
PG_FUNCTION_INFO_V1(chessboard_send);
PG_FUNCTION_INFO_V1(chessboard_constructor);
PG_FUNCTION_INFO_V1(chessboard_eq);
PG_FUNCTION_INFO_V1(chessboard_neq);
PG_FUNCTION_INFO_V1(chessboard_lt);
PG_FUNCTION_INFO_V1(chessboard_lte);
PG_FUNCTION_INFO_V1(chessboard_gt);
PG_FUNCTION_INFO_V1(chessboard_gte);
PG_FUNCTION_INFO_V1(chessboard_to_text);

chessboard *create_chessboard_from_fen(const char *fenStr)
{
    chessboard *board = (chessboard *)palloc(sizeof(chessboard));
    SCL_boardFromFEN(*board, fenStr);
    return board;
}

Datum chessboard_in(PG_FUNCTION_ARGS)
{
    char *fenStr = PG_GETARG_CSTRING(0);
    chessboard *board = create_chessboard_from_fen(fenStr);
    PG_RETURN_POINTER(board);
}

Datum chessboard_out(PG_FUNCTION_ARGS)
{
    chessboard *board = (chessboard *)PG_GETARG_POINTER(0);
    char *fenStr = palloc(SCL_FEN_MAX_LENGTH);
    SCL_boardToFEN(*board, fenStr);
    PG_FREE_IF_COPY(board, 0);
    PG_RETURN_CSTRING(fenStr);
}

Datum chessboard_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    chessboard *board = (chessboard *)palloc(sizeof(chessboard));

    // Assuming the binary format is a direct copy of the SCL_Board
    pq_copymsgbytes(buf, (char *)board, sizeof(chessboard));

    PG_RETURN_POINTER(board);
}

Datum chessboard_send(PG_FUNCTION_ARGS)
{
    chessboard *board = (chessboard *)PG_GETARG_POINTER(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendbytes(&buf, (char *)board, sizeof(chessboard));

    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum chessboard_eq(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(strcmp(*board1, *board2) == 0);
}

Datum chessboard_neq(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(strcmp(*board1, *board2) != 0);
}

Datum chessboard_lt(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(strcmp(*board1, *board2) < 0);
}

Datum chessboard_lte(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(strcmp(*board1, *board2) <= 0);
}

Datum chessboard_gt(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(strcmp(*board1, *board2) > 0);
}

Datum chessboard_gte(PG_FUNCTION_ARGS)
{
    chessboard *board1 = (chessboard *)PG_GETARG_POINTER(0);
    chessboard *board2 = (chessboard *)PG_GETARG_POINTER(1);
    PG_RETURN_BOOL(strcmp(*board1, *board2) >= 0);
}

Datum chessboard_to_text(PG_FUNCTION_ARGS)
{
    chessboard *board = (chessboard *)PG_GETARG_POINTER(0);
    char *fenStr = palloc(SCL_FEN_MAX_LENGTH);
    SCL_boardToFEN(*board, fenStr);
    text *result = cstring_to_text(fenStr);
    PG_FREE_IF_COPY(board, 0);
    PG_RETURN_TEXT_P(result);
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
    // header field for postgresql
    int32 length;
    // SAN moves encoded as uint8 pairs
    uint8 record[FLEXIBLE_ARRAY_MEMBER];
} chessgame;

uint8 getChessgameHalfMoves(chessgame *game)
{
    return SCL_recordLength(game->record);
}

char *getChessgameSanMoves(chessgame *game)
{
    return SCL_printPGN(game->record);
}

char *getChessgameBoard(chessgame *game, int halfMoveIndex)
{
    // Handle invalid index
    if (halfMoveIndex < 0)
        return NULL;
    // maximum length of fen can be 136
    char *fen = palloc(137 * sizeof(char));
    SCL_Board board = SCL_BOARD_START_STATE;
    // applies the SAN to the board for `halfMoveIndex` moves
    SCL_recordApply(game->record, board, halfMoveIndex);
    // stores the FEN in `fen`
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
PG_FUNCTION_INFO_V1(chessgame_get_all_states);

chessgame *create_chessgame(const char *SAN)
{
    // Create an empty SCL Record
    SCL_Record r;
    // Populate it with all of the SAN moves
    SCL_recordFromPGN(r, SAN);
    // Calculate the number of half moves
    uint16_t numHalfMoves = SCL_recordLength(r);
    // Each half move is encoded as a pair of numbers
    uint16_t recordNumElements = numHalfMoves * 2;
    // Calculate the total size of the elements in the record
    uint16_t recordSize = sizeof(uint8) * recordNumElements;
    // Calculate the total size of the chessgame structure
    uint16_t totalSize = VARHDRSZ + recordSize;
    chessgame *game = (chessgame *)palloc(totalSize);
    // Set the size of the chessgame structure
    SET_VARSIZE(game, totalSize);
    // Populate the record inside the chessgame by copying data from SCL Record
    memcpy(game->record, r, recordSize);
    return game;
}

Datum chessgame_in(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("input string must not be null")));

    char *sanMovesStr = PG_GETARG_CSTRING(0);
    chessgame *game = create_chessgame(sanMovesStr);
    PG_FREE_IF_COPY(sanMovesStr, 0);
    PG_RETURN_POINTER(game);
}

Datum chessgame_out(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame must not be null")));

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    char *SAN = getChessgameSanMoves(game);
    PG_FREE_IF_COPY(game, 0);
    PG_RETURN_CSTRING(SAN);
}

Datum chessgame_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);

    // Read the length of the record
    int32 recordLength;
    recordLength = pq_getmsgint(buf, sizeof(int32));

    // Validate record length
    if (recordLength < VARHDRSZ)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
                 errmsg("invalid length in external \"chessgame\" representation")));

    // Allocate memory for chessgame
    chessgame *game = (chessgame *)palloc(recordLength);
    SET_VARSIZE(game, recordLength);

    // Copy the SAN moves
    pq_copymsgbytes(buf, (char *)game->record, recordLength - VARHDRSZ);

    PG_RETURN_POINTER(game);
}

Datum chessgame_send(PG_FUNCTION_ARGS)
{
    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    StringInfoData buf;

    // Begin populating the buffer
    pq_begintypsend(&buf);

    // Send the length of the record
    pq_sendint32(&buf, VARSIZE(game));

    // Send the SAN moves
    pq_sendbytes(&buf, (char *)game->record, VARSIZE(game) - VARHDRSZ);

    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum text_to_chessgame(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("input string must not be null")));

    text *inputText = PG_GETARG_TEXT_PP(0);
    char *sanMovesStr = text_to_cstring(inputText);
    chessgame *game = create_chessgame(sanMovesStr);

    PG_FREE_IF_COPY(inputText, 0);
    PG_RETURN_POINTER(game);
}

/************************************************************************************************************
                                          Chessgame functions
************************************************************************************************************/

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
    if (halfMoveIndex > getChessgameHalfMoves(game))
        halfMoveIndex = getChessgameHalfMoves(game);
    // return the FEN of chessboard at index i
    char *fen = getChessgameBoard(game, halfMoveIndex);

    if (fen == NULL)
        PG_RETURN_NULL();

    text *result = cstring_to_text(fen);
    PG_FREE_IF_COPY(game, 0);
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
    // Null-terminate the string
    output[i] = '\0';
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

    PG_FREE_IF_COPY(game, 0);
    PG_RETURN_TEXT_P(cstring_to_text(trimmedSAN));
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

// Helper function to check if 'pre' is a prefix of 'str'
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

bool lessThan(chessgame *game1, chessgame *game2)
{
    /*
     * game1 is 'less than' game2 if game1 is a prefix of game2
     */
    const char *SAN1 = getChessgameSanMoves(game1);
    const char *SAN2 = getChessgameSanMoves(game2);
    return prefix(SAN1, SAN2);
}

Datum chessgame_lt(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    bool result = lessThan(game1, game2);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(result);
}

Datum chessgame_lte(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    char *SAN1 = getChessgameSanMoves(game1);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    char *SAN2 = getChessgameSanMoves(game2);
    bool result = lessThan(game1, game2) || strcmp(SAN1, SAN2) == 0;
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(result);
}

Datum chessgame_eq(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    bool result = strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2)) == 0;
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(result);
}

Datum chessgame_neq(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    bool result = strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2)) != 0;
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(result);
}

Datum chessgame_gt(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    bool result = lessThan(game2, game1);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(result);
}

Datum chessgame_gte(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    char *SAN1 = getChessgameSanMoves(game1);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    char *SAN2 = getChessgameSanMoves(game2);
    bool result = lessThan(game2, game1) || strcmp(SAN1, SAN2) == 0;
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(result);
}

Datum chessgame_cmp(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    int result = strcmp(getChessgameSanMoves(game1), getChessgameSanMoves(game2));
    if (result != 0)
    {
        // check less than
        if (lessThan(game1, game2))
            result = -1;
        // check greater than
        else if (lessThan(game2, game1))
            result = 1;
    }
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_INT32(result);
}

// return an array containing all the FEN states of the chessgame
Datum chessgame_get_all_states(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame must not be null")));

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    uint8 numHalfMoves = getChessgameHalfMoves(game);

    // Create an array to store text representations of FEN states
    Datum *fenStates = palloc(sizeof(Datum) * (numHalfMoves + 1));

    // Populate the array with FEN states
    for (int i = 0; i <= numHalfMoves; ++i)
    {
        char *fen = getChessgameBoard(game, i);
        if (fen != NULL)
        {
            // prepend move number to FEN state like this "i:FEN"
            // can be max 4 bytes (3 digit number and a colon)
            char *fenWithMoveNumber = palloc(strlen(fen) + 4);
            sprintf(fenWithMoveNumber, "%d:%s", i, fen);
            pfree(fen);
            // Convert C string to PostgreSQL text type
            text *fenText = cstring_to_text(fenWithMoveNumber);
            fenStates[i] = PointerGetDatum(fenText);
        }
        else
        {
            // Handle NULL FEN state
            fenStates[i] = PointerGetDatum(cstring_to_text(""));
        }
    }

    // Construct the array of text
    int16 elemTyplen;
    bool elemTypbyval;
    char elemTypalign;
    get_typlenbyvalalign(TEXTOID, &elemTyplen, &elemTypbyval, &elemTypalign);
    ArrayType *resultArray = construct_array(fenStates, numHalfMoves, TEXTOID, elemTyplen, elemTypbyval, elemTypalign);

    // Clean up
    for (int i = 0; i < numHalfMoves; ++i)
    {
        pfree(DatumGetPointer(fenStates[i]));
    }
    pfree(fenStates);
    PG_FREE_IF_COPY(game, 0);
    PG_RETURN_ARRAYTYPE_P(resultArray);
}