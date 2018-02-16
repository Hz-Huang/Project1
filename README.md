> # Project 1

## Installation
To build, use:
> `make`

To generate documentation:
> `make doc`

To clean, use:
> `make clean`

## Usage

To run, use:
> `./quash`
or
> `make test`

# Note
The original code contain variable HOST_NAME_MAX, which is only avaliable on Linux.<br />
In order to run this on macOS, src/quash.c is modified, HOST_NAME_MAX is defined as 255.
