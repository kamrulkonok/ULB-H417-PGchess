EXTENSION    = chessgame             # the name of the extension
MODULES      = chessgame             # the shared library to build
DATA         = chessgame--1.0.sql    # script files to install
# DOCS         = README.md             # documentation files (optional)

PG_CONFIG   ?= pg_config
PGXS         = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)