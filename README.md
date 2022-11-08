# uniconf

Universal configurator based on cJSON

Acceptable formats:
 .conf
 .env
 .ini
 .json
 .yml

## common

The configurator scans the specified directory|file and creates a JSON structure according to the contents of the files.

Files are interpreted first, then directories.

Files are sorted alphabetically.

Unknown extensions are ignored and can be used as documentation.

## references

Each string value can be expanded by a previusly defined variable or another file.

The reference to the variable is _$(VAR_NAME)_

The refence to the file is _@(FILE_NAME)_

## examples

The directory `config`
```
config
    .env
    comments.md
    first.ini
    second.json
    one_subdir
        data.yaml
    other_subdir
        config.conf
```
converts into
``` json
{
    "user": "somebody",
    "password": "password",
    "first": {
        "string": "string",
        "integer": 42
    },
    "second": {
        
    }
}
```

