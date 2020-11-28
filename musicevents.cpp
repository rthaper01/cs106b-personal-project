#include "musicevents.h"
#include <cstring>
#include "testing/MemoryDiagnostics.h"
#include "testing/SimpleTest.h"
#include "testing/TestDriver.h"
#include "testing/TextUtils.h"

using namespace std;

/** First approach: Recursive Backtracking
 *  Time Complexity: O(2^n), n is the number of events
 **/

/* This helper function takes in a state (money, index) representing
 * the amount of money left and the index of the current event being
 * considered. If the money is negative, a large negative number is
 * returned, and if all events have been explored (the index is equal
 * to the size of the vector), 0 is returned. Otherwise, if the cost
 * of the event at the current index is greater than the money left,
 * the function recursively calls itself on the next consecutive index, and
 * if the cost of the current event is at most the money remaining, the
 * maximum of two recursive calls is returned: one adding the duration of
 * the event to the current total and the other not choosing the event at
 * all.
 */
int maxMinutesNaive(Vector<pair<int, int>> events, int money, int minutes, int index) {
    if (money < 0) {
        return -1;
    }
    if (index == events.size()) {
        return minutes;
    }
    return max(maxMinutesNaive(events, money - events[index].second, minutes + events[index].first, index + 1),
               maxMinutesNaive(events, money, minutes, index + 1));
}

/* Main wrapper function that calls the helper function at index 0. */

int maxMinutesNaive(Vector<pair<int, int>> events, int money) {
    return maxMinutesNaive(events, money, 0, 0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** Second approach: Memoization
 *  Time Complexity: O(m*n), where m is the budget
 *  Space Complexity: O(m*n)
 **/

/* This helper function is the exact same as the complete search
 * (backtracking) function above but takes in a memoization table
 * as a parameter that continues to be filled with the result at
 * each state as the bactracking is performed. This allows for
 * additional pruning as when a state is encountered that has
 * previously been explored, the value from the memoization table
 * is returned rather than exploring further, which avoids unnecessary
 * recursive calls.
 */

int maxMinutesMemo(Vector<pair<int, int>> events, int money, int index, Grid<int>& memo) {
    if (money < 0) {
        return -1;
    }
    if (index == events.size()) {
        return 0;
    }
    if (memo[index][money] != -1) {
        return memo[index][money];
    }
    if (events[index].second > money) {
        return memo[index][money] = maxMinutesMemo(events, money, index + 1, memo);
    }
    else {
        return memo[index][money] = max(events[index].first + maxMinutesMemo(events,
                                        money - events[index].second, index + 1, memo),
                   maxMinutesMemo(events, money, index + 1, memo));
    }
}

/* Main wrapper function that initializes the memoization table
 * to all -1's and then calls the memoized function at index 0
 * with the initial memo table.
 */

int maxMinutesMemo(Vector<pair<int, int>> events, int money) {
    Grid<int> memoTable(events.size(), money + 1);
    for (int i = 0; i < events.size(); i++) {
        for (int j = 0; j <= money; j++) {
            memoTable[i][j] = -1;
        }
    }
    return maxMinutesMemo(events, money, 0, memoTable);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** Third (Best) Approach: Bottom-Up Dynamic Programming
 *  Time Complexity: O(m*n)
 *  Space Complexity: O(2*m)
 **/

/* Main DP function that most efficiently takes advantage
 * of the optimal substructure of the problem. A DP table
 * holds the values of a mathematical function f(i, m) that
 * computes the maximum total number of minutes across all
 * subsets of events 0,1,...,i-1 with total cost at most m.
 * This function satisfies the recursive definition
 * f(i, m) = 0, if i == 0 or j == 0,
 * f(i, m) = max(f(i - 1, m - "cost of ith event") +
 *                          "duration of ith event", f(i - 1, m), if "cost of ith event" <= i,
 * f(i, m) = f(i - 1, m), otherwise
 */

int maxMinutesDP(Vector<pair<int, int>> events, int money) {
    int dp[2][money + 1];
    memset(dp, 0, sizeof(dp));
    for (int i = 0; i <= events.size(); i++) {
        for (int j = 0; j <= money; j++) {
            if (i == 0 || j == 0) {
                continue;
            }
            else if (events[i - 1].second <= j) {
                dp[1][j] = max(dp[0][j - events[i - 1].second] + events[i - 1].first, dp[0][j]);
            }
            else {
                dp[1][j] = dp[0][j];
            }
        }
        for (int j = 0; j <= money; j++) {
            dp[0][j] = dp[1][j];
        }
    }
    return dp[1][money];
}

/* * * * * * Test Cases Below This Point * * * * * */

STUDENT_TEST("Test all three functions on small inputs") {
    Vector<pair<int, int>> musicEvents = {{40, 10}, {30, 5}, {15, 3}, {5, 1}};
    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 15), 70);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 15), 70);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 15), 70);

    musicEvents = {{60, 15}, {120, 25}, {40, 8}, {75, 15}, {65, 20}};
    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 50), 235);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 50), 235);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 50), 235);
}

