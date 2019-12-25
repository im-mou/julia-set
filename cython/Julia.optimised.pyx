import cython
import numpy as np
# area of complex space to investigate
cdef double x1 = -1.8;
cdef double x2 = 1.8;
cdef double y1 = -1.8;
cdef double y2 = 1.8;
cdef double c_real = -0.62772;
cdef double c_imag = -0.42193;

@cython.boundscheck(False)
@cython.wraparound(False)
cdef int _sum(int width, int [::1] output):
  cdef int i,output_sum = 0
  for i in range(width):
    output_sum += output[i]
  return output_sum


@cython.boundscheck(False)
@cython.wraparound(False)
cdef calc_cython(int desired_width, int max_iterations):
  """Create a list of complex coordinates (zs) and complex parameters (cs), build Julia set, and display"""

  cdef double x_step = (float(x2 - x1) / float(desired_width))
  cdef double y_step = (float(y1 - y2) / float(desired_width))

  cdef double [::1] x = np.empty((desired_width+1),dtype=np.double)
  cdef double [::1] y = np.empty((desired_width+1),dtype=np.double)

  cdef int sx=0, sy=0

  cdef double ycoord = y2
  while ycoord > y1:
    y[sy] = ycoord
    ycoord += y_step
    sy+=1


  cdef double xcoord = x1
  while xcoord < x2:
    x[sx] = xcoord
    xcoord += x_step
    sx+=1

  # Build a list of coordinates and the initial condition for each cell.
  # Note that our initial condition is a constant and could easily be removed;
  # we use it to simulate a real-world scenario with several inputs to
  # our function.

  cdef double complex [::1] zs = np.empty((sy*sx),dtype=np.complex)

  cdef double imag_part = 0
  cdef int i,j
  for i in range(sy):
      imag_part = y[i]
      for j in range(sx):
          zs[(i*sx)+j] = complex(x[j], imag_part)

  print("Length of x:", sx)
  print("Total elements:", sx*sy)

  cdef int [::1] output = calculate_z(max_iterations,(sy*sx), zs)

  print("output= ", _sum(sx*sy,output))

  # This sum is expected for a 1000^2 grid with 300 iterations.
  # It catches minor errors we might introduce when we're
  # working on a fixed set of inputs.
  # assert sum(output) == 33219980

@cython.boundscheck(False)
@cython.wraparound(False)
cdef int [::1] calculate_z(int maxiter,int width, double complex [::1] zs):
  """Calculate output list using Julia update rule"""
  cdef int [::1] output = np.zeros([width],dtype=np.int32)
  cdef int i,n=0
  cdef double real, imag, r2,i2
  cdef double complex z
  for i in range(len(zs)):
    n = 0
    z = zs[i]
    real = z.real
    imag = z.imag
    r2 = real*real
    i2 = imag*imag
    while i2 + r2 <= 4 and n < maxiter:
      imag = 2*real*imag + c_imag
      real = r2 - i2 + c_real
      r2 = real*real
      i2 = imag*imag
      n += 1
    output[i] = n
  return output

def run (desired_width=1000, max_iterations=300):
  calc_cython(desired_width, max_iterations)