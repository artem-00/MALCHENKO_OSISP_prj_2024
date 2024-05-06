#ifndef DELETE_IMAGES_H
#define DELETE_IMAGES_H

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <math.h>


#define MAX_PATH_LENGTH 512
#define MAX_FILENAME_LENGTH 256
#define HISTOGRAM_SIZE 256

void confirmAndDeleteImage(const std::string& imagePath) {
    cv::Mat image = cv::imread(imagePath);

    if (!image.empty()) {
        cv::namedWindow("Image Confirmation", cv::WINDOW_AUTOSIZE);
        cv::imshow("Image Confirmation", image);

        std::cout << "Are you sure you want to delete this image? (y/n): ";
        char choice;
        std::cin >> choice;

        if (choice == 'y' || choice == 'Y') {
            if (std::remove(imagePath.c_str()) == 0) {
                std::cout << "Image deleted successfully." << std::endl;
            } else {
                std::cerr << "Failed to delete the image." << std::endl;
            }
        } else {
            std::cout << "Image deletion canceled." << std::endl;
        }

        cv::destroyWindow("Image Confirmation");
    } else {
        std::cerr << "Failed to load the image: " << imagePath << std::endl;
    }
}

void deleteImageIfSimilar(const char* folderPath, const char* image1, const char* image2) {
    std::string image1Path = std::string(folderPath) + "/" + image1;
    std::string image2Path = std::string(folderPath) + "/" + image2;

    std::cout << "photo " << image1 << " and photo " << image2 << " are similar (delete yes/no): ";
    char choice;
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        confirmAndDeleteImage(image1Path);
    } else if (choice == 'n' || choice == 'N') {
        confirmAndDeleteImage(image2Path);
    } else {
        std::cout << "Invalid choice. No image will be deleted." << std::endl;
    }
}

#endif
