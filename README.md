<p align="center"><img src="https://user-images.githubusercontent.com/7409436/70395873-af825600-1a03-11ea-9ffc-a49d5f766bac.png" width="400"></p>

# An analysis of Julia Set sequence algorithm
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



# Análisis del algoritmo Julia-set


**Daniel Parra Martin**

**Mohsin Riaz**


## Índice

- Análisis del algoritmo
    - Descripción del algoritmo y análisis funcional
    - Análisis computacional
    - Accesos a Memoria
    - Analisis dependencia de datos
    - Posibles cuellos de botella
- Optimizaciones
    - Python
    - Cython
    - Python a Cython 
    - C 
    - Análisis: cabs vs abs_manual 
    - Análisis: Bucle principal 
    - Análisis: Maxiter 
- Paralelización
    - OpenMP 
    - OpenACC 
    - Gráficas 
- Conclusiones 
- Apéndice
    - Versión inicial (Python) 
    - Segunda versión (Cython) 
    - Tercera versión (C) 


## Análisis del algoritmo

El algoritmo de Julia es una secuencia que genera una imagen compleja a través de
inputs explícitos, este lo que hace es calculará cada píxel con un bucle y cada
iteración comprueba si el valor de la coordenada se va al infinito o tiene alguna
atracción.

Los que tienen pocas iteraciones se hacen más oscuros mientras que los que tienen
un número alto de iteraciones se colorean blanco. Por lo tanto las regiones blancas
son más complejas de generar.

### Descripción del algoritmo y análisis funcional

Definimos un conjunto de coordenadas ​_z_ que comprobamos y calculamos de la
siguiente forma:

## f(z)=z*z+c

Iteramos la función mientras comprobamos la condición de escape con la función
_abs()_ comprueba el valor absoluto de un número, en este caso de un número
complejo. Si se cumple la condición de salida, cortamos y contamos el número de
veces que hemos operado con esa coordenada. Si nunca se cumple la condición de
salida acabamos en un máximo de iteraciones que en nuestro caso será 300.

Después para un representación visual transformamos los valores de​ _z_ almacenados en la matriz de salida a pixel coloreados.

Ahora vamos a centrarnos más en profundidad en su análisis con el código C de
este algoritmo.

En primer lugar definimos las variables del área que vamos a analizar. En segundo
lugar se crea la lista de coordenadas  _z_ ​’s con la función  _cal_pure_c()_ ​, se hacen dos bucles para calcular los tamaños de la matriz con la que vamos a ejecutar en programa.

```C
double​ ycoord = Y2;
double​ xcoord = X1;
while​ (ycoord > Y1)
{
    y​[size_y] = ycoord;
    ycoord += y_step;
    size_y++;
}
while​ (xcoord < X2)
{
    ​x​[size_x] = xcoord;
    xcoord += x_step;
    size_x++;
}
```

Seguidamente se crea una lista de coordenadas y una condición inicial que es una constante que utilizamos para generar una simulación realista.
```C
double​ complex imag_part;
for​ (​int​ i = ​0​; i < size_y; i++)
{
    imag_part = ​y​[i] * I;
    ​for​ (​int​ j = ​0​; j < size_x; j++)
    {
        ​zs​[(i * size_x) + j] = ​x​[j] + imag_part;
    }
};
```
En segundo lugar con la lista de coordenadas que hemos creado(​ _zs_ ​), calculamos
una lista que cumpla la ley de Julia con la función ​ _calculate_z()_​. Para ello hacemos un bucle que cumpla las condiciones de escape o que llegue al máximo de
iteraciones añadido como argumento. Este es el bucle principal del problema.
```C
while​ (​cabs​(z) <= ​2.0​ && n < maxiter)
{
    z = z * z + c;
    n++;
};
```
En la versión C del código, se ha utilizado la librería​ _complex.h_ para operar sobre números complejos. para calcular el valor absoluto de un número complejo
usaremos la siguiente función de la librería ​ _complex.h_ ​.
```C
double​ ​cabs​(​double​ complex ​z​);
```
Por último, usando la función ​ _sum()_ ​, se calcula la suma total de la matriz de salidacon los valores de ​ _z_ para verificar que la ejecución se ha realizado de maneracorrecta. Finalmente, si hemos activado el parámetro de generar la imagen se
ejecuta la función ​ _generate_image()_ ​.

El tamaño del problema en este algoritmo es de X = 10000, Y = 10000  los cuales
definen el tamaño de la matriz del programa y max_iter =  300  que define el número
de veces que se va a calcular la z como máximo en el bucle principal de este.


#### Análisis computacional

En cuanto el análisis computacional del algoritmo lo podemos entender a partir de
su complejidad algoritmica y su complejidad espacial.

En primer lugar la complejidad algorítmica de este programa lo podemos aproximar
teniendo en cuenta el bucle principal o en el caso el doble bucle que hemos visto
anteriormente en la descripción del algoritmo y análisis funcional.
```C
for​(​int​ i = ​0​; i < width; i++){
    n = ​0​;
    z = zs;
    ​while​ (​cabs​(z) <= ​2.0​ && n < maxiter)
    {
        z = z * z + c;
        n++;
    };
}
```
Como hemos visto en el análisis funcional el tamaño del problema, de acuerdo con
ellos podemos que el número de instrucciones que se van a ejecutar en el peor de
los casos es de: 

X * Y * max_iter = 10000 * 10000 * 300 = 30G inst.

De acuerdo con el tamaño del problema también podemos extrapolar su
complejidad espacial, X * Y = 100M y como los datos son double 100M * 8B =
800MB.

### Accesos a Memoria

