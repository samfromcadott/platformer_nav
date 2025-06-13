#pragma once

template <typename T> T sign(T n) {
	return ( T(0) < n ) - ( n < T(0) );
}
