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
        TVT(const ksi::reader& reader) : CV(reader) {}

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
    	iterator begin();

        /**
         * Returns an iterator to the end of the TVT elements.
         *
         * @return An iterator to the end of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
    	iterator end();

        /**
         * Returns a const iterator to the beginning of the TVT elements.
         *
         * @return A const_iterator to the beginning of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
    	const_iterator cbegin() const;

        /**
         * Returns a const iterator to the end of the TVT elements.
         *
         * @return A const_iterator to the end of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
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
    public:

        /**
         * Constructs a new iterator.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        iterator();

        /**
         * Advances the iterator to the next element.
         *
         * @return A reference to the advanced iterator.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        iterator& operator++();

        /**
         * Advances the iterator to the next element (post-increment).
         *
         * @return A copy of the iterator before it was advanced.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        iterator operator++(int);

        /**
         * Checks if two iterators are equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        bool operator==(const iterator& other) const;

        /**
         * Checks if two iterators are not equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are not equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        bool operator!=(const iterator& other) const;

        /**
         * Compares two iterators.
         *
         * @param other The other iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        auto operator<=>(const iterator& other) const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A reference to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        TVT& operator*() const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A pointer to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        TVT* operator->() const;
    };

    /**
     * @class TVT::const_iterator
     * A class for read-only iteration over TVT elements.
     *
     * @date 2024-05-30
	 * @author Konrad Wnuk
     */
    class TVT::const_iterator {
    public:

        /**
         * Constructs a new const_iterator.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        const_iterator();

        /**
         * Advances the const iterator to the next element.
         *
         * @return A reference to the advanced const iterator.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        const_iterator& operator++();

        /**
         * Advances the const iterator to the next element (post-increment).
         *
         * @return A copy of the const iterator before it was advanced.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        const_iterator operator++(int);

        /**
         * Checks if two const iterators are equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        bool operator==(const const_iterator& other) const;

        /**
         * Checks if two const iterators are not equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are not equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        bool operator!=(const const_iterator& other) const;

        /**
         * Compares two const iterators.
         *
         * @param other The other const iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        auto operator<=>(const const_iterator& other) const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A reference to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        TVT& operator*() const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A pointer to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @warning This method is not yet implemented.
         */
        TVT* operator->() const;
    };
}

#endif

