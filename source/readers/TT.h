/** @file */

#ifndef TT_H
#define TT_H

#include "CV.h"
#include <tuple>

namespace ksi
{
    /**
     * @class TT
     * Class representing the TT (Training and Test) model, inheriting from CV.
     *
     * @date   2024-05-30
     * @author Konrad Wnuk
     */
    class TT : public CV
    {
    public:
        /**
         * @class iterator
         * A nested class for iterating over TT elements.
         *
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        class iterator;

        /**
         * @class const_iterator
         * A nested class for read-only iteration over TT elements.
         *
         * @date 2024-05-30
         * @author Konrad Wnuk
         */
        class const_iterator;

        /**
         * Constructor for TT.
         *
         * @param source_reader The reader object to initialize the base class with.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        TT(reader& source_reader);

        /**
         * Copy constructor for TT.
         *
         * @param other The TT object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TT(const TT& other);

        /**
         * Move constructor for TT.
         *
         * @param other The TT object to move.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TT(TT&& other) noexcept;

        /**
         * Copy assignment operator for TT.
         *
         * @param other The TT object to copy.
         * @return A reference to the copied TT object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TT& operator=(const TT& other);

        /**
         * Move assignment operator for TT.
         *
         * @param other The TT object to move.
         * @return A reference to the moved TT object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        TT& operator=(TT&& other) noexcept;

        /**
         * Splits the data into a specified number of subsets.
         *
         * @param base_dataset The dataset to be split.
         * @param n The number of subsets to split the data into. Default is 10.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        void split(const dataset& base_dataset, const int n = 10) override;

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
        dataset read_file(const std::filesystem::path& file_directory) override;

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
         * Returns an iterator to the beginning of the TT elements.
         *
         * @return An iterator to the beginning of the TT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        iterator begin();

        /**
         * Returns an iterator to the end of the TT elements.
         *
         * @return An iterator to the end of the TT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        iterator end();

        /**
         * Returns a const iterator to the beginning of the TT elements.
         *
         * @return A const_iterator to the beginning of the TT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        const_iterator cbegin() const;

        /**
         * Returns a const iterator to the end of the TT elements.
         *
         * @return A const_iterator to the end of the TT elements.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        const_iterator cend() const;
    };

    /**
     * @class TT::iterator
     * A class for iterating over TT elements.
     *
     * @date 2024-05-30
     * @author Konrad Wnuk
     */
    class TT::iterator {
    private:
        /** Pointer to the TT object */
        TT* pTT;

        /** Iterator pointing to the current test dataset */
        std::vector<dataset>::iterator test_iterator;

        /** Combined train dataset */
        dataset train_dataset;

    public:
        /**
         * Constructs a new iterator.
         *
         * @param tt Pointer to the TT object.
         * @param test_it Iterator pointing to the current test dataset.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        iterator(TT* tt, std::vector<dataset>::iterator test_it);

        /**
         * Copy constructor.
         *
         * @param other The iterator object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        iterator(const iterator& other);

        /**
         * Move constructor.
         *
         * @param other The iterator object to move.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        iterator(iterator&& other) noexcept;

        /**
         * Copy assignment operator.
         *
         * @param other The iterator object to copy.
         * @return Reference to the copied iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        iterator& operator=(const iterator& other);

        /**
         * Move assignment operator.
         *
         * @param other The iterator object to move.
         * @return Reference to the moved iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        iterator& operator=(iterator&& other) noexcept;

        /**
         * Advances the iterator to the next element.
         *
         * @return A reference to the advanced iterator.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        iterator& operator++();

        /**
         * Advances the iterator to the next element (post-increment).
         *
         * @return A copy of the iterator before it was advanced.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        iterator operator++(int);

        /**
         * Checks if two iterators are equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are equal, false otherwise.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        bool operator==(const iterator& other) const;

        /**
         * Checks if two iterators are not equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are not equal, false otherwise.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        bool operator!=(const iterator& other) const;

        /**
         * Compares two iterators.
         *
         * @param other The other iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        std::strong_ordering operator<=>(const iterator& other) const;

        /**
         * Dereferences the iterator to access the current TT element.
         *
         * @return A tuple containing the combined training dataset and the current test dataset.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        std::tuple<dataset, dataset> operator*() const;

        /**
         * Dereferences the iterator to access the current TT element.
         *
         * @return A tuple containing the combined training dataset and the current test dataset.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        std::tuple<dataset, dataset> operator->() const;

    private:
        /**
		 * Initializes the train dataset by combining all datasets except the current test dataset.
		 *
		 * @date 2024-06-09
		 * @autor Konrad Wnuk
		 */
        void initialize_test_dataset();
    };

    /**
     * @class TT::const_iterator
     * A class for read-only iteration over TT elements.
     *
     * @date 2024-05-30
     * @author Konrad Wnuk
     */
    class TT::const_iterator {
    private:
        /** Pointer to the TT object */
        const TT* pTT;

        /** Const iterator pointing to the current test dataset */
        std::vector<dataset>::const_iterator test_iterator;

        /** Combined train dataset */
        dataset train_dataset;

    public:
        /**
         * Constructs a new const_iterator.
         *
         * @param tt Pointer to the TT object.
         * @param test_it Const iterator pointing to the current test dataset.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        const_iterator(const TT* tt, std::vector<dataset>::const_iterator test_it);

        /**
         * Copy constructor.
         *
         * @param other The const_iterator object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        const_iterator(const const_iterator& other);

        /**
         * Move constructor.
         *
         * @param other The const_iterator object to move.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        const_iterator(const_iterator&& other) noexcept;

        /**
         * Copy assignment operator.
         *
         * @param other The const_iterator object to copy.
         * @return Reference to the copied const_iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        const_iterator& operator=(const const_iterator& other);

        /**
         * Move assignment operator.
         *
         * @param other The const_iterator object to move.
         * @return Reference to the moved const_iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        const_iterator& operator=(const_iterator&& other) noexcept;

        /**
         * Advances the const iterator to the next element.
         *
         * @return A reference to the advanced const iterator.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        const_iterator& operator++();

        /**
         * Advances the const iterator to the next element (post-increment).
         *
         * @return A copy of the const iterator before it was advanced.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        const_iterator operator++(int);

        /**
         * Checks if two const iterators are equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are equal, false otherwise.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        bool operator==(const const_iterator& other) const;

        /**
         * Checks if two const iterators are not equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are not equal, false otherwise.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        bool operator!=(const const_iterator& other) const;

        /**
         * Compares two const iterators.
         *
         * @param other The other const iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        std::strong_ordering operator<=>(const const_iterator& other) const;

        /**
         * Dereferences the const iterator to access the current TT element.
         *
         * @return A tuple containing the combined training dataset and the current test dataset.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        std::tuple<const dataset&, const dataset&> operator*() const;

        /**
         * Dereferences the const iterator to access the current TT element.
         *
         * @return A tuple containing the combined training dataset and the current test dataset.
         * @date 2024-06-05
         * @author Konrad Wnuk
         */
        std::tuple<const dataset&, const dataset&> operator->() const;

    private:
        /**
         * Initializes the train dataset by combining all datasets except the current test dataset.
         *
         * @date 2024-06-09
         * @autor Konrad Wnuk
         */
        void initialize_test_dataset();
    };
}

#endif
