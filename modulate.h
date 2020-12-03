#ifndef RELATEDKEYS_H
#define RELATEDKEYS_H

#endif // RELATEDKEYS_H

#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include "vector.h"
#include "queue.h"
#include "map.h"
#include "set.h"
#include "stack.h"
#include "priorityqueue.h"

Set<std::string> relatedKeys(std::string key, Set<int> allowed);

bool containsMusicalKey(Stack<std::string> path, std::string key);

Stack<std::string> modulateBFS(std::string startKey, std::string endKey, Set<int> allowed);

Stack<std::string> modulateDFS(std::string startKey, std::string endKey, Set<int> allowed);