STUDENT_TEST("Test all three versions on inputs with overlapping paths") {
    // Multiple paths to the state (3, 8): first four events considered and 8 dollars
    // remaining
    Vector<pair<int, int>> musicEvents = {{40, 10}, {28, 7}, {30, 5}, {18, 2}, {15, 3}, {5, 1}};

    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 15), 81);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 15), 81);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 15), 81);

    // Multiple paths to 15 dollars spent
    musicEvents = {{52, 13}, {40, 10}, {28, 7}, {35, 6}, {30, 5}, {15, 3}, {11, 2}, {5, 1}};

    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 15), 85);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 15), 85);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 15), 85);

    // Multiple paths to 15 dollars spent
    musicEvents = {{45, 12}, {40, 10}, {28, 7}, {23, 5}, {30, 5}, {15, 3}, {5, 1}};

    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 20), 96);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 20), 96);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 20), 96);

    // Two subsets with total duration 84 minutes and total cost 18 dollars
    musicEvents = {{75, 15}, {9, 3}, {60, 10}, {24, 8}, {10, 1}};

    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 20), 94);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 20), 94);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 20), 94);
}

STUDENT_TEST("Test all three functions in input in which all events cost more than budget") {
    Vector<pair<int, int>> musicEvents = {{40, 25}, {30, 30}, {60, 22}, {50, 24}, {70, 35}};
    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 20), 0);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 20), 0);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 20), 0);
}

STUDENT_TEST("Test all three functions on edge cases") {
    // When there are no events, the functions should return 0
    Vector<pair<int, int>> musicEvents = {};
    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 20), 0);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 20), 0);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 20), 0);

    musicEvents = {{75, 15}, {9, 3}, {60, 10}, {24, 8}, {10, 1}};

    // When the budget is 0, the functions should return 0
    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 0), 0);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 0), 0);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 0), 0);

    // With only one event, that event's duration should be returned if
    // cost is under budget
    musicEvents = {{80, 16}};
    EXPECT_EQUAL(maxMinutesNaive(musicEvents, 20), 80);
    EXPECT_EQUAL(maxMinutesMemo(musicEvents, 20), 80);
    EXPECT_EQUAL(maxMinutesDP(musicEvents, 20), 80);
}

STUDENT_TEST("Time trials on backtracking version of maxMinutes") {
    Vector<pair<int, int>> musicEvents;
    int startSize = 10;
    for (int n = startSize; n <= 18 + startSize; n += 3) {
        for (int i = 0; i < n; i++) {
            int time = randomInteger(50, 150);
            int cost = randomInteger(0, 30);
            musicEvents.add({time, cost});
        }
        TIME_OPERATION(n, maxMinutesNaive(musicEvents, 100));
        musicEvents.clear();
    }
    for (int i = 0; i < 20; i++) {
        int time = randomInteger(50, 150);
        int cost = randomInteger(0, 30);
        musicEvents.add({time, cost});
    }
    for (int m = 8; m <= 500; m *= 2) {
        TIME_OPERATION(m, maxMinutesNaive(musicEvents, m));
    }
}

STUDENT_TEST("Time trials on memoized version of maxMinutes") {
    Vector<pair<int, int>> musicEvents;
    int startSize = 10;
    for (int n = startSize; n <= 250 * startSize; n *= 2) {
        for (int i = 0; i < n; i++) {
            int time = randomInteger(50, 150);
            int cost = randomInteger(0, 30);
            musicEvents.add({time, cost});
        }
        TIME_OPERATION(n, maxMinutesMemo(musicEvents, 100));
        musicEvents.clear();
    }
    for (int i = 0; i < 20; i++) {
        int time = randomInteger(50, 150);
        int cost = randomInteger(0, 30);
        musicEvents.add({time, cost});
    }
    for (int m = 8; m <= 10000; m *= 2) {
        TIME_OPERATION(m, maxMinutesMemo(musicEvents, m));
    }
}

STUDENT_TEST("Time trials on DP version of maxMinutes") {
    Vector<pair<int, int>> musicEvents;
    int startSize = 50;
    for (int n = startSize; n <= 100 * startSize; n *= 2) {
        for (int i = 0; i < n; i++) {
            int time = randomInteger(50, 150);
            int cost = randomInteger(0, 30);
            musicEvents.add({time, cost});
        }
        TIME_OPERATION(n, maxMinutesDP(musicEvents, 100));
        musicEvents.clear();
    }
    for (int i = 0; i < 200; i++) {
        int time = randomInteger(50, 150);
        int cost = randomInteger(0, 30);
        musicEvents.add({time, cost});
    }
    for (int m = 8; m <= 5000; m *= 2) {
        TIME_OPERATION(m, maxMinutesDP(musicEvents, m));
    }
}