En cuanto a los accesos que se hacen a memoria durante la ejecución de este
programa se hacen de forma secuencial e independiente, esto quiere decir que
como la mayoría de accesos se hacen en el bucle principal del algoritmo, cada
cálculo de z se hace de forma seguida y los datos de un cálculo de z anterior no
repercute en el cálculo de z siguiente, como hemos comentado más en profundidad
en el apartado anterior se hacen 100M de accesos para leer doubles.


### Analisis dependencia de datos

La dependencia de datos principal de este algoritmo entonces la encontramos en el
bucle principal de este, en el cálculo de la z ( multiplicación de complejos + suma
de complejos) como vemos en la figura siguiente.

### Posibles cuellos de botella

En cuanto a los posibles cuellos de botellas que detectamos en el código se
encuentran la dependencia de datos comentada anteriormente y el cálculo del valor
absoluto que se hace en cada iteración del bucle.


## Optimizaciones

En Este apartado veremos las diferentes técnicas y decisiones que se han tomado
para obtener el mejor rendimiento de la ejecución del programa. El código inicial del
problema está programado en el lenguaje ​ _python_ ​.

### Python

Al realizar una revisión inicial del código, se ha visto que hay varios bucles en el
programa y se ha asumido que la mayor parte del trabajo se realiza en el bucle que
calcula el valor de la ​ _z_ ​. Utilizando las herramientas como ​ _Perf_ y ​ _iPython_ se ha observado que efectivamente la función del cálculo de la ​ _z_ realiza el 90% del trabajo.

Durante el curso se ha visto a profundidad el funcionamiento del lenguaje ​ _python_ ​. Es un lenguaje que permite crear aplicaciones muy rápidamente, con muy poco
código y es muy eficiente en cuanto a tiempo de desarrollo pero, para aplicaciones
que requieren resultados a partir de operaciones repetitivas y computacionalmente
intensivas usar ​ _python_ ​ es unas de las peores decisiones que uno puede tomar.

Al ser un lenguaje interpretado es considerablemente más lento que los lenguajes
compilados. Entre otros factores, al no tener que especificar tipos de variables o
tamaños de los arreglos hace que estas comprobaciones se tengan que hacer
durante el ​ _runtime_ y eso hace que el programa y los bucles se ejecuten de manera
sub óptima.

Hemos ejecutado el programa inicial con varios tamaños de matriz para tener
tiempos de referencia para luego compararlos con las versiones optimizadas. A
continuación podemos observar la tabla con el tamaño de matriz y tiempos
obtenidos.

[ table ]


#### Cython

A partir de los resultados anteriores decidimos pasar el código directamente al
lenguaje ​ _C_ y empezar a analizarlo más a profundidad para crear versiones más
óptimas. De todos modos, antes, se adaptó el código a ​ _Cython_ ​, se sustituyeron los arreglos nativos de ​ _python_ con las de ​ _numpy_ ​, se utilizaron directivas de ​ _Cython_ para los bucles y se hicieron varias pruebas con diferentes tamaños de matrices.

Obtuvimos los siguientes resultados:
**Size Iterations Time (Python) Time (Cython)**
1000 ​^2 300 10,202s 0.173s
5000 ​^2 300 >100s 13.9s
10000 ​^2 300 n/a 55.4s
Figura 2. Tiempos de ejecución usando Python y Cython

Estos resultados se han obtenido sin realizar ningún tipo de optimización. Se puede
ver que con tamaño de  1000 ​^2 ​hay un ​ _speedup_ de ​ _x58_ ​. A Partir de estos resultados se puede ver la gran diferencia entre el código interpretado y código compilado.

Al analizar el bucle principal de código, se ha observado que hay una dependencia
de datos entre la variable ​ _z_ que no se puede eliminar ni reordenado el código ni
utilizando otras técnicas. A parte de esta observación, también se ha visto que el en
cada iteración se está calculando el valor absoluto del número complejo que tiene
la siguiente fórmula:

## | a + bi |=√ a


Para calcular el valor absoluto de un número complejo hace falta calcular dos
exponentes, una suma y una raíz cuadrada. Normalmente los exponentes y las
sumas no son demasiado costosos para el procesador pero no es el caso para el
cálculo de las raíces. Si observamos la condición de salida del bucle principal
vemos que el resultado del valor absoluto se está comparando con el número 2.
while​ ​abs​(z) <= ​2.0​ and n < maxiter:
Para optimizar el código, en lugar de utilizar la función ​ _abs()_ se reescribió el código
para cálculo del valor absoluto descomponiendo la operación, eliminando la raíz y
comparando el resultado con el número 4. A continuación se pueden ver las
modificaciones:


**Versión inicial Versión optimizada**
```python
for​ i in ​range​(​len​(zs)):
    n = ​ 0 
    ​z = zs[i]
    ​while​ ​abs​(z)<=​2.0​ and n<maxiter:
        ​z = z * z + c
        n += ​ 1 
    output[i] = n
return ​output

for​ i in ​range​(​len​(zs)):
    n = ​ 0 
    ​z = zs[i]
    real = z.real
    imag = z.imag
    r2 = real * real
    i2 = imag * imag
    ​while​ ​i1+r2 <= ​4.0​ and n<maxiter:
        ​imag = 2 * real * imag + c_imag
        ​real = r2 - i2 + c_real
        ​r2 = real * real
        ​i2 = imag * imag
        n += ​ 1 
    ​output[i] = n
return ​output
```
En la versión modificada tenemos  4  multiplicaciones,  4  operaciones INT y  5  MOV.
Como se ha mencionado anteriormente estas operaciones no son muy costosas y
dependiendo del pipelining del código y el throughput de los recursos del
compilador se pueden realizar múltiples operaciones en paralelo.
Al ejecutar la versión optimizada se han obtenido los siguientes resultados:
**Size Iterations Time (Cython) Time (Cython Optimized)**
1000 ​^2 300 0.173s 0.292s
5000 ​^2 300 13.9s 6.91s
10000 ​^2 300 55.4s 27.7s
Figura 3. Tiempos de ejecución Cython versión inicial vs. modificada
Con esta simple optimización se ha conseguido una mejora considerable, se ha
visto que aun teniendo más instrucciones, el código ha sido más rápido que el de la
versión inicial.
Si calculamos el speedup vemos que ahora el código va el doble de rápido en los
casos de la matriz de  5000 ​^2 y  10000 ​^2 ​. El rendimiento empeora con el tamaño de
1000 ​^2 ​ y esto es debido a que hay pocas iteraciones y más instrucciones.


