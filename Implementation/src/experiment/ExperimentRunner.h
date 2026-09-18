#pragma once

#include "ExperimentConfig.h"
#include "ExperimentMetrics.h"

#include <string>
#include <vector>

ExperimentMetrics runExperiment(const ExperimentConfig& experimentConfig);
std::vector<ExperimentMetrics> runExperimentGrid();
std::vector<ExperimentMetrics> runComparisonExperiments();
std::vector<ComparisonSummaryRow> buildComparisonSummary(const std::vector<ExperimentMetrics>& experimentResults);
void exportExperimentResultsToCsv(const std::vector<ExperimentMetrics>& experimentResults, const std::string& filePath);
void exportComparisonSummaryToCsv(const std::vector<ComparisonSummaryRow>& comparisonSummaryRows, const std::string& filePath);
void printComparisonSummary(const std::vector<ComparisonSummaryRow>& comparisonSummaryRows);
