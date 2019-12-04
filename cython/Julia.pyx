import cython
import numpy as np
# area of complex space to investigate
cdef float x1 = -1.8;
cdef float x2 = 1.8;
cdef float y1 = -1.8;
cdef float y2 = 1.8;
cdef float c_real = -0.62772;
cdef float c_imag = -0.42193;

cdef calc_cython(desired_width, max_iterations):
  """Create a list of complex coordinates (zs) and complex parameters (cs), build Julia set, and display"""

  cdef float x_step = (float(x2 - x1) / float(desired_width))
  cdef float y_step = (float(y1 - y2) / float(desired_width))

  cdef float [::1] x = np.empty((desired_width+1),dtype=np.float32)
  cdef float [::1] y = np.empty((desired_width+1),dtype=np.float32)

  cdef sx=0, sy=0

  print(len(x))

  cdef float ycoord = y2
  while ycoord > y1:
    y[sy] = ycoord
    ycoord += y_step
    sy+=1
  

  cdef float xcoord = x1
  while xcoord < x2:
    x[sx] = xcoord
    xcoord += x_step
    sx+=1

  # Build a list of coordinates and the initial condition for each cell.
  # Note that our initial condition is a constant and could easily be removed;
  # we use it to simulate a real-world scenario with several inputs to
  # our function.

  # cdef complex [::1] zs = np.array([],dtype=np.complex)
  # cdef complex [::1] cs = np.array([],dtype=np.complex)
  zs = []
  cs = []
  cdef int _y=0, _x=0

  # Todo: cythonize these loops
  for ycoord in y:
    for xcoord in x:
      zs.append(complex(xcoord, ycoord))
      cs.append(complex(c_real, c_imag))

  # print("Length of x:", len(x))
  print("Total elements:", len(zs))

  # Todo: Esta comanda da error
  cdef int [::1] output = calculate_z(max_iterations, zs, cs)

  print("output= ", sum(output))

  # This sum is expected for a 1000^2 grid with 300 iterations.
  # It catches minor errors we might introduce when we're
  # working on a fixed set of inputs.
  # assert sum(output) == 33219980

cdef int [::1] calculate_z(int maxiter, complex [::1] zs, complex [::1] cs):
  """Calculate output list using Julia update rule"""
  cdef int [::1] output = np.zeros([len(zs)],dtype=np.int)
  cdef int i,n=0
  cdef complex z,c
  for i in range(len(zs)):
    n = 0
    z = zs[i]
    c = cs[i]
    while abs(z) < 2 and n < maxiter:
      z = z * z + c
      n += 1
    # print("n= ", n)
    output[i] = n
  return output

def run (desired_width=1000, max_iterations=1):
  calc_cython(desired_width, max_iterations)

