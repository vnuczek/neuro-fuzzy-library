/** @file */

#ifndef EXP_027_H
#define EXP_027_H

#include "../experiments/experiment.h"

#include <filesystem>
#include <map>
#include <thread>
#include <vector>
#include <string>

namespace ksi
{
    struct errors 
    {
        std::vector<double> train;
        std::vector<double> test;
    };

    typedef std::map < std::string, std::map<std::string, std::map<double, std::map<std::string, errors>>>> RESULTS;
    typedef std::map < std::string, std::map<std::string, std::map<double, std::map<std::string, std::map<int, errors>>>>> RESULTS_GR;

    /** EXPERIMENT 127  <br/>

    granular imputation of missing values

    @date 2025-02-28

    */
    class exp_127 : virtual public experiment
    {
    private:
        const std::string exp_number = "exp-127";

        const std::filesystem::path dataDir = "../data/" + exp_number;
        const std::filesystem::path resultDir = "../results/" + exp_number;

        const int NUMBER_OF_RULES;
        const int NUMBER_OF_CLUSTERING_ITERATIONS;
        const int NUMBER_OF_TUNING_ITERATIONS;
        const int NUMBER_OF_DATAPARTS;
        const int k;
        const double ETA;
        const bool NORMALISATION;
        const int ITERATIONS;
        std::vector<int> num_granules;

        std::vector<std::thread> threads;

   public:
    	exp_127(
           const int num_rules = 10,
           const int num_clustering_iters = 100,
           const int num_tuning_iters = 100,
           const int num_dataparts = 10,
           const int k_val = 5,
           const double eta_val = 0.001,
           const bool normalisation = false,
           // const int iterations = 13
           const int iterations = 1, ///<-- debug
            const std::vector<int>& granules = { 2, 3, 5, 10, 20, 25 }
       );

        /** The method executes an experiment. */
		virtual void execute();

    private:
        void processDatasetFolder(const std::filesystem::directory_entry& datasetFolder);

        std::pair<ksi::RESULTS, ksi::RESULTS_GR> runIteration(const std::filesystem::directory_entry& datasetFolder, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const int iteration);

        std::pair<ksi::RESULTS, ksi::RESULTS_GR> ksi::exp_127::runMissingRatio(const std::filesystem::directory_entry& missingRatioDir, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const int iteration, const double missing_ratio);

        std::pair<ksi::RESULTS, ksi::RESULTS_GR> ksi::exp_127::runCV(const std::filesystem::directory_entry& cvDir, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const int iteration, const double missing_ratio, const int cvNumber);

        ksi::RESULTS mergeResults(const std::vector<RESULTS>& resultsVector);

        ksi::RESULTS_GR mergeResultsGr(const std::vector<RESULTS_GR>& resultsGrVector);

        void writeResultsGrToFile(const std::filesystem::path& datasetResultDir, const std::string& datasetName, const RESULTS_GR& results_gr);

    	void writeResultsToFile(const std::filesystem::path& datasetResultDir, const std::string& datasetName, const RESULTS& results);
    };
}

#endif 
