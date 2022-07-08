# Grabcut

This is work in progress of a rewrite of my uni project I did in 2010 for image processing class to not use (ancient!) OpenCV 1.x and use Eigen.
It probably is not optimal and might be buggy.

# Grabcut algorithm

The grabcut algorithm works by first split foreground/background areas into GMM color models (I use orchard-bauman clustering method).
Next the maxflow/mincut algorithm is used to refine edges.
Lastly, the GMM is then refined by using new segmentation mask and maxflow/mincut is applied again - we continue that for n steps or until convergence.


# Example

1. Input image
2. User rectangle selection
3. Running grabcut (1 iteration)
![grabcut](https://user-images.githubusercontent.com/1038102/177412036-d8774782-7a8b-45ec-8455-f548c58a76b2.gif)

# Building

At the moment all batteries are included for library/tests but it requires OpenCV for user-facing app.

```
cmake -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release -DBUILD_GRABCUT_APPS=OFF
cmake --build build -j
```
for building library and tests, and if you have installed OpenCV findable by CMake:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_GRABCUT_APPS=ON
cmake --build build -j
```
which will build app which runs OpenCV and this implementation for comparision (this is slower and results may vary)

`build/apps/run_grabcut <img.jpg|png>`

# Comparision with OpenCV
A few examples to compare with OpenCV (5 iterations for each implementation):

![image](https://user-images.githubusercontent.com/1038102/177999791-ec49f3c1-bf36-489c-bcd4-10f932e440a7.png)
![image](https://user-images.githubusercontent.com/1038102/177999884-e22ea3d8-46cb-450f-8871-5399b495b802.png)

Flower:
![image](https://user-images.githubusercontent.com/1038102/178000233-604e83d6-3324-4b55-95a3-f7252456ddba.png)

People:

![image](https://user-images.githubusercontent.com/1038102/178004148-8c61cc24-3212-4ae6-b367-3c47248f1174.png)

![people](https://user-images.githubusercontent.com/1038102/178004295-177dbb26-6da8-4b82-a5c0-380061c9dcde.jpg)

![image](https://user-images.githubusercontent.com/1038102/178004192-0e51a903-b1b8-43ee-8412-d5bfae3a792d.png)

![image](https://user-images.githubusercontent.com/1038102/178004167-75656f5e-98c3-4a38-97b5-b4e4352975bb.png)



