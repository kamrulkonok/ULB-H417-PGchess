-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION chess" to load this file. \quit

/************************************************************************************************************
                                          Chessboard to represent FEN
************************************************************************************************************/

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE OR REPLACE FUNCTION chessboard_in(cstring)
    RETURNS chessboard
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessboard_out(chessboard)
    RETURNS cstring
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessboard_recv(internal)
   RETURNS chessboard
   AS 'MODULE_PATHNAME'
   LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION chessboard_send(chessboard)
   RETURNS bytea
   AS 'MODULE_PATHNAME'
   LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Create the datatype
 ******************************************************************************/

CREATE TYPE chessboard (
  internallength = -1,
  input          = chessboard_in,
  output         = chessboard_out,
  receive        = chessboard_recv,
  send           = chessboard_send
);

/******************************************************************************
 * Constructor
 ******************************************************************************/

CREATE OR REPLACE FUNCTION chessboard(fen text)
  RETURNS chessboard
  AS 'MODULE_PATHNAME', 'chessboard_constructor'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


/******************************************************************************
 * Operators
 ******************************************************************************/

CREATE FUNCTION chessboard_eq(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessboard_eq'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessboard_neq(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessboard_neq'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessboard_lt(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessboard_lt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessboard_le(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessboard_le'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessboard_gt(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessboard_gt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessboard_ge(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessboard_ge'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR = (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_eq,
  COMMUTATOR = =, NEGATOR = <>
);

CREATE OPERATOR <> (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_neq,
  COMMUTATOR = <>, NEGATOR = =
);

CREATE OPERATOR < (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_lt
);

CREATE OPERATOR <= (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_le
);

CREATE OPERATOR > (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_gt
);

CREATE OPERATOR >= (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_ge
);

/************************************************************************************************************
                                          Chessgame to represent SAN and FEN states
************************************************************************************************************/

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE FUNCTION chessgame_in(cstring) RETURNS chessgame
    AS 'MODULE_PATHNAME', 'chessgame_in'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_out(chessgame) RETURNS cstring
    AS 'MODULE_PATHNAME', 'chessgame_out'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_recv(internal) RETURNS chessgame
    AS 'MODULE_PATHNAME', 'chessgame_recv'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_send(chessgame) RETURNS bytea
    AS 'MODULE_PATHNAME', 'chessgame_send'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Create the datatype
 ******************************************************************************/

CREATE TYPE chessgame (
  internallength = -1,
  input          = chessgame_in,
  output         = chessgame_out,
  receive        = chessgame_recv,
  send           = chessgame_send
);


CREATE FUNCTION text_to_chessgame(text) RETURNS chessgame
    AS 'MODULE_PATHNAME', 'text_to_chessgame'
    LANGUAGE C STRICT;

CREATE CAST (text AS chessgame)
    WITH FUNCTION text_to_chessgame(text)
    AS IMPLICIT;

/******************************************************************************
 * Functions
 ******************************************************************************/

CREATE FUNCTION getBoard(chessgame, integer)
RETURNS text AS 'MODULE_PATHNAME', 'getBoard'
LANGUAGE C STABLE STRICT PARALLEL SAFE;

CREATE FUNCTION getFirstMoves(chessgame, integer)
RETURNS text AS 'MODULE_PATHNAME', 'getFirstMoves'
LANGUAGE C STABLE STRICT PARALLEL SAFE;

CREATE FUNCTION hasOpening(chessgame, chessgame)
RETURNS boolean AS 'MODULE_PATHNAME', 'hasOpening'
LANGUAGE C STABLE STRICT PARALLEL SAFE;

CREATE FUNCTION hasBoard(chessgame, chessboard, integer)
RETURNS boolean AS 'MODULE_PATHNAME', 'hasBoard'
LANGUAGE C STABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * B Tree Index for hasBoard
 ******************************************************************************/

CREATE FUNCTION chessgame_lt(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_lt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_lte(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_lte'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_eq(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_eq'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_neq(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_neq'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_gte(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_gte'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_gt(chessgame, chessgame)
  RETURNS boolean
  AS 'MODULE_PATHNAME', 'chessgame_gt'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_cmp(chessgame, chessgame)
  RETURNS int
  AS 'MODULE_PATHNAME', 'chessgame_cmp'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR = (
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    PROCEDURE = chessgame_eq,
    COMMUTATOR = =,
    NEGATOR = <>
);

CREATE OPERATOR <> (
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    PROCEDURE = chessgame_neq,
    COMMUTATOR = <>,
    NEGATOR = =
);

CREATE OPERATOR < (
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    PROCEDURE = chessgame_lt,
    COMMUTATOR = >,
    NEGATOR = >=
);

CREATE OPERATOR > (
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    PROCEDURE = chessgame_gt,
    COMMUTATOR = <,
    NEGATOR = <=
);

CREATE OPERATOR <= (
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    PROCEDURE = chessgame_lte,
    COMMUTATOR = >=,
    NEGATOR = >
);

CREATE OPERATOR >= (
    LEFTARG = chessgame,
    RIGHTARG = chessgame,
    PROCEDURE = chessgame_gte,
    COMMUTATOR = <=,
    NEGATOR = <
);

CREATE OPERATOR CLASS chessgame_ops
    DEFAULT FOR TYPE chessgame USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       chessgame_cmp(chessgame, chessgame);