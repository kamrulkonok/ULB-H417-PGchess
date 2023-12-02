DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

DROP TABLE IF EXISTS chessgames;
CREATE TABLE chessgames (
    id serial PRIMARY KEY,
    game chessgame
);

-- Assuming you have a chessgame table with a chessgame column
INSERT INTO chessgames (game)
VALUES ('1.e4 e5 2.Nf3 Nc6 3.Bb5 a6');
-- Test the getFirstMoves function
SELECT getFirstMoves(game, 0) as first_0_moves FROM chessgames;
SELECT getFirstMoves(game, 1) as first_1_moves FROM chessgames;
SELECT getFirstMoves(game, 2) as first_2_moves FROM chessgames;
SELECT getFirstMoves(game, 3) as first_3_moves FROM chessgames;
SELECT getFirstMoves(game, 4) as first_4_moves FROM chessgames;
SELECT getFirstMoves(game, 5) as first_5_moves FROM chessgames;
SELECT getFirstMoves(game, 6) as first_6_moves FROM chessgames;
SELECT getFirstMoves(game, 7) as first_7_moves FROM chessgames;
