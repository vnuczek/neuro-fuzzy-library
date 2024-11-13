/** @file */

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>
#include <thread>

#include "../experiments/exp-027.h"

#include "../readers/reader-complete.h"
#include "../readers/reader-incomplete.h"
#include "../readers/train_test_model.h"
#include "../readers/train_validation_test_model.h"
#include "../common/data-modifier-marginaliser.h"
#include "../common/data-modifier-normaliser.h"
#include "../common/data-modifier-imputer.h"
#include "../common/data-modifier-imputer-average.h"
#include "../common/data-modifier-imputer-median.h"
#include "../common/data-modifier-imputer-knn-average.h"
#include "../common/data-modifier-imputer-knn-median.h"
#include "../common/data-modifier-imputer-values-from-knn.h"
#include "../common/data_modifier_incompleter_random_without_last.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../partitions/fcm.h"

#include "../tnorms/t-norm-product.h"
#include "../tnorms/t-norm-min.h"
#include "../tnorms/t-norm-lukasiewicz.h"
#include "../tnorms/t-norm-einstein.h"

#include "../implications/imp-reichenbach.h"

#include "../neuro-fuzzy/neuro-fuzzy-system.h"
#include "../neuro-fuzzy/annbfis.h"
#include "../neuro-fuzzy/tsk.h"

#include "../auxiliary/utility-math.h"

ksi::exp_027::exp_027(int num_rules, int num_clustering_iters, int num_tuning_iters, int num_dataparts, int k_val, double eta_val, bool normalisation) :
	NUMBER_OF_RULES(num_rules),
	NUMBER_OF_CLUSTERING_ITERATIONS(num_clustering_iters),
	NUMBER_OF_TUNING_ITERATIONS(num_tuning_iters),
	NUMBER_OF_DATAPARTS(num_dataparts),
	k(k_val),
	ETA(eta_val),
	NORMALISATION(normalisation)
{}

void ksi::exp_027::processDataset(const std::filesystem::directory_entry& entry) {
	try {
		RESULTS results;
	    RESULTS_GR results_gr;

	    std::filesystem::path file_path = entry.path();
	    std::string datasetName = file_path.stem().string();
	    std::filesystem::path datasetResultDir = resultDir / datasetName;
	    std::filesystem::create_directories(datasetResultDir);

	    std::vector<int> num_granules = (datasetName == "BoxJ290") ? std::vector<int>{2, 3, 5, 10} : std::vector<int>{ 2, 3, 5, 10, 15, 20, 25 };

	    for (int iteration = 0; iteration < 13; iteration++) {
	        thdebugid(entry, iteration);

	        reader_complete DataReader;
	        ksi::train_test_model tt(DataReader);
	        auto data = tt.read_file(file_path);

	        data_modifier_normaliser normaliser;
	        normaliser.modify(data);
	        tt.split(data, NUMBER_OF_DATAPARTS);

	        std::vector<std::unique_ptr<ksi::neuro_fuzzy_system>> nfss;
	        ksi::t_norm_product tnorm;
	        ksi::imp_reichenbach implication;
	        nfss.push_back(std::make_unique<ksi::tsk>(NUMBER_OF_RULES, NUMBER_OF_CLUSTERING_ITERATIONS, NUMBER_OF_TUNING_ITERATIONS, ETA, NORMALISATION, tnorm));
	        nfss.push_back(std::make_unique<ksi::annbfis>(NUMBER_OF_RULES, NUMBER_OF_CLUSTERING_ITERATIONS, NUMBER_OF_TUNING_ITERATIONS, ETA, NORMALISATION, tnorm, implication));

	        std::vector<std::unique_ptr<ksi::data_modifier>> imputers;
	        imputers.push_back(std::make_unique<ksi::data_modifier_imputer_average>());
	        imputers.push_back(std::make_unique<ksi::data_modifier_imputer_knn_median>(k));
	        /*imputers.push_back(std::make_unique<ksi::data_modifier_imputer_knn_average>(k));
	        imputers.push_back(std::make_unique<ksi::data_modifier_imputer_values_from_knn>(k));
	        imputers.push_back(std::make_unique<ksi::data_modifier_marginaliser>());*/

	        // for (const auto missing_ratio : { 0.01, 0.02, 0.05, 0.10, 0.20, 0.50, 0.75 }) {
	        for (const auto missing_ratio : { 0.05 }) {
	            // thdebugid(entry, missing_ratio); 

	            data_modifier_incompleter_random_without_last incomplete(missing_ratio);
	            for (auto [train, test] : tt) {
	                // thdebugid(entry, __LINE__);

	                incomplete.modify(test);
	                for (const auto& imputer : imputers) {
	                    // thdebugid(entry, imputer->getName());

	                    imputer->modify(train);
	                    std::string output_name = std::format("{}-{}-r-{}.txt", imputer->getName(), missing_ratio, iteration);
	                    for (const auto& nfs : nfss) {
	                        // thdebugid(entry, nfs->get_brief_nfs_name());

	                        std::string output_file = datasetResultDir.string() + "/" + nfs->get_brief_nfs_name() + "-" + output_name;
	                        auto result = nfs->experiment_regression(train, test, output_file);
	                        results[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()].train.push_back(result.rmse_train);
	                        results[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()].test.push_back(result.rmse_test);
	                    }
	                }

	                for (const auto granules : num_granules) {
	                    // thdebugid(entry, granules);

	                    ksi::fcm test_partitioner(granules, NUMBER_OF_CLUSTERING_ITERATIONS);
	                    std::unique_ptr<ksi::data_modifier> imputer = std::make_unique < data_modifier_imputer_granular>(test_partitioner, tnorm);
	                    imputer->modify(train);

	                    std::string output_name = std::format("{}-{}-g-{}-r-{}.txt", imputer->getName(), missing_ratio, granules, iteration);
	                    for (const auto& nfs : nfss) {
	                        // thdebugid(entry, nfs->get_brief_nfs_name());

	                        std::string output_file = datasetResultDir.string() + "/" + nfs->get_brief_nfs_name() + "-" + output_name;
	                        auto result = nfs->experiment_regression(train, test, output_file);
	                        results_gr[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()][granules].train.push_back(result.rmse_train);
	                        results_gr[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()][granules].test.push_back(result.rmse_test);
	                    }
	                }
	            }
	        }
	    }

	    writeResultsToFile(datasetResultDir, datasetName, results, results_gr);
}
CATCH;
}

