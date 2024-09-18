/** @file */

#ifndef DATA_MODIFIER_IMPUTER_GRANULAR_H
#define DATA_MODIFIER_IMPUTER_GRANULAR_H

#include <memory>
 
#include "../common/data-modifier-imputer.h"
#include "../common/dataset.h"
#include "../common/datum.h"
#include "../partitions/partitioner.h"
#include "../tnorms/t-norm.h"

namespace ksi
{
	/**
     * @class data_modifier_imputer_granular
     * @brief Class responsible for granular data imputation using a partitioner.
     * 
     * This class performs data imputation by partitioning the complete dataset into clusters (granules) and imputing missing values in incomplete data tuples using these granules. The imputation is done based on granule centers and membership calculations.
     */
	class data_modifier_imputer_granular : public data_modifier_imputer 
	{
	protected:
		/** 
         * @brief Pointer to the partitioner used to create granules.
         * 
         * This shared pointer holds the instance of the partitioner, which is responsible for dividing the dataset into granules during the imputation process.
         */
		std::shared_ptr<ksi::partitioner> _pPartitioner = nullptr;

		/** 
         * @brief Pointer to the t-norm operator used for membership calculations.
         * 
         * This shared pointer holds the instance of the t-norm operator, which is used to aggregate membership values across different attributes of a tuple.
         */
		std::shared_ptr<ksi::t_norm> _pTnorm = nullptr;

		/** 
         * @todo
         */
		ksi::dataset complete_dataset;

		/** 
         * @brief Indices of incomplete data tuples.
         * 
         * Stores the indices of the tuples with missing attributes in the dataset.
         */
		std::vector<std::size_t> incomplete_indices;

		/** 
         * @brief Granules' fuzzifications.
         * 
         * Stores the fuzzification values for each granule, which are used to compute membership values.
         */
		std::vector<std::vector<double>> granules_fuzzifications;

		/** 
         * @brief Number of clusters (granules).
         * 
         * Stores the number of clusters (granules) created by the partitioner.
         */
		std::size_t cluster_numbers;

		/** 
         * @brief Centers of granules.
         * 
         * Stores the center of each granule, which is used during imputation.
         */
		std::vector<std::vector<double>> granules_centers;

		/** 
         * @brief Number of attributes in each tuple.
         * 
         * Stores the number of attributes in each tuple of the dataset.
         */
		std::size_t number_of_tuple_attributes;

	public:	
		
		/**
		 * @brief Constructor for the granular imputer.
         * 
         * Initializes the imputer with a partitioner and a t-norm operator.
         * 
         * @param Partitioner The partitioner used to create granules from the dataset.
         * @param Tnorm The t-norm operator used for membership calculations.
         * @date 2024-08-08
         * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (partitioner & Partitioner, t_norm & Tnorm); 

		/**
		 * @brief Copy constructor.
         * 
         * Creates a new instance by copying the given `data_modifier_imputer_granular` object.
         * 
         * @param other The object to be copied.
         * @date 2024-08-08
         * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (const data_modifier_imputer_granular & other); 

		/**
		 * @brief Move constructor.
         * 
         * Creates a new instance by moving the given `data_modifier_imputer_granular` object.
         * 
         * @param other The object to be moved.
         * @date 2024-08-08
         * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (data_modifier_imputer_granular && other) noexcept; 

		/**
		 * @brief Copy assignment operator.
         * 
         * Allows assignment from another `data_modifier_imputer_granular` object.
         * 
         * @param other The object to copy from.
         * @return Reference to the current object.
         * @date 2024-08-08
         * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular& operator =(const data_modifier_imputer_granular& other);

		/**
		 * @brief Move assignment operator.
         * 
         * Allows assignment by moving the given `data_modifier_imputer_granular` object.
         * 
         * @param other The object to move.
         * @return Reference to the current object.
         * @date 2024-08-08
         * @author Konrad Wnuk
		*/
		data_modifier_imputer_granular& operator =(data_modifier_imputer_granular&& other) noexcept;
				
		/**
		 * @brief Virtual destructor.
         * 
         * Cleans up resources used by the `data_modifier_imputer_granular` object.
         * 
         * @date 2024-08-08
         * @author Konrad Wnuk
		*/
		virtual ~data_modifier_imputer_granular();

		/**
		 * @brief Clones the current object.
         * 
         * Creates and returns a new copy of the current `data_modifier_imputer_granular` object.
         * 
         * @return A pointer to the newly created clone.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		virtual data_modifier * clone() const override;
		
		/** 
		  * @brief Modifies the dataset using granular imputation.
         * 
         * This method performs imputation on the provided dataset, modifying it in place.
         * 
         * @param ds The dataset to be modified.
         * @date 2024-08-08
         * @author Konrad Wnuk    
		 */
		virtual void modify (dataset & ds) override;
		
