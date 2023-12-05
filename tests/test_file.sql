DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

DROP TABLE IF EXISTS games;
CREATE TABLE games (
    id serial PRIMARY KEY,
    game chessgame
);

DROP TABLE IF EXISTS favoritegames;
CREATE TABLE favoritegames (
    id serial PRIMARY KEY,
    game chessgame
);

-- Inserting some games
INSERT INTO games (game) VALUES ('1.e4 c5 2.c3 Nf6 3.e5 Nd5 4.d4 Nc6 5.Nf3 cxd4 6.cxd4 e6 7.a3 d6 8.Bd3 Qa5+ 9.Bd2 Qb6 10.Nc3 Nxc3 11.Bxc3 dxe5 12.dxe5 Be7 13.O-O Bd7 14.Nd2 Qc7 15.Qg4 O-O-O 16.Rfc1 Kb8 17.Qc4 Rc8 18.b4 f6 19.Nf3 Qb6 20.Qe4 f5 21.Qe1 a6 22.Rab1 g5 23.Nd2 Nd4 24.Qe3 Rxc3 25.Rxc3 f4 26.Qe1 g4 27.Ne4 Bc6 28.Nc5 Ka7 29.a4 Bf3 30.a5 Qd8 31.Bc4 Bxc5 32.bxc5 Qh4 33.gxf3 gxf3 34.Kh1 Rg8 35.Qe4 Rg7 36.Qxd4 Qg5 37.c6+ Kb8 38.c7+ Rxc7 39.Rg1 Qh5 40.Rg8+ Rc8 41.Qd6+ Ka7  1-0'::chessgame);
INSERT INTO games (game) VALUES ('1.c4 e6 2.g3 d5 3.Bg2 Nf6 4.Nf3 Be7 5.O-O O-O 6.b3 c5 7.Bb2 Nc6 8.e3 d4 9.exd4 cxd4 10.Re1 Re8 11.a3 a5 12.d3 Bc5 13.Ne5 Nxe5 14.Rxe5 Qc7 15.Re1 e5 16.Nd2 Bg4 17.Qc2 Rab8 18.Ne4 Nxe4 19.Bxe4 Bd7 20.Bc1 Bc6 21.f4 exf4 22.Bxf4  1/2-1/2'::chessgame);
INSERT INTO games (game) VALUES ('1.d4 Nf6 2.Nf3 g6 3.Bg5 Bg7 4.Nbd2 O-O 5.c3 d6 6.e4 c5 7.dxc5 dxc5 8.Be2 Nc6 9.O-O b6 10.Qc2 Bb7 11.Bh4 Nh5 12.Rfd1 Qc7 13.Nc4 Bf6 14.Ne3 e6  1/2-1/2'::chessgame);
INSERT INTO games (game) VALUES ('1.Nf3 Nf6 2.c4 g6 3.Nc3 d5 4.cxd5 Nxd5 5.e4 Nxc3 6.dxc3 Qxd1+ 7.Kxd1 Nd7 8.Bf4 c6 9.Bd3 f6 10.e5 Nc5 11.Bc2 Bg4 12.Ke2 Bg7 13.Be3 Nd7 14.exf6 exf6 15.h3 Be6 16.Bb3 Bxb3 17.axb3 a6 18.Rhe1 Kf7 19.Rad1 Rhd8 20.Rd3 Nf8 21.Red1 Rxd3 22.Rxd3 Re8 23.Kd2 Ne6 24.Rd7+ Re7 25.Rxe7+ Kxe7 26.b4 f5 27.Kd3 Kd6 28.Nd2 b5 29.Nb3 Kd5 30.Nc5 Nxc5+ 31.Bxc5 a5 32.b3 axb4 33.Bxb4 h5 34.c4+ Ke6 35.Bd2  1/2-1/2'::chessgame);

