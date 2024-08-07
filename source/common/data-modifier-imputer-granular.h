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
	class data_modifier_imputer_granular : public data_modifier_imputer 
	{
	protected:

	
		/**
		*
		*/
		std::pair < dataset, dataset > dataset_marginalisation(const dataset& data);

		/**
		*
		*/
		dataset granular_imputation(const dataset& data);
		
	protected:
		std::shared_ptr<ksi::partitioner> _pPartitioner = nullptr; // KS: Wartość ustawiana przez konstruktor. Każdy partitioner ma metodę clone(). Obiekt partitioner ma juz w sobie informację o liczbie grup (granul), liczbie iteracji itd, dlatego tego nie musimy juz ustawiac w tej klasie. 
		
	public:	
		
		data_modifier_imputer_granular (const partitioner & Partitioner); ///< @todo 
		data_modifier_imputer_granular (const data_modifier_imputer_granular &); ///< @todo 
		data_modifier_imputer_granular (data_modifier_imputer_granular &&); ///< @todo 
		data_modifier_imputer_granular & operator =(const data_modifier_imputer_granular &);  ///< @todo 
		data_modifier_imputer_granular & operator =(data_modifier_imputer_granular &&);  ///< @todo 
		
		
		virtual ~data_modifier_imputer_granular();  ///< @todo 
		virtual data_modifier * clone() const;  ///< @todo Musi sklonowac _pPartitioner, czyli wywołać w nim metodę clone(); 
		
		/** The method first imputes, then calls the modify method in the next data_modifier. 
		 * @param  ds dataset to modify
		 * @author  
		 * @date    
		 */
		virtual void modify (dataset & ds);   // KS: Tu będzie całość.
	};
}

#endif
