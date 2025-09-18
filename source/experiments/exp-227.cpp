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

		createCsvHeader(
			csvPath,
			"Dataset;Missing Ratio;Imputer;Granules Number;Iteration;Frobenius Value"
		);

		if (runInParallel) 
		{
			std::vector<std::thread> threads;

			for (const auto& entry : std::filesystem::directory_iterator(dataDir)) {
				if (entry.is_regular_file())
				{
					// thdebugid(entry, entry);

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
		}
		else
		{
			for (const auto& entry : std::filesystem::directory_iterator(dataDir))
			{
				if (entry.is_regular_file())
				{
					// thdebugid(entry, entry);

					processDataset(entry);
				}
			}
		}

		thdebug(ksi::tempus::getDateTimeNowSafe());
		thdebug("dze end");
	}
	CATCH;

	return;
}

void ksi::exp_227::createCsvHeader(const std::filesystem::path& path, std::string_view header) const
{
	try
	{
		std::ofstream out(path, std::ios::trunc | std::ios::binary);
		if (!out)
		{
			std::cerr << "Unable to (re)create CSV file: " << path << std::endl;
			return;
		}
		out << header << '\n';
	}
	CATCH;
}

void ksi::exp_227::processDataset(const std::filesystem::directory_entry& entry) {
	try
	{
		const std::filesystem::path file_path = entry.path();
		const std::string datasetName = file_path.stem().string();

		const std::filesystem::path datasetResultDir = resultDir / datasetName;
		std::filesystem::create_directories(datasetResultDir);

		if (runInParallel) 
		{
			std::vector<std::thread> mThreads;
			mThreads.reserve(missing_ratios.size());

			for (const auto missing_ratio : missing_ratios) {
				mThreads.emplace_back(&ksi::exp_227::runMissingRatio, this,
					file_path, datasetName, datasetResultDir, missing_ratio);
			}

			for (auto& thread : mThreads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
		}
		else
		{
			for (const auto missing_ratio : missing_ratios) {
				runMissingRatio(file_path, datasetName, datasetResultDir, missing_ratio);
			}
		}
	}
	CATCH;
}

void ksi::exp_227::runMissingRatio(
	const std::filesystem::path& file_path, 
	const std::string& datasetName, 
	const std::filesystem::path& datasetResultDir, 
	const double missing_ratio
)
{
	try {
		const auto [completeDataset, data]  = loadCompleteDataAndPrepareData(file_path, missing_ratio);

		const std::string ratio_str = std::format("{:.2f}", missing_ratio);
		auto imputers = makeClassicalImputers();

		std::vector<ResultRow> results;
		results.reserve(imputers.size() + num_granules.size() * ITERATIONS);

		for (const auto& imputer : imputers)
		{
			/*std::string info = std::format("dataset: {}, missing ratio: {}, imputer: {}", datasetName, ratio_str, imputer->getName());
			thdebug(info);*/
			results.push_back(applyImputer(data, imputer.get(), datasetResultDir, ratio_str));
		}

		for (const auto granules : num_granules)
		{
			for (int iteration = 0; iteration < ITERATIONS; iteration++)
			{
            /*std::string info = std::format("dataset: {}, missing ratio: {}, granular imputer: {}, iteration: {}", datasetName, ratio_str, granules, iteration);
            thdebug(info);*/
				if (auto row = applyGranularImputer(data, granules, iteration, datasetResultDir, ratio_str))
				{
					results.push_back(std::move(*row));
				}
			}
		}

		appendPairwiseFrobenius(completeDataset, results, datasetName, ratio_str, csvPath);
	}
	CATCH;
}

std::pair<ksi::dataset, ksi::dataset> ksi::exp_227::loadCompleteDataAndPrepareData(
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

		auto completeDataset = data;

		data_modifier_incompleter_random_without_last incomplete(missing_ratio);
		incomplete.modify(data);

		return { completeDataset, data };
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
	const ksi::dataset& data,
	ksi::data_modifier* imputer,
	const std::filesystem::path& datasetResultDir,
	std::string_view ratio_str
) const
{
	auto experimentSet = data;
	imputer->modify(experimentSet);

	const std::string fname = std::format("{}-{}.txt", imputer->getName(), ratio_str);
	const auto outPath = datasetResultDir / fname;
	writeDatasetToFile(experimentSet, outPath);

	return ResultRow{ std::move(experimentSet), imputer->getName(), 0,  0};
}

std::optional<ksi::exp_227::ResultRow> ksi::exp_227::applyGranularImputer(
	const ksi::dataset& data,
	int granules,
	int iteration,
	const std::filesystem::path& datasetResultDir,
	std::string_view ratio_str
) const
{
	try
	{
		ksi::t_norm_product tnorm;
		ksi::fcm partitioner(granules, NUMBER_OF_CLUSTERING_ITERATIONS);
		std::unique_ptr<ksi::data_modifier> imputer = std::make_unique<data_modifier_imputer_granular>(partitioner, tnorm);

		auto experimentSet = data;
		imputer->modify(experimentSet);

		const std::string outputName = std::format("{}-{}-g-{}-r-{}.txt", imputer->getName(), ratio_str, granules, iteration);
		const auto outFilePath = datasetResultDir / outputName;
		writeDatasetToFile(experimentSet, outFilePath);

		return ResultRow{ std::move(experimentSet), imputer->getName(), granules,  iteration };
	}
	catch (const ksi::exception& e)
	{
		std::cerr << "[exp-227] Skipping granular case (ratio=" << ratio_str
			<< ", granules=" << granules
			<< ", iteration=" << iteration
			<< ") due to error: " << e.what() << std::endl;
		return std::nullopt;
	}
	catch (const std::exception& e)
	{
		std::cerr << "[exp-227] Skipping granular case (ratio=" << ratio_str
			<< ", granules=" << granules
			<< ", iteration=" << iteration
			<< ") due to std::error: " << e.what() << std::endl;
		return std::nullopt;
	}
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
	const ksi::dataset& completeDataset,
	const std::vector<ResultRow>& results,
	const std::string& datasetName,
	std::string_view ratio_str,
	const std::filesystem::path& csv_path
)
{
	try {
		ksi::frobenius_norm frob;

		for (std::size_t i = 0; i < results.size(); ++i) {
			double fval = 0.0;
			try {
				fval = frob.get_frobenius_norm(completeDataset, results[i].dataset);
			}
			catch (const std::exception& e) {
				std::cerr << "Frobenius error (" << datasetName << ", " << ratio_str << " at " << results[i].imputerName << "): " << e.what() << std::endl;
				continue;
			}

			std::lock_guard<std::mutex> lk(csv_mutex);
			std::ofstream csv(csv_path, std::ios::app);
			if (csv) {
				csv << datasetName << ';'
					<< ratio_str << ';'
					<< results[i].imputerName << ';'
					<< results[i].granules << ';'
					<< results[i].iteration << ';'
					<< std::format("{:.10f}", fval) << ';'
					<< '\n';
			}
			else {
				std::cerr << "Unable to open CSV for append: " << csv_path << std::endl;
			}
		}
	}
	CATCH;
}