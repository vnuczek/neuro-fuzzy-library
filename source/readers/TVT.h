/** @file */

#ifndef TVT_H
#define TVT_H

#include "CV.h"

#include <tuple>

namespace ksi 
{

    /**
     * @class TVT
     * Class representing the TVT (Training, Validation and Test) model, inheriting from CV.
     *
     * @date   2024-05-30
     * @author Konrad Wnuk
     */
    class TVT : public CV 
   {
    private:
        /**
         * @brief The size of the validation dataset.
         *
         * Determines how many subsets are used for validation in each iteration.
         * Default value is 1.
         */
        int validation_size = 1;

   public:
        /**
         * @class iterator
         * A nested class for iterating over TVT elements.
         *
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        class iterator;

        /**
         * @class const_iterator
         * A nested class for read-only iteration over TVT elements.
         *
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        class const_iterator;

        /**
         * Constructor for TVT.
         *
         * @param source_reader The reader object to initialize the base class with.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        TVT(reader& source_reader);

        /**
         * Constructor for TVT.
         *
         * @param reader The reader object to initialize the base class with.
         * @param validation_dataset_size Size of the validation dataset.
         * @date 2024-06-09
         * @author Konrad Wnuk
         */
        TVT(reader& reader, int validation_dataset_size);

        /**
         * Copy constructor for TVT.
         *
         * @param other The TVT object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TVT(const TVT& other);

        /**
         * Move constructor for TVT.
         *
         * @param other The TVT object to move.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TVT(TVT&& other) noexcept;

        /**
         * Copy assignment operator for TVT.
         *
         * @param other The TVT object to copy.
         * @return A reference to the copied TVT object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TVT& operator=(const TVT& other);

        /**
         * Move assignment operator for TVT.
         *
         * @param other The TVT object to move.
         * @return A reference to the moved TVT object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TVT& operator=(TVT&& other) noexcept;

        /**
         * Splits the data into a specified number of subsets.
         *
         * @param base_dataset The dataset to be split.
         * @param n The number of subsets to split the data into. Default is 10.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        void split(const dataset & base_dataset, const int n = 10) override;

        /**
         * Saves the data to a specified directory.
         *
         * @param directory The directory where the data will be saved.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        void save(const std::filesystem::path& directory) const override;

        /**
         * Reads data from a specified file.
         *
         * @param file_directory The path of the file to read the data from.
         * @return A dataset containing the data read from the file.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        virtual dataset read_file(const std::filesystem::path& file_directory) override;

        /**
         * Reads data from a specified directory.
         *
         * @param directory The directory to read the data from.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        void read_directory(const std::filesystem::path& directory) override;

        /**
         * Reads data from a specified file.
         *
         * @param filename The path of the file to read the data from.
         * @return A dataset containing the data read from the file.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        dataset read(const std::string& filename) override;

        /**
         * Clones the current reader object.
         *
         * @return A shared pointer to the cloned reader object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        std::shared_ptr<reader> clone() const override;

        /**
         * Returns an iterator to the beginning of the TVT elements.
         *
         * @return An iterator to the beginning of the TVT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        iterator begin();

        /**
         * Returns an iterator to the end of the TVT elements.
         *
         * @return An iterator to the end of the TVT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        iterator end();

        /**
         * Returns a const iterator to the beginning of the TVT elements.
         *
         * @return A const_iterator to the beginning of the TVT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        const_iterator cbegin() const;

        /**
         * Returns a const iterator to the end of the TVT elements.
         *
         * @return A const_iterator to the end of the TVT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        const_iterator cend() const;
    };

    /**
     * @class TVT::iterator
     * A class for iterating over TVT elements.
     *
     * @date 2024-05-30
	 * @author Konrad Wnuk
     */
    class TVT::iterator {
    private:
        /** Pointer to the TVT object */
        TVT* pTVT;

        /** Const iterator pointing to the current test dataset */
        std::vector<dataset>::iterator test_iterator;

        /** Combined validation dataset */
        dataset validation_dataset;

        /** Combined train dataset */
        dataset train_dataset;

    public:

