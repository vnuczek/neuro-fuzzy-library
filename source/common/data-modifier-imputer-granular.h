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
		 * @brief Performs granular imputation on the given dataset.
		 *
		 * This function processes the dataset by first partitioning the complete dataset, setting the granule attributes,
		 * and then imputing missing values for each incomplete tuple. For each incomplete tuple, missing attribute values
		 * are imputed based on a weighted average calculated from the relevant granule memberships.
		 *
		 * @param ds The dataset on which the granular imputation is performed.
		 * @return A new dataset with imputed missing values.
		 * @throws ksi::exception If an error occurs during imputation.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
		ksi::dataset granular_imputation(const dataset& ds);

	private:
        /**
         * @brief Extracts complete data and finds incomplete tuples.
         *
         * This method separates complete data tuples from incomplete ones in the dataset.
         * It identifies incomplete tuples and stores their indices.
         *
         * @param ds The dataset to extract from.
         * @date 2024-09-11
         * @author Konrad Wnuk
         */
		void extract_complete_dataset_and_incomplete_indices(const dataset& ds);

        /**
         * @brief Sets the attributes of granules.
         *
         * This method initializes the granules' centers and fuzzification values, which are essential for the imputation process.
         * It uses the partitioned dataset to assign these attributes.
         *
         * @param partitioned_data The partitioned dataset containing granule information.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
        void set_granules_attributes(const ksi::partition& partitioned_data);

        /**
		 * @brief Handles the imputation of missing attributes for an incomplete tuple.
		 *
		 * This function identifies the missing attribute indices in the tuple and calculates
		 * the imputed values by considering relevant granule memberships. A weighted average is
		 * then computed to produce the final imputed values.
		 *
		 * @param incomplete_tuple The incomplete tuple containing missing attributes.
		 * @return A pair consisting of the vector of imputed attribute values and the vector of missing attribute indices.
		 * @date 2024-09-22
		 * @author Konrad Wnuk
		 */
        std::pair<std::vector<double>, std::vector<std::size_t>> handle_incomplete_tuple(const datum& incomplete_tuple);

        /**
         * @brief Retrieves indices of incomplete attributes in a tuple.
         *
         * This method identifies the positions of missing attributes in an incomplete tuple.
         *
         * @param incomplete_tuple The tuple containing missing attributes.
         * @return A vector of indices representing the positions of the missing attributes.
         * @date 2024-09-11
         * @author Konrad Wnuk
         */
        std::vector<std::size_t> get_incomplete_attributes_indices(const ksi::datum& incomplete_tuple) const;

        /**
         * @brief Calculates imputed values and memberships for each granule.
         *
         * This method computes the imputed attributes and their corresponding membership values for each granule.
         *
         * @param incomplete_tuple_attributes The incomplete tuple attributes.
         * @param incomplete_attributes_indices Indices of missing attributes.
         * @return A pair containing the imputed attributes and the membership values for each granule.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
		std::pair<std::vector<std::vector<double>>, std::vector<double>> calculate_granule_imputations_and_memberships(const std::vector<double>& incomplete_tuple_attributes, const std::vector<std::size_t>& incomplete_attributes_indices);

        /**
         * @brief Imputes missing values for a specific granule.
         *
         * This method imputes the missing attributes of a tuple by using the corresponding granule's centers.
         *
         * @param incomplete_attributes_indices Indices of missing attributes in the tuple.
         * @param granule_centers The center values of the granule used for imputation.
         * @return A vector of imputed values for the missing attributes.
         * @date 2024-09-11
         * @author Konrad Wnuk
         */
		std::vector<double> impute_granule_attributes(const std::vector<std::size_t> &incomplete_attributes_indices, const std::vector<double> &granule_centers);

        /**
         * @brief Inserts imputed values into the incomplete tuple.
         *
         * This method replaces the missing values in an incomplete tuple with the corresponding imputed values.
         *
         * @param incomplete_tuple_attributes The incomplete tuple attributes.
         * @param imputed_granule_attributes The imputed attributes for the granule.
         * @param incomplete_attributes_indices Indices of missing attributes in the tuple.
         * @return The tuple with missing values replaced by imputed values.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
		std::vector<double> insert_missing_attributes(const std::vector<double>& incomplete_tuple_attributes, std::vector<double>& imputed_granule_attributes, const std::vector<std::size_t>& incomplete_attributes_indices);

        /**
         * @brief Calculates the membership of a tuple in a granule.
         *
         * This method computes the membership of an imputed tuple within a granule based on the attribute values, granule centers, and fuzzifications.
         *
         * @param estimated_tuple_attributes The imputed attributes of the tuple.
         * @param granule_centers The center values of the granule.
         * @param granule_fuzzifications The fuzzification values of the granule.
         * @return The membership value of the tuple within the granule.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
        double calculate_granule_membership(const std::vector<double>& estimated_tuple_attributes, const std::vector<double>& granule_centers, const std::vector<double>& granule_fuzzifications);

        /**
         * @brief Computes the weighted average of estimated values.
         *
         * This method calculates the weighted average of imputed values from different granules, using the membership values as weights.
         *
         * @param estimated_values The imputed values from different granules.
         * @param weights The membership values used as weights.
         * @return The weighted average of the imputed values.
         * @date 2024-08-20
         * @author Konrad Wnuk
         */
        std::vector<double> weighted_average(const std::vector < std::vector<double>>& estimated_values, const std::vector<double>& weights);
		
	};
}

#endif