INSERT INTO favoritegames (game) VALUES ('1.Nf3 Nf6 2.c4 g6 3.Nc3 d5 4.cxd5 Nxd5 5.e4 Nxc3 6.dxc3 Qxd1+ 7.Kxd1 Nd7 8.Bf4 c6 9.Bd3 f6 10.e5 Nc5 11.Bc2 Bg4 12.Ke2 Bg7 13.Be3 Nd7 14.exf6 exf6 15.h3 Be6 16.Bb3 Bxb3 17.axb3 a6 18.Rhe1 Kf7 19.Rad1 Rhd8 20.Rd3 Nf8 21.Red1 Rxd3 22.Rxd3 Re8 23.Kd2 Ne6 24.Rd7+ Re7 25.Rxe7+ Kxe7 26.b4 f5 27.Kd3 Kd6 28.Nd2 b5 29.Nb3 Kd5 30.Nc5 Nxc5+ 31.Bxc5 a5 32.b3 axb4 33.Bxb4 h5 34.c4+ Ke6 35.Bd2  1/2-1/2'::chessgame);
INSERT INTO favoritegames (game) VALUES ('1.c4 e6 2.g3 d5 3.Bg2 Nf6 4.Nf3 Be7 5.O-O O-O 6.b3 c5 7.Bb2 Nc6 8.e3 d4 9.exd4 cxd4 10.Re1 Re8 11.a3 a5 12.d3 Bc5 13.Ne5 Nxe5 14.Rxe5 Qc7 15.Re1 e5 16.Nd2 Bg4 17.Qc2 Rab8 18.Ne4 Nxe4 19.Bxe4 Bd7 20.Bc1 Bc6 21.f4 exf4 22.Bxf4  1/2-1/2'::chessgame);
INSERT INTO favoritegames (game) VALUES ('1.d4 e6 2.e4 d5 3.Nd2 b6 4.Ngf3 Bb7 5.Bb5+ c6 6.Bd3 Be7 7.O-O dxe4 8.Nxe4 Nf6 9.Nxf6+ Bxf6 10.Bf4 O-O 11.Qe2 Qd5 12.Be4 Qh5 13.a4 Nd7 14.a5 bxa5 15.Bd6 Rfc8 16.Qd2 Bd8 17.Ne5 Nxe5 18.dxe5 Ba6 19.Rfe1 f5 20.exf6 Bxf6 21.Rxa5 Bb5 22.c4 Rd8 23.Qb4 Rac8 24.cxb5 c5 25.b6 Rxd6 26.bxa7 Rdd8 27.Qa4 Be5 28.a8=Q Qxh2+ 29.Kf1 Rxa8 30.Rxa8  1-0'::chessgame);

----------------------------------------------- HAS BOARD -----------------------------------------------------------------------

-- How many games contain the given board state at any time during the game?
-- Expected Output: 0
-- IDEMPOTENCY ISSUE
SELECT count(*)
FROM games
WHERE hasboard(game, 'rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2', 10);

-- Expected Output: 0
SELECT count(*)
FROM games
WHERE hasboard(game, 'rnbqkb1r/pp1ppppp/5n2/2p1P3/8/2P5/PP1P1PPP/RNBQKBNR b KQkq - 0 3', 4);

-- Expected Output: 1
SELECT count(*)
FROM games
WHERE hasboard(game, 'rnbqkb1r/pp1ppppp/5n2/2p1P3/8/2P5/PP1P1PPP/RNBQKBNR b KQkq - 0 3', 5);

-- Expected Output: 1
SELECT count(*)
FROM games
WHERE hasboard(game, 'rnbqkb1r/pp1ppppp/5n2/2p1P3/8/2P5/PP1P1PPP/RNBQKBNR b KQkq - 0 3', 100);

-- Expected Output: 1
SELECT count(*)
FROM games
WHERE hasboard(game, '1kr3R1/1p5p/p2Qp3/P3P2q/2B2p2/2R2p2/5P1P/7K b - - 5 41', 100);

-- How many games contain the given board state at any time during the game?
-- Expected Output: 0
SELECT count(game)
FROM games
WHERE hasboard(game, 'rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2', 10);

-- How many games have the initial board state in the first twenty half moves?
-- Expected Output: All games i.e,. size of table.
SELECT count(*)
FROM games
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 20);

-- How many games have the initial board state in the first twenty half moves?
-- Expected Output: All games i.e,. size of table.
SELECT count(*)
FROM games
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 0);

