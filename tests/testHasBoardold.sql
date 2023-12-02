DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

DROP TABLE IF EXISTS chessgames CASCADE;
CREATE TABLE chessgames (
    id serial PRIMARY KEY,
    game chessgame,
);

COPY chessgames(game) FROM '/mnt/e/Documents/BDMA/ULB/DBSA/project/ULB-H417-PGchess/data/pgndemo.csv' DELIMITER ',' CSV;
CREATE INDEX fen_idx ON chessgames USING GIN(getAllStates(game));
VACUUM ANALYZE chessgames;

EXPLAIN SELECT * FROM chessgames
WHERE hasBoard(game, 'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3', 4);

-- Insert sample chessgames
-- INSERT INTO chessgames (game)
-- VALUES ('1.e4 e5 2.Nf3 Nc6 3.Bb5 a6'::chessgame); -- id = 1

-- INSERT INTO chessgames (game)
-- VALUES ('1.d4 d5 2.c4 e6 3.Nc3 Be7'::chessgame); -- id = 2

-- INSERT INTO chessgames (game)
-- VALUES ('1.f4 d5'::chessgame); -- id = 3

-- SELECT hasBoard(fenStates, 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 0)
-- from chessgames where id = 1221;

-- SELECT hasBoard(game, 'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3'::chessboard, 6) as has_board 
-- FROM chessgames WHERE id = 1;  -- Expected: true

-- SELECT hasBoard(game, 'r1bqkbnr/1ppp1ppp/p1n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 3'::chessboard, 6) as has_board 
-- FROM chessgames WHERE id = 1;  -- Expected: false

-- SELECT hasBoard(game, 'r1bqkbnr/1ppp1ppp/p1n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 4'::chessboard, 20) as has_board 
-- FROM chessgames WHERE id = 1;  -- Expected: true

-- SELECT hasBoard(game, 'r1bqkbnr/1ppp1ppp/p1n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 3'::chessboard, 20) as has_board 
-- FROM chessgames WHERE id = 1;  -- Expected: false


-- SELECT COUNT(*) FROM chessgames
-- WHERE hasBoard(fenStates, 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1'::chessboard, 1);

EXPLAIN SELECT * FROM chessgames 
WHERE fenStates && ARRAY['0:r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3', 
                         '1:r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3',
                         '2:r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3',
                         '3:r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3',
                         '4:r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3'];

EXPLAIN SELECT * FROM chessgames
WHERE hasBoard(game, 'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3', 4);