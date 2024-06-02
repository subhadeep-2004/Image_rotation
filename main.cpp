#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Halide.h"
#include "./Halide-17.0.1-x86-64-linux/share/Halide/tools/halide_image_io.h"

using namespace cv;
using namespace std;
using namespace Halide;
using namespace Halide::Tools;

void affine_transform(const Buffer<uint8_t> &input, Buffer<uint8_t> &output, const std::vector<std::vector<float>> &matrix) {
    int width = input.width();
    int height = input.height();
    int channels = input.channels();

    Var x, y, c;
    Func transform;

    // Ensure matrix values are correctly used in Halide expressions
    Expr newX = matrix[0][0] * cast<float>(x) + matrix[0][1] * cast<float>(y) + matrix[0][2];
    Expr newY = matrix[1][0] * cast<float>(x) + matrix[1][1] * cast<float>(y) + matrix[1][2];

    // Bilinear interpolation
    Expr x0 = cast<int>(floor(newX));
    Expr y0 = cast<int>(floor(newY));
    Expr x1 = clamp(x0 + 1, 0, input.width() - 1);
    Expr y1 = clamp(y0 + 1, 0, input.height() - 1);

    Expr a = newX - x0;
    Expr b = newY - y0;

    // Ensure input indices are clamped properly
    Expr value = (1 - a) * (1 - b) * input(clamp(x0, 0, input.width() - 1), clamp(y0, 0, input.height() - 1), c) +
                 a * (1 - b) * input(clamp(x1, 0, input.width() - 1), clamp(y0, 0, input.height() - 1), c) +
                 (1 - a) * b * input(clamp(x0, 0, input.width() - 1), clamp(y1, 0, input.height() - 1), c) +
                 a * b * input(clamp(x1, 0, input.width() - 1), clamp(y1, 0, input.height() - 1), c);

    // Set pixel to black if transformed coordinates are out of bounds
    transform(x, y, c) = select(
        newX >= 0 && newX < input.width() && newY >= 0 && newY < input.height(),
        cast<uint8_t>(clamp(value, 0.0f, 255.0f)),
        cast<uint8_t>(0)
    );

    output = Buffer<uint8_t>(width, height, channels);
    transform.realize(output);
}



int main(int argc, char** argv) {
     // Specify input and output image paths
     // Specify input and output image paths
         // Specify input and output image paths
    std::string inputImagePath = "../assets/test2.png";
    std::string outputImagePath = "../assets/rotated_2.png";

    try {
        // Load the input image using Halide's load_image function
        Buffer<uint8_t> input = load_image(inputImagePath);

        // Define the rotation angle (in degrees)
        float theta = 45.0f; // Rotation angle in degrees
        float radians = theta * M_PI / 180.0f; // Convert to radians

        // Calculate the bounding box dimensions for the rotated image (same as input)
        int width = input.width();
        int height = input.height();

        // Calculate the center of the original image
        float cx = width / 2.0f;
        float cy = height / 2.0f;

        float cosTheta = std::cos(radians);
        float sinTheta = std::sin(radians);


        // Calculate the translation values to center the rotated image within the original dimensions
        float tx = cx - (cx * static_cast<float>(cos(radians)) - cy * static_cast<float>(sin(radians)));
        float ty = cy - (cx * static_cast<float>(sin(radians)) + cy * static_cast<float>(cos(radians)));

        // Define the affine transformation matrix (rotation + translation)

        std::vector<std::vector<float>> matrix = {
            {cosTheta, -sinTheta, tx},
            {sinTheta, cosTheta, ty}
        };
        
        // Apply the affine transformation
        Buffer<uint8_t> output;
        affine_transform(input, output, matrix);

        // Save the output image using Halide's save_image function
        save_image(output, outputImagePath);

        std::cout << "Affine transformation applied and output saved successfully!" << std::endl;
    } catch (const Halide::Error &e) {
        std::cerr << "Halide Error: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "Standard Error: " << e.what() << std::endl;
        return -1;
    }


    return 0;
}