A continuación podemos observar la gráfica con el speedup y la comparación entre
el tiempo de ejecución de las dos versiones:
Figura 4. Speedup entre Cython versión inicial vs modificada
Figura 5. Tiempos de ejecución: Cython versión inicial vs modificada


**Python a Cython**
Para pasar el codigo de ​ _python_ ​ a ​ _cython_ ​ se han seguido los siguientes pasos:
● Crear fichero .pyx a partir de .py
● Definir los tipos de las funciones y las variables.
● Crear vistas de memoria usando numpy.
● Usar las directivas del compilador de cython.
● Cytonizar el código.
● Compilar con ​ _gcc_ ​ y ​ _-Ofast._
El código resultante:

(^)
(^)
Se puede observar que el bucle principal del programa no está marcado con el color
amarillo, esto indica que el código se ha traducido a ​ _C_ ​ puro.


#### C

A Partir del código ​ _Cython_ ​, pasar a código es bastante sencillo ya que todas las
funciones y las variables tienen el tipo de datos ya definidos. a parte de los tipos,
las matrices tienen tamaños fijos y están definidos en forma consecutiva en la
memoria. El hecho de tener el vector de resultados de  1  dimensión hace que se
pueda aprovechar mejor la caché y que las lecturas sean más eficientes.
En nuestro caso, por la naturaleza del problema la caché nunca llega a ser el cuello
de botella ya que hay otros factores que determinan el tiempo mínimo del
programa. Estos factores se verán a profundidad más adelante. De todas manera,
asignar memoria consecutiva y acceder a la memoria de forma secuencial siempre
es beneficiosa y es considerada una buena práctica para la ingeniería de
rendimiento.
Una vez pasado el código al lenguaje C se ha ejecutado el programa con diferentes
tamaños de matriz e número de iteraciones. Se han obtenido los siguientes
resultados:
**Size Iterations Time (C)**
1000 ​^2 300 0.198s
5000 ​^2 300 3.684s
10000 ​^2 300 14.69s
Figura 6. Tiempos de ejecución: C versión inicial
En esta tabla fig.  6  se puede observar que el rendimiento del programa ha mejorado
bastante en comparación a la version en ​ _Cython_ ​. En la fig.  7  se puede ver el
speedup y la comparación entre los tiempos de ejecución.
**Size Iterations Time (C initial) Time (** ​ **_Cython_** ​ **best) Speedup**
1000 ​^2 300 0.198s 0.292s 1,
5000 ​^2 300 3.684s 6.91s 1,
10000 ​^2 300 14.69s 27.7s 1,
Figura 7. Comparación entre tiempos de Cython y C y el speedup
Estas mejoras son debido a que en la versión de ​ _Cython_ aún hay funciones como la
creación de matrices y llamadas a otras funciones/librerías que no se han traducido
al código C de manera eficiente, esto hace que usando ​ _Cython_ se ejecutan más
instrucciones y el intérprete aún tenga que hacer comprobaciones durante el
runtime.


En cambio en el lenguaje C, el programa completo se compila de manera eficiente y
sin código implícito. En la fig.  8  se puede ver que el tiempo de ejecución usando C
ha mejorado casi el doble, el speedup con matrices de tamaño  5000 ​^2 y  10000 ​^2 es
de ~1.
Figura 8. Gráfica: Comparación entre tiempos de Cython y C y el speedup
Tal como se hizo en el apartado de la optimización en ​ _Cython_ ​, modificaremos la
función ​ _abs()_ del código C para que la operación sea más rápida. Nuestra hipótesis
es que como en ​ _Cython_ hubo una mejora al modificar la operacion del absoluto, el
tiempo en C también tendrá mejoras en cuanto al rendimiento.
El código resultante es el siguiente:
for​ (​int​ i = ​0​; i < width; i++)
{
n = ​0​;
z = ​zs​[i];
zreal = ​creal​(z); zimag = ​cimag​(z);
zreal2 = zreal * zreal;
zimag2 = zimag * zimag;
​while​ (​cabs​(z) <= ​2.0​ && n < maxiter)
{
zimag = ​2​ * zimag * zreal + c_imag;
zreal = zreal2 - zimag2 + c_real;
zreal2 = zreal * zreal;
zimag2 = zimag * zimag;
n++;
};
​output​[i] = n;
}
_Nota: En el apartado C siempre se ha compilado con gcc/8.2.0 y -Ofast_


