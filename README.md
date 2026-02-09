# cli_pos
A pos system in the terminal!

It connects to a postgresql database, that at the moment you have to configure yourself, will have to work on that.

### Features
Supports barcode scanners

Lightweight

Open source

## Compiling
To compile you need the following dependecies:
  ncurses, 
  C++ compiler capable of c++ 20

  NOTE: The source code comes with a copy of the pqxx library, 
  see their respective copyright notice at libpqxx/

This project uses CMake to build.
### Debian/Ubunutu
and other os's based on them

Install dependencies:
```bash
sudo apt install libncurses-dev pq gcc
```

Download from github using git:
```bash
git clone https://github.com/namenotfoundindb/cli_pos/
```
Generate a MakeFile:
```bash
cd cli_pos
mkdir build
cd build
cmake ..
```
Compile the code:
```bash
make
```
### Install (optional)
```bash
sudo make install
```

## Configuring postresql database
At the moment you have to configure the database. Will work on a config script






