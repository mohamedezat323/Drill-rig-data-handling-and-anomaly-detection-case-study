#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct WellData
{
    std::string well_id;
    std::string depth_string;
    std::string pressure;
    std::string temperature;
};

int main()
{
    std::ifstream input("input.csv");
    std::ofstream output("output.csv");
    std::string line;
    std::vector<WellData> data;

    // Copy the header
    if (std::getline(input, line))
    {
        output << "well_id,depth_feet,pressure_psi,temperature_c\n";
    }

    // Read all rows first
    while (std::getline(input, line))
    {
        std::stringstream ss(line);

        WellData row;

        std::getline(ss, row.well_id, ',');
        std::getline(ss, row.depth_string, ',');
        std::getline(ss, row.pressure, ',');
        std::getline(ss, row.temperature, ',');

        double depth = std::stod(row.depth_string);

        // Remove negative depths
        if (depth < 0)
        {
            std::cout << "Removing invalid depth: "
                      << depth << " ft\n";

            continue;
        }

        data.push_back(row);
    }

    // Process pressure values
    for (size_t i = 0; i < data.size(); i++)
    {
        try
        {
            // Try to convert pressure to a number
            std::stod(data[i].pressure);
        }
        catch (...)
        {
            // Pressure is not a number
            if (i > 0 && i < data.size() - 1)
            {
                try
                {
                    double pressure_before =
                        std::stod(data[i - 1].pressure);

                    double pressure_after =
                        std::stod(data[i + 1].pressure);

                    double average =
                        (pressure_before + pressure_after) / 2.0;

                    data[i].pressure = std::to_string(average);

                    std::cout << "Replacing invalid pressure at depth "
                              << data[i].depth_string
                              << " with average: "
                              << average << " psi\n";
                }
                catch (...)
                {
                    std::cout << "Could not calculate pressure average at depth "
                              << data[i].depth_string << '\n';
                }
            }
        }
    }

    // Write processed data
    for (const auto& row : data)
    {
        double temperature_f = std::stod(row.temperature);

        double temperature_c =
            (temperature_f - 32.0) * 5.0 / 9.0;

        output << row.well_id << ','
               << row.depth_string << ','
               << row.pressure << ','
               << temperature_c << '\n';
    }

    std::cout << "Filtering complete.\n";

    return 0;
}