# kNN-and-Clustering-on-Time-Series-and-Curves
LSH/Hypercube kNN and KMeans++ Clustering on polygonic curves and time series

In this project we will explore the knn method using LSH/Hypercube 

## Instalation
To download the project just `git clone` the repository.

To compile and execute the program using different algorithms:

```bash
~$ make search [verbose=1] [debug=1] # make the search executable
~$ ./search -i <input filename> -q <query filename> -o <output filename> -algorithm [LSH | Hypercube | Frechet] [-metric [discrete | continuous] default is discrete] [-k -L -M -N -probes -delta]
# In order to print the accuracy of each of the above algorithms in the output file you have to type the above commands like this: make search verbose=1
~$ make clustering [verbose=1] [debug=1] # make the clustering test case
~$ ./cluster -i <input filename> -o <output filename> -c <cluster config filename> -update [Mean_Vector | Mean_Curve] -assignment [Classic | LSH | Hypercube | LSH_Frechet] [-silhouette] [-complete] 
~$ make tests [verbose=1] [debug=1] # create test cases
~$ ./run_tests # run the unit tests
~$ make clean # delete objective files
```

## kNN on Curves
### LSH Algorithm
The LSH algorithm, based on the k-hash functions with formula:
![formula](https://render.githubusercontent.com/render/math?math=$h_i(\boldsymbol{p})=%20\frac{\boldsymbol{p}%20\cdot%20\boldsymbol{v_i}%20%2B%20t_i}{w}$)

was implemented  from the classes **Hashing** και **LSHHasing**. The Hashing class produces and saves the **v_i** and **t_i** as well as the production of the  **h_i** values. 
\par 
Τhe **LSHHashing**, calculates the functions **$g(\cdot)$**. Its also responsible for the estimation of the value: 
![formula](https://render.githubusercontent.com/render/math?math=\sum\limits_{i=1}^k%20r_i\cdot%20h(p)) where the r_i are separate for each amplified function g. 
\par 

The class that implements the LSH Nearest Neighbours, (**LSHNearestNeighbours**),initializes L functions and  Hashtables and contains helper functions  for the import of a point in the hashtable, for nearest neighbor search , k-nearest-neighbors search and range-nearest-neighbors search according to a query point.

### Hypercube Algorithm
Random display in Hypercube is a similar algorithmic technique to LSH. However, instead of L hashtables with their own Amplified HashFunction, our data set is stored at the top of a Hypercube.Initially for the implementation needs of Hypercube we created the HyperCubeHashing class. This class, as in LSH, is derived from Hashing and every time the input element is shifted and then the formula "h" is calculated in it. 
Then the f functions, all they need to do is map the result of h to {0,1} (depending on whether it already exists in f or not). Finally, a dd dimension hypercube is created at the same time as the d 'HashFunctions and f functions. Each time an item is entered, it is first mapped by each HyperCubeHash function and then mapped to {0.1} by f. The result is a binary number of length dd (which is equivalent to a decimal number, specifically with the index of the vertex to which the element will be inserted). \\
Next we compute a set of vertices with distance $hamming <hd$ and loop over the points that were hashed on the same vertex or on a vertex with a certain hamming distance. (To find the nearby vertices each time we use the retrospective function **nearVertices**, with an optimization as it uses a visited set and has essentially similar functionality to dfs)


### LSH/Hypercube with Discrete Frechet
The curve hashing algorithm gets as input a curve and returns its grid curve. In order to do that we iterate through each Point of the curve, find its hash according to this formula: **floor((point.getCoordinate(i) - t[i])/delta + 0.5) * delta + t[i]**
At the same time, we check so that the grid curve wont have any duplicate points and at the end we concatenate all the hashed points together.After creating the grid curve, we squeeze it (concatenate all the points of the grid curve to one point), we add (if necessary) padding so that we always have the same dimension at our points. At this point we are ready to use the LSH(or Hypercube) algorithm to find the k-Nearest Neighbors

### LSH/Hypercube with Continuous Frechet
To implement LSH/Hypercube with Continuous Frechet, we used a subclass of the base Class **HashingCurve**, **CLHHashingCurve**. 

According to the theory we implemented the algorithm for continuous curve hashing as follows: For each curve $c \ in Dataset$:
- filter with **CLSHHashing :: filter** to display the curve at $R$,
- we use the same function for grid snapping as in discrete curve hashing, to map the points of the filtered curve $c$ at the points of the grid 
![formula](https://render.githubusercontent.com/render/math?math=G_{\delta}^{t}=\{(a_1\delta%20%2B%20t)%20\;%20...%20\;%20(a_d\delta%20%2B%20t)%20|%20t\in%20(0,%20\delta)^d,%20\delta%20\in%20R^%2B,%20\forall%20a_i%20\in%20Z\}), according to the formula 
![formula](https://render.githubusercontent.com/render/math?math=$c'%20=%20floor(\frac{(x%20-%20t)}{\delta}%20%2B%20\frac{1}{2})%20\cdot%20\delta%20%2B%20t,%20\;%20x%20\in%20Points(c)$)
- in the grid curve we keep only the local maximums and minimums
- we make the concatenated curve vector, $x$
- we apply padding based on the highest value of all grids

This class also contains the filtering method, which based on an error, filters the given curve. For further use of this method, read the section **Clustering :: Mean Curve Update**

## Clustering


## Collaborators