-- How many games have the initial board state but with an number of moves?
-- Expected Output: 0
SELECT count(*)
FROM games
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', -1);


----------------------------------------------- HAS OPENING -----------------------------------------------------------------------

-- How many games started with the given sequence of moves?
-- Expected Output: 1
SELECT count(*)
FROM games
WHERE hasopening(game, '1.d4 Nf6 2.Nf3 g6');

-- How many games started with the given sequence of moves (Complete match)?
-- Expected Output: 1
SELECT count(*)
FROM games
WHERE hasopening(game, '1.d4 Nf6 2.Nf3 g6 3.Bg5 Bg7 4.Nbd2 O-O 5.c3 d6 6.e4 c5 7.dxc5 dxc5 8.Be2 Nc6 9.O-O b6 10.Qc2 Bb7 11.Bh4 Nh5 12.Rfd1 Qc7 13.Nc4 Bf6 14.Ne3 e6  1/2-1/2');

-- Which games have the same 10 first half-moves as any of the games stored in table favoriteGames?
-- Expected Output: 2
SELECT count(*)
FROM games g, favoritegames f
WHERE hasopening(g.game, getFirstMoves(f.game, 10));

-- How does hasOpening respond to syntactic issues?
SELECT count(*)
FROM games
WHERE hasopening(game, '1.');

SELECT count(*)
FROM games
WHERE hasopening(game, '1');


----------------------------------------------- GET BOARD -----------------------------------------------------------------------

-- Get board at 0 half moves (initial state)
-- Expected output: All games have the correct initial state
SELECT 
    CASE 
        WHEN COUNT(*) = 0 THEN 'All games have the correct initial state'
        ELSE 'Not all games have the correct initial state'
    END as check_result
FROM games
WHERE getBoard(game, 0) != 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1';

-- Get board at invalid number of moves (e.g., negative)
-- Expected output: No board states
SELECT id, getBoard(game, -1) AS board_state
FROM games;

-- Get board state beyond the last move of the game.
-- Expected output: Final board states
SELECT id, getBoard(game, 60) AS board_state
FROM games;

-- Count games that match the provided board state after 4 half-moves:
-- Expected output: 1
SELECT COUNT(*) AS matching_games_count
FROM games
WHERE getBoard(game, 4) = 'rnbqkb1r/pp1ppppp/5n2/2p5/4P3/2P5/PP1P1PPP/RNBQKBNR w KQkq - 1 3';

-- Count games that match the provided board state after 3 half-moves:
-- Expected output: 0
SELECT COUNT(*) AS matching_games_count
FROM games
WHERE getBoard(game, 20) = 'rnbqkb1r/pp1ppppp/5n2/2p5/4P3/2P5/PP1P1PPP/RNBQKBNR w KQkq - 1 3';

----------------------------------------------- GET FIRST MOVES  -----------------------------------------------------------------------

-- Get game at 0 half moves (initial state)
-- Expected output: No games
SELECT id, getFirstMoves(game, 0) as move_zero
FROM games;

-- Get game at invalid half moves (negative hald moves)
-- Expected output: No games
SELECT id, getFirstMoves(game, -1) as move_zero
FROM games;

-- Get final game (e.g., after 100 half moves)
-- Expected output: All games match
SELECT 
    CASE 
        WHEN COUNT(*) = (SELECT COUNT(*) FROM games WHERE getFirstMoves(game, 100)::chessgame = game::chessgame) 
        THEN 'All games match'
        ELSE 'Some games do not match'
    END AS result
FROM games;

-- Compare opening sequences in games and favoritegames
SELECT 
    g.opening_sequence, 
    g.count AS games_count, 
    COALESCE(fg.count, 0) AS favorite_games_count
FROM 
    (SELECT getFirstMoves(game, 2) AS opening_sequence, COUNT(*) AS count FROM games GROUP BY opening_sequence) g
LEFT JOIN 
    (SELECT getFirstMoves(game, 2) AS opening_sequence, COUNT(*) AS count FROM favoritegames GROUP BY opening_sequence) fg
ON g.opening_sequence = fg.opening_sequence;
