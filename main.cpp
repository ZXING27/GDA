#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <thread>   // for std::this_thread::sleep_for
#include <chrono>   // for std::chrono::seconds

struct PointData {
    std::string point_name;
    double red_price;
    int red_pips;
    double blue_price;
    int blue_pips;
};

int calculatePips(double prev, double curr) {
    return static_cast<int>(std::round((curr - prev) * 100));
}

bool isValidPriceInput(const std::string& input, double& value) {
    std::stringstream ss(input);
    ss >> value;
    return !ss.fail() && ss.eof() && value > 0;
}

void generateCSV(const std::vector<PointData>& points) {
    std::ofstream csv_file("graph_data.csv");
    csv_file << "Points,Red_Price,Red_Pips,Blue_Price,Blue_Pips\n";

    for(const auto& p : points) {
        csv_file << p.point_name << ","
                 << (p.red_price == 0 ? "" : std::to_string(p.red_price)) << ","
                 << (p.red_pips == 0 ? "" : std::to_string(p.red_pips)) << ","
                 << (p.blue_price == 0 ? "" : std::to_string(p.blue_price)) << ","
                 << (p.blue_pips == 0 ? "" : std::to_string(p.blue_pips)) << "\n";
    }

    std::cout << "Data saved to graph_data.csv\n";
}

int main() {
    std::vector<PointData> points;
    std::string inputStr;
    double previous_price = 0.0;
    double current_price = 0.0;
    std::string current_point;
    std::string previous_point;
    bool is_red_trend = false;
    bool first_point = true;
    std::string filename = "zigzag_pips_converted.csv";

    std::cout << std::fixed << std::setprecision(2);

        std::ifstream file("zigzag_pips_converted.csv");
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return 1;
        }
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);

            std::getline(ss, current_point, ',');
            
            while (true) {
                std::getline(ss, inputStr, ',');
                if (isValidPriceInput(inputStr, current_price)) break;
                std::cout << "Invalid input. Enter a positive numeric price: ";
            }

            if (!first_point) {
            is_red_trend = (current_price < previous_price);
            
                if (is_red_trend) {
                    // Add red data point
                    points.push_back({current_point, current_price, calculatePips(previous_price, current_price), 0.0, 0});
                    
                    // Add corresponding blue point with same price (if needed)
                    points.push_back({current_point, 0.0, 0, current_price, 0});
                } else {
                    // Add blue data point
                    points.push_back({current_point, 0.0, 0, current_price, calculatePips(previous_price, current_price)});
                    
                    // Add corresponding red point with same price
                    points.push_back({current_point, current_price, 0, 0.0, 0});
                }
            } else {
                // First point - just record the price
                points.push_back({current_point, current_price, 0, 0.0, 0});
                first_point = false;
            }

            previous_price = current_price;
            previous_point = current_point; // Update previous point for next iteration

        }

    file.close();

    for (int i = 5; i > 0; --i) {
        std::cout << i << "..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // sleeps for 1 second
    }

    generateCSV(points);
    return 0;
}