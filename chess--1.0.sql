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
  internallength = 69,
  input          = chessboard_in,
  output         = chessboard_out,
  receive        = chessboard_recv,
  send           = chessboard_send,
  alignment      = char
);


/******************************************************************************
 * Operators
 ******************************************************************************/

CREATE OR REPLACE FUNCTION chessboard_eq(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR = (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_eq,
  COMMUTATOR = =, NEGATOR = <>
);

CREATE OR REPLACE FUNCTION chessboard_neq(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <> (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_neq,
  COMMUTATOR = <>, NEGATOR = =
);

CREATE OR REPLACE FUNCTION chessboard_lt(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR < (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_lt,
  COMMUTATOR = >, NEGATOR = >=
);

CREATE OR REPLACE FUNCTION chessboard_lte(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <= (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_lte,
  COMMUTATOR = >=, NEGATOR = <
);

CREATE OR REPLACE FUNCTION chessboard_gt(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR > (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_gt,
  COMMUTATOR = <, NEGATOR = <=
);

CREATE OR REPLACE FUNCTION chessboard_gte(chessboard, chessboard)
  RETURNS boolean
  AS 'MODULE_PATHNAME'
  LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR >= (
  LEFTARG = chessboard, RIGHTARG = chessboard,
  PROCEDURE = chessboard_gte,
  COMMUTATOR = <=, NEGATOR = >
);

CREATE FUNCTION chessboard_to_text(chessboard) RETURNS text
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION text_to_chessboard(text) RETURNS chessboard
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (chessboard AS text)
    WITH FUNCTION chessboard_to_text(chessboard)
    AS IMPLICIT;

CREATE CAST (text AS chessboard)
    WITH FUNCTION text_to_chessboard(text)
    AS IMPLICIT;


/************************************************************************************************************
                                          Chessgame to represent SAN and FEN states
************************************************************************************************************/

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE FUNCTION chessgame_in(cstring) RETURNS chessgame
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_out(chessgame) RETURNS cstring
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_recv(internal) RETURNS chessgame
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION chessgame_send(chessgame) RETURNS bytea
    AS 'MODULE_PATHNAME'
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
    AS 'MODULE_PATHNAME'
    LANGUAGE C STRICT;

CREATE CAST (text AS chessgame)
    WITH FUNCTION text_to_chessgame(text)
    AS IMPLICIT;

/******************************************************************************
 * Functions
 ******************************************************************************/

CREATE FUNCTION getBoard(chessgame, integer)
RETURNS text AS 'MODULE_PATHNAME', 'getBoard'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION getFirstMoves(chessgame, integer)
RETURNS text AS 'MODULE_PATHNAME', 'getFirstMoves'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * B Tree Index for hasOpening
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

CREATE OR REPLACE FUNCTION hasOpening(game1 chessgame, game2 chessgame)
RETURNS boolean AS $$
  SELECT game1::text LIKE game2::text || '%';
$$ LANGUAGE sql IMMUTABLE PARALLEL SAFE;

-- Function to display the Array of chessgame states using `chessgame_get_all_states` as text[]
CREATE FUNCTION getAllStates(chessgame)
RETURNS text[] AS 'MODULE_PATHNAME', 'chessgame_get_all_states'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION CREATE_QUERIES(query TEXT, n INT)
RETURNS TEXT[] AS $$
SELECT array_agg(i || ':' || query)
FROM generate_series(0, n) as g(i)
$$ LANGUAGE sql IMMUTABLE;

CREATE OR REPLACE FUNCTION hasBoard(game chessgame, fen text, n INT)
RETURNS BOOLEAN AS $$
SELECT getAllStates(game) && CREATE_QUERIES(fen, n);
$$ LANGUAGE sql IMMUTABLE;