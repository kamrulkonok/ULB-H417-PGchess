-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION chessgame" to load this file. \quit

/******************************************************************************
 * Input/Output Functions for chessgame and chessboard types
 ******************************************************************************/

CREATE OR REPLACE FUNCTION chessgame_in(cstring)
  RETURNS chessgame
  AS '$libdir/chessgame', 'chessgame_in'
  LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION chessgame_out(chessgame)
  RETURNS cstring
  AS '$libdir/chessgame', 'chessgame_out'
  LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION chessboard_in(cstring)
  RETURNS chessboard
  AS '$libdir/chessgame', 'chessboard_in'
  LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION chessboard_out(chessboard)
  RETURNS cstring
  AS '$libdir/chessgame', 'chessboard_out'
  LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION chessgame_recv(internal)
  RETURNS chessgame
  AS '$libdir/chessgame', 'chessgame_recv'
  LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION chessgame_send(chessgame)
  RETURNS bytea
  AS '$libdir/chessgame', 'chessgame_send'
  LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION chessboard_recv(internal)
  RETURNS chessboard
  AS '$libdir/chessgame', 'chessboard_recv'
  LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION chessboard_send(chessboard)
  RETURNS bytea
  AS '$libdir/chessgame', 'chessboard_send'
  LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE chessgame AS (
  internallength = variable,
  input          = chessgame_in,
  output         = chessgame_out,
  receive        = chessgame_recv,
  send           = chessgame_send,
  alignment      = double,
  like           = bigint,
  category = 'N'
);

CREATE TYPE chessboard AS (
  internallength = variable,
  input          = chessboard_in,
  output         = chessboard_out,
  receive        = chessboard_recv,
  send           = chessboard_send,
  alignment      = double,
  like           = bigint,
  category = 'N'
);

/******************************************************************************
 * Functions to manipulate chess games
 ******************************************************************************/

CREATE FUNCTION getBoard(chessgame ChessGame, halfMoveNumber integer)
  RETURNS chessboard
  AS '$libdir/chessgame', 'getBoard'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION getFirstMoves(chessgame ChessGame, halfMoves integer)
  RETURNS chessgame
  AS '$libdir/chessgame', 'getFirstMoves'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION hasOpening(chessgame1 ChessGame, chessgame2 ChessGame)
  RETURNS boolean
  AS '$libdir/chessgame', 'hasOpening'
  LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION hasBoard(chessgame ChessGame, chessboard ChessBoard, halfMoves integer)
  RETURNS boolean
  AS '$libdir/chessgame', 'hasBoard'
  LANGUAGE C IMMUTABLE STRICT;

-- /******************************************************************************
--  * Additional functions and operators (if needed)
--  ******************************************************************************/

-- -- Define additional functions and operators for chessgame and chessboard types

-- /******************************************************************************
--  * End of chessgame Extension
--  ******************************************************************************/
