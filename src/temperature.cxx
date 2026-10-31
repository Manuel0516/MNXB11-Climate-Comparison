#include "temperature.h"

#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Read the temperature CSV file
std::vector<std::vector<std::string>> parseTemperatureCSV(
    const std::string& filename
) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open temperature file: " + filename);
    }

    std::vector<std::vector<std::string>> rows;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string date, time, temperature, quality;

        // Separate the four CSV columns
        std::getline(ss, date, ',');
        std::getline(ss, time, ',');
        std::getline(ss, temperature, ',');
        std::getline(ss, quality, ',');

        rows.push_back({date, time, temperature, quality});
    }

    return rows;
}

//Calculate the mean temperature for each day
std::vector<DailyTemperature> calculateDailyMean(
    const std::vector<std::vector<std::string>>& data,
    int firstYear,
    int lastYear
) {
    struct DayData {
        double sum = 0.0;
        int nObservations = 0;
        int nGood = 0;
        int nYellow = 0;
    };

    //Collect observations from the same date together
    std::map<std::string, DayData> dailyData;

    for (const auto& row : data) {
        if (row.size() < 4) continue;
        
        try {
            const std::string& date = row[0];
            int year = std::stoi(date.substr(0, 4));
            
            if (year < firstYear || year > lastYear) continue;
            
            double temperature = std::stod(row[2]);

            DayData& day = dailyData[date];
            
            day.sum += temperature;
            day.nObservations++;

            // Count observations by quality
            if (row[3] == "G") day.nGood++;
            if (row[3] == "Y") day.nYellow++;
            
        } catch (const std::exception&) {
            continue;
        }
    }

    std::vector<DailyTemperature> result;

    // Calculate the mean for each date    
    for (const auto& [date, day] : dailyData) {

        // Ignore dates with fewer than two observations
        if (day.nObservations < 2) continue;
        
        DailyTemperature dailyTemperature;
        dailyTemperature.date = date;
        dailyTemperature.meanTemperature =
            day.sum / day.nObservations;
        dailyTemperature.nObservations = day.nObservations;
        dailyTemperature.nGood = day.nGood;
        dailyTemperature.nYellow = day.nYellow;

        result.push_back(dailyTemperature);
    }

    return result;
}

//Calculate the mean temperature for each day of the year
TemperatureProfile calculateMeanTemperatureByDayOfYear(
    const std::vector<DailyTemperature>& dailyData
) {
    // Each position stores the daily means for one calendar day
    std::vector<std::vector<double>> values(365);

    // Number of days before each month in a non-leap year.
    const int daysBeforeMonth[] = {
        0, 31, 59, 90, 120, 151,
        181, 212, 243, 273, 304, 334
    };
    
    for (const auto& day : dailyData) {
        try {
            int year = std::stoi(day.date.substr(0, 4));
            int month = std::stoi(day.date.substr(5, 2));
            int dayOfMonth = std::stoi(day.date.substr(8, 2));

            // Avoid invalid month values before accessing the array.
            if (month < 1 || month > 12) continue;
            if (dayOfMonth < 1 || dayOfMonth > 31) continue;

            // Check whether the year contains February 29
            bool leapYear =
                (year % 4 == 0 && year % 100 != 0) ||
                (year % 400 == 0);

            // Exclude February 29 to use a common 365-day calendar
            if (leapYear && month == 2 && dayOfMonth == 29) {
                continue;
            }

            int dayOfYear =
                daysBeforeMonth[month - 1] + dayOfMonth;
            
            // Shift dates after February 29 back by one day
            if (leapYear && month > 2) {
                dayOfYear--;
            }
            
            values[dayOfYear - 1].push_back(day.meanTemperature);

        } catch (const std::exception&) {
            continue;
        }
    }

    TemperatureProfile result;

    for (int i = 0; i < 365; ++i) {
        if (values[i].empty()) continue;

        double sum = 0.0;

        for (double temperature : values[i]) {
            sum += temperature;
        }

        result.days.push_back(i + 1);
        result.meanValues.push_back(sum / values[i].size());
    }

    return result;
}

