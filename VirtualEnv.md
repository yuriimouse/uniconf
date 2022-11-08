# Virtual environment
#### Composer run and start

- `cd docker`
- `docker compose up -d`
- `docker compose exec uniconf-lib /bin/bash`

#### Make bin in virtual environment

- `export LD_LIBRARY_PATH=/usr/local/custom/lib/`
- `cd /uniconf`
- `make` basic make
- `make re` for remake
- `make test` for build tests
- `make install` for install
