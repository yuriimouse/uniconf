# Virtual environment
#### Composer run and start

- `cd docker`
- `docker compose up -d`
- `docker compose exec uniconf-lib /bin/bash`

#### Make bin in virtual environment

- `export LD_LIBRARY_PATH=/usr/local/custom/lib/:/usr/local/lib/`
- `git clone https://github.com/yuriimouse/lists.git && cd lists/ && make install && cd .. && rm -rf lists/`
- `cd /uniconf`
- `make` basic make
- `make re` for remake
- `make install` for install
- `make test` for build tests
