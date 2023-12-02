DROP EXTENSION chess CASCADE;
CREATE EXTENSION chess;

DROP TABLE IF EXISTS chessgames;
CREATE TABLE chessgames (
    id serial PRIMARY KEY,
    game chessgame
);

COPY chessgames(game) FROM '/mnt/e/Documents/BDMA/ULB/DBSA/project/ULB-H417-PGchess/data/mygreat1.csv' DELIMITER ',' CSV;
COPY chessgames(game) FROM '/mnt/e/Documents/BDMA/ULB/DBSA/project/ULB-H417-PGchess/data/pgndemo.csv' DELIMITER ',' CSV;
-- Insert sample chessgames
-- INSERT INTO chessgames (game)
-- VALUES ('1.e4 e5 2.Nf3 Nc6 3.Bb5 a6'::chessgame);

INSERT INTO chessgames (game)
VALUES ('1.e4 e5 2.Nf3'::chessgame);

-- create btree index
CREATE INDEX chessgames_idx ON chessgames USING btree (game chessgame_ops);

-- Test the hasOpening function
EXPLAIN SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 e5 2.Nf3'::chessgame);

SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 e52.Nf3'::chessgame);

SELECT * FROM chessgames WHERE hasOpening(game, '1.e4 e5 2.Nf3 Nc6 3.Bb5 a6'::chessgame);