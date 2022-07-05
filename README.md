# Grabcut

This is work in progress of a rewrite of my uni project I did in 2010 for image processing class to not use (ancient!) OpenCV 1.x and use Eigen.
It probably is not optimal and might be buggy.

# Grabcut algorithm

The grabcut algorithm works by first split foreground/background areas into GMM color models (I use orchard-bauman clustering method).
Next the maxflow/mincut algorithm is used to refine edges.
Lastly, the GMM is then refined by using new segmentation mask and maxflow/mincut is applied again - we continue that for n steps or until convergence.


# Example

Given input image:

![flower1](https://user-images.githubusercontent.com/1038102/176899395-7d2faf74-cc36-4eee-80db-5f5133b36557.jpg)

And user rectangle selection:
![masked_img](https://user-images.githubusercontent.com/1038102/177187342-7c284491-88f1-4b74-ac08-7272976430b1.png)

The algorithm iteratively refines selection (note: this is from wip debug version e.g it lacks proper beta parameter estimation):
![new_learn_grabcut](https://user-images.githubusercontent.com/1038102/177313472-12733a6e-181b-4133-889f-b1897e9de09f.gif)

# Building

At the moment it requires OpenCV and Eigen installed and it can be build by default cmake commands but there is no user-facing app yet.
