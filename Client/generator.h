#ifndef  _GENERATOR_H_
#define  _GENERATOR_H_

#include <bits/stdc++.h>
#include <vector>
#include <set>


// Type T can be double/float/long double
// Genetares unique random values in [-mod; mod]
// The amount of values is sz
template <typename T> void generateRandoms(const T mod, const uint32_t sz, std::vector<T> &result)
{// Can generate 2*RAND_MAX*RAND_MAX*RAND_MAX unique values ( i.e. > uint32_t can be )
	const T magic[3] = {275.783091, 17.91649297, 2.301577024};
	const T k = 1.0 / (RAND_MAX * (magic[2] + magic[1] + magic[0]));
	std::set<T> filter;
	unsigned int idx = 0;
	
	std::srand(time(0));
    result.resize(sz);
	while(idx < sz)
	{
		T val_01 = mod * k * (magic[2] * rand() + magic[1] * rand() + magic[0] * rand());
		T val = (rand() & 1) ? val_01 : -val_01;
		if (0 == filter.count(val))
		{// use only unique values
			result[idx++] = val;
			filter.emplace(val);
		}
	}
}


#endif
