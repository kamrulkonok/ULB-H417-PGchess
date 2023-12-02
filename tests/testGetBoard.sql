DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

DROP TABLE IF EXISTS chessgames;
CREATE TABLE chessgames (
    id serial PRIMARY KEY,
    game chessgame
);

-- Insert a sample chessgame
INSERT INTO chessgames (game)
VALUES ('1.e4 e5 2.Nf3 Nc6 3.Bb5 a6'::chessgame);

-- Test the getBoard function
SELECT getBoard(game, 0) as initial_board FROM chessgames;
SELECT getBoard(game, 1) as first_half_move FROM chessgames;
SELECT getBoard(game, 2) as second_half_move FROM chessgames;
SELECT getBoard(game, 3) as third_half_move FROM chessgames;
SELECT getBoard(game, 4) as fourth_half_move FROM chessgames;
SELECT getBoard(game, 5) as fifth_half_move FROM chessgames;
SELECT getBoard(game, 6) as sixth_half_move FROM chessgames;
SELECT getBoard(game, 7) as last_half_move FROM chessgames;
