---
layout: default
---

# Generating 3d Mountains

The goal of this project is to use OpenGL to render landscapes of some kind. We can render a mountain using a fractal algorithm. We are also able to navigate our landscape using mouse and keyboard input. In adition, we can generate an STL file version of the landscape and 3d print it. 

## Fractal Algorithm

The algorithm we used to generate our terrain is called the diamond-square algorithm. It consists of taking the average height of surrounding values and adding a random number to generate a new height. To understand in more detail what this algorithm does, this [blog](http://jmecom.github.io/blog/2015/diamond-square/) explains it. This [website](http://www.paulboxley.com/blog/2011/03/terrain-generation-mark-one) visualizes the result.


We used a 2d array of doubles initialized to 0, and ran the algorithm on it to generate a height map. To achieve a "mountain" like effect, we hardcoded the center value to be higher. This allowed all the other values to be calculated based off of it.

Below is the loop we run to generate the terrain.
```js
//ds_steps determines how many levels deep the algorithm needs to go
for (int step = 0; step < ds_steps; step++){
	
	// roughness factor is the bounds of the random number generator
	// this roughness factor decreases every time step, so that details become
	// sharper the deeper into the matrix we go
	int loRough = roughness/(step +1.0);

	//Executes the square step
	square_step(matrix, grid_split, sideLength, loRough);
	
	//make sure the heigh of the peak is maintained
	matrix->rows[size/2][size/2] = 30;

	//Executes the diamond step
	diamond_step(matrix, grid_split, sideLength, loRough, max_index);

	//change the increments for the next level
	sideLength /= 2;
	grid_split *= 2;
}
```

The diamond square algorithm isn't a perfect terrain generating algorithm, but a recommended action is to run the algorithm twice, which we did, to achieve a more natural looking effect.

## Visualizing in OpenGL

To visualize our mountain in OpenGL we used...

## 3D Printing

As a result of this, we can visualize our landscapes using STL files. Below are some examples:


