#include "modulate.h"
#include "testing/SimpleTest.h"
#include "console.h"

using namespace std;

/* Global constants */
const Vector<string> keys = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
const Map<string, string> enharmonics = {{"Bb","A#"}, {"Db","C#"}, {"Eb","D#"}, {"Gb","F#"}, {"Ab","G#"}};
const Map<string, string> standard = {{"A# major", "Bb major"}, {"A# minor", "Bb minor"}, {"D# major", "Eb major"},
                                      {"G# major", "Ab major"}, {"C# major", "Db major"}};

/* This helper function converts a key to its traditional spelling. */
string standardize(string key) {
    if (standard.containsKey(key)) {
        return standard.get(key);
    }
    return key;
}

/* This helper function generates the set of allowed related keys
 * to which a musical piece may modulate; i.e., the set of neighbors
 * in the decision tree.
 */
Set<string> relatedKeys(string key, Set<int> allowed) {
    Set<string> related;

    /* Extract tonality and modality from input */
    string tonality = key.substr(0, 2);
    string modality;
    if (!isspace(tonality[1])) {
        if (enharmonics.containsKey(tonality)) {
            tonality = enharmonics.get(tonality);
        }
        modality = key.substr(3);
    }
    else {
        tonality = tonality.substr(0, 1);
        modality = key.substr(2);
    }
    int index = keys.indexOf(tonality);

    /* Add allowed keys to set */
    if (modality == "major") {
        if (allowed.contains(0)) {
            related.add(standardize(tonality + " minor"));
        }
        if (allowed.contains(1)) {
            related.add(standardize(keys[(index + 2) % keys.size()] + " minor"));
        }
        if (allowed.contains(2)) {
            related.add(standardize(keys[(index + 4) % keys.size()] + " minor"));
        }
        if (allowed.contains(3)) {
            related.add(standardize(keys[(index + 5) % keys.size()] + " major"));
        }
        if (allowed.contains(4)) {
            related.add(standardize(keys[(index + 7) % keys.size()] + " major"));
        }
        if (allowed.contains(5)) {
            related.add(standardize(keys[(index + 9) % keys.size()] + " minor"));
        }
    }
    else {
        if (allowed.contains(0)) {
            related.add(standardize(tonality + " major"));
        }
        if (allowed.contains(1)) {
            related.add(standardize(keys[(index + keys.size() - 2) % keys.size()] + " major"));
        }
        if (allowed.contains(2)) {
            related.add(standardize(keys[(index + keys.size() - 4) % keys.size()] + " major"));
        }
        if (allowed.contains(3)) {
            related.add(standardize(keys[(index + keys.size() - 5) % keys.size()] +  " minor"));
        }
        if (allowed.contains(4)) {
            related.add(standardize(keys[(index + keys.size() - 7) % keys.size()] + " minor"));
        }
        if (allowed.contains(5)) {
            related.add(standardize(keys[(index + keys.size() - 9) % keys.size()] + " major"));
        }
    }
    return related;
}

/* This helper function checks if a key has already been explored
 * in a path, represented by a Stack.
 */
bool containsMusicalKey(Stack<string> path, string key) {
    while (!path.isEmpty()) {
        string check = path.pop();
        if (check == key) {
            return true;
        }
    }
    return false;
}

/** Solution 1: Breadth-First Search **/

 /* Stores paths in a Queue<Stack<string>>, adding a neighbor at each step
 * to expand outward by one level at a time. The Queue ensures that shorter
 * paths are explored before longer paths. As soon as the ending key is reached,
 * the current path is returned. If no path is found, an empty Stack is returned.
 */
Stack<string> modulateBFS(string startKey, string endKey, Set<int> allowed) {
    Stack<string> path = {startKey};
    Queue<Stack<string>> paths;
    paths.enqueue(path);

    while (!paths.isEmpty()) {
        path = paths.dequeue();
        string enharmonicEndKey = enharmonics.get(endKey.substr(0, 2)) + endKey.substr(2);
        /* Return path if endKey is reached */
        if (path.peek() == endKey) {
            return path;
        }
        else if (path.peek() == enharmonicEndKey) {
            path.pop();
            path.push(endKey);
            return path;
        }
        else {
            /* Copy current path and append every neighbor */
            Set<string> related = relatedKeys(path.peek(), allowed);
            Set<string> valid;
            for (string key : related) {
                if (!containsMusicalKey(path, key)) {
                    valid.add(key);
                }
            }
            for (string key : valid) {
                Stack<string> explore = path;
                explore.push(key);
                /* Enqueue longer paths */
                paths.enqueue(explore);
            }
        }
    }
    return {};
}

/** Solution 2: Iterative Deepening Depth-First Search **/

