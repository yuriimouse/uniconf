# Virtual environment
#### Composer run and start

- `cd docker`
- `docker compose up -d`
- `docker compose exec uniconf-lib /bin/bash`

#### Make bin in virtual environment

- `cd /uniconf`
- `make` basic make
- `make install` for install
- `make tests` for build tests
- `make run` execute tests under valgrind
