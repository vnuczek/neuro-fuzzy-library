/** @file */

#ifndef CV_H
#define CV_H

#include "reader.h"

#include <filesystem>

namespace ksi
{
    /**
     * @class CV
     * An abstract class representing the base class for models.
     *
     * @date   2024-05-30
     * @author Konrad Wnuk
     */
	class CV : public reader {
    public:
        virtual ~CV() = default;

        /**
         * Splits the data into a specified number of subsets.
         *
         * @param n The number of subsets to split the data into.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        virtual void split(const int n = 10) = 0;

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
         * @param file_name The name of the file to read the data from.
         * @return A dataset containing the data read from the file.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        virtual dataset read_file(const std::filesystem::path& file_name) = 0;

        /**
         * Reads data from a specified directory.
         *
         * @param directory The directory to read the data from.
         * @return A dataset containing the data read from the directory.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        virtual dataset read_directory(const std::filesystem::path& directory) = 0;
    };
}

#endif
