#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Функция для вычисления гистограммы изображения
Mat computeHistogram(Mat image) {
    Mat hist;
    int histSize = 256;
    float range[] = {0, 256}; // полный диапазон пиксельных значений
    const float *histRange = {range};
    bool uniform = true;
    bool accumulate = false;
    calcHist(&image, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);
    return hist;
}

// Функция для вычисления контуров изображения
vector<vector<Point>> computeContours(Mat image) {
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY); // конвертируем изображение в оттенки серого
    Mat edges;
    Canny(gray, edges, 100, 200); // используем алгоритм Canny для поиска контуров
    vector<vector<Point>> contours;
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // находим контуры
    return contours;
}

// Функция для сравнения гистограмм двух изображений
double compareHistograms(Mat hist1, Mat hist2) {
    return compareHist(hist1, hist2, HISTCMP_CORREL); // корреляция гистограмм
}

// Функция для сравнения контуров двух изображений
double compareContours(vector<vector<Point>> contours1, vector<vector<Point>> contours2) {
    double similarity = matchShapes(contours1[0], contours2[0], CONTOURS_MATCH_I1, 0); // сравнение контуров
    return similarity;
}

int main() {
    // Пример изображений для сравнения
    Mat image1 = imread("image1.jpg");
    Mat image2 = imread("image2.jpg");

    if (image1.empty() || image2.empty()) {
        cout << "Не удалось загрузить изображения." << endl;
        return -1;
    }

    // Вычисляем гистограммы и контуры для изображений
    Mat hist1 = computeHistogram(image1);
    Mat hist2 = computeHistogram(image2);
    vector<vector<Point>> contours1 = computeContours(image1);
    vector<vector<Point>> contours2 = computeContours(image2);

    // Сравниваем гистограммы и контуры
    double histSimilarity = compareHistograms(hist1, hist2);
    double contourSimilarity = compareContours(contours1, contours2);

    cout << "Схожесть гистограмм: " << histSimilarity << endl;
    cout << "Схожесть контуров: " << contourSimilarity << endl;

    return 0;
}
