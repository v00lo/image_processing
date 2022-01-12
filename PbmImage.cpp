#include "PbmImage.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <cmath>
#define M_PI 3.14159265358979323846

PbmImage::PbmImage(const std::string& filename)
{
    std::ifstream input_stream(filename);

    if (input_stream.is_open()) // gdy plik zostal otwarty wykonujemy ponizsze operacje
    {
        ReadHeader(input_stream); // czytam naglowek pliku
        
        pixels = std::vector<uint8_t>(width * height); // rezerwujemy odpowiednia ilosc pamieci w wektorze

        ReadPixels(input_stream); // wczytujemy piksele z obrazu
    }
    else
    {
        std::cout << "Error. Unable to open " << filename << std::endl;
    }

    input_stream.close(); 
}

void PbmImage::PrintImage()
{
    std::cout << "P1\n";
    std::cout << width;
    std::cout << " ";
    std::cout << height << "\n";

    for (unsigned int i = 0; i < width*height; ++i)
    {
        std::cout << pixels[i] << " ";
        if (i % width) std::cout << "\n";
    }
}

void PbmImage::SaveImage(const std::string& filename)
{
    std::ofstream input_stream(filename.c_str(), std::ios::out | std::ios::binary);
    if (input_stream.is_open())
    {
        input_stream << "P1\n";
        input_stream << width;
        input_stream << " ";
        input_stream << height << "\n";

        short aux;
        for (unsigned int j = 0; j < height; j++)
        {
            for (unsigned int i = 0; i < width; ++i)
            {
                aux = pixels[j*width + i];
                input_stream << aux << " ";
            }
            input_stream << "\n";
        }
    }
    else
    {
        std::cout << "Error. Unable to open " << filename << std::endl;
    }
    input_stream.close();
}

void PbmImage::RotateImage(float angle)
{
    // tworzymy tymczasowy wektor ktory bedzie przechowywac przetworzony obraz
    std::vector<uint8_t>temp(width * height); 
    
    // konwertujemy stopnie na radiany,
    // funkcja takie jak sin czy cos ze standardowej biblioteki
    // jako parametr przyjmuja liczbe w radianach
    float _angle = (-angle * M_PI) / 180;
    
    // znajdujemy srodkowy piksel,
    // bedzie to pozycja wokol ktorej bedziemy wykonywac obrot 
    float xs = width / 2.0;
    float ys = height / 2.0;

     for (int y = 0; y < height; y++)
        {
         for (int x = 0; x < width; x++)
         {
             // wyznaczamy nowa pozycje piksela zgodnie z wzorem zawartym w instrukcji pdf
             int xp = int(cos(_angle) * (x - xs) - sin(_angle) * (y - ys) + xs);
             int yp = int(sin(_angle) * (x - xs) + cos(_angle) * (y - ys) + ys);
            
             // sprawdzamy ktore z pikseli powinny zostac umieszczone w obrazie wynikowym
             if ((0 <= xp && xp < width) && ((0 <= yp && yp < height)))
                 temp[y * width + x] = pixels[yp * width + xp];
         }
     } 
     // podmieniamy piksele obrazu
     pixels = temp;   
}

void PbmImage::ResizeImage(int new_width, int new_height)
{
    // tworzymy tymczasowy wektor o rozmiarze nowego obrazu
    std::vector<uint8_t>temp(new_width * new_height);

    // szukamy wspolczynnika skalowania dla osi poziomej i pionowej
    float x_scale = width / (float)new_width;
    float y_scale = height / (float)new_height;

    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            // szukamy jaki piksel w oryginalnym obrazie
            // bedzie odpowiadal pikselowi w nowym wiekszym obrazie
            // zgodnie z algorytmem nearest neighbor interpolation
            int xp = floor(x * x_scale);
            int yp = floor(y * y_scale);

            // umieszczamy piksel w przetworzonym obrazie
            temp[y * new_width + x] = pixels[yp * width + xp];
        }
    }

    // podmieniamy rozmiaru obrazu
    width = new_width;
    height = new_height;

    // podmieniamy piksele obrazu na nowe
    pixels = temp;
}

