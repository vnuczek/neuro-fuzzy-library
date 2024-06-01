/** @file */

#ifndef TVT_H
#define TVT_H

#include "CV.h"

namespace ksi {

    /**
     * @class TVT
     * Class representing the TVT (Training, Validation and Test) model, inheriting from CV.
     *
     * @date   2024-05-30
     * @author Konrad Wnuk
     */
    class TVT : public CV {

        /**
         * @class Iterator
         * A nested class for iterating over TVT elements.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        class Iterator;

        /**
         * @class ConstIterator
         * A nested class for read-only iteration over TVT elements.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        class ConstIterator;

        /**
         * Clones the current reader object.
         *
         * @return A shared pointer to the cloned reader object.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        std::shared_ptr<reader> clone() const override;

        /**
         * Splits the data into a specified number of subsets.
         *
         * @param n The number of subsets to split the data into. Default is 10.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        void split(const int n = 10) override;

        /**
         * Saves the TVT data to a specified directory.
         *
         * @param directory The directory where the data will be saved.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        void save(const std::filesystem::path& directory) const override;

        /**
         * Reads data from a specified file.
         *
         * @param file_name The name of the file to read the data from.
         * @return A dataset containing the data read from the file.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        dataset read_file(const std::filesystem::path& file_name) override;

        /**
         * Reads data from a specified directory.
         *
         * @param directory The directory to read the data from.
         * @return A dataset containing the data read from the directory.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        dataset read_directory(const std::filesystem::path& directory) override;

        /**
         * Returns an iterator to the beginning of the TVT elements.
         *
         * @return An Iterator to the beginning of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
    	Iterator begin();

        /**
         * Returns an iterator to the end of the TVT elements.
         *
         * @return An Iterator to the end of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
    	Iterator end();

        /**
         * Returns a const iterator to the beginning of the TVT elements.
         *
         * @return A ConstIterator to the beginning of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
    	ConstIterator begin() const;

        /**
         * Returns a const iterator to the end of the TVT elements.
         *
         * @return A ConstIterator to the end of the TVT elements.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
    	ConstIterator end() const;
    };

    /**
     * @class TVT::Iterator
     * A class for iterating over TVT elements.
     *
     * @date 2024-05-30
	 * @author Konrad Wnuk
     */
    class TVT::Iterator {
    public:

        /**
         * Constructs a new Iterator.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        Iterator();

        /**
         * Advances the iterator to the next element.
         *
         * @return A reference to the advanced iterator.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        Iterator& operator++();

        /**
         * Advances the iterator to the next element (post-increment).
         *
         * @return A copy of the iterator before it was advanced.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        Iterator operator++(int);

        /**
         * Checks if two iterators are equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        bool operator==(const Iterator& other) const;

        /**
         * Checks if two iterators are not equal.
         *
         * @param other The other iterator to compare with.
         * @return True if the iterators are not equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        bool operator!=(const Iterator& other) const;

        /**
         * Compares two iterators.
         *
         * @param other The other iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        auto operator<=>(const Iterator& other) const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A reference to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        TVT& operator*() const;

        /**
         * Dereferences the iterator to access the current TVT element.
         *
         * @return A pointer to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        TVT* operator->() const;
    };

    /**
     * @class TVT::ConstIterator
     * A class for read-only iteration over TVT elements.
     *
     * @date 2024-05-30
	 * @author Konrad Wnuk
     */
    class TVT::ConstIterator {
    public:

        /**
         * Constructs a new ConstIterator.
         *
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        ConstIterator();

        /**
         * Advances the const iterator to the next element.
         *
         * @return A reference to the advanced const iterator.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        ConstIterator& operator++();

        /**
         * Advances the const iterator to the next element (post-increment).
         *
         * @return A copy of the const iterator before it was advanced.
         * @date 2024-05-30
		 * @author Konrad Wnuk
         */
        ConstIterator operator++(int);

        /**
         * Checks if two const iterators are equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        bool operator==(const ConstIterator& other) const;

        /**
         * Checks if two const iterators are not equal.
         *
         * @param other The other const iterator to compare with.
         * @return True if the const iterators are not equal, false otherwise.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        bool operator!=(const ConstIterator& other) const;

        /**
         * Compares two const iterators.
         *
         * @param other The other const iterator to compare with.
         * @return The result of the comparison.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        auto operator<=>(const ConstIterator& other) const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A reference to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        TVT& operator*() const;

        /**
         * Dereferences the const iterator to access the current TVT element.
         *
         * @return A pointer to the current TVT element.
         * @date 2024-05-30
		 * @author Konrad Wnuk
		 * @note This method is not yet implemented.
         */
        TVT* operator->() const;
    };
}

#endif