Se ha compilado el programa con el código anterior y se han obtenido los siguientes
resultados:
**Size Iterations Time (C initial) Time (C modified) Speedup**
1000 ​^2 300 0.198s 0.197s 1,
5000 ​^2 300 3.684s 4.118s 0,
10000 ​^2 300 14.69s 16.428s 0,
Figura 9. Comparación entre tiempos de C inicial y C modificado y el speedup
Si analizamos la tabla anterior vemos que el ​ _speedup_ ha empeorado con la versión
modificada, lo contrario de lo que teníamos pensado. Entonces antes de
profundizar en el profiling del código C decidimos analizar y hacer una comparación
del rendimiento entre la función ​ _abs()_ ​de la librería complex.h y el calcula manual
del valor absoluto.
**Análisis: cabs vs absmanual**
Creamos dos fichero, cada uno con un bucle for que calcula el valor absoluto de
valores que lee desde un vector. un fichero hace el cálculo usando la función ​ _cabs()_
y el otro hace el cálculo del valor absoluto de manera manual. A continuación
podemos ver el código correspondiente a cada fichero.
**cabs() abs() Manual**
double​ *​complex_lib_abs​(​int​ ​size​,
double​ complex *​zs​)
{
​int​ double​ *output = (​double​ *)
calloc​(size,​sizeof​(​double​));
​for​ (​int​ i = ​0​; i < size; i++)
​output​[i] = ​cabs​(​zs​[i]);
​return​ output;
};
double​ *​manual_abs​(​int​ ​size​, ​double
complex *​zs​)
{
​double​ zreal, zimag, zreal2, zimag2;
​double​ complex z;
​double ​c = c_real + c_imag * I;
​int​ double​ *output = (​double​ *)
calloc​(size,​sizeof​(​double​));
​for​ (​int​ i = ​0​; i < size; i++)
{
z = ​zs​[i];
zreal = ​creal​(z);
zimag = ​cimag​(z);
​output​[i] = (zreal*zreal) +
(zimag*zimag);
}
​return​ output;
};


Después de ejecutar los dos programas se ha conseguido los siguientes resultados:
**Size cabs() abs() manual speedup**
15000 ​^2 4.286s 3.587s 1.
20000 ​^2 23.636s 19.244s 1.
30000 ​^2 250.693s 211.333s 1.
Figura 10. Comparación entre cabs() y abs() manual y el speedup
Analizando la tabla anterior vemos que el código con la función ​ _abs()_ ​manual es
más rápido que el ​ _cabs()_ ​. Esto crea aún más confusión, el manual debería de ir
lento, por ello analizamos el código máquina de las dos funciones.
**cabs() abs() Manual**
​ 0,28 │​30: ┌─→movsd (%rsi),%xmm
4,28 │ │ movsd 0x8(%rsi),%xmm
0,08 │ │ add $0x10,%rsi
│ │ add $0x4,%rcx
│ │ mulsd %xmm0,%xmm
│ │ mulsd %xmm1,%xmm
│ │ addsd %xmm1,%xmm
**66,67 │ │ sqrtsd %xmm0,%xmm**
24,37 │ │ cvttsd2si %xmm0,%edx
4,32 │ │ mov %edx,-0x4(%rcx)
│ ├──cmp %rsi,%rdi
│ └──jne ​ 30
28,47 │​30: ┌─→movsd 0x8(%rsi),%xmm
15,84 │ │ movsd (%rsi),%xmm
0,11 │ │ add $0x10,%rsi
│ │ add $0x4,%rcx
5,28 │ │ mulsd %xmm0,%xmm
14,47 │ │ mulsd %xmm1,%xmm
3,96 │ │ addsd %xmm1,%xmm
​13,26 │ │ cvttsd2si %xmm0,%edx
18,37 │ │ mov %edx,-0x4(%rcx)
0,23 │ ├──cmp %rsi,%rdi
│ └──jne ​ 30
En el código máquina se puede ver que la versión manual va más rapido por que no
se calcula la raíz cuadrada. Entonces nuestra hipótesis inicial mencionada
anteriormente no era incorrecta del todo pero, aún no sabemos porque el bucle
principal del problema tarda más si usamos la versión manual para calcular el valor
absoluto.


**Análisis: Bucle principal**
Para ver lo que está pasado cuando se ejecuta el programa, perfilamos la ejecución
usando la herramienta ​ _perf_ ​. A continuación vemos el código máquina
correspondiente a las dos versiones del bucle principal del programa.
**C Initial C Modified**
0,09 │​2a0: │ cmp %eax,%r12d
│ │↓ jle 2e
0,01 │ │ mulsd %xmm2,%xmm
│ │ addsd %xmm5,%xmm
6,81 │ │ add $0x1,%eax
0,19 │ │ movapd %xmm1,%xmm
0,00 │ │ subsd %xmm3,%xmm
0,00 │ │ addsd %xmm0,%xmm
6,94 │ │ addsd %xmm6,%xmm
0,03 │2c0: │ movapd %xmm0,%xmm
│ │ movapd %xmm2,%xmm
3,08 │ │ mulsd %xmm0,%xmm
16,42 │ │ mulsd %xmm2,%xmm
1,69 │ │ movapd %xmm3,%xmm
20,59 │ │ addsd %xmm1,%xmm
20,65 │ │ comisd 0x830(%rip),%xmm
21,04 │ │↑ jbe ​2a
0,37 │2e2: │ add $0x10,%rbp
​0,03 │​2c8: │ mulsd %xmm2,%xmm
0,03 │ │ movapd %xmm5,%xmm
0,14 │ │ add $0x1,%eax
5,89 │ │ subsd %xmm1,%xmm
0,01 │ │ movapd %xmm7,%xmm
0,02 │ │ addsd %xmm0,%xmm
0,15 │ │ addsd %xmm3,%xmm
5,87 │ │ movapd %xmm0,%xmm
0,00 │ │ movapd %xmm1,%xmm
0,06 │ │ movapd %xmm1,%xmm
0,11 │ │ addsd %xmm6,%xmm
16,31 │ │ mulsd %xmm1,%xmm
0,00 │ │ movapd %xmm2,%xmm
7,13 │ │ mulsd %xmm2,%xmm
2,55 │ │ movapd %xmm3,%xmm
17,94 │ │ addsd %xmm1,%xmm
15,50 │ │ comisd 0x831(%rip),%xmm
19,17 │ │ setbe %cl
0,05 │ │ cmp %eax,%r12d
│ │ setg %dl
5,94 │ │ test %dl,%cl
0,04 │ │↑ jne 2c
A primera vista, vemos que la versión inicial tiene menos instrucciones que la
versión modificada pero, lo más extraño es que en el código de la versión inicial no
se está realizando ninguna operación que calcula la raíz cuadrada.
Si miramos con atención el código inicial, vemos que se leen los valores desde la
memoria, se hace la multiplicación, se suman los valores y finalmente se compara
el resultado con un valor almacenado en memoria en la dirección 0x830(%rip).
Inicialmente la condición de salida es z >= 2.0 pero, si eliminamos la raíz, la
condición pasa a ser z >= 4.0.
Entonces, esto significa que en la versión inicial, el compilador automáticamente
está reemplazando el 2.0 por un 4.0 y optimizando el cálculo del valor absoluto
mucho mejor que la versión modificada. Llegamos a la conclusión de que el
compilador es mucho más inteligente de lo que pensamos y dependiendo de la
versión y nivel de optimización, aun teniendo un código “normal” sin optimizar,
podemos obtener una gran eficiencia en cuanto al rendimiento del programa.


