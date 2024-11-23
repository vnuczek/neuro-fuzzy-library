/** @file */

#include "../experiments/exp-027.h"

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <map>
#include <thread>
#include <future>

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

ksi::exp_027::exp_027(const int num_rules, const int num_clustering_iters, const int num_tuning_iters, const int num_dataparts, const int k_val, const double eta_val, const bool normalisation, const int iteration) :
	NUMBER_OF_RULES(num_rules),
	NUMBER_OF_CLUSTERING_ITERATIONS(num_clustering_iters),
	NUMBER_OF_TUNING_ITERATIONS(num_tuning_iters),
	NUMBER_OF_DATAPARTS(num_dataparts),
	k(k_val),
	ETA(eta_val),
	NORMALISATION(normalisation),
	ITERATIONS(iteration)
{}

void ksi::exp_027::processDataset(const std::filesystem::directory_entry& entry) {
	try {
		const std::filesystem::path file_path = entry.path();
	    const std::string datasetName = file_path.stem().string();
	    const std::filesystem::path datasetResultDir = resultDir / datasetName;
	    std::filesystem::create_directories(datasetResultDir);

	    const std::vector<int> num_granules = (datasetName == "BoxJ290") ? std::vector<int> { 2, 3, 5, 10 } : std::vector<int>{ 2, 3, 5, 10, 15, 20, 25 };
	    // const std::vector<int> num_granules = { 5 }; ///<---debug

		std::vector<std::future<std::pair<RESULTS, RESULTS_GR>>> futures;
		futures.reserve(ITERATIONS);
		for (int iteration = 0; iteration < ITERATIONS; iteration++) 
      {
			// thdebugid((datasetName), iteration);

			futures.push_back(std::async(&ksi::exp_027::runIteration, this, file_path, datasetName, datasetResultDir, num_granules, iteration));
		}

		std::vector<RESULTS> resultsVector;
		std::vector<RESULTS_GR> resultsGrVector;
		for (auto& fut : futures) {
			auto [iterResult, iterResultGr] = fut.get();

			resultsVector.push_back(iterResult);
			resultsGrVector.push_back(iterResultGr);
		}

	    writeResultsToFile(datasetResultDir, datasetName, mergeResults(resultsVector), mergeResultsGr(resultsGrVector));
	}
	CATCH;	
}

ksi::RESULTS ksi::exp_027::mergeResults(const std::vector<RESULTS>& resultsVector) {
	try {
		RESULTS results;
		for (const auto& result : resultsVector) {
			for (const auto& [datasetName, datasetResult] : result) {
				for (const auto& [nfsName, nfsResult] : datasetResult) {
					for (const auto& [missing_ratio, missingRatioResult] : nfsResult) {
						for (const auto& [imputerName, imputerResult] : missingRatioResult) {
							results[datasetName][nfsName][missing_ratio][imputerName].train.insert(results[datasetName][nfsName][missing_ratio][imputerName].train.end(), imputerResult.train.begin(), imputerResult.train.end());
							results[datasetName][nfsName][missing_ratio][imputerName].test.insert(results[datasetName][nfsName][missing_ratio][imputerName].test.end(), imputerResult.test.begin(), imputerResult.test.end());
						}
					}
				}
			}
		}
		return results;
	}
	CATCH;
}

ksi::RESULTS_GR ksi::exp_027::mergeResultsGr(const std::vector<RESULTS_GR>& resultsGrVector) {
	RESULTS_GR results_gr;
	for (const auto& result : resultsGrVector) {
		for (const auto& [datasetName, datasetResult] : result) {
			for (const auto& [nfsName, nfsResult] : datasetResult) {
				for (const auto& [missing_ratio, missingRatioResult] : nfsResult) {
					for (const auto& [imputerName, imputerResult] : missingRatioResult) {
						for (const auto& [granulesNumber, granulesResults] : imputerResult) {
							results_gr[datasetName][nfsName][missing_ratio][imputerName][granulesNumber].train.insert(results_gr[datasetName][nfsName][missing_ratio][imputerName][granulesNumber].train.end(), granulesResults.train.begin(), granulesResults.train.end());
							results_gr[datasetName][nfsName][missing_ratio][imputerName][granulesNumber].test.insert(results_gr[datasetName][nfsName][missing_ratio][imputerName][granulesNumber].test.end(), granulesResults.test.begin(), granulesResults.test.end());
						}
					}
				}
			}
		}
	}
	return results_gr;
}

