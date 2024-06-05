/** @file */

#ifndef CV_H
#define CV_H

#include "reader.h"

#include <filesystem>
#include <memory>
#include <vector>
#include <mutex>

namespace ksi
{
    /**
     * @class CV
     * An abstract class representing the base class for models.
     *
     * @date   2024-05-30
     * @author Konrad Wnuk
     */
    class CV : public reader
    {
    protected:
        /** Pointer to a reader object which is used to read datasets */
        std::shared_ptr<ksi::reader> pReader = nullptr;

        /** Vector of datasets holds the subsets of the dataset */
        std::vector<ksi::dataset> datasets;

        /** Mutex for synchronizing access to datasets */
        mutable std::mutex datasets_mutex;

    public:
        /**
         * Constructor for CV.
         *
         * @param reader The reader object to initialize the pReader with.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        CV(const ksi::reader& reader) : pReader(std::make_shared<ksi::reader>(reader)) {}

        virtual ~CV() = default;

        /**
         * Splits the data into a specified number of subsets.
         *
         * @param dataset The dataset to be split.
         * @param n The number of subsets to split the data into.
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        virtual void split(const ksi::dataset dataset, const int n = 10) = 0;

        /**
         * Saves the data to a specified directory.
         *
         * @param directory The directory where the data will be saved.
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        virtual void save(const std::filesystem::path& directory) const = 0;

        /**
         * Reads data from a specified file.
         *
         * @param file_directory The path of the file to read the data from.
         * @return A dataset containing the data read from the file.
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        virtual ksi::dataset read_file(const std::filesystem::path& file_directory) = 0;

        /**
         * Reads data from a specified directory.
         *
         * @param directory The directory to read the data from.
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        virtual void read_directory(const std::filesystem::path& directory) = 0;
    };
}

#endif