Analizando los resultados obtenidos usando la herramienta ​ _perf_ se ha visto que se
ejecutan más instrucciones en la versión modificada. El IPC también es alto en la
versión modificada pero esto es debido a que en total se ejecutan más
instrucciones.
A continuación vemos los resultados tablas y gráficas con los resultados obtenidos:
**Size Iterations IPC (C initial) IPC (C modified)**
1000 ​^2 300 1.16 1.
5000 ​^2 300 1.19 1.
10000 ​^2 300 1.19 1.
Figura 11. IPC C inicial vs C modificado
**Size Iterations Inst. (C initial) Inst. (C modified)**
1000 ​^2 300 591M 759M
5000 ​^2 300 14,7G 18,9G
10000 ​^2 300 58,8G 75,6G
Figura 12. Instructions C inicial vs C modificado
Figura 13. Gráfica: Instructions C inicial vs C modificado


**Análisis: Maxiter**
En los apartados anteriores nos hemos enfocado en la primera condición de salida
del bucle principal. En es apartado veremos la segunda condición de salida que es:
n <= maxiter
Esta condición hace que el bucle no siga calculando el valor absoluto de manera
indefinida. Es un límite que indica las el número máximo de iteraciones por valor
que se pueden hacer.
A continuación se pueden ver los resultados de las ejecuciones con diferentes
tamaños de matriz e iteraciones.
**Size Iterations Time (C)**
1000 ​^2 5 0.018s
5000 ​^2 5 0.284s
10000 ​^2 5 1.086s
1000 ​^2 100 0.08s
5000 ​^2 100 1.506s
10000 ​^2 100 5.942s
1000 ​^2 300 0.198s
5000 ​^2 300 3.684s
10000 ​^2 300 14.69s
Figura 14. Comparación entre tiempos de Cython y C y el speedup
En la tabla anterior se puede apreciar que esta vez se ha ejecutado el programa con
diferentes números de iteraciones y vemos que cuanto menos iteraciones, más
rápido es el programa. La naturaleza no determinista del problema hace que esté
limitado por este factor.
A continuación se puede ver una imagen que se ha generado a partir de la matriz de
salida. En esta imagen cada color indica el número de iteración y va en orden
ascendente de fuera hacia dentro. los bordes de la forma de color negro marcan el
color correspondiente al máximo número de iteración (300).


Figura 15. representación gráfica de la matriz z
La lectura de la matriz se hace de izquierda a derecha y de arriba a abajo. Eso
significa que van a haber elementos de la matriz que harán mucho más trabajo que
otros y habrá un gran desbalance de carga. Tampoco hemos conseguido de alguna
manera predecir los valores sobre los que se tirará más.
Lo que se podría hacer para balancear mejor la carga es poner ​ _thresholds_ que
permitan parar y guardar el valor para calcularlo más tarde. Eso permitirá calcular
en conjunto los valores de mayor carga al final. No se ha llegado a implementar esta
funcionalidad por su complejidad.
Otra técnica para mejorar el rendimiento sería operar sobre la primera mitad de la
matriz, realizar una copia y rotarla 180° y finalmente agregarla a la matriz de salida.
Esto reduce el tiempo por la mitad pero esta técnica solo funciona si las
coordenadas de origen están en la mitad de la matriz.


Figura 16. Carga de trabajo por iteración
Para analizar mejor la carga que se lleva cada número de iteración se ha calculado
el número de veces que se incrementa la n teniendo el mismo número de iteración.
Si vemos la Fig.16 vemos que el 90% del trabajo lo realizan las iteraciones que
llegan al número 300.
En cambio de observamos la imagen de la Fig.15 veremos que los colores que
ocupan más espacio son el 0, 1,  2  y el 3. Esto significa que estas iteraciones aunque
sean muchas, no pesan casi nada cuando se comparan con el trabajo que se llevan
las iteraciones que llegan al número 300.
A continuación podemos ver la gráfica correspondiente al tiempo de ejecución de la
versión inicial y modificada.
Figura 17. benchmark: C initial vs C modified


### Paralelización

#### OpenMP

En este problema tenemos la ventaja de que cada elemento de la matriz es
independiente de los demás y eso permite que se pueda paralelizar el código. Hay
varias funciones de este programa que se pueden paralelizar por ejemplo la función
para crear la matriz inicial, función para realizar la suma y finalmente la función que
calcula los valores de la z.
Ya de antemano, se conoce que el problema principal a la hora de paralelizar el
código será el balanceo de carga pero gracias a la librería de OpenMP hay directivas
que permiten la planificación de trabajos de tal manera que todos los threads
ejecutes la misma cantidad de trabajo y que ninguna se quede en sin tareas a
ejecutar.
Haría falta destacar que la máquina aolin-login dispone de un procesador con  4 
núcleos, es decir 4 threads físicos.
A continuación podemos ver las directivas que se han utilizado para paralelizar el
código:
#pragma​ ​omp​ ​parallel​ ​num_threads​(​THREADS​)
{
​double​ complex imag_part;
#pragma​ ​omp​ ​for
​for​ (​int​ i = ​0​; i < size_y; i++)
{
imag_part = ​y​[i] * I;
​for​ (​int​ j = ​0​; j < size_x; j++)
{
​zs​[(i * size_x) + j] = ​x​[j] + imag_part;
}
}
}
Figura 18. Código para generar la matriz inicial
#pragma​ ​omp​ ​parallel​ ​for​ ​reduction​(+ : ​output_sum​)
​for​ (​int​ i = ​0​; i < width; i++)
output_sum += ​output_array​[i];
​return​ output_sum;
};
Figura 19. Código para obtener la suma total


