#pragma once

#include <time.h>
#include <random>

namespace random_utils
{
	static double rand()
	{
		static std::default_random_engine engine((unsigned)time(0));
		static std::uniform_real_distribution<> distr(0, 1);
		return distr(engine);
	}
}

