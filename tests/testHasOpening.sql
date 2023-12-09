DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

DROP TABLE IF EXISTS chessgames;
CREATE TABLE chessgames (
    id serial PRIMARY KEY,
    game chessgame
);

COPY chessgames(game) FROM '/mnt/e/Documents/BDMA/ULB/DBSA/project/ULB-H417-PGchess/data/pgndemo.csv' DELIMITER ',' CSV;
COPY chessgames(game) FROM '/mnt/e/Documents/BDMA/ULB/DBSA/project/ULB-H417-PGchess/data/pgndemo.csv' DELIMITER ',' CSV;
COPY chessgames(game) FROM '/mnt/e/Documents/BDMA/ULB/DBSA/project/ULB-H417-PGchess/data/pgndemo.csv' DELIMITER ',' CSV;

-- create btree index
-- CREATE INDEX hasOpening_idx ON chessgames USING btree (game chessgame_ops);
CREATE INDEX hasOpening_idx ON chessgames (CAST(game as TEXT) text_pattern_ops);

VACUUM ANALYZE chessgames;

-- doesn't use btree at the momoment (its not very selective)
EXPLAIN ANALYZE SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 c5 2.Nf3');
-- uses btree
EXPLAIN ANALYZE SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 c5 2.Nf3 Nc6');
-- doesn't use btree
EXPLAIN ANALYZE SELECT * FROM chessgames WHERE hasOpening(game, getFirstMoves('1.e4 c5 2.Nf3 Nc6', 4));
-- uses btree
EXPLAIN ANALYZE SELECT * FROM chessgames WHERE game::text LIKE getFirstMoves('1.e4 c5 2.Nf3 Nc6', 4) || '%';
-- uses btree
EXPLAIN ANALYZE SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 c5 2.Nf3 Nc6 3.d4 cxd4 4.Nxd4 Nf6 5.Nc3 e6');
-- uses btree
EXPLAIN ANALYZE SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 c5 2.Nf3 Nc6 3.d4 cxd4');

-- btree wont be used because planner doesnt know whats inside the subquery
EXPLAIN ANALYZE SELECT * FROM chessgames
WHERE hasOpening(game, (SELECT getFirstMoves(game, 6) FROM chessgames WHERE id = 2));
