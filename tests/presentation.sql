                                                                ---- getBoard (chessgame, int)

--- 1. Retrieves initial chessboard position and positions after 5th and 10th half-moves.
SELECT 
    id, 
    getBoard(game, 0) as initial_position, 
    getBoard(game, 5) as position_after_5_half_moves,
    getBoard(game, 10) as position_after_10_half_moves
FROM chessgames
WHERE id = 15;

--- 2. Most Common Board States for a Given Opening: What are the most common board states after 10 moves for games that start with the King's Pawn opening.
SELECT 
    getBoard(game, 10) AS board_state,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 1) = '1.e4 '::chessgame
GROUP BY board_state
ORDER BY frequency DESC
LIMIT 10;


                                                            ---- getFirstMoves(chessgame, int)

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


                                                        ---- hasOpening(chessgame, chessgame)

-- 1. Check each game to determine if the game starts with '1.e4' opening, and providing bool result 
SELECT 
    id, 
    hasOpening(game, '1.e4'::chessgame) as is_chess_opening
FROM chessgames 
LIMIT 15;

-- 2. How many games started with the given sequence of moves?
SELECT count(*)
FROM chessgames
WHERE hasopening(game, '1.d4 Nf6 2.Nf3 g6');

                                                        --- hasBoard(chessgame, chessboard, int)

--- 1. Determining if a boardstate appears within the first 10 half-moves by joining chessgame and chessboard
SELECT 
    cg.id AS game_id,
    cb.game_board,
    hasBoard(cg.game, cb.game_board, 10) AS board_in_first_10_half_moves
FROM chessgames cg 
JOIN chessboards cb ON cg.id = cb.game_id;

-- 2. How many games contain the given board state after 5th half moves?

SELECT count(*)
FROM chessgames
WHERE hasboard(game, 
'rnbqkb1r/pp1ppppp/5n2/2p1P3/8/2P5/PP1P1PPP/RNBQKBNR b KQkq - 0 3', 5);



                                                        ----Integration Testing - Queries
-- 1. Most Common Board States for Specific Openings: What are the most common board states after 8 moves between the Queen’s Gambit and the Sicilian Defense?
SELECT 
    getFirstMoves(game, 2) AS opening,
    getBoard(game, 8) AS board_state,
    COUNT(*) AS frequency
FROM chessgames
WHERE getFirstMoves(game, 2) IN ('1.d4 d5 2.c4 '::chessgame, '1.e4 c5 '::chessgame)
GROUP BY opening, board_state
ORDER BY opening, frequency DESC
LIMIT 5;

-- 2. Evolution of Board States in a Specific Opening Variation Objective: How do board states evolve in games with the Queen's Gambit?
SELECT 
    getBoard(game, 15) AS board_state_at_move_15,
    COUNT(*) AS number_of_games
FROM chessgames
WHERE hasOpening(game, '1.d4 d5 2.c4 e6')
GROUP BY board_state_at_move_15
ORDER BY number_of_games DESC
LIMIT 10;