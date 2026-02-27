/** @file */

#include "../experiments/exp-327.h"

#include <filesystem>
#include <thread>
#include <fstream>

#include "../service/exception.h"
#include "../readers/reader-complete.h"
#include "../partitions/fcm.h"
#include "../partitions/partition.h"
#include "../partitions/cluster.h"
#include "../descriptors/descriptor.h"
#include "../common/data_modifier_outlier_remove_granular.h"
#include "../tnorms/t-norm-min.h"
#include "../snorms/s-norm-max.h"
#include "../common/dataset.h"
#include "../common/data-modifier-outlier-remove-sigma.h"
#include "../dissimilarities/dis-log.h"
#include "../owas/sowa.h"
#include "../partitions/fcom.h"


void ksi::exp_327::execute()
{
    try
    {
        std::vector<std::thread> workers;

        std::string data_dir = "../data/" + this->name;
        for (const auto& entry : std::filesystem::directory_iterator(data_dir))
        {
            if (entry.is_regular_file() && entry.path().extension() == this->extention)
            {
                workers.emplace_back(
                    &exp_327::process_file,
                    this,
                    entry.path()
                );
            }
        }

        for (auto& worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }
    CATCH;
}

void ksi::exp_327::process_file(const std::filesystem::path& filePath)
{
	try
	{
		std::string outputDir = "../results/" + this->name;
        std::filesystem::create_directories(outputDir);

        ksi::reader_complete reader;
        const auto originalData = reader.read(filePath.string());

        process_granular(outputDir, filePath, originalData);
        process_sigma(outputDir, filePath, originalData);
		process_FCOM(outputDir, filePath, originalData);
	}
    CATCH;
}

void ksi::exp_327::process_granular(const std::string& outputDir, const std::filesystem::path& filePath, const ksi::dataset& originalData)
{
    try
    {
        for (const int g : this->granules)
        {
            for (const int it : this->iterations)
            {
                for (const auto th : this->thresholds)
                {
                    auto data = originalData;

                    ksi::fcm partitioner(g, it);
                    ksi::data_modifier_outlier_remove_granular remover(
                        partitioner,
                        ksi::t_norm_min(),
                        ksi::s_norm_max(),
                        th
                    );
                    remover.modify(data);

                    auto part = remover.get_partition();
                    save_granular_results(outputDir, filePath, g, it, th, originalData, data, part);
                }
            }
        }
    }
    CATCH;
}

void ksi::exp_327::save_granular_results(
    const std::string& outputDir,
    const std::filesystem::path& filePath,
    int g, int it, double th,
    const ksi::dataset& originalData,
    ksi::dataset& data,
    const ksi::partition& part)
{
    try
    {
        {
            std::ofstream cleaned_file(
                outputDir + "/" + make_granular_output_name(filePath, g, it, th, "cleaned")
            );
            cleaned_file << data;
        }

        ksi::dataset outliers = extract_outliers(originalData, data);
        {
            std::ofstream outliers_file(
                outputDir + "/" + make_granular_output_name(filePath, g, it, th, "outliers")
            );
            outliers_file << outliers;
        }

        {
            std::string granulesPath = outputDir + "/" + make_granular_output_name(filePath, g, it, th, "granules");
            save_granules(granulesPath, part);
        }

        {
            std::string cleanedSimPath = outputDir + "/" + make_granular_output_name(filePath, g, it, th, "data_similarity");
            save_similarities(cleanedSimPath, originalData, part);
        }

        /*{
            std::string cleanedSimPath = outputDir + "/" + make_granular_output_name(filePath, g, it, th, "cleaned_similarity");
            save_similarities(cleanedSimPath, data, part);
        }

        {
            std::string outliersSimPath = outputDir + "/" + make_granular_output_name(filePath, g, it, th, "outliers_similarity");
            save_similarities(outliersSimPath, outliers, part);
        }*/
    }
    CATCH;
}

void ksi::exp_327::save_granules(const std::string& filePath, const ksi::partition& part)
{
    try
    {
        std::ofstream file(filePath);

        auto nClusters = part.getNumberOfClusters();
        for (std::size_t c = 0; c < nClusters; ++c)
        {
            auto* cl = part.getCluster(c);
            if (!cl)
                continue;

            auto nDescriptors = cl->get_number_of_desciptors();
            for (std::size_t d = 0; d < nDescriptors; ++d)
            {
                auto* desc = cl->getAddressOfDescriptor(d);
                if (!desc)
                    continue;

                auto params = desc->getParameters();
                for (std::size_t p = 0; p < params.size(); ++p)
                {
                    if (p > 0)
                        file << "\t";
                    file << params[p];
                }
                file << "\t";
            }
            file << "\n";
        }
    }
    CATCH;
}

void ksi::exp_327::save_similarities(const std::string& filePath, const ksi::dataset& data, const ksi::partition& part)
{
    try
    {
        std::ofstream file(filePath);

        ksi::t_norm_min tnorm;
        ksi::s_norm_max snorm;

        auto nClusters = part.getNumberOfClusters();
        auto nData = data.size();

        for (std::size_t i = 0; i < nData; ++i)
        {
            const auto* d = data.getDatum(i);
            if (!d)
                continue;

            auto X = d->getVector();

            // s-norm across all granules (max membership)
            double similarity = 0.0;
            for (std::size_t c = 0; c < nClusters; ++c)
            {
                auto* cl = part.getCluster(c);
                if (!cl)
                    continue;

                // t-norm across all descriptors in a granule (min membership)
                double granuleMembership = 1.0;
                auto nDescriptors = cl->get_number_of_desciptors();
                for (std::size_t a = 0; a < nDescriptors; ++a)
                {
                    auto* desc = cl->getAddressOfDescriptor(a);
                    if (!desc)
                        continue;

                    auto* mutableDesc = const_cast<ksi::descriptor*>(desc);
                    double memb = mutableDesc->getMembership(X[a]);
                    granuleMembership = tnorm.tnorm(granuleMembership, memb);
                }
                similarity = snorm.snorm(similarity, granuleMembership);
            }

            // write: attribute values followed by similarity
            for (std::size_t a = 0; a < X.size(); ++a)
            {
                if (a > 0)
                    file << "\t";
                file << X[a];
            }
            file << "\t" << similarity << "\n";
        }
    }
    CATCH;
}

void ksi::exp_327::process_sigma(const std::string& outputDir, const std::filesystem::path& filePath, const ksi::dataset& originalData)
{
    try
    {
        for (const auto n : this->sigmas)
        {
            auto data = originalData;

            ksi::data_modifier_outlier_remove_sigma remover(n);
            remover.modify(data);
            {
                std::ofstream cleaned_file(
                    outputDir + "/" + make_sigma_output_name(filePath, n, "cleaned")
                );
                cleaned_file << data;
            }

            ksi::dataset outliers = extract_outliers(originalData, data);
            {
                std::ofstream outliers_file(
                    outputDir + "/" + make_sigma_output_name(filePath, n, "outliers")
                );
                outliers_file << outliers;
            }
        }
    }
    CATCH;
}

void ksi::exp_327::process_FCOM(const std::string& outputDir, const std::filesystem::path& filePath, const ksi::dataset& originalData)
{
	try
	{
		for(const auto number_of_clusters: granules)
		{
            auto data = originalData;
            auto number_of_items = data.getNumberOfData();

            ksi::dis_log dissimilarity;
            ksi::sowa owa(number_of_items, this->PC, this->PA);

            ksi::fcom algorithm(dissimilarity, owa);
            algorithm.setEpsilonForFrobeniusNorm(EPSILON);
            algorithm.setNumberOfClusters(number_of_clusters);

			auto partition = algorithm.doPartition(data);

            {
                std::ofstream partition_file(
                    outputDir + "/" + make_FCOM_output_name(filePath, number_of_clusters, "partition")
                );
                partition_file << partition;
            }

            {
                std::ofstream typicalities_file(
                    outputDir + "/" + make_FCOM_output_name(filePath, number_of_clusters, "typicalities")
                );
                typicalities_file << data;
            }
		}
	}
    CATCH;
}

std::string ksi::exp_327::make_granular_output_name(const std::filesystem::path& input, const int granules, const int iterations, const double threshold, const std::string& suffix)
{
    return input.stem().string()
        + "_g" + std::to_string(granules)
        + "_it" + std::to_string(iterations)
        + "_thr" + std::to_string(threshold)
        + "_" + suffix
        + this->extention;
}

std::string ksi::exp_327::make_sigma_output_name(const std::filesystem::path& input, const int n, const std::string& suffix)
{
    return input.stem().string()
        + "_n" + std::to_string(n)
        + "_" + suffix
        + this->extention;
}

std::string ksi::exp_327::make_FCOM_output_name(const std::filesystem::path& input, const int number_of_clusters, const std::string& suffix)
{
    return input.stem().string()
        + "_fcom_c" + std::to_string(number_of_clusters)
        + "_" + suffix
        + this->extention;
}

ksi::dataset ksi::exp_327::extract_outliers(const ksi::dataset& original, const ksi::dataset& cleaned)
{
    // Build a set of string representations of cleaned data items
    std::unordered_set<std::string> cleanedStrings;
    for (std::size_t i = 0; i < cleaned.size(); ++i)
    {
        const auto* d = cleaned.getDatum(i);
        if (d)
            cleanedStrings.insert(d->to_string());
    }

    // Collect original data items whose string representation is not in the cleaned set
    ksi::dataset outliers;
    for (std::size_t i = 0; i < original.size(); ++i)
    {
        const auto* d = original.getDatum(i);
        if (d)
        {
            auto repr = d->to_string();
            auto it = cleanedStrings.find(repr);
            if (it != cleanedStrings.end())
            {
                // This datum survived cleaning — remove one occurrence to handle duplicates correctly
                cleanedStrings.erase(it);
            }
            else
            {
                outliers.addDatum(*d);
            }
        }
    }

    return outliers;
}