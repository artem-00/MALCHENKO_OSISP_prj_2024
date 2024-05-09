#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <vector>

#define MAX_PATH_LENGTH 512
#define MAX_FILENAME_LENGTH 256
#define HISTOGRAM_SIZE 256

// Функция для вычисления гистограммы изображения
void calculateHistogram(cv::Mat image, int* hist) {
    // Инициализация гистограммы
    for (int i = 0; i < HISTOGRAM_SIZE; ++i) {
        hist[i] = 0;
    }

    // Проход по каждому пикселю изображения и увеличение соответствующего бина в гистограмме
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            int pixelValue = image.at<uchar>(y, x);
            hist[pixelValue]++;
        }
    }
}

// Функция для нормализации гистограммы
void normalizeHistogram(int* hist, int size) {
    // Поиск максимального значения в гистограмме
    int maxCount = 0;
    for (int i = 0; i < size; ++i) {
        if (hist[i] > maxCount) {
            maxCount = hist[i];
        }
    }

    // Нормализация значений в гистограмме
    for (int i = 0; i < size; ++i) {
        hist[i] = hist[i] / static_cast<double>(maxCount);
    }
}

// Функция для сравнения гистограмм изображений
double compareHistograms(int* hist1, int* hist2, int size) {
    // Вычисление суммы квадратов разностей между значениями бинов
    double sumOfProducts = 0;
    double sumOfSquaresHist1 = 0;
    double sumOfSquaresHist2 = 0;

    for (int i = 0; i < size; ++i) {
        sumOfProducts += hist1[i] * hist2[i];
        sumOfSquaresHist1 += hist1[i] * hist1[i];
        sumOfSquaresHist2 += hist2[i] * hist2[i];
    }

    // Вычисление коэффициента корреляции Пирсона
    double correlation = sumOfProducts / (sqrt(sumOfSquaresHist1) * sqrt(sumOfSquaresHist2));

    return correlation;
}

// Функция для удаления изображения
void deleteImage(const char* folderPath, const char* imageName) {
    char imagePath[MAX_PATH_LENGTH];
    snprintf(imagePath, MAX_PATH_LENGTH, "%s/%s", folderPath, imageName);

    if (remove(imagePath) == 0) {
        printf("Image %s deleted successfully.\n", imageName);
    } else {
        printf("Failed to delete image %s.\n", imageName);
    }
}

// Функция для архивирования изображения
void archiveImage(const char* folderPath, const char* imageName) {
    char imagePath[MAX_PATH_LENGTH];
    snprintf(imagePath, MAX_PATH_LENGTH, "%s/%s", folderPath, imageName);

    char command[MAX_PATH_LENGTH + 20];
    snprintf(command, sizeof(command), "tar -czf %s/%s.tar.gz -C %s %s", folderPath, imageName, folderPath, imageName);

    int status = system(command);
    if (status == 0) {
        printf("Image %s archived successfully.\n", imageName);
        deleteImage(folderPath, imageName);
    } else {
        printf("Failed to archive image %s.\n", imageName);
    }
}

// Функция для удаления или архивирования выбранного изображения
void deleteOrArchiveImage(const char* folderPath, const char* imageName) {
    printf("Image %s is similar to at least one other image. What would you like to do?\n", imageName);
    printf("1. Delete %s\n", imageName);
    printf("2. Archive %s\n", imageName);
    printf("3. Skip\n");
    printf("Enter your choice (1, 2, or 3): ");

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            deleteImage(folderPath, imageName);
            break;
        case 2:
            archiveImage(folderPath, imageName);
            break;
        case 3:
            printf("Skipping %s.\n", imageName);
            break;
        default:
            printf("Invalid choice. Skipping %s.\n", imageName);
            break;
    }
}

//
// Функция для поиска дубликатов изображений в заданной папке
void findDuplicateImages(const char* folderPath) {
    DIR* dir;
    struct dirent* entry;

    // Открытие указанной папки
    dir = opendir(folderPath);
    if (dir == NULL) {
        printf("Failed to open directory %s.\n", folderPath);
        return;
    }

    std::vector<std::string> imageNames;
    std::vector<int*> histograms;

    // Чтение всех файлов в папке
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Проверка, является ли элемент обычным файлом
            char filename[MAX_FILENAME_LENGTH];
            strncpy(filename, entry->d_name, MAX_FILENAME_LENGTH);

            // Проверка, является ли файл изображением (проверка расширения файла)
            std::string extension = filename;
            if (extension.find(".jpg") != std::string::npos ||
                extension.find(".jpeg") != std::string::npos ||
                extension.find(".png") != std::string::npos) {
                std::string imagePath = std::string(folderPath) + "/" + std::string(filename);

                // Загрузка изображения с использованием OpenCV
                cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
                if (image.empty()) {
                    printf("Failed to load image: %s\n", filename);
                    continue;
                }

                // Вычисление гистограммы изображения
                int* hist = new int[HISTOGRAM_SIZE];
                calculateHistogram(image, hist);

                // Нормализация гистограммы
                normalizeHistogram(hist, HISTOGRAM_SIZE);

                // Проверка наличия дубликата
                for (size_t i = 0; i < histograms.size(); ++i) {
                    double correlation = compareHistograms(hist, histograms[i], HISTOGRAM_SIZE);
                    if (correlation >= 0.9) {
                        printf("Images %s and %s are similar.\n", imageNames[i].c_str(), filename);

                        // Предоставление выбора пользователю
                        deleteOrArchiveImage(folderPath, imageNames[i].c_str());
                    }
                }

                imageNames.push_back(std::string(filename));
                histograms.push_back(hist);
            }
        }
    }

    // Освобождение памяти, выделенной для гистограмм
    for (size_t i = 0; i < histograms.size(); ++i) {
        delete[] histograms[i];
    }

    // Закрытие папки
    closedir(dir);

    printf("Duplicate image search completed.\n");
    printf("Total images: %lu\n", imageNames.size());
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <folder_path>\n", argv[0]);
        return 1;
    }

    const char* folderPath = argv[1];
    findDuplicateImages(folderPath);

    return 0;
}
