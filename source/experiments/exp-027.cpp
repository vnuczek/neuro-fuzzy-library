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

void ksi::exp_027::execute()
{
    try
    {
        std::string exp_number = "exp-027";

        std::filesystem::path dataDir = "../data/" + exp_number;
        std::filesystem::path resultDir = "../results/" + exp_number;

        const int NUMBER_OF_RULES = 5;
        const int NUMBER_OF_CLUSTERING_ITERATIONS = 100;
        const int NUMBER_OF_TUNING_ITERATIONS = 100;
		const int NUMBER_OF_DATAPARTS = 10;
        const int k = 5;
        const double ETA = 0.001;

        const bool NORMALISATION = false;

        struct errors
        {
			std::vector<double> train;
			std::vector<double> test;
        };
        typedef std::map < std::string, std::map<std::string, std::map<double, std::map<std::string, errors>>>> RESULTS;
        typedef std::map < std::string, std::map<std::string, std::map<double, std::map<std::string, std::map<int, errors>>>>> RESULTS_GR;
        
        std::vector<std::thread> threads;

        std::vector<RESULTS> resultsVec;
		std::vector<RESULTS_GR> resultsGrVec;
        
        for (const auto& entry : std::filesystem::directory_iterator(dataDir)) {
                if (entry.is_regular_file())
                {
                    //threads.emplace_back([=](const std::filesystem::directory_entry threadEntry) {
                        RESULTS results;
                        RESULTS_GR results_gr;

                        //std::filesystem::path file_path = threadEntry.path();
                        std::filesystem::path file_path = entry.path();
                        std::string datasetName = file_path.stem().string();
                        std::filesystem::path datasetResultDir = resultDir / datasetName;
                        std::filesystem::create_directories(datasetResultDir);

                        std::vector<int> num_granules;

                        if (datasetName == "BoxJ290") {
                            num_granules = { 2, 3, 5, 10 };
                        }
                        else {
                            num_granules = { 2, 3, 5, 10, 15, 20, 25 };
                        }

                       // for (int iteration = 0; iteration < 13; iteration++) {
                        for (int iteration = 0; iteration < 3; iteration++) {

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
                            imputers.push_back(std::make_unique<ksi::data_modifier_imputer_knn_average>(k));
                            imputers.push_back(std::make_unique<ksi::data_modifier_imputer_values_from_knn>(k));
                            imputers.push_back(std::make_unique<ksi::data_modifier_marginaliser>());

                            for (const auto missing_ratio : { 0.01, 0.02, 0.05, 0.10, 0.20, 0.50, 0.75 }) {
                                data_modifier_incompleter_random_without_last incomplete(missing_ratio);

                                for (auto [train, test] : tt) {
                                    incomplete.modify(test);

                                    for (auto& imputer : imputers) {
                                        imputer->modify(train);

                                        std::string output_name = imputer->getName() + '-' + std::to_string(missing_ratio) + "-r-" + std::to_string(iteration) + ".txt";

                                        for (const auto& nfs : nfss)
                                        {
                                            std::string output_file = datasetResultDir.string() + "/" + nfs->get_brief_nfs_name() + "-" + output_name;
                                            auto result = nfs->experiment_regression(train, test, output_file);
                                            results[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()].train.push_back(result.rmse_train);
                                            results[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()].test.push_back(result.rmse_test);
                                        }
                                    }

                                    for (const auto granules : num_granules) {
                                        ksi::fcm test_partitioner(granules, NUMBER_OF_CLUSTERING_ITERATIONS);
                                        std::unique_ptr<ksi::data_modifier> imputer = std::make_unique < data_modifier_imputer_granular>(test_partitioner, tnorm);
                                        imputer->modify(train);

                                        std::string output_name = std::format("{}-{}-g-{}-r-{}.txt", imputer->getName(), missing_ratio, granules, iteration);
                                        for (const auto& nfs : nfss)
                                        {
                                            std::string output_file = datasetResultDir.string() + "/" + nfs->get_brief_nfs_name() + "-" + output_name;
                                            auto result = nfs->experiment_regression(train, test, output_file);
                                            results_gr[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()][granules].train.push_back(result.rmse_train);
                                            results_gr[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()][granules].test.push_back(result.rmse_test);
                                        }
                                    }
                                }
                            }

                            std::filesystem::path results_file_path = datasetResultDir / (datasetName + "_" + std::to_string(iteration) + "_results.txt");
                            std::ofstream results_file(results_file_path);
                            if (results_file.is_open()) {
                                results_file << "Iteration: " << iteration << std::endl;
                                for (const auto& [datasetName, datasetResult] : results)
                                {
                                    results_file << "\t" << "Dataset: " << datasetName << std::endl;
                                    for (const auto& [nfsName, nfsResult] : datasetResult)
                                    {
                                        results_file << "\t\t" << "NFS: " << nfsName << std::endl;
                                        for (const auto& [missing_ratio, missingRatioResult] : nfsResult)
                                        {
                                            results_file << "\t\t\t" << "Missing Ratio: " << missing_ratio << std::endl;
                                            for (const auto& [imputerName, imputerResult] : missingRatioResult)
                                            {
                                                results_file << "\t\t\t\t" << "Imputer: " << imputerName << std::endl;
                                                results_file << "\t\t\t\t\t" << "Train Values: " << std::endl;
                                                for (const auto& train_val : imputerResult.train)
                                                {
                                                    results_file << "\t\t\t\t\t" << train_val << std::endl;
                                                }
                                                results_file << "\t\t\t\t\t" << "Train Average +- std_dev: ";
                                                auto [train_mean, train_dev] = ksi::utility_math::getMeanAndStandardDeviation(imputerResult.train.begin(), imputerResult.train.end());
                                                results_file << train_mean << ' ' << train_dev << std::endl;
                                                results_file << "\t\t\t\t\t" << "Train Values: " << std::endl;
                                                for (const auto& test_val : imputerResult.test)
                                                {
                                                    results_file << "\t\t\t\t\t" << test_val << std::endl;
                                                }
                                                results_file << std::endl;
                                                results_file << "\t\t\t\t\t" << "Test Average +- std_dev: ";
                                                auto [test_mean, test_dev] = ksi::utility_math::getMeanAndStandardDeviation(imputerResult.test.begin(), imputerResult.test.end());
                                            	results_file << test_mean << ' '<< test_dev << std::endl;
                                            }
                                        }
                                    }
                                }
                                results_file.close();
                            }
                            else {
                                std::cerr << "Error: Unable to open file " << results_file_path << " for writing results!" << std::endl;
                            }

                            std::filesystem::path results_gr_file_path = datasetResultDir / (datasetName + "_" + std::to_string(iteration) + "_results.txt");
                            std::ofstream results_gr_file(results_file_path);
                            if (results_file.is_open()) {
                                results_file << "Iteration: " << iteration << std::endl;
                                for (const auto& [datasetName, datasetResult] : results_gr)
                                {
                                    results_file << "\t" << "Dataset: " << datasetName << std::endl;
                                    for (const auto& [nfsName, nfsResult] : datasetResult)
                                    {
                                        results_file << "\t\t" << "NFS: " << nfsName << std::endl;
                                        for (const auto& [missing_ratio, missingRatioResult] : nfsResult)
                                        {
                                            results_file << "\t\t\t" << "Missing Ratio: " << missing_ratio << std::endl;
                                            for (const auto& [imputerName, imputerResult] : missingRatioResult)
                                            {
                                                results_file << "\t\t\t\t" << "Imputer: " << imputerName << std::endl;
                                                for (const auto& [granulesNumber, granulesResults] : imputerResult) {
                                                    results_file << "\t\t\t\t\t" << "Granules Number: " << granulesNumber << std::endl;
                                                    results_file << "\t\t\t\t\t\t" << "Train Values: " << std::endl;
                                                    for (const auto& train_val : granulesResults.train)
                                                    {
                                                        results_file << "\t\t\t\t\t\t" << train_val << std::endl;
                                                    }
                                                    results_file << "\t\t\t\t\t\t" << "Train Average +- std_dev: ";
                                                    auto [train_mean, train_dev] = ksi::utility_math::getMeanAndStandardDeviation(granulesResults.train.begin(), granulesResults.train.end());
                                                    results_file << train_mean << ' ' << train_dev << std::endl;
                                                    results_file << "\t\t\t\t\t\t" << "Train Values: " << std::endl;
                                                    for (const auto& test_val : granulesResults.test)
                                                    {
                                                        results_file << "\t\t\t\t\t\t" << test_val << std::endl;
                                                    }
                                                    results_file << std::endl;
                                                    results_file << "\t\t\t\t\t\t" << "Test Average +- std_dev: ";
                                                    auto [test_mean, test_dev] = ksi::utility_math::getMeanAndStandardDeviation(granulesResults.test.begin(), granulesResults.test.end());
                                                    results_file << test_mean << ' ' << test_dev << std::endl;
                                                }
                                            }
                                        }
                                    }
                                }
                                results_file.close();
                            }
                            else {
                                std::cerr << "Error: Unable to open " << results_gr_file_path << " file for writing results." << std::endl;
                            }
                        }
                    //}, entry);
                
                //for (auto& thread : threads) {
                  //  if (thread.joinable()) {
                    //    thread.join();
                    //}
                //}


				
            }
        }
    }
	CATCH;

    return;
}