/* Uses recursive backtracking to explore as far as possible along
 * a single branch of the decision tree. When the maximum depth
 * allowed is reached, the function returns to previous levels
 * and considers other paths. Upon encountering the ending key, the
 * current path is saved in another variable, and "true" is returned.
 */
int modulateDFS(string startKey, string endKey, Set<int> allowed, Stack<string>& bestPath,
                Stack<string>& currentPath, int maxDepth) {
    /* If maximum depth is reached, return false */
    if (maxDepth < 0) {
        return false;
    }
    string enharmonicEndKey = enharmonics.get(endKey.substr(0, 2)) + endKey.substr(2);
    /* If endKey is reached, return true */
    if (currentPath.peek() == endKey || currentPath.peek() == enharmonicEndKey) {
        bestPath = currentPath;
        bestPath.pop();
        bestPath.push(endKey);
        return true;
    }
    /* Recursive case: loop through all neighbors, add each one to current
     * path, and explore
     */
    Set<string> related = relatedKeys(currentPath.peek(), allowed);
    Set<string> valid;
    for (string key : related) {
        if (!containsMusicalKey(currentPath, key)) {
            valid.add(key);
        }
    }
    for (string key : valid) {
        currentPath.push(key);
        if (modulateDFS(startKey, endKey, allowed, bestPath, currentPath, maxDepth - 1)) {
            return true;
        }
        currentPath.pop();
    }
    return false;
}

/* Main wrapper function that gradually expands the allowed depth of
 * the search from 0 to 24. We can be confident that any path will
 * be of length between these two values, so the iterative deepening
 * works well.
 */
Stack<string> modulateDFS(string startKey, string endKey, Set<int> allowed) {
    Stack<string> currPath;
    Stack<string> best;
    currPath.push(startKey);
    /* Iteratively deepen the search */
    for (int i = 0; i <= 24; i++) {
        if (modulateDFS(startKey, endKey, allowed, best, currPath, i)) {
            return best;
        }
    }
    return {};
}


/* * * * * * * * * * Test cases below this point * * * * * * * * * */

STUDENT_TEST("Test relatedKeys") {
    // Keys with no accidentals
    Set<string> expected = {"D minor", "E minor", "F major", "G major", "A minor", "C minor"};
    EXPECT_EQUAL(relatedKeys("C major",{0,1,2,3,4,5}), expected);
    expected = {"C major", "Bb major", "Ab major", "G minor", "F minor", "Eb major"};
    EXPECT_EQUAL(relatedKeys("C minor", {0,1,2,3,4,5}), expected);

    // Keys with accidentals
    expected = {"F# minor", "G# minor", "Bb minor", "B major", "Db major", "D# minor"};
    EXPECT_EQUAL(relatedKeys("F# major", {0,1,2,3,4,5}), expected);
    expected = {"Ab major", "F# major", "E major", "D# minor", "C# minor", "B major"};
    EXPECT_EQUAL(relatedKeys("G# minor", {0,1,2,3,4,5}), expected);

    // Restricted set of related keys
    expected = {"Ab major", "F major", "C minor"};
    EXPECT_EQUAL(relatedKeys("F minor", {0,3,5}), expected);

    // Empty allowed set
    expected = {};
    EXPECT_EQUAL(relatedKeys("F minor", {}), expected);
}

STUDENT_TEST("Test modulate") {
    /* Same start and end key */
    EXPECT_EQUAL(modulateBFS("C major", "C major", {0,1,2,3,4,5}).size(), 1);

    /* Enharmonic manipulation required, minor end key */
    Stack<string> result = modulateBFS("C major", "Bb minor", {0,1,2,3,4,5});
    EXPECT_EQUAL(result.size(), 4);
    EXPECT_EQUAL(result.pop(), "Bb minor");

    /* Restricted set of allowed related keys */
    EXPECT_EQUAL(modulateBFS("Db major", "A minor",{3,5}).size(), 7);

    /* No path from start to end */
    EXPECT_EQUAL(modulateBFS("G minor", "Ab major",{1}).size(), 0);

    /* Same start and end key */
    EXPECT_EQUAL(modulateDFS("C major", "C major", {0,1,2,3,4,5}).size(), 1);

    /* Enharmonic manipulation required, minor end key */
    result = modulateDFS("C major", "Bb minor", {0,1,2,3,4,5});
    EXPECT_EQUAL(result.size(), 4);
    EXPECT_EQUAL(result.pop(), "Bb minor");

    /* Restricted set of allowed related keys */
    EXPECT_EQUAL(modulateDFS("Db major", "A minor",{3,5}).size(), 7);

    /* No path from start to end */
    EXPECT_EQUAL(modulateDFS("G minor", "Ab major",{1}).size(), 0);
}
