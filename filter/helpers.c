#include "helpers.h"
#include <math.h>


// Helper function for grabbing the 3x3 around pixel
void kernel_avg(int i, int j, int height, int width, RGBTRIPLE image[height][width], RGBTRIPLE temp_image[height][width]);

// Calculates gY value of R, G, or B and returns a value from 0 - 255
int gy(char color, int y, int x, int height, int width, RGBTRIPLE image[height][width]);

// Calculates gX value of R, G, or B and returns a value from 0 - 255
int gx(char color, int y, int x, int height, int width, RGBTRIPLE image[height][width]);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    float pxl_avg;

    // Iterate over each rgb triple in the image[height][width] array
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Take the average of all 3 values and round it to nearest int
            pxl_avg = round((image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0);

            // Set each RGB (BGR for BMP) value equal to average so that brightness of gray is taken into account
            image[i][j].rgbtBlue = pxl_avg;
            image[i][j].rgbtGreen = pxl_avg;
            image[i][j].rgbtRed = pxl_avg;
        }
    }

    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // Create temp image to avoid using modified pixels in other calculations
    RGBTRIPLE temp_image[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = width; j > 0; j--)
        {
            temp_image[i][width - j].rgbtBlue = image[i][j - 1].rgbtBlue;
            temp_image[i][width - j].rgbtGreen = image[i][j - 1].rgbtGreen;
            temp_image[i][width - j].rgbtRed = image[i][j - 1].rgbtRed;
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtBlue = temp_image[i][j].rgbtBlue;
            image[i][j].rgbtGreen = temp_image[i][j].rgbtGreen;
            image[i][j].rgbtRed = temp_image[i][j].rgbtRed;
        }
    }

    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Create same size temp image to avoid using modified pixels in other calculations
    RGBTRIPLE temp_image[height][width];

    // For each pixel create a kernel average value and write it to the temp image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            kernel_avg(i, j, height, width, image, temp_image);
        }
    }

    // Copy temp image into regular image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtBlue = temp_image[i][j].rgbtBlue;
            image[i][j].rgbtGreen = temp_image[i][j].rgbtGreen;
            image[i][j].rgbtRed = temp_image[i][j].rgbtRed;
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a temp copy to avoid modifying values that are then used in other calculations
    RGBTRIPLE temp_image[height][width];

    float sobel_b = 0.;
    float sobel_g = 0.;
    float sobel_r = 0.;

    // For each pixel find the sobel value of gx and gy combined by square root of gx^2 + gy^2
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            sobel_b = sqrt(powf(gx('b', i, j, height, width, image), 2) + powf(gy('b', i, j, height, width, image), 2));
            sobel_g = sqrt(powf(gx('g', i, j, height, width, image), 2) + powf(gy('g', i, j, height, width, image), 2));
            sobel_r = sqrt(powf(gx('r', i, j, height, width, image), 2) + powf(gy('r', i, j, height, width, image), 2));

            // Set a cap of 255 since max hex value for rgb is 255
            if (sobel_b > 255.0)
            {
                sobel_b = 255.0;
            }

            if (sobel_g > 255.0)
            {
                sobel_g = 255.0;
            }

            if (sobel_r > 255.0)
            {
                sobel_r = 255.0;
            }

            // Round the value of sobel for rgb
            temp_image[i][j].rgbtBlue = round(sobel_b);
            temp_image[i][j].rgbtGreen = round(sobel_g);
            temp_image[i][j].rgbtRed = round(sobel_r);
        }
    }

    // Write copy image over new image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtBlue = temp_image[i][j].rgbtBlue;
            image[i][j].rgbtGreen = temp_image[i][j].rgbtGreen;
            image[i][j].rgbtRed = temp_image[i][j].rgbtRed;
        }
    }

    return;
}

// Find the gY value by {-1, -2, -1}, {0, 0, 0}, {1, 2, 1}