        /**
         * Constructs a new iterator.
         *
         * @param tvt Pointer to the TVT object.
         * @param test_it Iterator pointing to the current test dataset.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        iterator(TVT* tvt, std::vector<dataset>::iterator test_it);

        /**
         * Copy constructor.
         *
         * @param other The iterator object to copy.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        iterator(const iterator& other);

        /**
         * Move constructor.
         *
         * @param other The iterator object to move.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        iterator(iterator&& other) noexcept;

        /**
         * Copy assignment operator.
         *
         * @param other The iterator object to copy.
         * @return Reference to the copied iterator object.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        iterator& operator=(const iterator& other);

        /**
         * Move assignment operator.
         *
         * @param other The iterator object to move.
         * @return Reference to the moved iterator object.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        iterator& operator=(iterator&& other) noexcept;

        /**
         * Advances the iterator to the next element.
         *
         * @return A reference to the advanced iterator.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        iterator& operator++();

        /**
         * Advances the iterator to the next element (post-increment).
         *
         * @return A copy of the iterator before it was advanced.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        iterator operator++(int);

        /**
         * Checks if two iterators are equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are equal, false otherwise.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        bool operator==(const iterator& other) const;

        /**
         * Checks if two iterators are not equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are not equal, false otherwise.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        bool operator!=(const iterator& other) const;

        /**
         * Compares two iterators.
         *
         * @param other The other iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        std::strong_ordering operator<=>(const iterator& other) const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A tuple containing the combined training dataset, the combined validation dataset and the current test dataset.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        std::tuple<dataset, dataset, dataset> operator*() const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A tuple containing the combined training dataset, the combined validation dataset and the current test dataset.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        std::tuple<dataset, dataset, dataset> operator->() const;

    private:
        /**
         * Initializes the train and validation datasets by combining all datasets except the current test dataset.
         *
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        void initialize_train_and_validation_datasets();
    };

    /**
     * @class TVT::const_iterator
     * A class for read-only iteration over TVT elements.
     *
     * @date 2024-05-30
	 * @author Konrad Wnuk
     */
    class TVT::const_iterator {
    private:
        /** Pointer to the TVT object */
        const TVT* pTVT;

        /** Const iterator pointing to the current test dataset */
        std::vector<dataset>::const_iterator test_iterator;

        /** Combined validation dataset */
        dataset validation_dataset;

        /** Combined train dataset */
        dataset train_dataset;

    public:

        /**
         * Constructs a new const_iterator.
         *
         * @param tvt Pointer to the TVT object.
         * @param test_it Const iterator pointing to the current test dataset.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        const_iterator(const TVT* tvt, std::vector<dataset>::const_iterator test_it);

        /**
         * Copy constructor.
         *
         * @param other The const_iterator object to copy.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        const_iterator(const const_iterator& other);

        /**
         * Move constructor.
         *
         * @param other The const_iterator object to move.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        const_iterator(const_iterator&& other) noexcept;

        /**
         * Copy assignment operator.
         *
         * @param other The const_iterator object to copy.
         * @return Reference to the copied const_iterator object.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        const_iterator& operator=(const const_iterator& other);

        /**
         * Move assignment operator.
         *
         * @param other The const_iterator object to move.
         * @return Reference to the moved const_iterator object.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        const_iterator& operator=(const_iterator&& other) noexcept;

        /**
         * Advances the const iterator to the next element.
         *
         * @return A reference to the advanced const iterator.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        const_iterator& operator++();

        /**
         * Advances the const iterator to the next element (post-increment).
         *
         * @return A copy of the const iterator before it was advanced.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        const_iterator operator++(int);

        /**
         * Checks if two const iterators are equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are equal, false otherwise.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        bool operator==(const const_iterator& other) const;

        /**
         * Checks if two const iterators are not equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are not equal, false otherwise.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        bool operator!=(const const_iterator& other) const;

        /**
         * Compares two const iterators.
         *
         * @param other The other const iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        std::strong_ordering operator<=>(const const_iterator& other) const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A tuple containing the combined training dataset, the combined validation dataset and the current test dataset.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        std::tuple<const dataset&, const dataset&, const dataset&> operator*() const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A tuple containing the combined training dataset, the combined validation dataset and the current test dataset.
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        std::tuple<const dataset&, const dataset&, const dataset&> operator->() const;

    private:
        /**
         * Initializes the train and validation datasets by combining all datasets except the current test dataset.
         *
         * @date 2024-06-10
         * @author Konrad Wnuk
         */
        void initialize_train_and_validation_datasets();
    };
}

#endif

