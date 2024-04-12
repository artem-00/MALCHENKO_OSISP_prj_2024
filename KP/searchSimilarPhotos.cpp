#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

// Функция для сравнения изображений
double compareImages(const cv::Mat& image1, const cv::Mat& image2) {
    cv::Mat hist1, hist2;
    cv::cvtColor(image1, hist1, cv::COLOR_BGR2HSV);
    cv::cvtColor(image2, hist2, cv::COLOR_BGR2HSV);

    // Вычисление гистограммы изображений
    int histSize = 256;
    float hRanges[] = { 0, 180 };
    const float* ranges[] = { hRanges };
    int channels[] = { 0 };
    cv::MatND histImage1, histImage2;
    cv::calcHist(&hist1, 1, channels, cv::Mat(), histImage1, 1, &histSize, ranges, true, false);
    cv::calcHist(&hist2, 1, channels, cv::Mat(), histImage2, 1, &histSize, ranges, true, false);

    // Нормализация гистограмм
    cv::normalize(histImage1, histImage1, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histImage2, histImage2, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    // Вычисление коэффициента схожести (нормализованное расстояние Чебышева)
    double similarity = cv::compareHist(histImage1, histImage2, cv::HISTCMP_INTERSECT);

    return similarity;
}

// Функция для поиска и обработки похожих изображений
void findSimilarImages(const std::string& folderPath) {
    // Обход папки и поиск изображений
    std::vector<cv::Mat> loadedImages;
    std::vector<std::string> imagePaths;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        const std::string filePath = entry.path().string();

        // Проверка, является ли файл изображением (можно использовать расширения файлов или другие признаки)
        // Если файл является изображением, загрузите его с помощью OpenCV и обработать
        if (fs::is_regular_file(entry) && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {
            cv::Mat image = cv::imread(filePath);
            if (!image.empty()) {
                loadedImages.push_back(image);
                imagePaths.push_back(filePath);
            }
        }
    }

    const int numImages = loadedImages.size();

    if (numImages == 0) {
        std::cout << "No images found in the directory." << std::endl;
        return;
    }

    // Выполнение сравнения изображения с другими ранее найденными изображениями  и вывод процента схожести
    for (int i = 0; i < numImages; ++i) {
        for (int j = i + 1; j < numImages; ++j) {
            double similarity = compareImages(loadedImages[i], loadedImages[j]);
            std::cout << "Similarity between " << imagePaths[i] << " and " << imagePaths[j] << ": " << similarity * 100 << "%" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./image_similarity <folder_path>" << std::endl;
        return 1;
    }

    std::string folderPath = argv[1];
    findSimilarImages(folderPath);

    return 0;
}
