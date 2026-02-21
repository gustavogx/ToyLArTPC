#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <fstream>
#include <iomanip>
#include <png.h>

class SimpleImage {
public:
    int width, height;
    std::vector<unsigned char> data; // RGB values

    SimpleImage(int w, int h) : width(w), height(h) {
        data.resize(w * h * 3, 0);
    }

    void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            int idx = (y * width + x) * 3;
            data[idx] = r;
            data[idx + 1] = g;
            data[idx + 2] = b;
        }
    }

    void savePNG(const std::string& filename) {
        FILE *fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }

        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) {
            fclose(fp);
            return;
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_write_struct(&png, nullptr);
            fclose(fp);
            return;
        }

        if (setjmp(png_jmpbuf(png))) {
            png_destroy_write_struct(&png, &info);
            fclose(fp);
            return;
        }

        png_init_io(png, fp);

        // Write header
        png_set_IHDR(
            png,
            info,
            width, height,
            8,
            PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
        );
        png_write_info(png, info);

        // Write image data
        std::vector<png_bytep> row_pointers(height);
        for (int y = 0; y < height; y++) {
            row_pointers[y] = &data[y * width * 3];
        }

        png_write_image(png, row_pointers.data());
        png_write_end(png, nullptr);

        png_destroy_write_struct(&png, &info);
        fclose(fp);
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <root_file> [output_dir]" << std::endl;
        return 1;
    }

    std::string rootFile = argv[1];
    std::string outputDir = (argc > 2) ? argv[2] : "output_images";

    // Create output directory
    std::string mkdirCmd = "mkdir -p " + outputDir;
    system(mkdirCmd.c_str());

    // Open ROOT file
    TFile *file = TFile::Open(rootFile.c_str());
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open ROOT file: " << rootFile << std::endl;
        return 1;
    }

    TTree *tree = (TTree*)file->Get("PhotonCounts");
    if (!tree) {
        std::cerr << "Error: PhotonCounts tree not found in ROOT file" << std::endl;
        file->Close();
        return 1;
    }

    long long numEvents = tree->GetEntries();
    std::cout << "Found " << numEvents << " events" << std::endl;

    // Read sensor data (stored as integers in ROOT file)
    std::vector<int> sensorValues(25); // 5x5 grid
    std::vector<TBranch*> branches(25);

    // Set up branches
    for (int i = 0; i < 25; i++) {
        std::string branchName = "sensor_" + std::to_string(i);
        branches[i] = tree->GetBranch(branchName.c_str());
        if (!branches[i]) {
            std::cerr << "Error: Branch " << branchName << " not found" << std::endl;
            file->Close();
            return 1;
        }
        branches[i]->SetAddress(&sensorValues[i]);
    }

    // Find min and max values for normalization
    int minVal = 2147483647, maxVal = -2147483647;
    for (long long entry = 0; entry < numEvents; entry++) {
        for (int i = 0; i < 25; i++) {
            branches[i]->GetEntry(entry);
        }
        for (int i = 0; i < 25; i++) {
            minVal = std::min(minVal, sensorValues[i]);
            maxVal = std::max(maxVal, sensorValues[i]);
        }
    }

    std::cout << "Sensor value range: [" << minVal << ", " << maxVal << "]" << std::endl;

    // Generate images for each event
    for (long long entry = 0; entry < numEvents; entry++) {
        // Read data for this event
        for (int i = 0; i < 25; i++) {
            branches[i]->GetEntry(entry);
        }

        // Create image (upscale 5x5 to 250x250 for visibility)
        SimpleImage img(250, 250);
        int pixelSize = 50; // 250/5

        // Fill image with sensor data
        for (int sensor = 0; sensor < 25; sensor++) {
            // Map sensor index to 5x5 grid
            int sensorX = sensor % 5;
            int sensorY = sensor / 5;

            // Normalize value to 0-1 range
            int val = sensorValues[sensor];
            float normalized = (maxVal > minVal) ? (float)(val - minVal) / (float)(maxVal - minVal) : 0.0f;
            unsigned char intensity = (unsigned char)(normalized * 255);

            // Create heatmap color (blue -> cyan -> green -> yellow -> red)
            unsigned char r, g, b;
            if (normalized < 0.25) {
                r = 0;
                g = (unsigned char)(normalized * 4 * 255);
                b = 255;
            } else if (normalized < 0.5) {
                r = 0;
                g = 255;
                b = (unsigned char)((1 - (normalized - 0.25) * 4) * 255);
            } else if (normalized < 0.75) {
                r = (unsigned char)((normalized - 0.5) * 4 * 255);
                g = 255;
                b = 0;
            } else {
                r = 255;
                g = (unsigned char)((1 - (normalized - 0.75) * 4) * 255);
                b = 0;
            }

            // Draw pixel block
            for (int py = 0; py < pixelSize; py++) {
                for (int px = 0; px < pixelSize; px++) {
                    int imgX = sensorX * pixelSize + px;
                    int imgY = sensorY * pixelSize + py;
                    img.setPixel(imgX, imgY, r, g, b);
                }
            }
        }

        // Save image
        char filename[256];
        snprintf(filename, sizeof(filename), "%s/event_%06lld.png", outputDir.c_str(), entry);
        img.savePNG(filename);
        std::cout << "Saved " << filename << std::endl;

        if ((entry + 1) % 100 == 0) {
            std::cout << "  Processed " << (entry + 1) << "/" << numEvents << " events" << std::endl;
        }
    }

    std::cout << "Image generation complete! Output in: " << outputDir << std::endl;
    file->Close();
    return 0;
}
