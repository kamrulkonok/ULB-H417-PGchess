#include <stdio.h>
#include <postgres.h>
#include <fmgr.h>
#include <libpq/pqformat.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "utils/builtins.h"
#include "smallchesslib.h"

PG_MODULE_MAGIC;
/* Data type representations */
typedef struct {
    char *pgn; // Standard Algebraic Notation for chessgame
} ChessGame;

typedef struct {
    char *fen; // Forsyth-Edwards Notation for chessboard
} ChessBoard; // current state of the board


/* Function declarations */
PG_FUNCTION_INFO_V1(chessgame_in);
PG_FUNCTION_INFO_V1(chessgame_out);
PG_FUNCTION_INFO_V1(chessboard_in);
PG_FUNCTION_INFO_V1(chessboard_out);
PG_FUNCTION_INFO_V1(chessgame_recv);
PG_FUNCTION_INFO_V1(chessboard_send);
PG_FUNCTION_INFO_V1(chessgame_send);
PG_FUNCTION_INFO_V1(chessboard_recv);
PG_FUNCTION_INFO_V1(getBoard);
PG_FUNCTION_INFO_V1(getFirstMoves);
PG_FUNCTION_INFO_V1(hasOpening);
PG_FUNCTION_INFO_V1(hasBoard);


/* Input function for chessgame */
Datum
chessgame_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    ChessGame *game = (ChessGame *) palloc(sizeof(ChessGame));

    // Duplicate the string using PostgreSQL's memory functions
    // Ensure to check if str is NULL to handle STRICT compliance
    game->pgn = (str != NULL) ? pstrdup(str) : NULL;

    PG_RETURN_POINTER(game);
}

/* Output function for chessgame */
Datum
chessgame_out(PG_FUNCTION_ARGS) {
    ChessGame *game = (ChessGame *) PG_GETARG_POINTER(0);
    // Return the PGN string
    PG_RETURN_CSTRING(pstrdup(game->pgn));
}
/* Send function for chessgame */

Datum
chessgame_send(PG_FUNCTION_ARGS) {
    ChessGame *game = (ChessGame *) PG_GETARG_POINTER(0);
    StringInfoData buf;
    pq_begintypsend(&buf);
    pq_sendstring(&buf, game->pgn);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}
// /* Receive function for chessgame */
Datum
chessgame_recv(PG_FUNCTION_ARGS) {
    StringInfo  buf = (StringInfo) PG_GETARG_POINTER(0);
    ChessGame *game = (ChessGame *) palloc(sizeof(ChessGame));
    game->pgn = pstrdup(pq_getmsgstring(buf));
    PG_RETURN_POINTER(game);
}


/* Input/output functions for chessboard */
/* Input function for chessboard */
Datum
chessboard_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    ChessBoard *board = (ChessBoard *) palloc(sizeof(ChessBoard));

    // Duplicate the string using PostgreSQL's memory functions
    // and check if str is NULL to handle STRICT compliance
    board->fen = (str != NULL) ? pstrdup(str) : NULL;

    PG_RETURN_POINTER(board);
}

/* Output function for chessboard */
Datum
chessboard_out(PG_FUNCTION_ARGS) {
    ChessBoard *board = (ChessBoard *) PG_GETARG_POINTER(0);
    // Return the FEN string
    PG_RETURN_CSTRING(pstrdup(board->fen));
}

// /* Send function for chessboard */
Datum
chessboard_send(PG_FUNCTION_ARGS) {
    ChessBoard *board = (ChessBoard *) PG_GETARG_POINTER(0);
    StringInfoData buf;
    pq_begintypsend(&buf);
    pq_sendstring(&buf, board->fen);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

// /* Implementations of the specified functions */
Datum getBoard(PG_FUNCTION_ARGS) {
    ChessGame *game = (ChessGame *) PG_GETARG_POINTER(0);
    int halfMoveNumber = PG_GETARG_INT32(1);
    
    // Logic to parse the PGN and get the FEN at the specified half-move
    ChessBoard *board = calculateBoardFromPGN(game->pgn, halfMoveNumber);
    
    // Return the result
    PG_RETURN_CHESSBOARD_P(board);
}
Datum getFirstMoves(PG_FUNCTION_ARGS) {
    ChessGame *game = (ChessGame *) PG_GETARG_POINTER(0);
    int halfMoves = PG_GETARG_INT32(1);
    
    // Logic to truncate the PGN
    ChessGame *truncatedGame = truncatePGN(game->pgn, halfMoves);
    
    // Return the result
    PG_RETURN_CHESSGAME_P(truncatedGame);
}


Datum hasOpening(PG_FUNCTION_ARGS) {
    ChessGame *game1 = (ChessGame *) PG_GETARG_POINTER(0);
    ChessGame *game2 = (ChessGame *) PG_GETARG_POINTER(1);
    
    // Logic to compare the openings
    bool result = compareOpenings(game1->pgn, game2->pgn);
    
    // Return the result
    PG_RETURN_BOOL(result);
}

Datum hasBoard(PG_FUNCTION_ARGS) {
    ChessGame *game = (ChessGame *) PG_GETARG_POINTER(0);
    ChessBoard *board = (ChessBoard *) PG_GETARG_POINTER(1);
    int halfMoves = PG_GETARG_INT32(2);
    
    // Logic to check if the board state is within the first N half-moves
    bool result = checkBoardState(game->pgn, board->fen, halfMoves);
    
    // Return the result
    PG_RETURN_BOOL(result);
}

/* Additional helper functions specific to chessgame and chessboard types */

/* 
   Note: The actual parsing of SAN and FEN, the logic to manipulate chess game states,
   and any validation checks should be implemented within the appropriate functions.
*/