int​ *​calculate_z​(​int​ ​maxiter​, ​int​ ​width​, ​double​ complex *​zs​)
{
​int​ *output = (​int​ *)​calloc​(width, ​sizeof​(​int​));
​double​ complex z, c = c_real + c_imag * I;
#pragma​ ​omp​ ​parallel​ ​num_threads​(​THREADS​)
{
​int​ n;
#pragma​ ​omp​ ​for schedule(dynamic,100)
​for​ (​int​ i = ​0​; i < width; i++)
{
n = ​0​;
z = ​zs​[i];
​while​ (​cabs​(z) <= ​2.0​ && n < maxiter)
{
z = (z * z) + c;
n += ​1​;
};
​output​[i] = n;
}
}
​return​ output;
};
Figura 20. Código para calcular el vector de salida
A continuación podemos ver los resultados de la ejecución en paralelo:
**Threads Time Speed Up CPUs used cache-misses**
1  14,77s 1,00 1  
4  4,32s 3,42 3,951 50,169 %
Figura 21. Resultados de la ejecución en paralelo OpenMP
Analizando la tabla anterior podemos ver que se está utilizando el máximo de la
cpu. el tiempo total de ejecución es de 4.32s que es 3,42 veces mejor que la
versión sin paralelizar.
Para manejar el balanceo de carga se ha utilizado la siguiente directiva:
#pragma​ ​omp​ ​for schedule(dynamic,100)


En la imagen siguiente se puede ver el funcionamiento del scheduling dinámico en
caso de tener 4 threads.
Como se puede apreciar aquí, en el timeline de la ejecución no hay tiempos de
esperas entre la ejecución de cada tarea. En cuanto un thread acaba una tarea, el
scheduler ya tiene más trabajo preparado para ser ejecutado. Normalmente, el
scheduler asigna trabajo en función de orden de llegada y usa el algoritmo FIFO
(First come first serve).
El segundo parámetro del la directiva es el ​ _grainsize_ ​, que se podría denominar
bloques de elementos del vector. Esto hace que se divide el vector, en nuestro caso
en bloques de 100 elementos y se van ejecutando por las tareas.
Hemos probado con diferentes tamaños de ​ _grainsize_ ​mayores a  100  pero hemos
obtenido el resultado.


#### OpenACC

Como bien hemos visto en las optimizaciones anteriores la forma más idónea de
optimizar este código no es a través de modificaciones en el código en C ya que no
hace cambios notables en la ejecución y por lo tanto no es lo óptimo para él y
después de añadir las directivas de OpenMP hemos visto cambios más notorios
pero que suponemos que con OpenACC podemos todavía mejorar.
En este apartado veremos las optimizaciones propuestas con directivas OpenACC y
si estas tendrán un impacto mayor que las anteriores como pensamos.
En primer lugar pensamos en optimizar el bucle que mas se ejecuta en el programa
y que se lleva la mayoría del cómputo que es el doble bucle for y while que calcula
la fórmula en la que se basa el algoritmo (Figura 22), para ello tuvimos que hacer
una nueva función que nos calculase el valor absoluto de z (Figura 23) ya que la
función abs() que en C se ejecutaba correctamente, con las directivas OpenACC no
funciona cosa que tuvimos que modificar.
Para esta optimización añadimos las directivas “​#pragma acc data
copyin(zs[:width])​” para copiar en la GPU, que ejecuta el programa, desde el
disco para así tener el array zs ya en el host junto a la directiva “​#pragma acc
data copyout(output[:width])​” para modificar el array output desde la GPU
hasta el disco y por último la directiva “​#pragma acc kernels​” la cual propone a
la GPU ejecutar de forma paralela el bucle. (Figura 24)
Figura 22.


Figura 23.
Figura 24.
En el apartado de las gráficas comprobaremos si estas modificaciones han tenido
un gran impacto en la mejora de la ejecución del código.
En segundo lugar, pensamos en los otros bucles que se ejecutan en el programa
para paralelizarlos y observar el impacto que tenían en la mejora del rendimiento de
este. Por eso pensamos en el bucle que calcula la suma del output (Figura 25) y en
el doble for que crea la matriz (Figura 26).
Para el primer bucle añadimos la directiva “​#pragma acc kernels​” para
paralelizar la ejecución (Figura 27) y para el doble bucle for la directiva “​#pragma
acc data copyout(zs[:size_y*size_x])​” para guardar en el disco el cálculo
del array zs desde la GPU y las directivas “#pragma acc parallel” junto a sus
respectivos “​pragma acc loop independent​” antes de los dos for’s para
paralelizar los dos bucles. (Figura 28)


Figura 25.
Figura 26.
Figura 27.
Figura 28.


**Gráficas**
En este apartado comentaremos los resultados obtenidos de las optimizaciones de
OpenACC respecto al código principal en C.
Figura 29. C vs ACC
En la Figura  29  comprobamos que la primera optimización de OpenACC mejora
notablemente la ejecución de la primera versión del programa en C y que mientras
más aumentamos el valor de la constante T más notable es la mejora en el
rendimiento.


