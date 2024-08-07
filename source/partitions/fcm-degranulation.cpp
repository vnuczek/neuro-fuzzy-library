#include "fcm-degranulation.h"
#include "fcm.h"

std::pair<ksi::dataset, ksi::dataset> ksi::fcm_degranulation::dataset_marginalisation(const dataset& data)
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


ksi::dataset ksi::fcm_degranulation::granular_imputation(cosnt dataset& data, const int& granules_number)
{
	auto marginalised_data = dataset_marginalisation(data);



	for (const auto& : )

	for ()
	{

	}
}