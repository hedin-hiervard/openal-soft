#ifndef PHEROES_GAME_PERF_H_
#define PHEROES_GAME_PERF_H_

#pragma once

inline uint64  htime()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter( &t );
	return t.QuadPart;
}

#define TIMER_START(tname) \
	uint64 tname##_start = htime(); \
	/* */

#define TIMER_END(tname) \
	uint64 tname##_end = htime(); \
	/* */

#define TIMER_VAL(tname) (uint32)((tname##_end - tname##_start) / 100)

#endif //PHEROES_GAME_PERF_H_
