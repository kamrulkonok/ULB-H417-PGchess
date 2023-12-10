                                                    --- Creating Extension --- 
DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;


                                                    --- Necessary tables for testing ---
DROP TABLE IF EXISTS chessgames;
CREATE TABLE chessgames (
    id serial PRIMARY KEY,
    game chessgame
);


DROP TABLE IF EXISTS game_test;
CREATE TABLE game_test (
    id serial PRIMARY KEY,
    game chessgame
);

DROP TABLE IF EXISTS favoritegames;
CREATE TABLE favoritegames (
    id serial PRIMARY KEY,
    game chessgame
);

DROP TABLE IF EXISTS games;
CREATE TABLE games (
    id serial PRIMARY KEY,
    game chessgame
);

DROP TABLE IF EXISTS chessboards;
CREATE TABLE chessboards (
    game_id serial PRIMARY KEY,
    game_board chessboard
);
--- Populate the tables
\COPY game_test(game) FROM '/mnt/c/Users/Konok/Desktop/ULB-H417-PGchess-main/tests/chessgames_data.csv' DELIMITER ',' CSV;

                                                    --- chessboard datatype ---

-- to check the size of chessgame datatype
SELECT text_to_chessgame('1.e4 c5 2.Nf3 d6 3.Bb5+ Nd7 4.d4 Nf6 5.Nc3 cxd4 6.Qxd4 e5 7.Qd3 h6 8.Be3 Be7 9.O-O O-O 10.Rad1 a6');


                                                    --- chessgame datatype ---

-- to check the size of chessboard datatype
select text_to_chessboard('rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1');

                                                    --- getBoard Function ---

--- Basic queries to test the getBoard function
SELECT getBoard(game, 0) as initial_board FROM chessgames;
SELECT getBoard(game, 1) as first_half_move FROM chessgames;
SELECT getBoard(game, 2) as second_half_move FROM chessgames;
SELECT getBoard(game, 3) as third_half_move FROM chessgames;
SELECT getBoard(game, 4) as fourth_half_move FROM chessgames;
SELECT getBoard(game, 5) as fifth_half_move FROM chessgames;
SELECT getBoard(game, 6) as sixth_half_move FROM chessgames;
SELECT getBoard(game, 7) as last_half_move FROM chessgames;

--- Interesting test cases for getBoard function

-- 1. Retrieves initial chessboard position and positions after 5th and 10th half-moves.

SELECT 
    id, 
    getBoard(game, 0) as initial_position, 
    getBoard(game, 5) as position_after_5_half_moves,
    getBoard(game, 10) as position_after_10_half_moves
FROM chessgames
WHERE id = 15;

-- 2. Displaying the initial position in the table, along with the board states after the first moves by white and black

SELECT 
    id,
    getBoard(game, 0) as initial_position,
    getBoard(game, 1) as after_white_first_move,
    getBoard(game, 2) as after_black_first_move
FROM chessgames LIMIT 5;

-- 3. Get board at 0 half moves (initial state)
SELECT 
    CASE 
        WHEN COUNT(*) = 0 THEN 'All games have the correct initial state'
        ELSE 'Not all games have the correct initial state'
    END as check_result
FROM chessgames
WHERE getBoard(game, 0) != 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1';

-- 4. Get board at invalid number of moves (e.g., negative)
SELECT id, getBoard(game, -1) AS board_state
FROM chessgames;

-- 5. Get board state beyond the last move of the game.
SELECT id, getBoard(game, 60) AS board_state
FROM games;

-- 6. Count games that match the provided board state after 4 half-moves:
SELECT COUNT(*) AS matching_games_count
FROM chessgames
WHERE getBoard(game, 4) = 'rnbqkb1r/pp1ppppp/5n2/2p5/4P3/2P5/PP1P1PPP/RNBQKBNR w KQkq - 1 3';

-- 7. Count games that match the provided board state after 3 half-moves:
SELECT COUNT(*) AS matching_games_count
FROM chessgames
WHERE getBoard(game, 20) = 'rnbqkb1r/pp1ppppp/5n2/2p5/4P3/2P5/PP1P1PPP/RNBQKBNR w KQkq - 1 3';
-- 8. Evolution of Game Boards in King's Pawn Opening: What are the most common board states at the 10th and 20th moves in games starting with 1.e4 e5?
SELECT 
    getBoard(game, 10) AS board_state_at_move_10,
    getBoard(game, 20) AS board_state_at_move_20,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 2) = '1.e4 e5 '::chessgame
GROUP BY board_state_at_move_10, board_state_at_move_20
ORDER BY frequency DESC
LIMIT 5;

