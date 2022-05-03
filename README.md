# filter
Created the Filter helper functions to apply Blur, Flip Horizontal, Grayscale, and Edge Detection to an Image (BMP)

Project: Filter | Language: C‍

For this homework assignment, we had to implement algorithms such that if a bmp was input, we could output a grayscale version, a reflected version, a box blurred version, and an edge detection algorithmThis assignment helped me gain confidence in my 2D array skills. I also learned to account for corner, and edge cases so that I wouldn't touch and manipulate memory that I wasn't supposed to. The most challenging part was the edge detection algorithm. I had to iterate over each pixel, and multiply the pixels around it by either a positive or negative value. Then add the result together and if the number was close to 0 it meant the pixels around it were of a similar color and therefore not an edge.One of the most important things I learned form this project was to create a temporary variable to store the manipulations of the pixels, because otherwise I would be change pixels in the original data and using the changed pixel data in my calculations of future pixels which would throw everything off!

The assignment was as follows:

Implement the functions in helpers.c such that a user can apply grayscale, reflection, blur, or edge detection filters to their images.

The function grayscale should take an image and turn it into a black-and-white version of the same image.

The reflect function should take an image and reflect it horizontally.

The blur function should take an image and turn it into a box-blurred version of the same image.

The edges function should take an image and highlight the edges between objects, according to the Sobel operator.

You should not modify any of the function signatures, nor should you modify any other files other than helpers.c.