std::pair<ksi::RESULTS, ksi::RESULTS_GR> ksi::exp_027::runIteration(const std::filesystem::path& file_path, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const std::vector<int>& num_granules, const int iteration)
{
	try {
		const auto missing_ratios = { 0.01, 0.02, 0.05, 0.10, 0.20, 0.50, 0.75 };
		// const auto missing_ratios = { 0.50 }; ///<--debug

		std::vector < std::future<std::pair<RESULTS, RESULTS_GR>>> futures;
		futures.reserve(missing_ratios.size());
		for (const auto missing_ratio : missing_ratios) {
			thdebugid((datasetName + ' ' + std::to_string(iteration)), missing_ratio);

			futures.push_back(std::async(&ksi::exp_027::runMissingRatio, this, file_path, datasetName, datasetResultDir, num_granules, iteration, missing_ratio));
		}

		std::vector<RESULTS> resultsVector;
		std::vector<RESULTS_GR> resultsGrVector;
		for (auto& fut : futures) {
			auto [iterResult, iterResultGr] = fut.get();

			resultsVector.push_back(iterResult);
			resultsGrVector.push_back(iterResultGr);
		}

		return { mergeResults(resultsVector), mergeResultsGr(resultsGrVector) };
	}
	CATCH;
}

std::pair<ksi::RESULTS, ksi::RESULTS_GR> ksi::exp_027::runMissingRatio(const std::filesystem::path& file_path, const std::string& datasetName, const std::filesystem::path& datasetResultDir, const std::vector<int>& num_granules, const int iteration, const double missing_ratio)
{
	try {
		ksi::RESULTS results;
		ksi::RESULTS_GR results_gr;

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
		// imputers.push_back(std::make_unique<ksi::data_modifier_imputer_values_from_knn>(k)); // <--- tego nie bedziemy robic
		imputers.push_back(std::make_unique<ksi::data_modifier_marginaliser>());

		data_modifier_incompleter_random_without_last incomplete(missing_ratio);

		std::size_t cross_val_iter = 0;
		for (auto [train, test] : tt) 
      {
			incomplete.modify(train);
			for (const auto& imputer : imputers) 
         {
            thdebugid(datasetName + ", iter: " + std::to_string(iteration) + ", miss: " + std::to_string(missing_ratio) + ", cross: " + std::to_string(cross_val_iter), imputer->getName());

            ksi::dataset trainSet = train;

            imputer->modify(trainSet);

            std::string output_name = std::format("{}-{}-r-{}.txt", imputer->getName(), missing_ratio, iteration);
            for (const auto& nfs : nfss) 
            {
               // thdebugid(datasetName + ' ' + std::to_string(iteration), nfs->get_brief_nfs_name());

               try 
               {
                  std::string output_file = datasetResultDir.string() + "/" + nfs->get_brief_nfs_name() + "-" + output_name;
                  auto result = nfs->experiment_regression(trainSet, test, output_file);
                  results[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()].train.push_back(result.rmse_train);
                  results[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()].test.push_back(result.rmse_test);
               }
               catch (const ksi::exception & ex)
               {
                  std::stringstream sos;
                  sos << ex.what() << std::endl;
                  sos << datasetName << ", " << nfs->get_brief_nfs_name() << ", miss: " << missing_ratio << ", " << imputer->getName() << std::endl;
                  std::string problem = sos.str();
                  thdebug(problem);
               }
            }
         }

			for (const auto granules : num_granules) 
         {
            try 
            {
               ksi::fcm test_partitioner(granules, NUMBER_OF_CLUSTERING_ITERATIONS);
               std::unique_ptr<ksi::data_modifier> imputer = std::make_unique< data_modifier_imputer_granular>(test_partitioner, tnorm);
               thdebugid(datasetName + ", iter: " + std::to_string(iteration) + ", miss: " + std::to_string(missing_ratio) + ", cross: " + std::to_string(cross_val_iter) + ", gr: " + std::to_string(granules), imputer->getName());

               ksi::dataset trainSet = train;
               imputer->modify(trainSet);

               std::string output_name = std::format("{}-{}-g-{}-r-{}.txt", imputer->getName(), missing_ratio, granules, iteration);
               for (const auto& nfs : nfss) 
               {
                  // thdebugid(datasetName + ' ' + std::to_string(iteration), nfs->get_brief_nfs_name());

                  try 
                  {
                     std::string output_file = datasetResultDir.string() + "/" + nfs->get_brief_nfs_name() + "-" + output_name;
                     auto result = nfs->experiment_regression(trainSet, test, output_file);
                     results_gr[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()][granules].train.push_back(result.rmse_train);
                     results_gr[datasetName][nfs->get_brief_nfs_name()][missing_ratio][imputer->getName()][granules].test.push_back(result.rmse_test);
                  }
                  catch (const ksi::exception & ex)
                  {
                     std::stringstream sos;
                     sos << ex.what() << std::endl;
                     sos << datasetName << ", " << nfs->get_brief_nfs_name() << ", miss: " << missing_ratio << ", gr: " << granules << ", " << imputer->getName() << std::endl;
                     std::string problem = sos.str();
                     thdebug(problem);
                  }
               }
            }
            catch (const ksi::exception& e)
            {
                thdebug(e.what());
            }
            catch (const std::exception& e)
            {
                thdebug(e.what());
            }
            catch (...)
            {
                thdebug("Unknown exception");
            }
			}

			++cross_val_iter;
		}

		return { results, results_gr };
	}
	CATCH;
}

