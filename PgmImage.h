#pragma once 

#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>

class PgmImage
{
private:
    int width;
    int height;
    int resolution;
    short grey_scale;
    std::vector<short> pixels;
public:
    PgmImage(const std::string& filename);
    void ReadHeader(std::ifstream& input);
    void ReadPixels(std::ifstream& input);
    void ResizeImage(int new_width, int new_height);
    void RotateImage(float ang);
    void BinaryImage(const short& treshold);
    void ApplyBlur();
    void ReduceNoise();
    void NegativeImage();
    void GradientImage();
    void PrintImage();
    void SaveImage(const std::string& filename);
    int len_trim(std::string s);
    void extract_word(std::string s, std::string& s1, std::string& s2);

    template<typename T>
    void BubbleSort(std::vector<T>& vec);
};