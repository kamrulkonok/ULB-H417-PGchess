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

typedef struct
{
    char vl_len_[4];
    int32 numHalfMoves;
    // Following this are:
    // 1. SAN moves string (null-terminated)
    // 2. Array of chessboard states
} chessgame;

int32 getChessgameNumHalfMoves(chessgame *game)
{
    return game->numHalfMoves;
}

char *getChessgameSanMoves(chessgame *game)
{
    // We need to skip over the vl_len_ field and numHalfMoves field to reach the SAN moves string
    return (char *)game + offsetof(chessgame, numHalfMoves) + sizeof(game->numHalfMoves);
}

char *getChessgameBoard(chessgame *game, int halfMoveIndex)
{
    char *dataPtr;
    if (halfMoveIndex < 0)
    {
        return NULL; // Handle invalid index
    }
    // the chessgame board start right after the SAN moves string
    dataPtr = getChessgameSanMoves(game); // + strlen(getChessgameSanMoves(game)) + 1;
    // elog(INFO, "dataPtr: %s", dataPtr);
    for (int i = 0; i < halfMoveIndex; i++)
    {
        dataPtr += strlen(dataPtr) + 1;
    }
    return dataPtr;
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

// Helper function to calculate the length of a SAN string without move numbers
int calculateShortSANLength(int halfMoves, int originalLength)
{
    int fullMoves = (halfMoves % 2 == 0) ? halfMoves / 2 : (halfMoves + 1) / 2;
    int singleDigitFullMoves = (fullMoves > 9) ? 9 : fullMoves;
    int doubleDigitFullMoves = (fullMoves > 9) ? fullMoves - 9 : 0;
    int tripleDigitFullMoves = (fullMoves > 99) ? fullMoves - 99 : 0;

    // Calculate the total characters occupied by move numbers
    int charsInMoveNumbers = (singleDigitFullMoves * 2) + (doubleDigitFullMoves * 3) + (tripleDigitFullMoves * 4);

    // Calculate and return the length of SAN string without move numbers
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

    // Calculate and return the length of SAN string with move numbers
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

// Helper function to check if 'pre' is a prefix of 'str'
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

char *trim_san_moves(const char *sanMovesStr)
{
    // Copy the original string to avoid modifying it directly
    char *trimmedStr = pstrdup(sanMovesStr);

    // Define tokens that indicate the end of the moves and start of the result
    const char *endTokens[] = {" 1-0", " 0-1", " 1/2-1/2", NULL};

    for (int i = 0; endTokens[i] != NULL; i++)
    {
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

int calculateHalfMoves(const char *sanMovesStr)
{
    SCL_Record r;
    SCL_recordInit(r);
    SCL_recordFromPGN(r, sanMovesStr);
    SCL_Board board = SCL_BOARD_START_STATE;
    // the longest number of chess moves ever played is 269
    // so we set the limit to 540 half moves
    SCL_recordApply(r, board, 540);
    int halfMoves = board[65];
    return halfMoves;
}

char **returnBoardStates(const char *sanMoves, int numHalfMoves)
{
    char **fenStates = (char **)palloc(sizeof(char *) * (numHalfMoves + 1));
    SCL_Record r;

    char fen[4096];

    // Initialize the record and set it from the SAN moves
    SCL_recordInit(r);
    SCL_recordFromPGN(r, sanMoves);

    // Iterate over each half-move
    for (int halfMove = 0; halfMove <= numHalfMoves; halfMove++)
    {
        // Reset the board to the starting state for each iteration
        SCL_Board board = SCL_BOARD_START_STATE;
        // Apply moves up to the current half-move
        SCL_recordApply(r, board, halfMove);
        // Convert the board to its FEN representation
        SCL_boardToFEN(board, fen);
        // Store the FEN string in the array
        fenStates[halfMove] = pstrdup(fen);
    }
    return fenStates;
}

chessgame *create_chessgame(const char *sanMoves)
{
    chessgame *game;
    char *dataPtr;
    // remove the result of the game from the SAN moves
    char *sanMovesStr = trim_san_moves(sanMoves);
    int numHalfMoves = calculateHalfMoves(sanMovesStr);
    // calculate each board state with the original SAN moves
    char **boardStates = returnBoardStates(sanMovesStr, numHalfMoves);
    // we store the SAN without the numbers to save space
    int sanMovesLen = calculateShortSANLength(numHalfMoves, strlen(sanMovesStr)) + 1;
    // create a shorter SAN without the move numbers
    char *shortSAN = removeMoveNumbers(sanMovesStr, numHalfMoves, sanMovesLen);
    // we don't need the original SAN anymore
    pfree(sanMovesStr);

    // Calculate total size required for chessgame and all FEN strings
    int totalSize = sizeof(chessgame) + sanMovesLen;
    for (int i = 0; i <= numHalfMoves; ++i)
    {
        totalSize += strlen(boardStates[i]) + 1; // +1 for null terminator
    }

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

    // Copy FEN strings
    for (int i = 0; i <= numHalfMoves; ++i)
    {
        int fenLen = strlen(boardStates[i]) + 1;
        // elog(INFO, "FEN %d: %s", i, boardStates[i]);
        memcpy(dataPtr, boardStates[i], fenLen);
        dataPtr += fenLen;
        pfree(boardStates[i]);
    }
    pfree(boardStates);

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
    // Extract the SAN moves string (invalid SAN because it does not contain move numbers)
    shortSANStr = getChessgameSanMoves(game);
    int originalLength = calculateOriginalSANLength(game->numHalfMoves, strlen(shortSANStr));
    // Add move numbers to the SAN string
    sanMovesStr = addMoveNumbers(shortSANStr, originalLength);
    PG_FREE_IF_COPY(game, 0);
    PG_RETURN_CSTRING(sanMovesStr);
}

Datum chessgame_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
    const char *sanMovesStr = pq_getmsgstring(buf);
    int numHalfMoves = pq_getmsgint(buf, sizeof(int32));

    // Calculate total size for the chessgame structure and FEN strings
    int sanMovesLen = strlen(sanMovesStr) + 1; // +1 for null terminator of SAN moves
    int totalSize = sizeof(chessgame) + sanMovesLen;

    for (int i = 0; i < numHalfMoves; ++i)
    {
        const char *fenStr = pq_getmsgstring(buf);
        totalSize += strlen(fenStr) + 1; // +1 for null terminator of each FEN string
    }

    // Allocate memory
    chessgame *game = (chessgame *)palloc0(totalSize + VARHDRSZ);
    SET_VARSIZE(game, totalSize + VARHDRSZ);
    game->numHalfMoves = numHalfMoves;

    // Copy SAN moves string
    char *dataPtr = getChessgameSanMoves(game);
    strcpy(dataPtr, sanMovesStr);
    dataPtr += sanMovesLen;

    // Copy FEN strings
    for (int i = 0; i < numHalfMoves; ++i)
    {
        const char *fenStr = pq_getmsgstring(buf);
        strcpy(dataPtr, fenStr);
        dataPtr += strlen(fenStr) + 1;
    }

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

    char *fenPtr = sanMovesStr + strlen(sanMovesStr) + 1;
    for (int i = 0; i <= game->numHalfMoves; ++i)
    {
        pq_sendstring(&buf, fenPtr);
        fenPtr += strlen(fenPtr) + 1;
    }

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
    // elog(INFO, "halfMoveIndex: %d", halfMoveIndex);
    // elog(INFO, "numHalfMoves: %d", getChessgameNumHalfMoves(game));
    if (halfMoveIndex > getChessgameNumHalfMoves(game))
        halfMoveIndex = getChessgameNumHalfMoves(game);
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
    char *truncatedSanMoves = pstrdup(sanMoves); // Duplicate the SAN moves string

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

Datum hasOpening(PG_FUNCTION_ARGS)
{
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        ereport(ERROR,
                (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
                 errmsg("chessgame arguments must not be null")));

    chessgame *game1 = (chessgame *)PG_GETARG_POINTER(0);
    chessgame *game2 = (chessgame *)PG_GETARG_POINTER(1);

    char *sanMovesGame1 = getChessgameSanMoves(game1);
    char *sanMovesGame2 = getChessgameSanMoves(game2);

    // Use the prefix function to compare the SAN moves
    bool result = prefix(sanMovesGame2, sanMovesGame1);

    PG_RETURN_BOOL(result);
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

    char *boardFen = board->fen; // Directly access the FEN string from the chessboard object
    bool found = false;

    for (int i = 0; i <= nHalfMoves && !found; ++i)
    {
        char *currentFen = getChessgameBoard(game, i);
        if (currentFen != NULL && strcmp(boardFen, currentFen) == 0)
        {
            found = true;
            break;
        }
    }

    PG_RETURN_BOOL(found);
}
