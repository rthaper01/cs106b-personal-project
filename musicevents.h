#ifndef MUSICEVENTS_H
#define MUSICEVENTS_H
#pragma once

#include <iostream>
#include <cstdio>
#include <utility>
#include <algorithm>
#include <cstring>
#include <cmath>
#include "vector.h"
#include "grid.h"
#include "console.h"

int maxMinutesNaive(Vector<std::pair<int, int>> events, int money, int minutes, int index);
int maxMinutesNaive(Vector<std::pair<int, int>> events, int money);

int maxMinutesMemo(Vector<std::pair<int, int>> events, int money, int index, Grid<int>& memo);
int maxMinutesMemo(Vector<std::pair<int, int>> events, int money);

int maxMinutesDP(Vector<std::pair<int, int>> events, int money);


#endif // MUSICEVENTS_H

