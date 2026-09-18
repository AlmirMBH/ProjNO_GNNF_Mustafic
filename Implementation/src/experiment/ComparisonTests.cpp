#include "ComparisonTests.h"

#include "ExperimentRunner.h"

#include <fstream>
#include <iostream>
#include <string>

static bool csvHasExpectedSummaryColumns(const std::string& filePath, int expectedRowCount) {
    std::ifstream inputFile(filePath);
    std::string headerLine;
    std::getline(inputFile, headerLine);

    if (headerLine.find("gn_iterations") == std::string::npos
        || headerLine.find("newton_iterations") == std::string::npos
        || headerLine.find("methods_differ") == std::string::npos) {
        return false;
    }

    int rowCount = 0;
    std::string dataLine;
    while (std::getline(inputFile, dataLine)) {
        if (!dataLine.empty()) {
            rowCount++;
        }
    }

    return rowCount == expectedRowCount;
}

bool runComparisonTests() {
    std::vector<ExperimentMetrics> comparisonResults = runComparisonExperiments();
    std::vector<ComparisonSummaryRow> comparisonSummaryRows = buildComparisonSummary(comparisonResults);

    exportExperimentResultsToCsv(comparisonResults, "comparison_results.csv");
    exportComparisonSummaryToCsv(comparisonSummaryRows, "comparison_summary.csv");
    printComparisonSummary(comparisonSummaryRows);

    bool summaryCsvValid = csvHasExpectedSummaryColumns("comparison_summary.csv", 18);
    int methodsDifferCount = 0;
    for (size_t rowIndex = 0; rowIndex < comparisonSummaryRows.size(); rowIndex++) {
        if (comparisonSummaryRows[rowIndex].methodsDiffer) {
            methodsDifferCount++;
        }
    }

    std::cout << "comparison result_rows=" << comparisonResults.size()
              << " summary_rows=" << comparisonSummaryRows.size()
              << " summary_csv_valid=" << summaryCsvValid
              << " methods_differ_count=" << methodsDifferCount
              << std::endl;

    return summaryCsvValid
        && comparisonResults.size() == 36
        && comparisonSummaryRows.size() == 18
        && methodsDifferCount >= 1;
}