Figura 30. C vs ACC1 vs ACC2
En esta gráfica vemos la comparativa de los tiempos de ejecución del programa en
C con la primera optimización en OpenACC y estas con la segunda versión que
como vemos en los tiempos de ejecución todavía ha mejorado más estos aunque
llega un punto que deja de mejorar a causa de las copias de Host a Device de las
ejecuciones y las copias de Device a Host.


### Conclusiones

Durante la realización del análisis de un algoritmo tan interesante como es el caso
del de Julia, hemos ido extrayendo varias conclusiones acerca de él que
seguidamente comentaremos.
Este es un algoritmo cuya optimización no se basa en modificaciones en el propio
código como hemos estado viendo en anteriores algoritmos, si no que para mejorar
de forma más eficiente este nos debemos centrar en la utilización de las directivas
OpenMP y OpenACC, en cuanto a OpenMP se reflejaba un speedup que depende del
número de threads y el balanceo de carga, en el caso de  4  threads producía una
mejora de 3,42x, en cuanto a OpenACC el speedup era todavía mayor a medida que
aumentabamos el número de threads, llegando a speedups de x23.
Relacionado con el primer punto, llegábamos a entender algo muy importante por
no decir lo más importante en la optimización de este código: el balanceo de carga.
Cuando empezamos a añadir sobretodo directivas OpenACC veíamos un gran
cambio en la ejecución del programa y esto era debido a que en el programa
paralelizado había threads que hacían mas cómputo que otros ya que a causa del
parámetro max_iter, había algunos que podían llegar a 300x iteraciones más que
otros y esto era algo que nosotros corregimos añadiendo una gran cantidad de
threads que hacía que la carga se balanceara de una forma muy brusca ya que, en
la ejecución es importante las directivas de schedule para controlar este balanceo
de carga entre los diferentes threads.


### Apéndice

#### Versión inicial (Python)

# area of complex space to investigate
x1, x2, y1, y2 = -​1.8​, ​1.8​, -​1.8​, ​1.8
c_real, c_imag = -​0.62772​, -​.42193
def​ ​calc_pure_python​(​desired_width​, ​max_iterations​):
​"""Create a list of complex coordinates (zs) and complex parameters (cs), build
Julia set, and display"""
x_step = (​float​(x2 - x1) / ​float​(desired_width))
y_step = (​float​(y1 - y2) / ​float​(desired_width))
x = []
y = []
ycoord = y2
​while​ ycoord > y1:
y.append(ycoord)
ycoord += y_step
xcoord = x1
​while​ xcoord < x2:
x.append(xcoord)
xcoord += x_step
​# Build a list of coordinates and the initial condition for each cell.
​# Note that our initial condition is a constant and could easily be removed;
​# we use it to simulate a real-world scenario with several inputs to
​# our function.
zs = []
​for​ ycoord ​in​ y:
​for​ xcoord ​in​ x:
zs.append(​complex​(xcoord, ycoord))
​# print("Length of x:", len(x))
​# print("Total elements:", len(zs))
output = calculate_z(max_iterations, zs)
​print​(​"output= "​, ​sum​(output))
​# This sum is expected for a 1000^2 grid with 300 iterations.
​# It catches minor errors we might introduce when we're
​# working on a fixed set of inputs.
​# assert sum(output) == 33219980
def​ ​calculate_z​(​maxiter​, ​zs​):
​"""Calculate output list using Julia update rule"""
c = ​complex​(c_real, c_imag)
output = [​0​] * ​len​(zs)
​for​ i ​in​ ​range​(​len​(zs)):
n = ​ 0 
z = zs[i]
​while​ ​abs​(z) < ​2​ ​and​ n < maxiter:
z = z * z + c
n += ​ 1 
output[i] = n
​return​ output
if​ ​__name__​ == ​"__main__"​:
​# Calculate the Julia set using a pure Python solution
calc_pure_python(​desired_width​=​5000​, ​max_iterations​=​300​)


#### Segunda versión (Cython)

import​ cython
import​ numpy ​as​ np
# area of complex space to investigate
cdef​ ​double​ x1 = -​1.8​;
cdef​ ​double​ x2 = ​1.8​;
cdef​ ​double​ y1 = -​1.8​;
cdef​ ​double​ y2 = ​1.8​;
cdef​ ​double​ c_real = -​0.62772​;
cdef​ ​double​ c_imag = -​0.42193​;
@cython.boundscheck​(​False​)
@cython.wraparound​(​False​)
cdef​ ​int​ _sum(​int​ width, ​int​ [::​1​] output):
​cdef​ ​int​ i,output_sum = ​ 0 
​for​ i in ​range​(width):
output_sum += output[i]
​return​ output_sum
@cython.boundscheck​(​False​)
@cython.wraparound​(​False​)
cdef​ ​calc_cython​(​int​ desired_width, ​int​ max_iterations):
​"""Create a list of complex coordinates (zs) and complex parameters (cs), build
Julia set, and display"""
​cdef​ ​double​ x_step = (​float​(x2 - x1) / ​float​(desired_width))
​cdef​ ​double​ y_step = (​float​(y1 - y2) / ​float​(desired_width))
​cdef​ ​double​ [::​1​] x = np.empty((desired_width+​1​),​dtype​=np.double)
​cdef​ ​double​ [::​1​] y = np.empty((desired_width+​1​),​dtype​=np.double)
​cdef​ ​int​ sx=​0​, sy=​ 0 
​cdef​ ​double​ ycoord = y2
​while​ ycoord > y1:
y[sy] = ycoord
ycoord += y_step
sy+=​ 1 
​cdef​ ​double​ xcoord = x1
​while​ xcoord < x2:
x[sx] = xcoord
xcoord += x_step
sx+=​ 1 
​# Build a list of coordinates and the initial condition for each cell.
​# Note that our initial condition is a constant and could easily be removed;
​# we use it to simulate a real-world scenario with several inputs to
​# our function.
​cdef​ ​double​ ​complex​ [::​1​] zs = np.empty((sy*sx),​dtype​=np.complex)
​cdef​ ​double​ imag_part = ​ 0 
​cdef​ ​int​ i,j
​for​ i in ​range​(sy):
imag_part = y[i]
​for​ j in ​range​(sx):
zs[(i*sx)+j] = ​complex​(x[j], imag_part)


