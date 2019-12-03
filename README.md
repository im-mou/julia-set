# An optimization proposal for Julia Set sequence
## About Julia Set

The Julia set is a fractal sequence that generates a complex output image, named after Gaston Julia. It is an interesting CPU-bound problem with a very explicit set of inputs, which allows us to profile both the CPU usage and the RAM usage so we can understand which parts of our code are consuming two of our scarce computing resources.

## Usage
### Python
To run the python implementation of the code, run the following commands in your terminal:

<code>
python Julia.py
</code>

### C
To compile the C code, run the following commands in your terminal:

<code>
gcc -Ofast Julia.c -o Julia

./Julia `<size>` `<iterations>` 
</code>

## Authors
Mohsin Riaz [github@im-mou](https://github.com/im-mou/)

Daniel Parra Martin [github@username](https://github.com/)
