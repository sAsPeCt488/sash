# sash

A toy shell, developed in order to understand process manipulation through POSIX API.

## Features:
- Redirections: `ls -la > list.txt` & `cat < list.txt`
- Pipes: `ls -la | grep sash.bin`
- Different prompts based on eUID

## How to Βuild:
```sh
git clone https://github.com/sAsPeCt488/sash.git
cd sash
make
```

## Disclaimer:

It is advisable, that you **__DO NOT utilize it on production environments__** as it has not been thoroughly audited for vulnerabilites.


	