void ksi::exp_027::writeResultsToFile(const std::filesystem::path& datasetResultDir, const std::string& datasetName, RESULTS results, RESULTS_GR results_gr) {
	try {
		std::filesystem::path results_file_path = datasetResultDir / (datasetName + "_results.txt");
		std::ofstream resultsStream(results_file_path);
		if (resultsStream.is_open()) {
			for (const auto& [datasetName, datasetResult] : results)
			{
				resultsStream << "\t" << "Dataset: " << datasetName << std::endl;
				for (const auto& [nfsName, nfsResult] : datasetResult)
				{
					resultsStream << "\t\t" << "NFS: " << nfsName << std::endl;
					for (const auto& [missing_ratio, missingRatioResult] : nfsResult)
					{
						resultsStream << "\t\t\t" << "Missing Ratio: " << missing_ratio << std::endl;
						for (const auto& [imputerName, imputerResult] : missingRatioResult)
						{
							resultsStream << "\t\t\t\t" << "Imputer: " << imputerName << std::endl;
							resultsStream << "\t\t\t\t\t" << "Train Values: " << std::endl;
							for (const auto& train_val : imputerResult.train)
							{
								resultsStream << "\t\t\t\t\t" << train_val << std::endl;
							}
							auto [train_mean, train_dev] = ksi::utility_math::getMeanAndStandardDeviation(imputerResult.train.begin(), imputerResult.train.end());
							resultsStream << "\t\t\t\t\t" << "Train Average +- std_dev: " << train_mean << ' ' << train_dev << std::endl;

							resultsStream << "\t\t\t\t\t" << "Train Values: " << std::endl;
							for (const auto& test_val : imputerResult.test)
							{
								resultsStream << "\t\t\t\t\t" << test_val << std::endl;
							}
							auto [test_mean, test_dev] = ksi::utility_math::getMeanAndStandardDeviation(imputerResult.test.begin(), imputerResult.test.end());
							resultsStream << "\t\t\t\t\t" << "Test Average +- std_dev: " << test_mean << ' ' << test_dev << std::endl;
						}
					}
				}
			}
			resultsStream.close();
		}
		else {
			std::cerr << "Error: Unable to open file " << results_file_path << " for writing results!" << std::endl;
		}

		std::filesystem::path results_gr_file_path = datasetResultDir / (datasetName + "_resultsGr.txt");
		std::ofstream resultsGrStream(results_file_path);
		if (resultsStream.is_open()) {
			for (const auto& [datasetName, datasetResult] : results_gr)
			{
				resultsStream << "\t" << "Dataset: " << datasetName << std::endl;
				for (const auto& [nfsName, nfsResult] : datasetResult)
				{
					resultsStream << "\t\t" << "NFS: " << nfsName << std::endl;
					for (const auto& [missing_ratio, missingRatioResult] : nfsResult)
					{
						resultsStream << "\t\t\t" << "Missing Ratio: " << missing_ratio << std::endl;
						for (const auto& [imputerName, imputerResult] : missingRatioResult)
						{
							resultsStream << "\t\t\t\t" << "Imputer: " << imputerName << std::endl;
							for (const auto& [granulesNumber, granulesResults] : imputerResult) {
								resultsStream << "\t\t\t\t\t" << "Granules Number: " << granulesNumber << std::endl;
								resultsStream << "\t\t\t\t\t\t" << "Train Values: " << std::endl;
								for (const auto& train_val : granulesResults.train)
								{
									resultsStream << "\t\t\t\t\t\t" << train_val << std::endl;
								}
								auto [train_mean, train_dev] = ksi::utility_math::getMeanAndStandardDeviation(granulesResults.train.begin(), granulesResults.train.end());
								resultsStream << "\t\t\t\t\t\t" << "Train Average +- std_dev: " << train_mean << ' ' << train_dev << std::endl;

								resultsStream << "\t\t\t\t\t\t" << "Train Values: " << std::endl;
								for (const auto& test_val : granulesResults.test)
								{
									resultsStream << "\t\t\t\t\t\t" << test_val << std::endl;
								}
								auto [test_mean, test_dev] = ksi::utility_math::getMeanAndStandardDeviation(granulesResults.test.begin(), granulesResults.test.end());
								resultsStream << "\t\t\t\t\t\t" << "Test Average +- std_dev: " << test_mean << ' ' << test_dev << std::endl;
							}
						}
					}
				}
			}
			resultsStream.close();
		}
		else {
			std::cerr << "Error: Unable to open " << results_gr_file_path << " file for writing results." << std::endl;
		}
	}
	CATCH;
}

void ksi::exp_027::execute()
{
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(dataDir)) {
            thdebugid(entry, entry);

            if (entry.is_regular_file()) {
                threads.emplace_back(&ksi::exp_027::processDataset, this, entry);
            }
        }

    	thdebug(threads.size());
	    for (auto& thread : threads) {
			if (thread.joinable()) {
				thread.join();
			}
	    }
	    thdebug("dze end");
       }
       CATCH;

       return;
    }
