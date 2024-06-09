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
         * @param reader The reader object to initialize the base class with.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        ksi::TVT(const ksi::reader& reader);

        /**
         * Copy constructor for TVT.
         *
         * @param other The TVT object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT(const ksi::TVT& other);

        /**
         * Move constructor for TVT.
         *
         * @param other The TVT object to move.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT(ksi::TVT&& other) noexcept;

        /**
         * Copy assignment operator for TVT.
         *
         * @param other The TVT object to copy.
         * @return A reference to the copied TVT object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT& operator=(const ksi::TVT& other);

        /**
         * Move assignment operator for TVT.
         *
         * @param other The TVT object to move.
         * @return A reference to the moved TVT object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT& operator=(ksi::TVT&& other) noexcept;

        /**
         * Splits the data into a specified number of subsets.
         *
         * @param dataset The dataset to be split.
         * @param n The number of subsets to split the data into. Default is 10.
         * @date 2024-06-04
         * @author Konrad Wnuk
         */
        void split(const ksi::dataset & dataset, const int n = 10) override;

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
        virtual ksi::dataset read_file(const std::filesystem::path& file_directory) override;

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
        ksi::dataset read(const std::string& filename) override;

        /**
         * Clones the current reader object.
         *
         * @return A shared pointer to the cloned reader object.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        std::shared_ptr<reader> clone() const override;

        /**
         * Returns an iterator to the beginning of the TVT elements.
         *
         * @return An iterator to the beginning of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::iterator begin();

        /**
         * Returns an iterator to the end of the TVT elements.
         *
         * @return An iterator to the end of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::iterator end();

        /**
         * Returns a const iterator to the beginning of the TVT elements.
         *
         * @return A const_iterator to the beginning of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::const_iterator cbegin() const;

        /**
         * Returns a const iterator to the end of the TVT elements.
         *
         * @return A const_iterator to the end of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::const_iterator cend() const;
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
        ksi::TVT* tvt;

        std::vector<ksi::dataset>::iterator train_iterator;

        int validation_size = 1;

        ksi::dataset validation_dataset;

        ksi::dataset test_dataset;

    public:

        /**
         * Constructs a new iterator.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::iterator();

        /**
         * Copy constructor.
         *
         * @param other The iterator object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::iterator(const ksi::TVT::iterator& other);

        /**
         * Move constructor.
         *
         * @param other The iterator object to move.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::iterator(ksi::TVT::iterator&& other) noexcept;

        /**
         * Copy assignment operator.
         *
         * @param other The iterator object to copy.
         * @return Reference to the copied iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::iterator& operator=(const ksi::TVT::iterator& other);

        /**
         * Move assignment operator.
         *
         * @param other The iterator object to move.
         * @return Reference to the moved iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::iterator& operator=(iterator&& other) noexcept;

        /**
         * Advances the iterator to the next element.
         *
         * @return A reference to the advanced iterator.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::iterator& operator++();

        /**
         * Advances the iterator to the next element (post-increment).
         *
         * @return A copy of the iterator before it was advanced.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        ksi::TVT::iterator operator++(int);

        /**
         * Checks if two iterators are equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        bool operator==(const ksi::TVT::iterator& other) const;

        /**
         * Checks if two iterators are not equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are not equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        bool operator!=(const ksi::TVT::iterator& other) const;

        /**
         * Compares two iterators.
         *
         * @param other The other iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        std::strong_ordering operator<=>(const ksi::TVT::iterator& other) const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A reference to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        std::tuple<ksi::dataset, ksi::dataset, ksi::dataset> operator*() const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A pointer to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        std::tuple<ksi::dataset, ksi::dataset, ksi::dataset> operator->() const;
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
        ksi::TVT* tvt;

        std::vector<ksi::dataset>::iterator train_iterator;

        int validation_size = 1;

        ksi::dataset validation_dataset;

        ksi::dataset test_dataset;

    public:

        /**
         * Constructs a new const_iterator.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::const_iterator();

        /**
         * Copy constructor.
         *
         * @param other The const_iterator object to copy.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::const_iterator(const ksi::TVT::const_iterator& other);

        /**
         * Move constructor.
         *
         * @param other The const_iterator object to move.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::const_iterator(ksi::TVT::const_iterator&& other) noexcept;

        /**
         * Copy assignment operator.
         *
         * @param other The const_iterator object to copy.
         * @return Reference to the copied const_iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::const_iterator& operator=(const ksi::TVT::const_iterator& other);

        /**
         * Move assignment operator.
         *
         * @param other The const_iterator object to move.
         * @return Reference to the moved const_iterator object.
         * @date 2024-06-08
         * @author Konrad Wnuk
         */
        ksi::TVT::const_iterator& operator=(ksi::TVT::const_iterator&& other) noexcept;

        /**
         * Advances the const iterator to the next element.
         *
         * @return A reference to the advanced const iterator.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        ksi::TVT::const_iterator& operator++();

        /**
         * Advances the const iterator to the next element (post-increment).
         *
         * @return A copy of the const iterator before it was advanced.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        ksi::TVT::const_iterator operator++(int);

        /**
         * Checks if two const iterators are equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        bool operator==(const ksi::TVT::const_iterator& other) const;

        /**
         * Checks if two const iterators are not equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are not equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        bool operator!=(const ksi::TVT::const_iterator& other) const;

        /**
         * Compares two const iterators.
         *
         * @param other The other const iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        std::strong_ordering operator<=>(const ksi::TVT::const_iterator& other) const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A reference to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        std::tuple<const ksi::dataset&, const ksi::dataset&, const ksi::dataset&> operator*() const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A pointer to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        std::tuple<const ksi::dataset&, const ksi::dataset&, const ksi::dataset&> operator->() const;
    };
}

#endif

