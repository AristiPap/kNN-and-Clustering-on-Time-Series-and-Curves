# kNN-and-Clustering-on-Time-Series-and-Curves
LSH/Hypercube kNN and KMeans++ Clustering on polygonic curves and time series

In this project we will explore the knn method using LSH/Hypercube 

## Instalation
To download the project just `git clone` the repository.
To install prerequisities:

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

In the root directory there are 3 python files that implement the tasks at hand.

## Time Series Forecasting with LSTMs

## Outlier Detection using LSTM Autoencoder

## Dimensionality Reduction using CNNs Autoencoder

## Miscellaneous

## Collaborators