int gy(char color, int y, int x, int height, int width, RGBTRIPLE image[height][width])
{
    // Initialize sobel y value
    int sobel_y = 0;

    // Corner
    if (x == 0 && y == 0)
    {
        switch (color)
        {
            // Blue
            case 'b':
                sobel_y =
                    (image[y + 1][x].rgbtBlue * 2) +
                    (image[y + 1][x + 1].rgbtBlue * 1);
                break;

            // Green
            case 'g':
                sobel_y =
                    (image[y + 1][x].rgbtGreen * 2) +
                    (image[y + 1][x + 1].rgbtGreen * 1);
                break;

            // Red
            case 'r':
                sobel_y =
                    (image[y + 1][x].rgbtRed * 2) +
                    (image[y + 1][x + 1].rgbtRed * 1);
                break;
        }

        return sobel_y;
    }

    // Corner 2
    if (x == 0 && y == height - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_y =
                    (image[y - 1][x].rgbtBlue * -2) +
                    (image[y - 1][x + 1].rgbtBlue * -1);
                break;

            case 'g':
                sobel_y =
                    (image[y - 1][x].rgbtGreen * -2) +
                    (image[y - 1][x + 1].rgbtGreen * -1);
                break;

            case 'r':
                sobel_y =
                    (image[y - 1][x].rgbtRed * -2) +
                    (image[y - 1][x + 1].rgbtRed * -1);
                break;
        }

        return sobel_y;
    }

    // Corner 3
    if (x == width - 1 && y == 0)
    {
        switch (color)
        {
            case 'b':
                sobel_y =
                    (image[y + 1][x].rgbtBlue * 2) +
                    (image[y + 1][x - 1].rgbtBlue * 1);
                break;

            case 'g':
                sobel_y =
                    (image[y + 1][x].rgbtGreen * 2) +
                    (image[y + 1][x - 1].rgbtGreen * 1);
                break;

            case 'r':
                sobel_y =
                    (image[y + 1][x].rgbtRed * 2) +
                    (image[y + 1][x - 1].rgbtRed * 1);
                break;
        }

        return sobel_y;
    }

    // Corner 4
    if (x == width - 1 && y == height - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_y =
                    (image[y - 1][x].rgbtBlue * -2) +
                    (image[y - 1][x - 1].rgbtBlue * -1);
                break;

            case 'g':
                sobel_y =
                    (image[y - 1][x].rgbtGreen * -2) +
                    (image[y - 1][x - 1].rgbtGreen * -1);
                break;

            case 'r':
                sobel_y =
                    (image[y - 1][x].rgbtRed * -2) +
                    (image[y - 1][x - 1].rgbtRed * -1);
                break;
        }

        return sobel_y;
    }

    // Edge 1
    if (x == 0)
    {
        switch (color)
        {
            case 'b':
                sobel_y =
                    (image[y - 1][x].rgbtBlue * -2) +
                    (image[y - 1][x + 1].rgbtBlue * -1) +
                    (image[y + 1][x + 1].rgbtBlue * 1) +
                    (image[y + 1][x].rgbtBlue * 2);
                break;

            case 'g':
                sobel_y =
                    (image[y - 1][x].rgbtGreen * -2) +
                    (image[y - 1][x + 1].rgbtGreen * -1) +
                    (image[y + 1][x + 1].rgbtGreen * 1) +
                    (image[y + 1][x].rgbtGreen * 2);
                break;

            case 'r':
                sobel_y =
                    (image[y - 1][x].rgbtRed * -2) +
                    (image[y - 1][x + 1].rgbtRed * -1) +
                    (image[y + 1][x + 1].rgbtRed * 1) +
                    (image[y + 1][x].rgbtRed * 2);
                break;
        }

        return sobel_y;
    }

    // Edge 2
    if (x == width - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_y =
                    (image[y - 1][x].rgbtBlue * -2) +
                    (image[y - 1][x - 1].rgbtBlue * -1) +
                    (image[y + 1][x - 1].rgbtBlue * 1) +
                    (image[y + 1][x].rgbtBlue * 2);
                break;

            case 'g':
                sobel_y =
                    (image[y - 1][x].rgbtGreen * -2) +
                    (image[y - 1][x - 1].rgbtGreen * -1) +
                    (image[y + 1][x - 1].rgbtGreen * 1) +
                    (image[y + 1][x].rgbtGreen * 2);
                break;

            case 'r':
                sobel_y =
                    (image[y - 1][x].rgbtRed * -2) +
                    (image[y - 1][x - 1].rgbtRed * -1) +
                    (image[y + 1][x - 1].rgbtRed * 1) +
                    (image[y + 1][x].rgbtRed * 2);
                break;
        }

        return sobel_y;
    }

    // Edge 3
    if (y == 0)
    {
        switch (color)
        {
            case 'b':
                sobel_y =
                    (image[y + 1][x + 1].rgbtBlue * 1) +
                    (image[y + 1][x].rgbtBlue * 2) +
                    (image[y + 1][x - 1].rgbtBlue * 1);
                break;

            case 'g':
                sobel_y =
                    (image[y + 1][x + 1].rgbtGreen * 1) +
                    (image[y + 1][x].rgbtGreen * 2) +
                    (image[y + 1][x - 1].rgbtGreen * 1);
                break;

            case 'r':
                sobel_y =
                    (image[y + 1][x + 1].rgbtRed * 1) +
                    (image[y + 1][x].rgbtRed * 2) +
                    (image[y + 1][x - 1].rgbtRed * 1);
                break;
        }

        return sobel_y;
    }

    // Edge 4
    if (y == height - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_y =
                    (image[y - 1][x + 1].rgbtBlue * -1) +
                    (image[y - 1][x].rgbtBlue * -2) +
                    (image[y - 1][x - 1].rgbtBlue * -1);
                break;

            case 'g':
                sobel_y =
                    (image[y - 1][x + 1].rgbtGreen * -1) +
                    (image[y - 1][x].rgbtGreen * -2) +
                    (image[y - 1][x - 1].rgbtGreen * -1);
                break;

            case 'r':
                sobel_y =
                    (image[y - 1][x + 1].rgbtRed * -1) +
                    (image[y - 1][x].rgbtRed * -2) +
                    (image[y - 1][x - 1].rgbtRed * -1);
                break;
        }

        return sobel_y;
    }

    // 3x3 kernel
    switch (color)
    {
        case 'b':
            sobel_y =
                (image[y - 1][x - 1].rgbtBlue * -1) +
                (image[y + 1][x - 1].rgbtBlue * 1) +
                (image[y - 1][x].rgbtBlue * -2) +
                (image[y + 1][x].rgbtBlue * 2) +
                (image[y - 1][x + 1].rgbtBlue * -1) +
                (image[y + 1][x + 1].rgbtBlue * 1);
            break;

        case 'g':
            sobel_y =
                (image[y - 1][x - 1].rgbtGreen * -1) +
                (image[y + 1][x - 1].rgbtGreen * 1) +
                (image[y - 1][x].rgbtGreen * -2) +
                (image[y + 1][x].rgbtGreen * 2) +
                (image[y - 1][x + 1].rgbtGreen * -1) +
                (image[y + 1][x + 1].rgbtGreen * 1);
            break;

        case 'r':
            sobel_y =
                (image[y - 1][x - 1].rgbtRed * -1) +
                (image[y + 1][x - 1].rgbtRed * 1) +
                (image[y - 1][x].rgbtRed * -2) +
                (image[y + 1][x].rgbtRed * 2) +
                (image[y - 1][x + 1].rgbtRed * -1) +
                (image[y + 1][x + 1].rgbtRed * 1);
            break;
    }

    return sobel_y;
}

