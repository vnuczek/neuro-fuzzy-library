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
     * An abstract class representing the base class for the cross-validation models.
     *
     * @date   2024-05-30
     * @author Konrad Wnuk
     */
    class CV : public reader
    {
    protected:
        /** Pointer to a reader object which is used to read datasets */
        std::shared_ptr<reader> pReader = nullptr;

        /** Vector of datasets holds the subsets of the dataset */
        std::vector<dataset> datasets;

        /** Mutex for synchronizing access to datasets */
        mutable std::mutex datasets_mutex; 

    public:
        /**
         * Constructor for CV.
         *
         * @param source_reader The reader object to initialize the pReader with.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        CV(reader& source_reader);

        /**
         * Copy constructor.
         *
         * @param other The CV object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        CV(const CV& other);

        /**
        * Move constructor.
        *
        * @param other The CV object to move.
        * @date 2024-06-08
        * @author Konrad Wnuk
        */
        CV(CV&& other) noexcept;

        /**
         * Copy assignment operator.
         *
         * @param other The CV object to copy.
         * @return Reference to the copied CV object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        CV& operator=(const CV& other);

        /**
         * Move assignment operator.
         *
         * @param other The CV object to move.
         * @return Reference to the moved CV object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        CV& operator=(CV&& other) noexcept;

        /**
         * Virtual destructor for CV.
         */
        virtual ~CV() = default;

        /**
         * Splits the data into a specified number of subsets.
         *
         * @param base_dataset The dataset to be split.
         * @param n The number of subsets to split the data into. Default is 10.
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        virtual void split(const dataset & base_dataset, const int n = 10) = 0;

        /**
         * Saves the data to a specified directory.
         *
         * @param directory The directory where the data will be saved.
         * @date 2024-05-30
         * @author Konrad Wnuk
         * 
         * @todo W tej metodzie  trzeba jeszcze dodać nazwę pliku i rozszerzenie. 
         *       Teraz jest domyślna nazwa i rozszerzenie.
         *       Dobrze by było, gdyby po dodaniu zer do numeru pliku długość 
         *       liczby była taka sama dla wszystkich plików. Teraz długośc 
         *       jest zawsze 2. Gdy dane są podzielone na więcej niż 100 plików,
         *       mamy numery dwucyfrowe 00-99 i trzy- lub więcejcyfrowe dla 
         *       pozostałych.
         * @todo Teraz metoda nadpisuje zawartość folderu. Byłoby lepiej,
         *       gdyby to było kontrolowane. Np. domyślny paramter o wartości
         *       false (czyli nie nadpisuj). Dopiero podanie wartości true,
         *       nadpisze katalog.
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
        virtual dataset read_file(const std::filesystem::path& file_directory) = 0;

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
