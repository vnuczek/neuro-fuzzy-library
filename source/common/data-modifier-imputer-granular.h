/** @file */

#ifndef DATA_MODIFIER_IMPUTER_GRANULAR_H
#define DATA_MODIFIER_IMPUTER_GRANULAR_H

#include <memory>
 
#include "../common/data-modifier-imputer.h"
#include "../partitions/partitioner.h"
#include "../common/dataset.h"
#include "../common/datum.h"

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
		// KS: Wartość ustawiana przez konstruktor. Każdy partitioner ma metodę clone().  

	protected:

	
		/**
		*
		*/
		std::pair < dataset, dataset > dataset_marginalisation(const dataset& data);

		/**
		*
		*/
		dataset granular_imputation(const dataset& data);
		
	public:	
		
		/**
		 * Constructor that initializes the object with a partitioner.
		 * 
		 * @param Partitioner The object used to manage data partitions.
		 * @date 2024-08-08
		 * @author Konrad Wnuk
		 */
		data_modifier_imputer_granular (const partitioner & Partitioner); 

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
		 * @author  
		 * @date    
		 */
		virtual void modify (dataset & ds);   // KS: Tu będzie całość.
	};
}

#endif
