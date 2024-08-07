/** @file */

#ifndef FCM_DEGRANULATION
#define FCM_DEGRANULATION

#include "../common/dataset.h"

namespace ksi
{
	class fcm_degranulation 
	{
	private:

	public:
		/**
		*
		*/
		std::pair < dataset, dataset > dataset_marginalisation(const dataset& data);

		/**
		*
		*/
		dataset granular_imputation(const dataset& data, const int& granules_number);
	};
}

#endif