-- 9. Most Common Board States for a Given Opening: What are the most common board states after 10 moves for games that start with the King's Pawn opening.

SELECT 
    getBoard(game, 10) AS board_state,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 1) = '1.e4 '::chessgame
GROUP BY board_state
ORDER BY frequency DESC
LIMIT 10;
-- 10. Distribution of Board States in a Popular Opening: What are the popular board states after the first 10 half-moves in games that start with the King's Pawn opening.
SELECT 
    getBoard(game, 10) AS board_state_after_10_moves,
    COUNT(*) AS number_of_games
FROM chessgames
WHERE getFirstMoves(game, 1) = '1.e4 '::chessgame
GROUP BY board_state_after_10_moves
ORDER BY number_of_games DESC
LIMIT 5;

-- 11. Evolution of Board States in a Specific Opening VariationObjective: How do board states evolve in games with the Queen's Gambit?
SELECT 
    getBoard(game, 15) AS board_state_at_move_15,
    COUNT(*) AS number_of_games
FROM chessgames
WHERE hasOpening(game, '1.d4 d5 2.c4 e6')
GROUP BY board_state_at_move_15
ORDER BY number_of_games DESC
LIMIT 10;
                                                    --- getFirstMoves function ---

-- Basic queries to test the getFirstMoves function
SELECT getFirstMoves(game, 0) as first_0_moves FROM chessgames;
SELECT getFirstMoves(game, 1) as first_1_moves FROM chessgames;
SELECT getFirstMoves(game, 2) as first_2_moves FROM chessgames;
SELECT getFirstMoves(game, 3) as first_3_moves FROM chessgames;
SELECT getFirstMoves(game, 4) as first_4_moves FROM chessgames;
SELECT getFirstMoves(game, 5) as first_5_moves FROM chessgames;
SELECT getFirstMoves(game, 6) as first_6_moves FROM chessgames;
SELECT getFirstMoves(game, 7) as first_7_moves FROM chessgames;

--- Interesting test cases to check getFirstMoves function 

-- 1. Displaying full chessgame and truncated to first 2 and 3 full moves for each game

SELECT 
    id,
    game as full_game,
    getFirstMoves(game, 4) as first_2_moves_opening,
    getFirstMoves(game, 6) as first_3_moves_opening
FROM chessgames;


-- 2. Displaying common openings by categorizing and counting the number of chess games by their opening sequence 

SELECT 
    getFirstMoves(game, 6) as opening_sequence,
    COUNT(*) as number_of_games
FROM chessgames
GROUP BY opening_sequence
ORDER BY number_of_games DESC LIMIT 5;


-- 3. Get game at 0 half moves (initial state)
-- Expected output: No games
SELECT id, getFirstMoves(game, 0) as move_zero
FROM chessgames;

-- 4. Get game at invalid half moves (negative hald moves)
-- Expected output: No games
SELECT id, getFirstMoves(game, -1) as move_zero
FROM chessgames LIMIT 5;

-- 5. Get final game (e.g., after 100 half moves)
SELECT 
    CASE 
        WHEN COUNT(*) = (SELECT COUNT(*) FROM chessgames WHERE getFirstMoves(game, 100)::chessgame = game::chessgame) 
        THEN 'All games match'
        ELSE 'Some games do not match'
    END AS result
FROM chessgames;

-- 6. Compare opening sequences in games and favoritegames

EXPLAIN SELECT 
    g.opening_sequence, 
    g.count AS games_count, 
    COALESCE(fg.count, 0) AS favorite_games_count
FROM 
    (SELECT getFirstMoves(game, 2) AS opening_sequence, COUNT(*) AS count FROM chessgames GROUP BY opening_sequence) g
LEFT JOIN 
    (SELECT getFirstMoves(game, 2) AS opening_sequence, COUNT(*) AS count FROM favoritegames GROUP BY opening_sequence) fg
ON g.opening_sequence = fg.opening_sequence;

-- 7. Popularity of Certain Openings: How popular are the Italian Game (1.e4 e5) and the Sicilian Defense (1.e4 c5) compared to each other?

SELECT 
    getFirstMoves(game, 2) AS opening,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 2) IN ('1.e4 e5 '::chessgame, '1.e4 c5 '::chessgame)
GROUP BY opening;

-- 8. Most Common Responses to a Specific Opening: What are the most common responses to the opening move '1.e4'?

SELECT 
    getFirstMoves(game, 2) AS opening_response,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 1) = '1.e4 '::chessgame
GROUP BY opening_response
ORDER BY frequency DESC;
--  9. Evolution of an Opening: How do the games evolve after the first three moves of the Queen's Gambit (1.d4 d5 2.c4)?
SELECT 
    getFirstMoves(game, 6) AS first_six_half_moves,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 3) = '1.d4 d5 2.c4 '::chessgame
