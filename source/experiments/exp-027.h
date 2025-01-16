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

    /** EXPERIMENT 027  <br/>

    granular imputation of missing values

    @date 2024-09-23

    */
    class exp_027 : virtual public experiment
    {
    private:
        const std::string exp_number = "exp-027";

        const std::filesystem::path dataDir = "../data/" + exp_number + "/nowe/";
        const std::filesystem::path resultDir = "../results/" + exp_number;

        const int NUMBER_OF_RULES;
        const int NUMBER_OF_CLUSTERING_ITERATIONS;
        const int NUMBER_OF_TUNING_ITERATIONS;
        const int NUMBER_OF_DATAPARTS;
        const int k;
        const double ETA;
        const bool NORMALISATION;
        const int ITERATIONS;

        std::vector<std::thread> threads;

   public:
    	exp_027(
           const int num_rules = 10,
           const int num_clustering_iters = 100,
           const int num_tuning_iters = 100,
           const int num_dataparts = 10,
           const int k_val = 5,
           const double eta_val = 0.001,
           const bool normalisation = false,
           // const int iterations = 13
           const int iterations = 1 ///<-- debug
       );

        /** The method executes an experiment. */
		virtual void execute();

    private:
        /**
		 * @brief Processes a dataset by performing data imputation and model training.
		 *
		 * This method reads the dataset, applies various data preprocessing techniques,
		 * trains different neuro-fuzzy models, and evaluates their performance on imputed data.
		 *
		 * @param entry A directory entry representing the dataset file to process.
		 *
		 * @author Konrad Wnuk
		 * @date 2024-11-23
		 */
        void processDataset(const std::filesystem::directory_entry& entry);

        std::pair<ksi::RESULTS, ksi::RESULTS_GR> runIteration(const std::filesystem::path& file_path, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const std::vector<int>& num_granules, const int iteration);

        ksi::RESULTS mergeResults(const std::vector<RESULTS>& resultsVector);

        ksi::RESULTS_GR mergeResultsGr(const std::vector<RESULTS_GR>& resultsGrVector);

        std::pair<ksi::RESULTS, ksi::RESULTS_GR> runMissingRatio(const std::filesystem::path& file_path, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const std::vector<int>& num_granules, const int iteration, const double missing_ratio);

         ksi::RESULTS runComplete(const std::filesystem::path& file_path, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const int iteration);
        /**
         * @brief Writes experiment results to a file.
         *
         * This method saves the results of the experiments (e.g., training and test errors)
         * to a specified file in the results directory.
         *
         * @param datasetResultDir The directory where the results file will be saved.
         * @param datasetName The name of the dataset being processed.
         * @param results The container storing training and test errors for each imputation method and missing ratio.
         * @param results_gr The container storing training and test errors for granular imputation with varying numbers of granules.
         *
         * @author Konrad Wnuk
		 * @date 2024-11-23
         */
        void writeResultsToFile(const std::filesystem::path& datasetResultDir, const std::string& datasetName, const RESULTS& results, const RESULTS_GR& results_gr);
    };
}

#endif 
