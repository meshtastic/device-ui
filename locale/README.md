To install:

```
    npm i lv_i18n -g
```

To extract:

```
    lv_i18n extract -s 'generated/**/*.+(c|cpp|h|hpp)' -s 'source/**/*.+(c|cpp|h|hpp)' -t 'locale/*.yml'
```

To compile:

```
    cd locale
    npx lv_i18n compile -t './*.yml' -o '.'
```