GROUP BY first_six_half_moves
ORDER BY frequency DESC;
-- 10. Most Common Board States for Specific Openings: What are the most common board states after 8 moves between the Queen’s Gambit and the Sicilian Defense?
SELECT 
    getFirstMoves(game, 2) AS opening,
    getBoard(game, 8) AS board_state,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 2) IN ('1.d4 d5 2.c4 '::chessgame, '1.e4 c5 '::chessgame)
GROUP BY opening, board_state
ORDER BY opening, frequency DESC
LIMIT 5;

                                                            --- hasOpening Function ---


-- 1. Check each game to determine if the game starts with '1.e4 e5 2.Nf3 Nc6 3.Bb5' opening, and providing bool result 
SELECT 
    id, 
    hasOpening(game, '1.e4 e5 2.Nf3 Nc6'::chessgame) as is_chess_opening
FROM chessgames 
LIMIT 5;

-- 2. How many games started with the given sequence of moves?
SELECT count(*)
FROM chessgames
WHERE hasopening(game, '1.d4 Nf6 2.Nf3 g6');

-- 3. How many games started with the given sequence of moves (Complete match)?
SELECT count(*)
FROM chessgames
WHERE hasopening(game, '1.d4 Nf6 2.Nf3 g6 3.Bg5 Bg7 4.Nbd2 O-O 5.c3 d6 6.e4 c5 7.dxc5 dxc5 8.Be2 Nc6 9.O-O b6 10.Qc2 Bb7 11.Bh4 Nh5 12.Rfd1 Qc7 13.Nc4 Bf6 14.Ne3 e6  1/2-1/2');

-- 4. Which games have the same 10 first half-moves as any of the games stored in table favoriteGames?
SELECT count(*)
FROM chessgames g, favoritegames f
WHERE hasopening(g.game, getFirstMoves(f.game, 10));


                                                    --- hasBoard Function ---

-- 1. How many games contain the given board state at any time during the game?
SELECT count(*)
FROM chessgames
WHERE hasboard(game, 'rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2', 10);

SELECT count(*)
FROM chessgames
WHERE hasboard(game, 'rnbqkb1r/pp1ppppp/5n2/2p1P3/8/2P5/PP1P1PPP/RNBQKBNR b KQkq - 0 3', 4);

SELECT count(*)
FROM chessgames
WHERE hasboard(game, 
'rnbqkb1r/pp1ppppp/5n2/2p1P3/8/2P5/PP1P1PPP/RNBQKBNR b KQkq - 0 3', 5);

SELECT count(*)
FROM chessgames
WHERE hasboard(game, 'rnbqkb1r/pp1ppppp/5n2/2p1P3/8/2P5/PP1P1PPP/RNBQKBNR b KQkq - 0 3', 100);

SELECT count(*)
FROM chessgames
WHERE hasboard(game, '1kr3R1/1p5p/p3p3/P3P2q/2BQ1p2/2R2p2/5P1P/7K w - - 4 41', 100);

-- 2. How many games contain the given board state at any time during the game?
SELECT count(game)
FROM chessgames
WHERE hasboard(game, 'rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2', 10);

-- 3. How many games have the initial board state in the first twenty-half moves?
SELECT count(*)
FROM chessgames
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 20);

-- 4. How many games have the initial board state in the first twenty-half moves?
SELECT count(*)
FROM chessgames
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 0);

-- 5. How many games have the initial board state but with several moves?
SELECT count(*)
FROM chessgames
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', -1);

--- 6. Determining if a boardstate appears within the first 10 half-moves by joining chessgame and chessboard
SELECT 
    cg.id AS game_id,
    cb.game_board,
    hasBoard(cg.game, cb.game_board, 10) AS board_in_first_10_half_moves
FROM chessgames cg 
JOIN chessboards cb ON cg.id = cb.game_id;




                                            --- B-Tree Index---

CREATE INDEX hasOpening_idx ON chessgames (CAST(game as TEXT) text_pattern_ops);
VACUUM ANALYZE chessgames;

-- Test the hasOpening function
EXPLAIN ANALYZE SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 c5 2.Nf3 Nc6 3.d4 cxd4');


                                            --- GIN Index---

CREATE INDEX fen_idx ON chessgames USING GIN(getAllStates(game));
VACUUM ANALYZE chessgames;

-- Test the getBoard function
EXPLAIN ANALYZE SELECT COUNT(*) FROM chessgames
WHERE hasBoard(game, 'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3', 4);