​print​(​"Length of x:"​, sx)
​print​(​"Total elements:"​, sx*sy)
​cdef​ ​int​ [::​1​] output = calculate_z(max_iterations,(sy*sx), zs)
​print​(​"output= "​, _sum(sx*sy,output))
​# This sum is expected for a 1000^2 grid with 300 iterations.
​# It catches minor errors we might introduce when we're
​# working on a fixed set of inputs.
​# assert sum(output) == 33219980
@cython.boundscheck​(​False​)
@cython.wraparound​(​False​)
cdef​ ​int​ [::​1​] calculate_z(​int​ maxiter,​int​ width, ​double​ ​complex​ [::​1​] zs):
​"""Calculate output list using Julia update rule"""
​cdef​ ​int​ [::​1​] output = np.zeros([width],​dtype​=np.int32)
​cdef​ ​int​ i,n=​ 0 
​cdef​ ​double​ ​complex​ z
​for​ i in ​range​(​len​(zs)):
n = ​ 0 
z = zs[i]
​while​ ​abs​(z) <= ​2​ and n < maxiter:
z = z * z + c
n += ​ 1 
output[i] = n
​return​ output
def​ ​run​ (​desired_width​=​1000​, ​max_iterations​=​300​):
calc_cython(desired_width, max_iterations)


#### Tercera versión (C)

_Esta es a version con la que se ha trabajado en este proyecto_
#include​ ​<math.h>
#include​ ​<string.h>
#include​ ​<stdio.h>
#include​ ​<stdlib.h>
#include​ ​<complex.h>
#include​ ​<assert.h>
//area of complex space to investigate
double​ X1 = -​1.8​; ​double​ X2 = ​1.8​;
double​ Y1 = -​1.8​; ​double​ Y2 = ​1.8​;
double​ c_real = -​0.62772​; ​double​ c_imag = -​0.42193​;
int​ size_y = ​0​;
int​ size_x = ​0​;
int​ *​calculate_z​(​int​ ​maxiter​, ​int​ ​size_x​, ​int​ ​size_y​, ​double​ complex *​zs​)
{
​//Calculate output list using Julia update rule
​int​ n;
​double​ complex z, c = c_real + c_imag * I;
​int​ *output = (​int​ *)​malloc​((size_x * size_y) * ​sizeof​(​int​));
​for​ (​int​ i = ​0​; i < size_x * size_y; i++)
{
n = ​0​;
z = ​zs​[i];
​while​ (​cabs​(z) <= ​2.0​ && n < maxiter)
{
z = z * z + c;
n++;
};
​output​[i] = n;
}
​return​ output;
};
int​ ​sum​(​int​ *​output_array​, ​int​ ​width​)
{
​int​ output_sum = ​0​;
​for​ (​int​ i = ​0​; i < width; i++)
output_sum += ​output_array​[i];
​return​ output_sum;
};
int​ *​calc_pure_c​(​int​ ​desired_width​, ​int​ ​max_iterations​)
{
​// Create a list of complex coordinates (zs), build Julia set, and display
​double​ x_step = (X2 - X1) / (​double​)desired_width;
​double​ y_step = (Y1 - Y2) / (​double​)desired_width;
​double​ *x = (​double​ *)​malloc​((desired_width + ​1​) * ​sizeof​(​double​));
​double​ *y = (​double​ *)​malloc​((desired_width + ​1​) * ​sizeof​(​double​));
​int​ *output;
​double​ ycoord = Y2;
​double​ xcoord = X1;


while​ (ycoord > Y1)
{
​y​[size_y] = ycoord;
ycoord += y_step;
size_y++;
}
​while​ (xcoord < X2)
{
​x​[size_x] = xcoord;
xcoord += x_step;
size_x++;
}
​// una vez calculado los tamaños de las matrices, creamos la matriz
​// para las coor. en plano complejo
​double complex *zs = (​double complex *)​malloc​((size_x * size_y) * ​sizeof​(​double
complex));
​// Build a list of coordinates and the initial condition for each cell.
​double​ complex imag_part;
​for​ (​int​ i = ​0​; i < size_y; i++)
{
imag_part = ​y​[i] * I;
​for​ (​int​ j = ​0​; j < size_x; j++)
​zs​[(i * size_x) + j] = ​x​[j] + imag_part;
}
​printf​(​"Length of x:%d​\n​"​, size_x);
​printf​(​"Total elements:%d​\n​"​, size_y * size_x);
​// calcular la Z
output = ​calculate_z​(max_iterations, size_y, size_x, zs);
​// This sum is expected for a 1000^2 grid with 300 iterations.
​// It catches minor errors we might introduce when were
​// working on a fixed set of inputs.
​//assert(sum(output, (size_y * size_x)) == 33219980);
​printf​(​"Sum:%d​\n​"​, ​sum​(output, (size_y * size_x)));
​return​ output;
};
int​ ​main​(​int​ ​argc​, ​char​ **​argv​)
{
​int​ desired_width = ​1000​, max_iterations = ​300​, image = ​0​;
​int​ *output;
​// obtener argumentos proporcionados en tiempo de ejecución
​if​ (argc > ​1​) { desired_width = ​atoi​(​argv​[​1​]); }
​if​ (argc > ​2​) { max_iterations = ​atoi​(​argv​[​2​]); }
​// Calculate the Julia set using a pure C solution
output = ​calc_pure_c​(desired_width, max_iterations);
​return​ ​0​;
}


