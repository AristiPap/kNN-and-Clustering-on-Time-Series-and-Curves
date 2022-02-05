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
### LSH with Discrete Frechet
The curve hashing algorithm gets as input a curve and returns its grid curve. In order to do that we iterate through each Point of the curve, find its hash according to this formula: **floor((point.getCoordinate(i) - t[i])/delta + 0.5) * delta + t[i]**
At the same time, we check so that the grid curve wont have any duplicate points and at the end we concatenate all the hashed points together.After creating the grid curve, we squeeze it (concatenate all the points of the grid curve to one point), we add (if necessary) padding so that we always have the same dimension at our points. At this point we are ready to use the LSH algorithm to find the k-Nearest Neighbours

### LSH Algorithm
The LSH algorithm, based on the k-hash functions with formula: $h_i(\boldsymbol{p}) = \frac{\boldsymbol{p} \cdot \boldsymbol{v_i} + t_i}{w}$
was implemented  from the classes **Hashing** και **LSHHasing**. The Hashing class produces and saves the **v_i** and **t_i** as well as the production of the  **h_i** values. 
\par 
Τhe **LSHHashing**, calculates the functions **g(\cdot)**. Its also responsible for the estimation of the value: 
$$\sum\limits_{i=1}^k r_i\cdot h(p),$$ where the r_i are separate for each amplified function $g$. 
\par 

Η κλάση που υλοποιεί το LSH Nearest Neighbours, (\textit{LSHNearestNeighbours}), αρχικοποιεί L συναρτήσεις και αντίστοιχα Hashtables και παρέχει συναρτήσεις για εισαγωγή του σημείου στα hashtable, για αναζήτηση nearest neighbour, k-nearest-neighbours και range-nearest-neighbours με βάση ένα σημείο query.

## Clustering


## Dimensionality Reduction using CNNs Autoencoder



## Collaborators

