<p align="center"><img src="https://user-images.githubusercontent.com/7409436/70395873-af825600-1a03-11ea-9ffc-a49d5f766bac.png" width="400"></p>

<<<<<<< HEAD
# An optimization proposal for Julia Set sequence
=======
# An analysis of Julia Set sequence algorithm
>>>>>>> mohsin
## About Julia Set

The Julia set is a fractal sequence that generates a complex output image, named after Gaston Julia. It is an interesting CPU-bound problem with a very explicit set of inputs, which allows us to profile both the CPU usage and the RAM usage so we can understand which parts of our code are consuming two of our scarce computing resources.

## Usage

### C
To compile the C code, run the following commands in your terminal:

```console
$ gcc -Ofast Julia.c -o Julia
$ ./Julia <(int) size> <(int) iterations> <(int)[0|1] Output Image>
```

### Python
To run the python implementation of the code, run the following commands in your terminal:

```sh
$ python Julia.py
```

### Cython
To compile and run cython code, run the following commands in your terminal:

```sh
$ cythonize -3 -i -a cython/Julia.pyx
$ ipython

> import Julia as J
> J.run(<size>,<iterations>)
```


## Authors
Mohsin Riaz [github@im-mou](https://github.com/im-mou/)

Daniel Parra Martin [github@username](https://github.com/)
