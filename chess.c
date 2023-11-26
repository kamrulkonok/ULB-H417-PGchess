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

// Helper function to trim the result from a SAN moves string
char *trim_san_moves(const char *sanMovesStr)
{
    // Copy the original string to avoid modifying it directly
    char *trimmedStr = pstrdup(sanMovesStr);
    // Define tokens that indicate the end of the moves and start of the result
    const char *endTokens[] = {" 1-0", " 0-1", " 1/2-1/2", NULL};
    for (int i = 0; endTokens[i] != NULL; i++)
    {
        // Finds the first occurrence of s2 in s1
        char *found = strstr(trimmedStr, endTokens[i]);
        if (found != NULL)
        {
            // Truncate the string at the start of the result
            *found = '\0';
            break;
        }
    }
    return trimmedStr;
}

// Helper function to calculate the length of a SAN string without move numbers
int calculateShortSANLength(int halfMoves, int originalLength)
{
    int fullMoves = (halfMoves % 2 == 0) ? halfMoves / 2 : (halfMoves + 1) / 2;
    int singleDigitFullMoves = (fullMoves > 9) ? 9 : fullMoves;
    int doubleDigitFullMoves = (fullMoves > 9) ? fullMoves - 9 : 0;
    int tripleDigitFullMoves = (fullMoves > 99) ? fullMoves - 99 : 0;
    // Calculate the total characters occupied by move numbers
    int charsInMoveNumbers = (singleDigitFullMoves * 2) + (doubleDigitFullMoves * 3) + (tripleDigitFullMoves * 4);
    return originalLength - charsInMoveNumbers;
}

// Function to remove move numbers from a SAN string
char *removeMoveNumbers(const char *originalSAN, int numHalfMoves, int size)
{
    char *processedSAN = palloc(size + 1);
    int i = 0, j = 0;
    while (originalSAN[i] != '\0')
    {
        // Check if the current character is a digit and if it's followed by a period
        if (isdigit(originalSAN[i]))
        {
            int start = i;
            while (isdigit(originalSAN[i]))
            { // Skip all consecutive digits
                i++;
            }
            if (originalSAN[i] == '.')
            {             // Check if the digits are followed by a period
                i++;      // Skip the period
                continue; // Move to the next character in the original string
            }
            else
            {
                i = start; // Reset i to start to copy the numbers as they are not part of a move number
            }
        }
        // Copy the character to processedSAN
        processedSAN[j++] = originalSAN[i++];
    }
    // Null-terminate the processed string
    processedSAN[j] = '\0';
    return processedSAN;
}

// Helper function to calculate the length of a SAN string with move numbers
int calculateOriginalSANLength(int halfMoves, int lengthWithoutMoveNumbers)
{
    int fullMoves = (halfMoves % 2 == 0) ? halfMoves / 2 : (halfMoves + 1) / 2;
    int singleDigitFullMoves = (fullMoves > 9) ? 9 : fullMoves;
    int doubleDigitFullMoves = (fullMoves > 9) ? fullMoves - 9 : 0;
    int tripleDigitFullMoves = (fullMoves > 99) ? fullMoves - 99 : 0;
    // Calculate the total characters to be added by move numbers
    int charsAddedByMoveNumbers = (singleDigitFullMoves * 2) + (doubleDigitFullMoves * 3) + (tripleDigitFullMoves * 4);
    return lengthWithoutMoveNumbers + charsAddedByMoveNumbers;
}

// Function to add move numbers to a shortened SAN string
char *addMoveNumbers(const char *processedSAN, int size)
{
    char *originalSAN = palloc(size + 1);
    int i = 0, j = 0, moveNumber = 1;
    int isWhiteMove = 1; // Start with White's move

    while (processedSAN[i] != '\0')
    {
        if (isWhiteMove)
        {
            // Add move number for White's move
            j += sprintf(&originalSAN[j], "%d.", moveNumber++);
        }
        // Copy the move
        while (processedSAN[i] != ' ' && processedSAN[i] != '\0')
        {
            originalSAN[j++] = processedSAN[i++];
        }
        // Add space after the move if not at the end of string
        if (processedSAN[i] == ' ')
        {
            originalSAN[j++] = processedSAN[i++];
        }
        // Toggle between White's and Black's move
        isWhiteMove = !isWhiteMove;
    }
    // Null-terminate the original string
    originalSAN[j] = '\0';
    return originalSAN;
}

typedef struct
{
    char vl_len_[4];
    int32 numHalfMoves;
    // following this is the SAN moves string
    // we don't store it inside the struct because it is variable length
} chessgame;

char *getChessgameSanMoves(chessgame *game)
{
    // Skip over the vl_len_ field and numHalfMoves field to reach the SAN moves string
    char *SAN = (char *)game + offsetof(chessgame, numHalfMoves) + sizeof(game->numHalfMoves);
    return SAN;
}

char *getFullSAN(chessgame *game)
{
    char *SAN = getChessgameSanMoves(game);
    // Add the move numbers back to the SAN
    int originalLength = calculateOriginalSANLength(game->numHalfMoves, strlen(SAN));
    char *fullSAN = addMoveNumbers(SAN, originalLength);
    return fullSAN;
}