// Calculate gx sobel value by multiplying as such {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}
int gx(char color, int y, int x, int height, int width, RGBTRIPLE image[height][width])
{
    int sobel_x = 0;

    // Corner
    if (x == 0 && y == 0)
    {
        switch (color)
        {
            // Blue
            case 'b':
                sobel_x =
                    (image[y][x + 1].rgbtBlue * 2) +
                    (image[y + 1][x + 1].rgbtBlue * 1);
                break;

            // Green
            case 'g':
                sobel_x =
                    (image[y][x + 1].rgbtGreen * 2) +
                    (image[y + 1][x + 1].rgbtGreen * 1);
                break;

            // Red
            case 'r':
                sobel_x =
                    (image[y][x + 1].rgbtRed * 2) +
                    (image[y + 1][x + 1].rgbtRed * 1);
                break;
        }

        return sobel_x;
    }

    // Corner 2
    if (x == 0 && y == height - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_x =
                    (image[y][x + 1].rgbtBlue * 2) +
                    (image[y - 1][x + 1].rgbtBlue * 1);
                break;

            case 'g':
                sobel_x =
                    (image[y][x + 1].rgbtGreen * 2) +
                    (image[y - 1][x + 1].rgbtGreen * 1);
                break;

            case 'r':
                sobel_x =
                    (image[y][x + 1].rgbtRed * 2) +
                    (image[y - 1][x + 1].rgbtRed * 1);
                break;
        }

        return sobel_x;
    }

    // Corner 3
    if (x == width - 1 && y == 0)
    {
        switch (color)
        {
            case 'b':
                sobel_x =
                    (image[y][x - 1].rgbtBlue * -2) +
                    (image[y + 1][x - 1].rgbtBlue * -1);
                break;

            case 'g':
                sobel_x =
                    (image[y][x - 1].rgbtGreen * -2) +
                    (image[y + 1][x - 1].rgbtGreen * -1);
                break;

            case 'r':
                sobel_x =
                    (image[y][x - 1].rgbtRed * -2) +
                    (image[y + 1][x - 1].rgbtRed * -1);
                break;
        }

        return sobel_x;
    }

    // Corner 4
    if (x == width - 1  && y == height - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_x =
                    (image[y][x - 1].rgbtBlue * -2) +
                    (image[y - 1][x - 1].rgbtBlue * -1);
                break;

            case 'g':
                sobel_x =
                    (image[y][x - 1].rgbtGreen * -2) +
                    (image[y - 1][x - 1].rgbtGreen * -1);
                break;

            case 'r':
                sobel_x =
                    (image[y][x - 1].rgbtRed * -2) +
                    (image[y - 1][x - 1].rgbtRed * -1);
                break;
        }

        return sobel_x;
    }

    // Edge 1
    if (x == 0)
    {
        switch (color)
        {
            case 'b':
                sobel_x =
                    (image[y - 1][x + 1].rgbtBlue * 1) +
                    (image[y][x + 1].rgbtBlue * 2) +
                    (image[y + 1][x + 1].rgbtBlue * 1);
                break;

            case 'g':
                sobel_x =
                    (image[y - 1][x + 1].rgbtGreen * 1) +
                    (image[y][x + 1].rgbtGreen * 2) +
                    (image[y + 1][x + 1].rgbtGreen * 1);
                break;

            case 'r':
                sobel_x =
                    (image[y - 1][x + 1].rgbtRed * 1) +
                    (image[y][x + 1].rgbtRed * 2) +
                    (image[y + 1][x + 1].rgbtRed * 1);
                break;
        }

        return sobel_x;
    }

    // Edge 2
    if (x == width - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_x =
                    (image[y - 1][x - 1].rgbtBlue * -1) +
                    (image[y][x - 1].rgbtBlue * -2) +
                    (image[y + 1][x - 1].rgbtBlue * -1);
                break;

            case 'g':
                sobel_x =
                    (image[y - 1][x - 1].rgbtGreen * -1) +
                    (image[y][x - 1].rgbtGreen * -2) +
                    (image[y + 1][x - 1].rgbtGreen * -1);
                break;

            case 'r':
                sobel_x =
                    (image[y - 1][x - 1].rgbtRed * -1) +
                    (image[y][x - 1].rgbtRed * -2) +
                    (image[y + 1][x - 1].rgbtRed * -1);
                break;
        }

        return sobel_x;
    }

    // Edge 3
    if (y == 0)
    {
        switch (color)
        {
            case 'b':
                sobel_x =
                    (image[y][x - 1].rgbtBlue * -2) +
                    (image[y + 1][x - 1].rgbtBlue * -1) +
                    (image[y][x + 1].rgbtBlue * 2) +
                    (image[y + 1][x + 1].rgbtBlue * 1);
                break;

            case 'g':
                sobel_x =
                    (image[y][x - 1].rgbtGreen * -2) +
                    (image[y + 1][x - 1].rgbtGreen * -1) +
                    (image[y][x + 1].rgbtGreen * 2) +
                    (image[y + 1][x + 1].rgbtGreen * 1);
                break;

            case 'r':
                sobel_x =
                    (image[y][x - 1].rgbtRed * -2) +
                    (image[y + 1][x - 1].rgbtRed * -1) +
                    (image[y][x + 1].rgbtRed * 2) +
                    (image[y + 1][x + 1].rgbtRed * 1);
                break;
        }

        return sobel_x;
    }

    // Edge 4
    if (y == height - 1)
    {
        switch (color)
        {
            case 'b':
                sobel_x =
                    (image[y][x - 1].rgbtBlue * -2) +
                    (image[y - 1][x - 1].rgbtBlue * -1) +
                    (image[y][x + 1].rgbtBlue * 2) +
                    (image[y - 1][x + 1].rgbtBlue * 1);
                break;

            case 'g':
                sobel_x =
                    (image[y][x - 1].rgbtGreen * -2) +
                    (image[y - 1][x - 1].rgbtGreen * -1) +
                    (image[y][x + 1].rgbtGreen * 2) +
                    (image[y - 1][x + 1].rgbtGreen * 1);
                break;

            case 'r':
                sobel_x =
                    (image[y][x - 1].rgbtRed * -2) +
                    (image[y - 1][x - 1].rgbtRed * -1) +
                    (image[y][x + 1].rgbtRed * 2) +
                    (image[y - 1][x + 1].rgbtRed * 1);
                break;
        }

        return sobel_x;
    }

    // 3x3 kernel
    switch (color)
    {
        case 'b':
            sobel_x =
                (image[y - 1][x - 1].rgbtBlue * -1) +
                (image[y][x - 1].rgbtBlue * -2) +
                (image[y + 1][x - 1].rgbtBlue * -1) +
                (image[y - 1][x + 1].rgbtBlue * 1) +
                (image[y][x + 1].rgbtBlue * 2) +
                (image[y + 1][x + 1].rgbtBlue * 1);
            break;

        case 'g':
            sobel_x =
                (image[y - 1][x - 1].rgbtGreen * -1) +
                (image[y][x - 1].rgbtGreen * -2) +
                (image[y + 1][x - 1].rgbtGreen * -1) +
                (image[y - 1][x + 1].rgbtGreen * 1) +
                (image[y][x + 1].rgbtGreen * 2) +
                (image[y + 1][x + 1].rgbtGreen * 1);
            break;

        case 'r':
            sobel_x =
                (image[y - 1][x - 1].rgbtRed * -1) +
                (image[y][x - 1].rgbtRed * -2) +
                (image[y + 1][x - 1].rgbtRed * -1) +
                (image[y - 1][x + 1].rgbtRed * 1) +
                (image[y][x + 1].rgbtRed * 2) +
                (image[y + 1][x + 1].rgbtRed * 1);
            break;
    }

    return sobel_x;
}

