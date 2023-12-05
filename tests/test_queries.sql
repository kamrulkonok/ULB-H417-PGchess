DROP TABLE IF EXISTS game_test;
CREATE TABLE game_test (
    id serial PRIMARY KEY,
    game chessgame
);

\copy game_test(game) FROM '/mnt/h/BDMA/DBSA/ULB-H417-PGchess/chessgames_data.csv' WITH (FORMAT csv); 

-- Popularity of Certain Openings: How popular are the Italian Game (1.e4 e5) and the Sicilian Defense (1.e4 c5) compared to each other?
SELECT 
    getFirstMoves(game, 2) AS opening,
    COUNT(*) AS frequency
FROM game_test
WHERE getFirstMoves(game, 2) IN ('1.e4 e5 ', '1.e4 c5 ')
GROUP BY opening;

-- Most Common Responses to a Specific Opening: What are the most common responses to the opening move '1.e4'?
SELECT 
    getFirstMoves(game::text, 2) AS opening_response,
    COUNT(*) AS frequency
FROM game_test
WHERE getFirstMoves(game::text, 1) = '1.e4 '
GROUP BY opening_response
ORDER BY frequency DESC;

--  Evolution of an Opening: How do the games evolve after the first three moves of the Queen's Gambit (1.d4 d5 2.c4)?
SELECT 
    getFirstMoves(game, 6) AS first_six_half_moves,
    COUNT(*) AS frequency
FROM game_test
WHERE getFirstMoves(game, 3) = '1.d4 d5 2.c4 '
GROUP BY first_six_half_moves
ORDER BY frequency DESC;

-- Distribution of Board States in a Popular Opening: What are the popular board states after the first 10 half-moves in games that start with the King's Pawn opening.
SELECT 
    getBoard(game, 10) AS board_state_after_10_moves,
    COUNT(*) AS number_of_games
FROM game_test
WHERE getFirstMoves(game, 1) = '1.e4 '
GROUP BY board_state_after_10_moves
ORDER BY number_of_games DESC
LIMIT 10;

-- Evolution of Board States in a Specific Opening VariationObjective: How do board states evolve in games with the Queen's Gambit?
SELECT 
    getBoard(game, 15) AS board_state_at_move_15,
    COUNT(*) AS number_of_games
FROM game_test
WHERE hasOpening(game, '1.d4 d5 2.c4 e6')
GROUP BY board_state_at_move_15
ORDER BY number_of_games DESC
LIMIT 10;

-- Most Common Board States for a Given Opening: What are the most common board states after 10 moves for games that start with the King's Pawn opening.
SELECT 
    getBoard(game, 10) AS board_state,
    COUNT(*) AS frequency
FROM game_test
WHERE getFirstMoves(game, 1) = '1.e4 '
GROUP BY board_state
ORDER BY frequency DESC
LIMIT 10;

-- Most Common Board States for Specific Openings: What are the most common board states after 8 moves between the Queen’s Gambit and the Sicilian Defense?
SELECT 
    getFirstMoves(game, 2) AS opening,
    getBoard(game, 8) AS board_state,
    COUNT(*) AS frequency
FROM game_test
WHERE getFirstMoves(game, 2) IN ('1.d4 d5 2.c4 ', '1.e4 c5 ')
GROUP BY opening, board_state
ORDER BY opening, frequency DESC
LIMIT 10;

-- Evolution of Game Boards in King's Pawn Opening: What are the most common board states at the 10th and 20th moves in games starting with 1.e4 e5?
SELECT 
    getBoard(game, 10) AS board_state_at_move_10,
    getBoard(game, 20) AS board_state_at_move_20,
    COUNT(*) AS frequency
FROM game_test
WHERE getFirstMoves(game, 2) = '1.e4 e5 '
GROUP BY board_state_at_move_10, board_state_at_move_20
ORDER BY frequency DESC
LIMIT 5;


