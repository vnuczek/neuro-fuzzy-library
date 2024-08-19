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
	 */
	class data_modifier_imputer_granular : public data_modifier_imputer 
	{
	protected:
		std::shared_ptr<ksi::partitioner> _pPartitioner = nullptr; ///< Pointer to a `partitioner` object that manages data partitions.
		std::shared_ptr<ksi::t_norm> _pTnorm = nullptr; ///< @todo dodać komentarze doxy i zmodyfikować konstruktory
		std::vector<std::size_t> incomplete_indices; ///< @todo dodać komentarze doxy
	protected:
			
		/**
		* @todo add comment
		* @date 2024-08-08
		* @author Konrad Wnuk
		*/
		std::pair < dataset, dataset > split_complete_incomplete(const dataset& ds);

		/**
		* @todo add comment
		* @date 2024-08-08
		* @author Konrad Wnuk
		*/
		dataset granular_imputation(const dataset& ds);
		
	public:	
		
		/** @todo update comment
		 * Constructor that initializes the object with a partitioner.
		 * 
		 * @param Partitioner The object used to manage data partitions.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (const partitioner & Partitioner, const t_norm& Tnorm); 

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
		virtual data_modifier * clone() const;
		
		/** 
		 * The method first imputes, then calls the modify method in the next data_modifier. 
		 * 
		 * @param ds dataset to modify
		 * @date 2024-08-08
		 * @author Konrad Wnuk   
		 */
		virtual void modify (dataset & ds);   // KW: mam tu przełożyć całą logikę działania czy jak? KS: Wystarczy wywolac metode.

		private:

		/**
		* @todo dodać komentarz doxy i zmienić nazwy
		*
		* @date 2024-08-19
		* @author Konrad Wnuk
		*/
		std::vector<double> weighted_average(const std::vector < std::vector<double>>& estimated_values, const std::vector<double>& weights);

		/**
		 * @todo dodać komentarz doxy i zmienić nazwy
		 *
		 * @date 2024-08-19
		 * @author Konrad Wnuk
		 */
		double calculate_granule_membership(const std::vector<double>& X, const std::vector<double>& V, const std::vector<double>& S);
	};
}

#endif

