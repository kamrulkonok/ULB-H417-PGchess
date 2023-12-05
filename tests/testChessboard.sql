DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

CREATE TABLE chessboards (
    game_id serial PRIMARY KEY,
    game_board chessboard
);

INSERT INTO chessboards (game_board) VALUES 
('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'),  -- Starting position
('rnbqkbnr/ppp1pppp/8/3p4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'), -- After 1...d5
('r1bqkbnr/pppppppp/n7/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 2 2'); -- After 1.e4 Na6

SELECT * FROM chess_games;

SELECT (
    SELECT game_board FROM chess_games WHERE game_id = 1
    ) = (
        SELECT game_board FROM chess_games WHERE game_id = 2
    ) AS are_equal;

UPDATE chess_games 
SET game_board = 'rnbqkb1r/ppp1pppp/5n2/3p4/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 1 3' 
WHERE game_id = 2;

SELECT game_id, game_board 
FROM chess_games 
WHERE game_board LIKE '% w %';