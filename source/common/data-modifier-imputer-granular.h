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
	 * This class inherits from `data_modifier_imputer` and extends its functionality by allowing granular data imputation based on partitions. 
     * @todo napisać na nowo, uwzglednić co robi klasa - KW
	 */
	class data_modifier_imputer_granular : public data_modifier_imputer 
	{
	protected:
		/**
		 * @brief Pointer to the partitioner used to create granules from the dataset.
		 *
		 * This shared pointer holds an instance of the partitioner, which is responsible for dividing the complete dataset into granules during the imputation process.
		 */
		std::shared_ptr<ksi::partitioner> _pPartitioner = nullptr;

		/**
		 * @brief Pointer to the t-norm operator used for granule membership calculations.
		 *
		 * This shared pointer holds an instance of the t-norm operator, which is used to aggregate membership values across different attributes of a tuple.
		 */
		std::shared_ptr<ksi::t_norm> _pTnorm = nullptr;

		std::vector<std::size_t> complete_indices;

		/**
		 * @brief Stores the indices of incomplete tuples in the dataset.
		 *
		 * This vector holds the indices of tuples in the dataset that have missing attributes.
		 * These indices are used to identify and process the incomplete tuples during imputation.
		 */
		std::vector<std::size_t> incomplete_indices;

		std::vector<std::vector<double>> granules_fuzzifications;

		std::size_t cluster_numbers;

		std::vector<std::vector<double>> granules_centers;

		std::size_t number_of_tuple_attributes;

	public:	
		
		/**
		 * Constructs a granular imputer with a specified partitioner and t-norm.
		 *
		 * @param Partitioner The partitioner used to create granules from the dataset.
		 * @param Tnorm The t-norm operator used in membership calculations.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (partitioner & Partitioner, t_norm& Tnorm); 

		/**
		 * @brief Copy constructor.
		 * Creates a new instance of the class by copying another object.
		 *
		 * @param other The object to be copied.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (const data_modifier_imputer_granular & other); 

		/**
		 * @brief Move constructor.
		 * Creates a new instance of the class by moving data from another object.
		 *
		 * @param other The object to be moved.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (data_modifier_imputer_granular && other) noexcept; 

		/**
		 * @brief Copy assignment operator.
		 * Allows assigning values from another object to the current object.
		 *
		 * @param other The object to be copied.
		 * @return Returns a reference to the current object.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular& operator =(const data_modifier_imputer_granular& other);

		/**
		* @brief Move assignment operator.
		* Allows assigning values from another  object to the current object by moving.
		*
		* @param other The object to be moved.
		* @return Returns a reference to the current object.
		* @date 2024-08-08
		* @author Konrad Wnuk
		*/
		data_modifier_imputer_granular& operator =(data_modifier_imputer_granular&& other) noexcept;
				
		/**
		* @brief Virtual destructor.
		* Ensures proper cleanup of resources in derived classes.
		* 
		* @date 2024-08-08
		* @author Konrad Wnuk
		*/
		virtual ~data_modifier_imputer_granular();

		/**
		 * @brief Creates a copy of the current object.
		 * This method returns a new instance of the object, which is a copy of the current one.
		 *
		 * @return A pointer to the newly created copy of the current object.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		virtual data_modifier * clone() const override;
		
		/** 
		 * @brief Modifies a dataset using granular imputation.
		 * 
		 * This method modifies the given dataset by imputing missing values through granular imputation.
		 * 
		 * @param ds dataset to modify
		 * @date 2024-08-08
		 * @author Konrad Wnuk   
		 */
		virtual void modify (dataset & ds) override;
		
		/**
		 * @return short description of the modifier 
		 */
		virtual std::string getDescription() const override;

	protected:
		/**
		 * @brief Performs granular imputation on the given dataset.
		 *
		 * This method imputes missing values in the dataset by first splitting it into complete and incomplete parts, then partitioning the complete dataset into granules, and finally imputing the incomplete data using these granules.
		 *
		 * @param ds The dataset to be imputed.
		 * @return A new dataset with imputed values.
		 * @date 2024-08-20
		 * @author Konrad Wnuk
		 */
		ksi::dataset granular_imputation(const dataset& ds);

		/**
		 * @brief Splits the dataset into complete and incomplete subsets.
		 *
		 * This method iterates through the dataset and separates complete data tuples from incomplete ones.
		 * The indices of the incomplete tuples are stored for further processing.
		 *
		 * @param ds The dataset to be split.
		 * @return A pair of datasets, where the first dataset contains complete data and the second contains incomplete data.
		 * @date 2024-08-20
		 * @author Konrad Wnuk
		 */
		std::pair < dataset, dataset > split_complete_incomplete(const dataset& ds);

	private:

		/**
		 * @brief Processes a single incomplete tuple and imputes its missing values.
	 	 *
	 	 * This method handles the imputation of missing attributes for a specific tuple.
		 * It calculates the granule memberships and imputes the attributes based on the weighted average.
		 *
		 * @param incomplete_dataset The dataset containing incomplete tuples.
		 * @param partitioned_data The partitioned data with granules.
		 * @date 2024-08-20
		 * @author Konrad Wnuk
		 * @todo poprawić opis - KW
		 */
		std::vector<double> handle_incomplete_tuple(const datum& incomplete_tuple);

		/**
		 * @brief Calculates granule imputations and memberships for a given incomplete tuple.
		 *
		 * This method computes the membership of the tuple in each granule and generates the corresponding imputations based on granule centers and fuzzifications.
		 *
		 * @param incomplete_datum The incomplete tuple to be imputed.
		 * @param partitioned_data The partitioned data with granules.
		 * @return A pair containing the list of imputed tuples and their granule memberships.
		 * @date 2024-08-20
		 * @author Konrad Wnuk
		 * @todo poprawić opis - KW
		 */
		std::pair<std::vector<std::vector<double>>, std::vector<double>> calculate_granule_imputations_and_memberships(const std::vector<double>& incomplete_tuple_attributes, const std::vector<std::size_t>& incomplete_attributes_indice);

		std::vector<double> impute_granule_attributes(const std::vector<std::size_t> &incomplete_attributes_indice, const std::vector<double> &granule_centers);

		std::vector<std::size_t> get_incomplete_attributes_indices(const ksi::datum& incomplete_tuple);

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
		std::vector<double> impute_tuple_attributes(const std::vector<double>& incomplete_tuple_attributes, std::vector<double>& imputed_granule_attributes, const std::vector<std::size_t>& incomplete_attributes_indice);

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
		 * @brief Calculates the membership of an estimated value vector to a granule.
		 *
		 * This method computes the granule membership of an estimated value vector based on the distance between the estimated values and the granule centers, adjusted by the granule fuzzifications.
		 * The membership is calculated using a Gaussian-like function, and the results are combined using a t-norm operator.
		 *
		 * @param estimated_values A vector of estimated values for which the granule membership is being calculated.
		 * @param granule_centers A vector of center values for the granule.
		 * @param granule_fuzzifications A vector of fuzzification values (S variable) for the granule.
		 * @return A double value representing the granule membership of the estimated values.
		 * @date 2024-08-20
		 * @author Konrad Wnuk
		 */
		double calculate_granule_membership(const std::vector<double>& estimated_tuple_attributes, const std::vector<double>& granule_centers, const std::vector<double>& granule_fuzzifications);
	};
}

#endif