void PbmImage::ErodeImage()
{
    std::vector<uint8_t> temp(width * height);
    std::vector<uint8_t> mask = {0, 1, 0, 1, 1, 1, 0, 1, 0};

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (CheckBorderType(y, x) == LEFT_TOP) temp[y * width + x] = ((pixels[y * width + x] & 1) && (pixels[(y + 1) * width + x]) && (pixels[y * width + x + 1] & 1));

            if (CheckBorderType(y, x) == RIGHT_TOP) temp[y * width + x] = ((pixels[y * width + x] & 1) && (pixels[(y + 1) * width + x] & 1) && (pixels[y * width + x - 1] & 1));

            if (CheckBorderType(y, x) == LEFT_BOTTOM) temp[y * width + x] = ((pixels[y * width + x] & 1) && (pixels[(y - 1) * width + x] & 1) && (pixels[y * width + x + 1] & 1));

            if (CheckBorderType(y, x) == RIGHT_BOTTOM) temp[y * width + x] = ((pixels[y * width + x] & 1) && (pixels[(y - 1) * width + x] & 1) && (pixels[y * width + x - 1] & 1));

            if (CheckBorderType(y, x) == LEFT_EDGE) temp[y * width + x] = ((pixels[(y - 1) * width + x] & 1) && (pixels[y * width + x]) && (pixels[y * width + x + 1] & 1) && (pixels[(y + 1) * width + x] & 1));

            if (CheckBorderType(y, x) == TOP_EDGE) temp[y * width + x] = ((pixels[y * width + x - 1] & 1) && (pixels[y * width + x] & 1) && (pixels[y * width + x + 1] & 1) && (pixels[(y + 1) * width + x] & 1));

            if (CheckBorderType(y, x) == RIGHT_EDGE) temp[y * width + x] = ((pixels[(y - 1) * width + x] & 1) && (pixels[y * width + x] & 1) && (pixels[(y + 1) * width + x] & 1) && (pixels[y * width + x - 1] & 1));

            if (CheckBorderType(y, x) == BOTTOM_EDGE) temp[y * width + x] = ((pixels[y * width + x - 1] & 1) && (pixels[y * width + x] & 1) && (pixels[y * width + x + 1] & 1) && (pixels[(y - 1) * width + x] & 1));

            if (CheckBorderType(y, x) == CENTER) temp[y * width + x] = ((pixels[(y - 1) * width + x] & 1) && (pixels[y * width + x - 1] & 1) && (pixels[y * width + x] & 1) && (pixels[y * width + x + 1] & 1) && (pixels[(y + 1) * width + x] & 1));
        }
    }

    pixels = temp;
}

PbmImage::image_part PbmImage::CheckBorderType(const int& y, const int& x)
{
    // lewy gorny
    if (y == 0 && x == 0) return LEFT_TOP;

    // lewy dolny
    if (y == height - 1 && x == 0) return LEFT_BOTTOM;

    // prawy gorny
    if (y == 0 && x == width - 1) return LEFT_TOP;

    // prawy dolny
    if (y == height - 1 && x == width - 1) return LEFT_BOTTOM;
    
    // lewy brzeg
    if (y != 0 && y != (height - 1) && x == 0) return LEFT_EDGE;

    // gorny brzeg
    if (y == 0 && x != 0 && x != width - 1) return TOP_EDGE;

    // prawy brzeg
    if (y != 0 && y != (height - 1) && x == (width - 1)) return RIGHT_EDGE;

    // dolny brzeg
    if (y == (height - 1) && x != 0 && x != (width -1)) return BOTTOM_EDGE;

    return CENTER;
}

void PbmImage::DilateImage()
{

}

void PbmImage::ReadHeader(std::ifstream& input)
{
    std::string line;
    std::string rest;
    int step;
    std::string word;

    step = 0;

    while (1)
    {
        std::getline(input, line); 

        if (input.eof()) 
        {
            std::cout << "error" << std::endl;
            exit(1);
        }

        if (line[0] == '#') // jesli # to komentarz, wiec pomijamy linie do konca
        {
            continue;
        }

        if (step == 0)
        {
            extract_word(line, word, rest);

            if (len_trim(word) == 0)
                continue;
            
            line = rest;

            // sprawdzamy naglowek pliku, powinien skalda sie z dwoch znakow - P2
            if ((word[0] != 'P' && word[0] != 'p') ||
                word[1] != '2')
            {
                std::cout << "error" << std::endl;
                exit(1);
            }
            step = 1;
        }

        // pobieramy kolejny parametr - szerokosc obrazu z naglowka
        if (step == 1)
        {
            extract_word(line, word, rest);

            if (len_trim(word) == 0)
                continue;
            
            width = atoi(word.c_str());
            line = rest;
            step = 2;
        }

        // pobieramy kolejny parametr - wysokosc obrazu z naglowka
        if (step == 2)
        {
            extract_word(line, word, rest);

            if (len_trim(word) == 0)
                continue;
           
            height = atoi(word.c_str());
            line = rest;
            break;
        }

    }

    return;
}

void PbmImage::extract_word(std::string s, std::string& s1, std::string& s2)
{
    int i, mode, len;

    len = s.length();
    s1 = s2 = ""; 

    mode = 1;

    for (i = 0; i < len; i++)
    {
        if (mode == 1)
        {
            if (s[i] != ' ')
                mode = 2;
        }
        else if (mode == 2)
        {
            if (s[i] == ' ')
                mode = 3;
        }
        else if (mode == 3)
        {
            if (s[i] != ' ')
                mode = 4;
        }
        if (mode == 2)
        {
            s1 = s1 + s[i];
        }
        else if (mode == 4)
        {
            s2 = s2 + s[i];
        }
    }

    return;
}

void PbmImage::NegativeImage()
{
    std::for_each(pixels.begin(), pixels.end(), [&](short& x) { x = 1 - x; return x; });
}

int PbmImage::len_trim(std::string s)
{
    int n;

    n = s.length();

    while (0 < n)
    {
        if (s[n - 1] != ' ')
            return n;
        n = n - 1;
    }

    return n;
}

void PbmImage::ReadPixels(std::ifstream& input)
{
    int idx = 0;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            short px;
            input >> px;
            pixels[idx++] = px; 
        }
    }

    return;
}