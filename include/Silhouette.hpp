#pragma once

#include <set>
#include <list>
#include "GenericClusterSolver.hpp"
#include "Point.hpp"
#include "Curves.hpp"

template<class T, class CentroidsT>
vector<double> * evaluate_w_silhouette_per_centroid(std::vector<CentroidsT> &centroids, const std::list<T *> &dataset){
    
    uint32_t _size = dataset.size();
    int K = centroids.size();
    //result array that contains the silhouette function results
    vector<double> * silhouette_dist = new vector<double> ();
    
    double dist;
    silhouette_dist->resize(K, 0);
    
    // Following loops evaluate:
    // 1) second closest centroid of each point
    // 2) average distance between points of the same cluster
    // 3) calculate the average distance of the clusters point from the points of the second closest cluster
    
    for (int i = 0; i < K; i++){
        uint32_t points = 0;
        for(auto it2 : centroids[i].second){
            
            if(centroids[i].second.size() == 1){
                (*silhouette_dist)[i] = 0;
                break;
            }
                
            //get points, second closest centroid
            ClosestCentroid sec_closest_center = find_second_closest_centroid(*it2.first, centroids);
            it2.first->setSecCluster(sec_closest_center.first);
            // calcucate a
            dist = 0.0;
            for (auto it3 : centroids[i].second)
                dist += (*it2.first).dist(*it3.first);
            
            double a = dist/(1.0*centroids[i].second.size() - 1.0 + 1e-8);

            // calculate b
            dist = 0.0;
            // number of points in second closest cluster
            points = centroids[it2.first->getSecCluster()].second.size();
            for (auto it3 : centroids[it2.first->getSecCluster()].second)
                dist += (*it2.first).dist(*it3.first);

            double b = dist / (1.0 * points + 1e-8);

            if ((double)std::max(a,b) == 0)
                continue;
            double avg_pp = (b - a) / ((double)std::max(a,b));

            //for each points calculated silhouette distance, save also the cluster id 
            (*silhouette_dist)[i] += avg_pp; 
        }
        (*silhouette_dist)[i] = (*silhouette_dist)[i] / (centroids[i].second.size() * 1.0 + 1e-8);
    }
    
    return silhouette_dist;
}

template<class T, class CentroidsT>
vector<double> *  evaluate_w_silhouette(std::vector<CentroidsT>& clusters, const std::list<T *> &dataset){
    
    vector<double> *silhouette_dist = evaluate_w_silhouette_per_centroid(clusters,dataset);
    int K = clusters.size();
    
    //compute the sum of all the silhouettes in order to later compute the average 
    double sum = 0.0;
    for (int i = 0; i < K; i++)
        sum += (*silhouette_dist)[i];
        
    // compute average silhouette
    double average_silhouette = ((double) sum) / (1.0*silhouette_dist->size());
    
    silhouette_dist->push_back(average_silhouette);
    
    return silhouette_dist;
}
