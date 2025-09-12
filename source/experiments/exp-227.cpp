/** @file */

#include "../experiments/exp-227.h"

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
// #include "../common/data-modifier-imputer.h"
#include "../common/data-modifier-imputer-average.h"
#include "../common/data-modifier-imputer-median.h"
#include "../common/data-modifier-imputer-knn-average.h"
#include "../common/data-modifier-imputer-knn-median.h"
// #include "../common/data-modifier-imputer-values-from-knn.h"
#include "../common/data_modifier_incompleter_random_without_last.h"
#include "../common/data-modifier-imputer-granular.h"
#include "../partitions/fcm.h"
#include "../auxiliary/tempus.h"

#include "../tnorms/t-norm-product.h"
// #include "../tnorms/t-norm-min.h"
// #include "../tnorms/t-norm-lukasiewicz.h"
// #include "../tnorms/t-norm-einstein.h"

#include "../implications/imp-reichenbach.h"

#include "../neuro-fuzzy/neuro-fuzzy-system.h"
#include "../neuro-fuzzy/annbfis.h"
#include "../neuro-fuzzy/tsk.h"

#include "../auxiliary/frobenius_norm.h"  

#include "../auxiliary/utility-math.h"

ksi::exp_227::exp_227(const int k_val, const int iteration, const int num_clustering_iters) :
	k(k_val),
	ITERATIONS(iteration),
	NUMBER_OF_CLUSTERING_ITERATIONS(num_clustering_iters)
{}

void ksi::exp_227::execute()
{
	try
	{
		thdebug(ksi::tempus::getDateTimeNowSafe());

		std::ofstream csv(csvPath, std::ios::app);
		if (csv.is_open()) {
			csv << "Dataset;Missing Ratio;Frobenius Value;1. Imputer;1. Granules Number;2. Imputer;2. Granules Number\n";
		}
		else {
			std::cerr << "Unable to open file: " << csvPath << std::endl;
		}

      //=============================================
      bool runInParallel = false;
      if (not runInParallel) 
      {
         for (const auto& entry : std::filesystem::directory_iterator(dataDir)) 
         {
            processDataset(entry);
         }
         return;
      }
      //=============================================


		for (const auto& entry : std::filesystem::directory_iterator(dataDir)) {
			// thdebugid(entry, entry);

			if (entry.is_regular_file())
			{
				threads.emplace_back(&ksi::exp_227::processDataset, this, entry);
			}
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}



		thdebug(ksi::tempus::getDateTimeNowSafe());
		thdebug("dze end");
	}
	CATCH;

	return;
}

