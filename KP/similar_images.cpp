#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <opencv2/opencv.hpp>

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
        hist[i] = hist[i] / (double)maxCount;
    }
}

// Функция для сравнения гистограмм изображений
double compareHistograms(int* hist1, int* hist2, int size) {
    // Вычисление суммы квадратов разностей между значениями бинов
    int sumOfSquares = 0;
    for (int i = 0; i < size; ++i) {
        int diff = hist1[i] - hist2[i];
        sumOfSquares += diff * diff;
    }

    // Вычисление коэффициента схожести (нормализованная Евклидова норма)
    double similarity = sqrt(sumOfSquares);

    return similarity;
}

// Функция для поиска и обработки похожих изображений
void findSimilarImages(const char* folderPath) {
    DIR* dir;
    struct dirent* entry;
    char imagePath[MAX_PATH_LENGTH];
    cv::Mat images[MAX_FILENAME_LENGTH];
    char imageNames[MAX_FILENAME_LENGTH][MAX_FILENAME_LENGTH];
    int imageCount = 0;

    // Открытие указанной папки
    dir = opendir(folderPath);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open folder.\n");
        return;
    }

    // Чтение изображений из указанной папки
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            snprintf(imagePath, MAX_PATH_LENGTH, "%s/%s", folderPath, entry->d_name);
            cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
            if (!image.empty()) {
                images[imageCount] = image;
                strncpy(imageNames[imageCount], entry->d_name, MAX_FILENAME_LENGTH);
                imageCount++;
            }
        }
    }
    closedir(dir);

    // Проверка, что найдено не менее двух изображений
    if (imageCount < 2) {
        fprintf(stderr, "Not enough images found in the folder.\n");
        return;
    }

    // Сравнение каждой пары изображений
    for (int i = 0; i < imageCount; ++i) {
        for (int j = i + 1; j < imageCount; ++j) {
            int hist1[HISTOGRAM_SIZE];
            int hist2[HISTOGRAM_SIZE];

            calculateHistogram(images[i], hist1);
            calculateHistogram(images[j], hist2);

            normalizeHistogram(hist1, HISTOGRAM_SIZE);
            normalizeHistogram(hist2, HISTOGRAM_SIZE);

            double similarity = compareHistograms(hist1, hist2, HISTOGRAM_SIZE);
            if (similarity == 0){
                printf("photo %s and photo %s are similar\n", imageNames[i], imageNames[j]);
            }else {
                printf("photo %s and photo %s are NOT similar\n", imageNames[i], imageNames[j]);
            }
        
        }
    }
}

int main(int argc, char* argv[]) {
    // Проверка, что передан достаточно аргументов командной строки
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <folder_path>\n", argv[0]);
        return 1;
    }

    // Получение пути к папке из аргументов командной строки
    const char* folderPath = argv[1];

    // Вызов функции для поиска и обработки похожих изображений
    findSimilarImages(folderPath);

    return 0;
}
