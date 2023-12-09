DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

DROP TABLE IF EXISTS chessgames CASCADE;
CREATE TABLE chessgames (
    id serial PRIMARY KEY,
    game chessgame,
    fenStates TEXT[]
);

COPY chessgames(game) FROM '/mnt/e/Documents/BDMA/ULB/DBSA/project/ULB-H417-PGchess/data/pgndemo.csv' DELIMITER ',' CSV;
UPDATE chessgames SET fenStates = getAllStates(game);
CREATE INDEX hasBoard_idx ON chessgames USING GIN(getAllStates(game));
VACUUM ANALYZE chessgames;

EXPLAIN ANALYZE SELECT COUNT(*) FROM chessgames
WHERE hasBoard(game, 'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3', 4);

EXPLAIN ANALYZE SELECT COUNT(*) FROM chessgames
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1', 1);

SELECT COUNT(*) FROM chessgames
WHERE hasBoard(game, 'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3', 4);

SELECT COUNT(*) FROM chessgames
WHERE hasBoard(game, 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1', 1);