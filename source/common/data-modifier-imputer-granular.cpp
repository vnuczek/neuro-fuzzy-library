
#include "../common/data-modifier-imputer-granular.h"
#include "../common/data-modifier-imputer.h"
#include "../partitions/partitioner.h"
#include "../common/dataset.h"
#include "../common/datum.h"

std::pair<ksi::dataset, ksi::dataset> ksi::data_modifier_imputer_granular::dataset_marginalisation(const dataset& data)
{
	dataset complete_data, incomplete_data;

	for (std::size_t i = 0; i < data.size(); ++i)
	{
		datum tuple = data.getDatum(i);
		if (tuple->get_complete_flag())
			complete_data.addDatum(tuple);
		else
			incomplete_data.addDatum(tuple);
	}

	return std::make_pair(complete_data, incomplete_data);
}


ksi::dataset ksi::data_modifier_imputer_granular::granular_imputation(cosnt dataset& data, const int& granules_number)
{
	auto marginalised_data = dataset_marginalisation(data);



	for (const auto& : )

	for ()
	{

	}
}