void ksi::exp_227::processDataset(const std::filesystem::directory_entry& entry) {
	try
	{
		const std::filesystem::path file_path = entry.path();
		const std::string datasetName = file_path.stem().string();
		const std::filesystem::path datasetResultDir = resultDir / datasetName;
		std::filesystem::create_directories(datasetResultDir);

		const std::vector<int> num_granules = (datasetName == "BoxJ290")
			? std::vector<int>{ 2, 3, 5, 10 }
		: std::vector<int>{ 2, 3, 5, 10, 15, 20, 25 };

		const auto missing_ratios = { 0.01, 0.02, 0.03, 0.04, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30 };

		std::vector<std::thread> mThreads;
		mThreads.reserve(std::size(missing_ratios));

      //=============================================
      bool runInParallel = false;
      if (not runInParallel) 
      {
         for (const auto missing_ratio : missing_ratios) {
            runMissingRatio(file_path, datasetName, datasetResultDir, num_granules, missing_ratio);
         }
         return;
      }
      //=============================================
		for (const auto missing_ratio : missing_ratios) {
			mThreads.emplace_back(&ksi::exp_227::runMissingRatio, this,
				file_path, datasetName, datasetResultDir, num_granules, missing_ratio);
		}

		for (auto& thread : mThreads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}
	CATCH;
}

void ksi::exp_227::runMissingRatio(
	const std::filesystem::path& file_path, 
	const std::string& datasetName, 
	const std::filesystem::path& datasetResultDir, 
	const std::vector<int>& num_granules, 
	const double missing_ratio
)
{
	try {
		const auto data = loadAndPrepareData(file_path, missing_ratio);

		const std::string ratio_str = std::format("{:.2f}", missing_ratio);
		auto imputers = makeClassicalImputers();

		std::vector<ResultRow> results;
		results.reserve(imputers.size() + static_cast<std::size_t>(num_granules.size()) * ITERATIONS);

		for (const auto& imputer : imputers)
		{
         std::string info = std::format("dataset: {}, missing ratio: {}, imputer: {}", datasetName, ratio_str, imputer->getName());
         thdebug(info);
			results.push_back(applyImputer(data, imputer.get(), datasetResultDir, ratio_str));
		}

		for (const auto granules : num_granules)
		{
			for (int iteration = 0; iteration < ITERATIONS; iteration++)
			{
            std::string info = std::format("dataset: {}, missing ratio: {}, granular imputer: {}, iteration: {}", datasetName, ratio_str, granules, iteration);
            thdebug(info);
				results.push_back(applyGranularImputer(data, granules, iteration, datasetResultDir, ratio_str));
			}
		}

		appendPairwiseFrobenius(datasetName, ratio_str, results, csvPath);
	}
	CATCH;
}

ksi::dataset ksi::exp_227::loadAndPrepareData(
	const std::filesystem::path& file_path,
	double missing_ratio
) const
{
	try
	{
		reader_complete DataReader;
		auto data = DataReader.read(file_path.string());

		data_modifier_normaliser normaliser;
		normaliser.modify(data);

		data_modifier_incompleter_random_without_last incomplete(missing_ratio);
		incomplete.modify(data);

		return data;
	}
	CATCH;
}

std::vector<std::unique_ptr<ksi::data_modifier>> ksi::exp_227::makeClassicalImputers() const
{
	try 
   {
		std::vector<std::unique_ptr<ksi::data_modifier>> v;
      v.reserve(4);
      v.push_back(std::make_unique<ksi::data_modifier_imputer_average>());
		v.push_back(std::make_unique<ksi::data_modifier_imputer_median>());
		v.push_back(std::make_unique<ksi::data_modifier_imputer_knn_average>(k));
		v.push_back(std::make_unique<ksi::data_modifier_imputer_knn_median>(k));
		return v;
	}
	CATCH;
}

ksi::exp_227::ResultRow ksi::exp_227::applyImputer(
	const ksi::dataset& base,
	ksi::data_modifier* imputer,
	const std::filesystem::path& outDir,
	std::string_view ratio_str) const
{
	auto experimentSet = base;
	imputer->modify(experimentSet);

	const std::string fname = std::format("{}-{}.txt", imputer->getName(), ratio_str);
	const auto outPath = outDir / fname;
	writeDatasetToFile(experimentSet, outPath);

	return ResultRow{ imputer->getName(), 0, std::move(experimentSet) };
}

ksi::exp_227::ResultRow ksi::exp_227::applyGranularImputer(const ksi::dataset& base,
	int granules,
	int iteration,
	const std::filesystem::path& datasetResultDir,
	std::string_view ratio_str) const
{
	ksi::t_norm_product tnorm;
	ksi::fcm partitioner(granules, NUMBER_OF_CLUSTERING_ITERATIONS);
	std::unique_ptr<ksi::data_modifier> imputer =
		std::make_unique<data_modifier_imputer_granular>(partitioner, tnorm);

	auto experimentSet = base;
	imputer->modify(experimentSet);

	const std::string outputName = std::format("{}-{}-g-{}-r-{}.txt", imputer->getName(), ratio_str, granules, iteration);
	const auto outFilePath = datasetResultDir / outputName;
	writeDatasetToFile(experimentSet, outFilePath);

	return ResultRow{ imputer->getName(), granules, std::move(experimentSet) };
}

void ksi::exp_227::writeDatasetToFile(
	const ksi::dataset& ds,
	const std::filesystem::path& outFilePath
) const
{
	try {
		std::ofstream file(outFilePath);
		if (file) {
			file << ds.to_string();
			std::cout << "Saved: " << outFilePath << std::endl;
		}
		else {
			std::cerr << "Unable to save to file: " << outFilePath << std::endl;
		}
	}
	CATCH;
}

void ksi::exp_227::appendPairwiseFrobenius(
	const std::string& datasetName,
	std::string_view ratio_str,
	const std::vector<ResultRow>& results,
	const std::filesystem::path& csvPath
)
{
	try {
		ksi::frobenius_norm frob;

		for (std::size_t i = 0; i < results.size(); ++i) {
			for (std::size_t j = i + 1; j < results.size(); ++j) {
				double fval = 0.0;
				try {
					fval = frob.get_frobenius_norm(results[i].dataset, results[j].dataset);
				}
				catch (const std::exception& e) {
					std::cerr << "Frobenius error (" << results[i].imputerName << " vs " << results[j].imputerName << "): " << e.what() << '\n';
					continue;
				}

				std::lock_guard<std::mutex> lk(csv_mutex);
				std::ofstream csv(csvPath, std::ios::app);
				if (csv) {
					csv << datasetName << ';'
						<< ratio_str << ';'
						<< std::format("{:.10f}", fval) << ';'
						<< results[i].imputerName << ';'
						<< results[i].granules << ';'
						<< results[j].imputerName << ';'
						<< results[j].granules << '\n';
				}
				else {
					std::cerr << "Unable to open CSV for append: " << csvPath << std::endl;
				}
			}
		}
	}
	CATCH;
}