// Grab 3x3 area around, plus handle corner cases and edge cases
void kernel_avg(int y, int x, int height, int width, RGBTRIPLE image[height][width], RGBTRIPLE temp_image[height][width])
{
    float b_avg = 0.0;
    float g_avg = 0.0;
    float r_avg = 0.0;

    // Corner 1
    if (x == 0 && y == 0)
    {
        for (int i = 0; i <= 1; i++)
        {
            for (int j = 0; j <= 1; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 4.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 4.0);
        temp_image[y][x].rgbtRed = round(r_avg / 4.0);

        return;
    }

    // Corner 2
    if (x == 0 && y == height - 1)
    {
        for (int i = y - 1; i <= y; i++)
        {
            for (int j = 0; j <= 1; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 4.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 4.0);
        temp_image[y][x].rgbtRed = round(r_avg / 4.0);

        return;
    }

    // Corner 3
    if (x == width - 1 && y == 0)
    {
        for (int i = 0; i <= 1; i++)
        {
            for (int j = x - 1; j <= x; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 4.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 4.0);
        temp_image[y][x].rgbtRed = round(r_avg / 4.0);

        return;
    }

    // Corner 4
    if (x == width - 1 && y == height - 1)
    {
        for (int i = y - 1; i <= y; i++)
        {
            for (int j = x - 1; j <= x; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 4.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 4.0);
        temp_image[y][x].rgbtRed = round(r_avg / 4.0);

        return;
    }

    // Edge 1
    if (x == 0)
    {
        for (int i = y - 1; i <= y + 1; i++)
        {
            for (int j = 0; j <= 1; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 6.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 6.0);
        temp_image[y][x].rgbtRed = round(r_avg / 6.0);

        return;
    }

    // Edge 2
    if (x == width - 1)
    {
        for (int i = y - 1; i <= y + 1; i++)
        {
            for (int j = x - 1; j <= x; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 6.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 6.0);
        temp_image[y][x].rgbtRed = round(r_avg / 6.0);

        return;
    }

    // Edge 3
    if (y == 0)
    {
        for (int i = 0; i <= 1; i++)
        {
            for (int j = x - 1; j <= x + 1; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 6.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 6.0);
        temp_image[y][x].rgbtRed = round(r_avg / 6.0);

        return;
    }

    // Edge 4
    if (y == height - 1)
    {
        for (int i = y - 1; i <= y; i++)
        {
            for (int j = x - 1; j <= x + 1; j++)
            {
                b_avg += image[i][j].rgbtBlue;
                g_avg += image[i][j].rgbtGreen;
                r_avg += image[i][j].rgbtRed;
            }
        }

        temp_image[y][x].rgbtBlue = round(b_avg / 6.0);
        temp_image[y][x].rgbtGreen = round(g_avg / 6.0);
        temp_image[y][x].rgbtRed = round(r_avg / 6.0);

        return;
    }

    // 3x3 kernel
    for (int i = y - 1; i <= y + 1; i++)
    {
        for (int j = x - 1; j <= x + 1; j++)
        {
            b_avg += image[i][j].rgbtBlue;
            g_avg += image[i][j].rgbtGreen;
            r_avg += image[i][j].rgbtRed;
        }
    }

    temp_image[y][x].rgbtBlue = round(b_avg / 9.0);
    temp_image[y][x].rgbtGreen = round(g_avg / 9.0);
    temp_image[y][x].rgbtRed = round(r_avg / 9.0);

    return;
}