char *getChessgameBoard(chessgame *game, int halfMoveIndex)
{
    // Handle invalid index
    if (halfMoveIndex < 0)
        return NULL;
    // maximum length of fen can be 136
    char *fen = palloc(137 * sizeof(char));
    char *sanMoves = getFullSAN(game);
    // get the FEN using SCL library
    SCL_Record r;
    // puts the SAN moves in the record
    SCL_recordFromPGN(r, sanMoves);
    SCL_Board board = SCL_BOARD_START_STATE;
    // applies the SAN to the board for `halfMoveIndex` moves
    SCL_recordApply(r, board, halfMoveIndex);
    // stores the FEN in `fen`
    SCL_boardToFEN(board, fen);
    // free memory
    pfree(sanMoves);
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

chessgame *create_chessgame(const char *SAN)
{
    char *dataPtr;
    // Allocate memory for the chessgame
    chessgame *game = palloc(sizeof(chessgame));
    // remove the result of the game from the SAN moves
    char *sanMovesStr = trim_san_moves(SAN);
    // calculate the number of half moves
    SCL_Record r;
    SCL_recordFromPGN(r, sanMovesStr);
    int numHalfMoves = SCL_recordLength(r);
    // we store the SAN without the numbers to save space
    int sanMovesLen = calculateShortSANLength(numHalfMoves, strlen(sanMovesStr)) + 1;
    // create a shorter SAN without the move numbers
    char *shortSAN = removeMoveNumbers(sanMovesStr, numHalfMoves, sanMovesLen);
    // we don't need the original SAN anymore
    pfree(sanMovesStr);
    // Calculate total size required for chessgame and all FEN strings
    int totalSize = sizeof(chessgame) + sanMovesLen;
    // Allocate memory
    game = (chessgame *)palloc0(totalSize + VARHDRSZ);
    // Set the total size including the header size
    SET_VARSIZE(game, totalSize + VARHDRSZ);
    // Set the numHalfMoves property
    game->numHalfMoves = numHalfMoves;
    // Copy the SAN moves string
    dataPtr = (char *)(game + 1);
    memcpy(dataPtr, shortSAN, sanMovesLen);
    dataPtr += sanMovesLen;
    // Free the short SAN string
    pfree(shortSAN);
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
    if (PG_ARGISNULL(0))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame must not be null")));

    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    char *SAN = getFullSAN(game);
    PG_FREE_IF_COPY(game, 0);
    PG_RETURN_CSTRING(SAN);
}

Datum chessgame_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    const char *sanMovesStr = pq_getmsgstring(buf);
    int numHalfMoves = pq_getmsgint(buf, sizeof(int32));

    // Calculate total size for the chessgame structure and SAN moves string
    int sanMovesLen = strlen(sanMovesStr) + 1;
    int totalSize = sizeof(chessgame) + sanMovesLen;

    // Allocate memory
    chessgame *game = (chessgame *)palloc0(totalSize + VARHDRSZ);
    SET_VARSIZE(game, totalSize + VARHDRSZ);
    game->numHalfMoves = numHalfMoves;

    // Copy SAN moves string
    char *dataPtr = getChessgameSanMoves(game);
    strcpy(dataPtr, sanMovesStr);
    dataPtr += sanMovesLen;

    PG_RETURN_POINTER(game);
}

Datum chessgame_send(PG_FUNCTION_ARGS)
{
    chessgame *game = (chessgame *)PG_GETARG_POINTER(0);
    StringInfoData buf;
    pq_begintypsend(&buf);

    pq_sendint(&buf, game->numHalfMoves, sizeof(int32));
    char *sanMovesStr = getChessgameSanMoves(game);
    pq_sendstring(&buf, sanMovesStr);

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
    if (halfMoveIndex > game->numHalfMoves)
        halfMoveIndex = game->numHalfMoves;
    // return the FEN of chessboard at index i
    char *fen = getChessgameBoard(game, halfMoveIndex);

    if (fen == NULL)
        PG_RETURN_NULL();

    text *result = cstring_to_text(fen);
    PG_RETURN_TEXT_P(result);
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
    int nHalfMoves = PG_GETARG_INT32(1);

    char *sanMoves = getChessgameSanMoves(game);
    // Duplicate the SAN moves string to avoid modifying it directly
    char *truncatedSanMoves = pstrdup(sanMoves);

    // Parse the SAN moves string to count half-moves and truncate accordingly
    int moveCount = 0;
    char *current = truncatedSanMoves;
    while (*current != '\0' && moveCount < nHalfMoves)
    {
        if (*current == ' ')
        {
            moveCount++;
        }
        current++;
    }

    // If current points to a space, we need to include this last half-move
    if (*current == ' ' && moveCount == nHalfMoves)
    {
        current++;
        while (*current != ' ' && *current != '\0')
        {
            current++;
        }
    }

    // Truncate the string at the current position
    *current = '\0';

    // Add move numbers to the string
    int originalLength = calculateOriginalSANLength(nHalfMoves, strlen(truncatedSanMoves));
    char *sanMovesWithNumbers = addMoveNumbers(truncatedSanMoves, originalLength);
    // Return the truncated SAN moves as a new chessgame
    text *result = cstring_to_text(sanMovesWithNumbers);
    // Free the memory allocated by the helper functions
    pfree(truncatedSanMoves);
    pfree(sanMovesWithNumbers);
    PG_RETURN_TEXT_P(result);
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

    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(lessThan(game1, game2));
}

Datum chessgame_lte(PG_FUNCTION_ARGS)
{
    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    char *SAN1 = getChessgameSanMoves(game1);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    char *SAN2 = getChessgameSanMoves(game2);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(lessThan(game1, game2) || strcmp(SAN1, SAN2) == 0);
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
    char *SAN1 = getChessgameSanMoves(game1);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);
    char *SAN2 = getChessgameSanMoves(game2);
    PG_FREE_IF_COPY(game1, 0);
    PG_FREE_IF_COPY(game2, 1);
    PG_RETURN_BOOL(lessThan(game2, game1) || strcmp(SAN1, SAN2) == 0);
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