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
		const std::filesystem::path file_path = entry.path();
		const std::string datasetName = file_path.stem().string();
		const std::filesystem::path datasetResultDir = resultDir / datasetName;
		std::filesystem::create_directories(datasetResultDir);

		const std::vector<int> num_granules = (datasetName == "BoxJ290")
			? std::vector<int>{ 2, 3, 5, 10 }
			: std::vector<int>{ 2, 3, 5, 10, 15, 20, 25 };

		const auto missing_ratios = { 0.01, 0.02, 0.03, 0.04, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30 };

		for (const auto missing_ratio : missing_ratios)
		{
			runMissingRatio(file_path, datasetName, datasetResultDir, num_granules, missing_ratio);
		}
}

void ksi::exp_227::runMissingRatio(
	const std::filesystem::path& file_path, 
	const std::string& datasetName, 
	const std::filesystem::path& datasetResultDir, 
	const std::vector<int>& num_granules, 
	const double missing_ratio
)
{
	reader_complete DataReader;
	auto data = DataReader.read(file_path.string());

	data_modifier_normaliser normaliser;
	normaliser.modify(data);

	data_modifier_incompleter_random_without_last incomplete(missing_ratio);
	incomplete.modify(data);

	std::vector<std::unique_ptr<ksi::data_modifier>> imputers;
	imputers.push_back(std::make_unique<ksi::data_modifier_imputer_average>());
	imputers.push_back(std::make_unique<ksi::data_modifier_imputer_median>());
	imputers.push_back(std::make_unique<ksi::data_modifier_imputer_knn_average>(k));
	imputers.push_back(std::make_unique<ksi::data_modifier_imputer_knn_median>(k));
	imputers.push_back(std::make_unique<ksi::data_modifier_marginaliser>());

	std::vector<ResultRow> results;
	results.reserve(imputers.size() + static_cast<std::size_t>(num_granules.size()) * ITERATIONS);

	const std::string ratio_str = std::format("{:.2f}", missing_ratio);

	for (const auto& imputer : imputers)
	{
		auto experimentSet = data;
		imputer->modify(experimentSet);

		const std::string output_name = std::format("{}-{}.txt", imputer->getName(), ratio_str);
		const auto output_path = datasetResultDir / output_name;
		std::ofstream file(output_path);
		if (file.is_open()) {
			file << experimentSet.to_string();
			file.close();
		}
		else {
			std::cerr << "Unable to save to file: " << output_path << std::endl;
		}

		results.push_back(ResultRow{ imputer->getName(), 0, std::move(experimentSet) });
	}

	for (const auto granules : num_granules)
	{
		for (int iteration = 0; iteration < ITERATIONS; iteration++)
		{
			ksi::t_norm_product tnorm;
			ksi::fcm test_partitioner(granules, NUMBER_OF_CLUSTERING_ITERATIONS);
			std::unique_ptr<ksi::data_modifier> imputer = std::make_unique< data_modifier_imputer_granular>(test_partitioner, tnorm);

			auto experimentSet = data;
			imputer->modify(experimentSet);

			std::string output_name = std::format("{}-{}-g-{}-r-{}.txt", imputer->getName(), ratio_str, granules, iteration);
			std::string output_file = datasetResultDir.string() + "/" + output_name;

			std::ofstream file(output_file);
			if (file.is_open()) {
				file << experimentSet.to_string();
				file.close();

				std::cout << "Saved: " << output_file << std::endl;
			}
			else {
				std::cerr << "Unable to save to file: " << output_file << std::endl;
			}

			results.push_back(ResultRow{ imputer->getName(), granules, std::move(experimentSet) });
		}
	}

	ksi::frobenius_norm frob;

	for (size_t i = 0; i < results.size(); ++i) {
		for (size_t j = i + 1; j < results.size(); ++j) {
			double fval;
			try {
				fval = frob.get_frobenius_norm(results[i].dataset, results[j].dataset);
			}
			catch (const std::exception& e) {
				std::cerr << "Frobenius error (" << results[i].imputerName << " vs " << results[j].imputerName
					<< "): " << e.what() << std::endl;
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

