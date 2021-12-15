#pragma once

#include <set>
#include <list>
#include "GenericClusterSolver.hpp"
#include "Point.hpp"
#include "Curves.hpp"

template<class T, class CentroidsT>
ClosestCentroid find_closest_centroid(T& q, vector<CentroidsT> &centroids)
{
  // find the distance to first centroid 
  int closest_center = 0;
  double min_distance = q.dist(centroids[0].first);
  int K = centroids.size();

  // compare with the rest cluster centers
  for (int i = 1; i < K; i++)
  {
    double distance = q.dist(centroids[i].first);
    if (distance < min_distance)
    {
      closest_center = i;
      min_distance = distance;
    }
  }

  return make_pair(closest_center,min_distance);
}

template<class T, class CentroidsT>
ClosestCentroid find_second_closest_centroid(T& q, vector<CentroidsT> &centroids){
  
  int closest_center = -1, second_closest_center= -1;
  double min_dist = -1, sec_min_dist = -1;
  int K = centroids.size();
  
  // check which of the first 2 clusters are closer
  if (q.dist(centroids[0].first) < q.dist(centroids[1].first)){
    second_closest_center = 1;
    closest_center = 0;
    sec_min_dist = q.dist(centroids[1].first);
    min_dist = q.dist(centroids[0].first);
  }
  else{
    second_closest_center = 0;
    closest_center = 1;
    sec_min_dist = q.dist(centroids[0].first);
    min_dist = q.dist(centroids[1].first);
  }

  // compare with the rest of the centroids 
  for (int c = 2; c < K; c++)
  {
    // compute the distance to the current center
    double dist = q.dist(centroids[c].first);

    // check if the new center is closest or second closest 
    if (dist <= min_dist ){
      second_closest_center = closest_center;
      closest_center = c;
      sec_min_dist = min_dist;
      min_dist = dist;
    }
    else if (dist <= sec_min_dist){
      second_closest_center = c;
      sec_min_dist = dist;
    }
  }

  /* return the second closest center */
  return make_pair(second_closest_center,sec_min_dist);
}



template<class T, class CentroidsT>
uint32_t __Lloyd__ (std::vector<CentroidsT> &centroids, std::list<T *> &dataset){
	
    uint32_t points_updated = 0;
    // loop through data points to find the closest cluster center 
    for (auto it : dataset) {
        // find the closest centroid for training example i 
        ClosestCentroid closest_center = find_closest_centroid(*it, centroids);
        
        if (it->getCluster() == -1){
          // add data point to the cluster 
            centroids[closest_center.first].second[it] = closest_center.second;
          // mark it 
            it->setMarked(true);
            it->setCluster(closest_center.first);
            points_updated++; 
        }
        //if its assigned to a centroid then we are at the recalibration stage of the clusters
        else{
            //update distance of point to centroid
            if(it->getCluster() == closest_center.first)
                centroids[closest_center.first].second[it] = closest_center.second;
            
            else{
                //add point to new centroid
                centroids[closest_center.first].second[it] = closest_center.second;
                //keep statistics
                points_updated ++;
                
                //delete point from old centroid
                centroids[it->getCluster()].first.setMarked(false);
                centroids[it->getCluster()].second.erase(it);

                //update reverse mapping
                it->setMarked(true);
                it->setCluster(closest_center.first);
            }
        }    
    }

    cout << "points_updated: " << points_updated << endl;
    return points_updated;
}