		/**
		 * @brief Returns a description of the modifier.
         * 
         * Provides a short description of the granular imputation performed by this class.
         * 
         * @return A string description of the granular imputer.
		 * @author Krzysztof Simiński
		 */
		virtual std::string getDescription() const override;

	protected:
		/**
		 * @brief Performs granular imputation on a dataset.
         * 
         * This method imputes missing values by first partitioning the complete tuples into granules and then imputing the incomplete tuples using these granules.
         * 
         * @param ds The dataset on which imputation is performed.
         * @return A new dataset with imputed values.
         * @date 2024-08-20
         * @author Konrad Wnuk
		 */
		ksi::dataset granular_imputation(const dataset& ds);

	private:
		/**
		 * @todo
		 */
		void extract_complete_dataset_and_incomplete_indices(const dataset& ds);

		/**
         * @brief Processes an incomplete tuple and imputes its missing values.
         * 
         * Imputes missing attributes by calculating granule memberships and weighted averages.
         * 
         * @param incomplete_tuple The tuple with missing attributes to be imputed.
         * @return A vector of imputed attribute values.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
		std::vector<double> handle_incomplete_tuple(const datum& incomplete_tuple);

		/**
         * @brief Processes an incomplete tuple and imputes its missing values.
         * 
         * Imputes missing attributes by calculating granule memberships and weighted averages.
         * 
         * @param incomplete_tuple The tuple with missing attributes to be imputed.
         * @return A vector of imputed attribute values.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
		std::pair<std::vector<std::vector<double>>, std::vector<double>> calculate_granule_imputations_and_memberships(const std::vector<double>& incomplete_tuple_attributes, const std::vector<std::size_t>& incomplete_attributes_indices);

		/** @todo KW */
		std::vector<double> impute_granule_attributes(const std::vector<std::size_t> &incomplete_attributes_indices, const std::vector<double> &granule_centers);

		/** @todo KW */
		std::vector<std::size_t> get_incomplete_attributes_indices(const ksi::datum& incomplete_tuple) const;

		/**
		 * @brief Imputes missing attributes in a tuple using the given granule center.
		 *
		 * This method replaces the missing attributes in the tuple with the corresponding values from the specified granule center.
		 *
		 * @param incomplete_datum The incomplete tuple to be imputed.
		 * @param granule_center The center of the granule used for imputation.
		 * @return A vector containing the attributes of the tuple after imputation.
		 * @date 2024-08-20
		 * @author Konrad Wnuk
		 * @todo poprawić opis - KW
		 */
		std::vector<double> insert_missing_attributes(const std::vector<double>& incomplete_tuple_attributes, std::vector<double>& imputed_granule_attributes, const std::vector<std::size_t>& incomplete_attributes_indices);

		/**
         * @brief Sets the attributes of the granules.
         * 
         * Sets the centers and fuzzifications of the granules using the partitioned complete data.
         * 
         * @param complete_ds The dataset containing complete data.
         * @date 2024-00-11
         * @author Konrad Wnuk
         */
		void set_granules_atributes(const ksi::partition& partitioned_data);

		/**
		 * @brief Computes the weighted average of estimated values using the provided weights.
		 *
		 * This method calculates the weighted average of a set of estimated values by multiplying each value by its corresponding weight, summing these products, and then dividing by the sum of the weights.
		 *
		 * @param estimated_values A vector of vectors containing the estimated values to be averaged. 
		 *						   Each inner vector represents a set of estimated values for a particular dimension.
		 * @param weights A vector of weights corresponding to each set of estimated values.
		 * @return A vector representing the weighted average of the estimated values.
		 * @date 2024-08-20
		 * @author Konrad Wnuk
		 */
		std::vector<double> weighted_average(const std::vector < std::vector<double>>& estimated_values, const std::vector<double>& weights);

		/**
         * @brief Calculates the membership of a tuple in a granule.
         * 
         * Uses a Gaussian function to compute the membership value for the tuple with respect to a granule.
         * 
         * @param incomplete_tuple_attributes Attributes of the incomplete tuple.
         * @param granule_centers The center of the granule.
         * @param granule_fuzzification The fuzzification of the granule.
         * @return The calculated membership value.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
		double calculate_granule_membership(const std::vector<double>& estimated_tuple_attributes, const std::vector<double>& granule_centers, const std::vector<double>& granule_fuzzifications);
	};
}

#endif