void ksi::exp_027::writeResultsToFile(const std::filesystem::path& datasetResultDir, const std::string& datasetName, const RESULTS& results, const RESULTS_GR& results_gr) {
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

							resultsStream << "\t\t\t\t\t" << "Test Values: " << std::endl;
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
		std::ofstream resultsGrStream(results_gr_file_path);
		if (resultsGrStream.is_open()) {
			for (const auto& [datasetName, datasetResult] : results_gr)
			{
				resultsGrStream << "\t" << "Dataset: " << datasetName << std::endl;
				for (const auto& [nfsName, nfsResult] : datasetResult)
				{
					resultsGrStream << "\t\t" << "NFS: " << nfsName << std::endl;
					for (const auto& [missing_ratio, missingRatioResult] : nfsResult)
					{
						resultsGrStream << "\t\t\t" << "Missing Ratio: " << missing_ratio << std::endl;
						for (const auto& [imputerName, imputerResult] : missingRatioResult)
						{
							resultsGrStream << "\t\t\t\t" << "Imputer: " << imputerName << std::endl;
							for (const auto& [granulesNumber, granulesResults] : imputerResult) {
								resultsGrStream << "\t\t\t\t\t" << "Granules Number: " << granulesNumber << std::endl;
								resultsGrStream << "\t\t\t\t\t\t" << "Train Values: " << std::endl;
								for (const auto& train_val : granulesResults.train)
								{
									resultsGrStream << "\t\t\t\t\t\t" << train_val << std::endl;
								}
								auto [train_mean, train_dev] = ksi::utility_math::getMeanAndStandardDeviation(granulesResults.train.begin(), granulesResults.train.end());
								resultsGrStream << "\t\t\t\t\t\t" << "Train Average +- std_dev: " << train_mean << ' ' << train_dev << std::endl;

								resultsGrStream << "\t\t\t\t\t\t" << "Test Values: " << std::endl;
								for (const auto& test_val : granulesResults.test)
								{
									resultsGrStream << "\t\t\t\t\t\t" << test_val << std::endl;
								}
								auto [test_mean, test_dev] = ksi::utility_math::getMeanAndStandardDeviation(granulesResults.test.begin(), granulesResults.test.end());
								resultsGrStream << "\t\t\t\t\t\t" << "Test Average +- std_dev: " << test_mean << ' ' << test_dev << std::endl;
							}
						}
					}
				}
			}
			resultsGrStream.close();
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

         if (entry.is_regular_file()) 
         {
            threads.emplace_back(&ksi::exp_027::processDataset, this, entry);
         }
      }

      for (auto& thread : threads) 
      {
         if (thread.joinable()) 
         {
            thread.join();
         }
      }
      thdebug("dze end");
   }
   CATCH;

   return;
}
