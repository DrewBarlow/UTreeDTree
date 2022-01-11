/***************************
* File:     mytest.cpp
* Project:  Project 2
* Author:   Drew Barlow
* Date:     3/13/2021
* Section:  4
* E-mail:   abarlow1@umbc.edu
*
* Unit tests for an AVL Tree that holds Discord Trees.
***************************/
#include <iostream>
#include <random>
#include <vector>
#include "utree.h"

#define NUMACCTS 20
#define RANDDISC (distAcct(rng))

std::mt19937 rng(10);
std::uniform_int_distribution<> distAcct(0, 9999);

using std::vector;
using std::ostream;
using std::cout;
using std::endl;
using std::to_string;
typedef unsigned int uint;

const int BASE_ACCOUNT_DISC = 5;  // discriminator used for many dtree accounts
const int NUM_INSERTIONS = 2e4;  // amount of insertions for time tests
const int NUM_TRIALS = 5;  // amount of trials for time tests
const int NUM_CHARS = 26;  // amount of characters in the alphabet
const char ALPHABET[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};  // characters of the alphabet

class Tester {
public:
    bool verifyDSizes(DTree&);
    void putDIntoArr(DTree&, vector<DNode*>&);
    DNode* getDRoot(DTree&);
    DNode* getDNodeLeft(DNode*);
    DNode* getDNodeRight(DNode*);
    bool checkDTreeEquality(DTree&, DTree&);
    bool checkDNodeEquality(DNode*, DNode*);
    bool testDTreeOverloadedAssignment(DTree&);
    void dTreeInsertionTime(int, int);

    bool verifyUHeights(UTree&);
    UNode* getURoot(UTree&);
    UNode* getUNodeLeft(UNode*);
    UNode* getUNodeRight(UNode*);
    void putUIntoArr(UTree &obj, vector<UNode*> &arr);
    void uTreeInsertionTime(int, int);

private:
    bool verifyDSizes(DTree&, DNode*&);
    bool verifyUHeights(UTree&, UNode*&);
    void putDIntoArr(vector<DNode*>&, DNode*&);
    void putUIntoArr(vector<UNode*> &arr, UNode *&node);
};

// see function definitions for details, these should be self explanatory from names though
Account createAccount(int disc=0, string username="placeholder");
void assertFinish(bool expr, int &numTests, int &numTestsPassed);

// insertion and retrieval of 5 elems
// insertion of a node that already exists
// trigger rebalances
// insertion into a variety of vacant nodes
void dTreeInsertTests(int&, int&);

// removal and retrieval of a node
// removal of root with and without children
// removing a disc that doesnt exist
// removing from an empty tree
// removing all nodes
// removing and repopulating all nodes
void dTreeRemoveTests(int&, int&);

// rebuilding entire tree from right with root
// rebuilding entire tree from left with root
// rebuilding right subtree and entire tree
// rebuilding left subtree and entire tree
// rebuilding with vacant nodes
void dTreeRebalanceTests(int&, int&);

// copy three nodes to empty obj
// copy three nodes to populated obj
// copy one node to empty obj
// copy one node to populated obj
// copy empty to empty
void dTreeOverloadedAssignmentTests(int&, int&);

// testing insertion time
void dTreeInsertionTimeRun();

// basic insertion and retrieval
// trigger L and RL rotations
// trigger R and LR rotations
void uTreeInsertTests(int&, int&);

// remove from root
// removal of a user that DNE
// removal of a disc that DNE
// removal from an empty tree
// empty and delete a leaf node
// empty and delete the root
void uTreeRemoveTests(int&, int&);

// test L rotation
// test R rotation
// test LR rotation
// test RL rotation
void uTreeRotationTests(int&, int&);

// testing insertion time
void uTreeInsertionTimeRun();

int main() {
    int numTests = 0;
    int numTestsPassed = 0;

    dTreeInsertTests(numTestsPassed, numTests);
    cout << endl;
    dTreeRemoveTests(numTestsPassed, numTests);
    cout << endl;
    dTreeRebalanceTests(numTestsPassed, numTests);
    cout << endl;
    dTreeOverloadedAssignmentTests(numTestsPassed, numTests);
    cout << endl;
    dTreeInsertionTimeRun();
    cout << endl;

    uTreeInsertTests(numTestsPassed, numTests);
    cout << endl;
    uTreeRemoveTests(numTestsPassed, numTests);
    cout << endl;
    uTreeRotationTests(numTestsPassed, numTests);
    cout << endl;
    uTreeInsertionTimeRun();
    cout << endl;

    cout << numTestsPassed << " tests passed, ran " << numTests << '.' << endl << endl;

    return 0;
}

void dTreeInsertTests(int &numTestsPassed, int &numTests) {
    Tester tester;

    // insertion and retrieval
    {
        cout << "Testing DTree: Insertion: Insert five elems and retrieve them (root, two left, two right)." << endl;
        cout << "Expects: True returns from both fxns." << endl;
        DTree obj;
        bool passed = true;

        try {
            // insert a bunch of accounts, checking to make sure they were inserted along the way
            if (!obj.insert(createAccount(BASE_ACCOUNT_DISC))) { passed = false; }
            else if (!obj.insert(createAccount(BASE_ACCOUNT_DISC + 1))) { passed = false; }
            else if (!obj.insert(createAccount(BASE_ACCOUNT_DISC + 2))) { passed = false; }
            else if (!obj.insert(createAccount(BASE_ACCOUNT_DISC - 1))) { passed = false; }
            else if (!obj.insert(createAccount(BASE_ACCOUNT_DISC - 2))) { passed = false; }

            // ensure that retrieve works by checking all nose that we inserted
            else if (!obj.retrieve(BASE_ACCOUNT_DISC)) { passed = false; }
            else if (!obj.retrieve(BASE_ACCOUNT_DISC + 1)) { passed = false; }
            else if (!obj.retrieve(BASE_ACCOUNT_DISC + 2)) { passed = false; }
            else if (!obj.retrieve(BASE_ACCOUNT_DISC - 1)) { passed = false; }
            else if (!obj.retrieve(BASE_ACCOUNT_DISC - 2)) { passed = false; }

            // verify the sizes of the entire tree
            else if (!tester.verifyDSizes(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // insertion of a child node that already exists
    {
        cout << "Testing DTree: Insertion: Insert a node that already exists (not root)." << endl;
        cout << "Expects: A false return value." << endl;
        DTree obj;
        bool passed = false;

        try {
            // insert two nodes into the tree for comparison
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));

            // insertion should return false since the discriminator already exists
            // in the tree
            if (!obj.insert(createAccount(BASE_ACCOUNT_DISC + 1))) { passed = true; }

            if (!tester.verifyDSizes(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // insertion of root node that already exists
    {
        cout << "Testing DTree: Insertion: Insert a node that already exists (root)." << endl;
        cout << "Expects: A false return value." << endl;
        DTree obj;
        bool passed = false;

        try {
            // insert a node at the root and try to insert the same disc again
            obj.insert(createAccount(BASE_ACCOUNT_DISC));

            if (!obj.insert(createAccount(BASE_ACCOUNT_DISC))) { passed = true; }

            if (!tester.verifyDSizes(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // rebalance on right
    {
        cout << "Testing DTree: Insertion: Insert 20 elems on the right to trigger rebalances." << endl;
        cout << "Expects: No segfault and working retrieval." << endl;
        DTree obj;
        bool passed = true;

        try {
            // insert 20 nodes in increasing order from 0-19
            for (int i = 0; i < 20; i++) {
                if (!obj.insert(createAccount(i))) { passed = false; }
            }

            // create a vector rather than an array for easy insertion and put
            // the tree into it
            vector<DNode*> arr;
            tester.putDIntoArr(obj, arr);

            // iterate through the vector, verifying that all sizes are greater
            // than the previous
            for (uint i = 0; i < arr.size(); i++) {
                if (!i) { continue; }
                if (arr.at(i)->getDiscriminator() < arr.at(i - 1)->getDiscriminator()) {
                    passed = false;
                    break;
                }
            }

            if (!tester.verifyDSizes(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // rebalance on left
    {
        cout << "Testing DTree: Insertion: Insert 20 elems on the left to trigger rebalances." << endl;
        cout << "Expects: No segfault and working retrieval." << endl;
        DTree obj;
        bool passed = true;

        try {
            // insert 20 nodes in decreasing order from 20-1
            for (int i = 0; i < 20; i++) {
                obj.insert(createAccount(20 - i));
            }

            // create a vector rather than an array for easy insertion and put
            // the tree into it
            vector<DNode*> arr;
            tester.putDIntoArr(obj, arr);

            // iterate through the vector, verifying that all sizes are greater
            // than the previous
            for (uint i = 0; i < arr.size(); i++) {
                if (!i) { continue; }
                else if (arr.at(i)->getDiscriminator() < arr.at(i - 1)->getDiscriminator()) {
                    passed = false;
                    break;
                }
            }

            if (!tester.verifyDSizes(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // insertion into vacant node
    {
        cout << "Testing DTree: Insertion: Insert two elems and remove the second, then insert another." << endl;
        cout << "Expects: Working replacement." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // insert two elements, then remove the second and insert a node that maintains
            // the order of the tree that should replace the vacant node
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.remove(BASE_ACCOUNT_DISC + 1, removed);

            // verify that the vacant node was replaced
            if (obj.insert(createAccount(BASE_ACCOUNT_DISC + 2)) && removed) { passed = true; }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // insertion into surrounded vacant node
    {
        cout << "Testing DTree: Insertion: Insert three elems, remove the second, then insert as normal." << endl;
        cout << "Expects: Working replacement." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // insert three elements, then remove the node that is surrounded (second)
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 3));
            obj.remove(BASE_ACCOUNT_DISC + 1, removed);

            // insert a node that maintains the order of the tree that should replace the
            // surrounded node
            if (obj.insert(createAccount(BASE_ACCOUNT_DISC + 2)) && removed) { passed = true; }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // insertion into vacant node, out of order
    {
        cout << "Testing DTree: Insertion: Insert four (to avoid triggering a rebalance) elems, "
             << "remove the second, then insert an out-of-order node." << endl;
        cout << "Expects: Inserted node appears at the end." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // insert four nodes then remove the second as in the previous test,
            // then insert a node that does not maintain the tree's order if it
            // were to be inserted into the vacant node
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));
            obj.remove(BASE_ACCOUNT_DISC + 1, removed);

            // verify that the node inserted is not in the position of the vacant node
            if (obj.insert(createAccount(BASE_ACCOUNT_DISC + 3)) && removed) {
                DNode *root = tester.getDRoot(obj);
                if (tester.getDNodeRight(root)->getDiscriminator() != BASE_ACCOUNT_DISC + 3) {
                    passed = true;
                }
            }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    return;
}

void dTreeRemoveTests(int &numTestsPassed, int &numTests) {
    Tester tester;

    // removal of a node
    {
        cout << "Testing DTree: Removal: Remove a node and retrieve it." << endl;
        cout << "Expects: The node is now vacant and retrieved as nullptr." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // insert a node then remove it
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.remove(BASE_ACCOUNT_DISC, removed);

            // verify that the the removed node is vacant, and that retrieving it
            // returns a nullptr
            if (removed) {
                if (removed->isVacant() && removed->getDiscriminator() == BASE_ACCOUNT_DISC) {
                    if (obj.retrieve(BASE_ACCOUNT_DISC) == nullptr) { passed = true; }
                }

                delete removed;
            }

        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal of root with no children
    {
        cout << "Testing DTree: Removal: Remove the root node, then insert into it." << endl;
        cout << "Expects: Root is repopulated." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // remove the root node
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.remove(BASE_ACCOUNT_DISC, removed);

            // insert into the root node given that it is vacant
            if (removed) {
                if (removed->isVacant() && removed->getDiscriminator() == BASE_ACCOUNT_DISC) {
                    obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
                    if (tester.getDRoot(obj)->getDiscriminator() == BASE_ACCOUNT_DISC + 1) { passed = true; }
                }

                delete removed;
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal of root with one two children
    {
        cout << "Testing DTree: Removal: Insert three nodes, then remove the root and insert into it." << endl;
        cout << "Expects: Root is repopulated and children are maintained." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // insert three nodes and remove the root
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.remove(BASE_ACCOUNT_DISC, removed);

            // if the root is vacant, insert into the root with a node that
            // fits the order of the tree
            if (removed) {
                if (removed->isVacant() && removed->getDiscriminator() == BASE_ACCOUNT_DISC) {
                    obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));

                    DNode *root = tester.getDRoot(obj);
                    if (root != removed) {
                        bool leftEqual = false;
                        bool rightEqual = false;

                        // verify that the children are the same
                        if (tester.getDNodeLeft(root) == tester.getDNodeLeft(removed)) {
                            leftEqual = true;
                        }

                        if (tester.getDNodeRight(root) == tester.getDNodeRight(removed)) {
                            rightEqual = true;
                        }

                        passed = leftEqual && rightEqual;
                    }
                }

                delete removed;
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal of a discriminator that DNE
    {
        cout << "Testing DTree: Removal: Insert three nodes, then remove a node that doesn't exist." << endl;
        cout << "Expects: A false return value." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // insert three nodes
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));

            // remove a node that doesn't exist in the tree
            if (!obj.remove(BASE_ACCOUNT_DISC * BASE_ACCOUNT_DISC, removed) && !removed) {
                passed = true;
            }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal of empty tree
    {
        cout << "Testing DTree: Removal: Removing from an empty tree." << endl;
        cout << "Expects: A false return value." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed = nullptr;

            // remove a node from the tree with it emptied
            if (!obj.remove(BASE_ACCOUNT_DISC, removed) && !removed) { passed = true; }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal of ALL nodes
    {
        cout << "Testing DTree: Removal: Insert 5 nodes, then remove all of them." << endl;
        cout << "Expects: All nodes are vacant." << endl;
        DTree obj;
        bool passed = false;

        try {
            DNode *removed1 = nullptr;
            DNode *removed2 = nullptr;
            DNode *removed3 = nullptr;
            DNode *removed4 = nullptr;
            DNode *removed5 = nullptr;

            // insert five nodes
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));

            // remove all of them, making sure that they were removed along the way,
            // then verify that all of those nodes are vacant
            if (obj.remove(BASE_ACCOUNT_DISC, removed1) && obj.remove(BASE_ACCOUNT_DISC - 1, removed2)
                                                        && obj.remove(BASE_ACCOUNT_DISC - 2, removed3)
                                                        && obj.remove(BASE_ACCOUNT_DISC + 1, removed4)
                                                        && obj.remove(BASE_ACCOUNT_DISC + 2, removed5)) {
                if (removed1 && removed2 && removed3 && removed4 && removed5) {
                    if (removed1->isVacant() && removed2->isVacant()
                                             && removed3->isVacant()
                                             && removed4->isVacant()
                                             && removed5->isVacant()) {
                        passed = true;
                    }
                }
            }

            if (removed1) { delete removed1; }
            if (removed2) { delete removed2; }
            if (removed3) { delete removed3; }
            if (removed4) { delete removed4; }
            if (removed5) { delete removed5; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal AND repopulation of all nodes
    {
        cout << "Testing DTree: Removal: Insert five nodes, remove all of them, then repopulate them." << endl;
        cout << "Expects: All inserted nodes are replaced." << endl;
        DTree obj;
        bool passed = false;

        // THIS IS HELL IM SO SORRY I CANT EVEN MAKE UP AN EXCUSE AT THIS POINT
        try {
            DNode *removed1 = nullptr;
            DNode *removed2 = nullptr;
            DNode *removed3 = nullptr;
            DNode *removed4 = nullptr;
            DNode *removed5 = nullptr;

            // insert five nodes into the tree
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));

            // remove all of the nodes, making sure that they were actually removed
            if (obj.remove(BASE_ACCOUNT_DISC, removed1) && obj.remove(BASE_ACCOUNT_DISC - 1, removed2)
                                                        && obj.remove(BASE_ACCOUNT_DISC - 2, removed3)
                                                        && obj.remove(BASE_ACCOUNT_DISC + 1, removed4)
                                                        && obj.remove(BASE_ACCOUNT_DISC + 2, removed5))
            {
                // make sure that they are all vacant
                if (removed1 && removed2 && removed3 && removed4 && removed5) {
                    if (removed1->isVacant() && removed2->isVacant()
                                             && removed3->isVacant()
                                             && removed4->isVacant()
                                             && removed5->isVacant())
                    {
                        // insert into all of the vacant nodes
                        obj.insert(createAccount(BASE_ACCOUNT_DISC));
                        obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
                        obj.insert(createAccount(BASE_ACCOUNT_DISC - 2));
                        obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
                        obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));

                        // verify that remnoved root is still vacant and the root
                        // was repopulated with a different node
                        if (removed1->isVacant() && !tester.getDRoot(obj)->isVacant()) {
                            if (removed1->getDiscriminator() == tester.getDRoot(obj)->getDiscriminator()) {
                                passed = true;
                            }
                        }
                    }
                }
            }

            if (removed1) { delete removed1; }
            if (removed2) { delete removed2; }
            if (removed3) { delete removed3; }
            if (removed4) { delete removed4; }
            if (removed5) { delete removed5; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    return;
}

void dTreeRebalanceTests(int &numTestsPassed, int &numTests) {
    Tester tester;

    // rebuilding entire tree from right
    {
        cout << "Testing DTree: Rebalance: Rebuilding entire tree, triggered from right side." << endl;
        cout << "Expects: The entire tree rebuilt, sorted and balanced." << endl;
        DTree obj;
        bool passed = false;

        try {
            // insert a root, then four nodes to the right of it to trigger a rebalance
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 3));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 4));

            // verify that all nodes are in the order in which they should be
            // should have gone from 5->6->7->8->9 to 6<-5<-7->9->8
            DNode *root = tester.getDRoot(obj);
            if (root->getDiscriminator() == BASE_ACCOUNT_DISC + 2) {
                if (tester.getDNodeLeft(root)->getDiscriminator() == BASE_ACCOUNT_DISC) {
                    if (tester.getDNodeRight(root)->getDiscriminator() == BASE_ACCOUNT_DISC + 3) {
                        if (tester.verifyDSizes(obj)) {
                            passed = true;
                        }
                    }
                }
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // rebuilding entire tree from left
    {
        cout << "Testing DTree: Rebalance: Rebuilding entire tree, triggered from left side." << endl;
        cout << "Expects: The entire tree rebuilt, sorted and balanced." << endl;
        DTree obj;
        bool passed = false;

        try {
            // insert a root, then four nodes to the left of it to trigger a rebalance
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 3));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 4));

            // verify that all nodes are in the order in which they should be
            // should have gone from 1<-2<-3<-4<-5 to 2<-1<-3->5->4
            DNode *root = tester.getDRoot(obj);
            if (root->getDiscriminator() == BASE_ACCOUNT_DISC - 2) {
                if (tester.getDNodeLeft(root)->getDiscriminator() == BASE_ACCOUNT_DISC - 4) {
                    if (tester.getDNodeRight(root)->getDiscriminator() == BASE_ACCOUNT_DISC - 1) {
                        if (tester.verifyDSizes(obj)) {
                            passed = true;
                        }
                    }
                }
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // double rebalance from left
    {
        cout << "Testing DTree: Rebalance: Rebuild the entire tree from left." << endl;
        cout << "Expects: Two rebalances." << endl;
        DTree obj;
        bool passed = false;

        try {
            // insert a root
            obj.insert(createAccount(BASE_ACCOUNT_DISC));

            // insert three nodes on the right
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 3));

            // insert five nodes on the left
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 3));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 4));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 5));

            DNode *root = tester.getDRoot(obj);
            DNode *leftSubRoot = tester.getDNodeLeft(root);
            DNode *rightSubRoot = tester.getDNodeRight(root);

            // the left subtree should rebalance first, then the entire tree
            if (root->getDiscriminator() == BASE_ACCOUNT_DISC - 1) {
                if (leftSubRoot->getDiscriminator() == BASE_ACCOUNT_DISC - 4
                        && rightSubRoot->getDiscriminator() == BASE_ACCOUNT_DISC + 1)
                {
                    if (tester.getDNodeLeft(leftSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC - 5
                            && tester.getDNodeLeft(rightSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC
                            && tester.getDNodeRight(leftSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC - 3
                            && tester.getDNodeRight(rightSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC + 2)
                    {
                        if (tester.verifyDSizes(obj)) {
                            passed = true;
                        }
                    }
                }
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // double rebalance from right
    {
        cout << "Testing DTree: Rebalance: Rebuild the entire tree from right." << endl;
        cout << "Expects: Two rebalances." << endl;
        DTree obj;
        bool passed = false;

        try {
            // insert a root
            obj.insert(createAccount(BASE_ACCOUNT_DISC));

            // insert three nodes on the left
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC - 3));

            // insert five nodes on the right
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 3));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 4));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 5));

            DNode *root = tester.getDRoot(obj);
            DNode *leftSubRoot = tester.getDNodeLeft(root);
            DNode *rightSubRoot = tester.getDNodeRight(root);

            // the right subtree should rebalance first, then the entire tree
            if (root->getDiscriminator() == BASE_ACCOUNT_DISC + 1) {
                if (leftSubRoot->getDiscriminator() == BASE_ACCOUNT_DISC - 2
                        && rightSubRoot->getDiscriminator() == BASE_ACCOUNT_DISC + 3)
                {
                    if (tester.getDNodeLeft(leftSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC - 3
                            && tester.getDNodeLeft(rightSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC + 2
                            && tester.getDNodeRight(leftSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC - 1
                            && tester.getDNodeRight(rightSubRoot)->getDiscriminator() == BASE_ACCOUNT_DISC + 4)
                    {
                        if (tester.verifyDSizes(obj)) {
                            passed = true;
                        }
                    }
                }
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // rebuild with vacant nodes
    {
        cout << "Testing DTree: Rebalance: Rebuild the entire tree with vacant nodes." << endl;
        cout << "Expects: No vacant nodes are present after rebuilding." << endl;
        DTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            // insert five nodes, then remove one of them
            obj.insert(createAccount(BASE_ACCOUNT_DISC));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 3));
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 4));

            obj.remove(BASE_ACCOUNT_DISC + 4, removed);

            // insert a node that is out of order from the vacant node, and also
            // triggers a rotation
            obj.insert(createAccount(BASE_ACCOUNT_DISC + 2));

            // the tree should have no vacant nodes
            if (tester.getDRoot(obj)->getNumVacant() == 0) {
                if (tester.verifyDSizes(obj)) {
                    passed = true;
                }
            }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    return;
}

void dTreeOverloadedAssignmentTests(int &numTestsPassed, int &numTests) {
    Tester tester;

    // three to empty
    {
        cout << "Testing DTree: Overloaded Assignment: Three nodes to empty tree." << endl;
        cout << "Expects: A deep copy of all nodes." << endl;
        DTree copyTo;
        DTree copyFrom;
        bool passed = false;

        try {
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC));
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC - 1));

            // run the assignment operator and verify that a deep copy was made into empty copyTo
            passed = tester.checkDTreeEquality(copyTo, copyFrom);
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // three to populated
    {
        cout << "Testing DTree: Overloaded Assignment: Three nodes to populated tree." << endl;
        cout << "Expects: No memory leaks and a deep copy." << endl;
        DTree copyTo;
        DTree copyFrom;
        bool passed = false;

        try {
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC));
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC - 1));

            copyTo.insert(createAccount(BASE_ACCOUNT_DISC));
            copyTo.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            copyTo.insert(createAccount(BASE_ACCOUNT_DISC - 1));

            // run the assignment operator and verify that a deep copy was made into populated copyTo
            passed = tester.checkDTreeEquality(copyTo, copyFrom);
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // one to empty
    {
        cout << "Testing DTree: Overloaded Assignment: One node to empty tree." << endl;
        cout << "Expects: The new tree to have a deep copy of the rhs root." << endl;
        DTree copyTo;
        DTree copyFrom;
        bool passed = false;

        try {
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC));

            // run the assignment operator and verify that a deep copy was made into empty copyTo
            passed = tester.checkDTreeEquality(copyTo, copyFrom);
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // one to populated
    {
        cout << "Testing DTree: Overloaded Assignment: One node to populated tree." << endl;
        cout << "Expects: A deep copy of one node to the lhs tree and no mem leaks." << endl;
        DTree copyTo;
        DTree copyFrom;
        bool passed = false;

        try {
            copyFrom.insert(createAccount(BASE_ACCOUNT_DISC));

            copyTo.insert(createAccount(BASE_ACCOUNT_DISC));
            copyTo.insert(createAccount(BASE_ACCOUNT_DISC + 1));
            copyTo.insert(createAccount(BASE_ACCOUNT_DISC - 1));

            // run the assignment operator and verify that a deep copy was made into populated copyTo
            passed = tester.checkDTreeEquality(copyTo, copyFrom);
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // no nodes
    {
        cout << "Testing DTree: Overloaded Assignment: No nodes." << endl;
        cout << "Expects: Nothing to happen." << endl;
        DTree copyTo;
        DTree copyFrom;
        bool passed = false;

        try {
            // run an empty overloaded assignment, nothing should happen
            passed = tester.checkDTreeEquality(copyTo, copyFrom);
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // many nodes
    {
        cout << "Testing DTree: Overloaded Assignment: Many nodes." << endl;
        cout << "Expects: All nodes will be present." << endl;
        DTree copyTo;
        DTree copyFrom;
        bool passed = false;

        try {
            copyTo.insert(createAccount(0));

            // insert 100 nodes to trigger several rotations
            for (int i = 0; i < 100; i++) {
                copyFrom.insert(createAccount(i));
            }

            passed = tester.checkDTreeEquality(copyTo, copyFrom);
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    return;
}

void dTreeInsertionTimeRun() {
    Tester tester;

    cout << "Testing DTree: Insertion time." << endl;
    tester.dTreeInsertionTime(NUM_TRIALS, NUM_INSERTIONS);

    return;
}

//////////////////////////// vvv uTree vvv ///////////////////////////////
void uTreeInsertTests(int &numTestsPassed, int &numTests) {
    Tester tester;

    // L & RL
    {
        cout << "Testing UTree: Insertion: Insert 26 elems on the right to trigger L & LR Rotations." << endl;
        cout << "Expects: Rotations to happen and retrieval to return true." << endl;
        UTree obj;
        bool passed = true;

        try {
            // insert alphabet from a-z to trigger L & RL rotations
            for (int i = 0; i < NUM_CHARS; i++) {
                string letter(1, ALPHABET[i]);

                if (!obj.insert(createAccount(0, letter))) {
                    passed = false;
                }
            }

            // retrieve the alphabet, verifying all letters are present
            for (int i = 0; i < NUM_CHARS; i++) {
                string letter(1, ALPHABET[i]);

                if (!obj.retrieve(letter)) {
                    passed = false;
                }
            }

            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // R & LR
    {
        cout << "Testing UTree: Insertion: Insert 26 elems on the left to trigger R & LR Rotations." << endl;
        cout << "Expects: Rotations to happen and retrieval to return true." << endl;
        UTree obj;
        bool passed = true;

        try {
            // insert the alphabet from z-a to trigger R & LR rotations
            for (int i = NUM_CHARS - 1; i >= 0; i--) {
                string letter(1, ALPHABET[i]);

                if (!obj.insert(createAccount(0, letter))) {
                    passed = false;
                }
            }

            // retrieve the alphabet, verifying all letters are present
            for (int i = 0; i < NUM_CHARS; i++) {
                string letter(1, ALPHABET[i]);

                if (!obj.retrieve(letter)) {
                    passed = false;
                }
            }

            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    return;
}

void uTreeRemoveTests(int &numTestsPassed, int &numTests) {
    Tester tester;

    // basic removal from root
    {
        cout << "Testing UTree: Removal: Proper removal from root." << endl;
        cout << "Expects: A proper DNode object to be returned." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            Account acc1 = createAccount(0, "Chief");
            Account acc2 = createAccount(1, "Chief");

            obj.insert(acc1);
            obj.insert(acc2);

            // remove from the DTree & UNode root
            if (obj.removeUser("Chief", 0, removed)) {
                if (removed) {
                    if (removed->getUsername() == "Chief" && removed->getDiscriminator() == 0) {
                        passed = true;
                    }
                }
            }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal of a username that doesn't exist
    {
        cout << "Testing UTree: Removal: Removing a username that doesn't exist." << endl;
        cout << "Expects: Removed node should be nullptr and fxn should return false." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            obj.insert(createAccount(0, "God Himself"));

            // remove from UNode that doesnt exist
            if (!obj.removeUser("WOAHHHHH I DONT EXIST :D", 0, removed)) {
                if (removed == nullptr) { passed = true; }
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal of a username that does exist but a discriminator that doesn't
    {
        cout << "Testing UTree: Removal: Removing a username that does exist but a discriminator that doesn't." << endl;
        cout << "Expects: Removed should be nullptr and fxn should return false." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            obj.insert(createAccount(0, "Stinky Pancake"));
            obj.insert(createAccount(0, "I exist but I wish I didn't."));

            if (!obj.removeUser("Stinky Pancake", 1, removed)) {
                if (removed == nullptr) { passed = true; }
            }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // removal from an empty tree
    {
        cout << "Testing UTree: Removal: Removing from an empty tree." << endl;
        cout << "Expects: Removed should be nullptr and fxn should return false." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            if (!obj.removeUser("HRRRRNGHHHHHHHHH", 0, removed)) {
                if (removed == nullptr) { passed = true; }
            }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // make a leaf node empty
    {
        cout << "Testing UTree: Removal: Making a leaf left UNode empty." << endl;
        cout << "Expects: The UNode leaf is deleted." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            obj.insert(createAccount(0, "Poopsock."));

            Account acc = createAccount(0, "PLEAS EMANM PLEASSE DONT DELET ME");
            obj.insert(acc);

            // make the dtree of a leaf UNode empty, which should delete the node
            // and make the left pointer a nullptr
            if (obj.removeUser("PLEAS EMANM PLEASSE DONT DELET ME", 0, removed)) {
                if (tester.getUNodeLeft(tester.getURoot(obj)) == nullptr) {
                    if (removed->getUsername() == acc.getUsername()) {
                        passed = true;
                    }
                }
            }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // make a leaf node empty
    {
        cout << "Testing UTree: Removal: Making a leaf right UNode empty." << endl;
        cout << "Expects: The UNode leaf is deleted." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            // insert five nodes to avoid a rotation, one root, two left, two right
            obj.insert(createAccount(0, "PLEAS EMANM PLEASSE DONT DELET ME"));
            obj.insert(createAccount(0, "I am literally just vibing."));

            Account acc = createAccount(0, "Poopsock.");
            obj.insert(acc);

            obj.insert(createAccount(0, "E."));
            obj.insert(createAccount(0, "ZEBRAMAN"));

            // make the dtree of a leaf UNode empty, which should delete the node
            // and make the right pointer a nullptr
            if (obj.removeUser("Poopsock.", 0, removed)) {
                if (tester.getUNodeRight(tester.getURoot(obj))->getUsername() == "ZEBRAMAN") {
                    if (removed->getUsername() == acc.getUsername()) {
                        passed = true;
                    }
                }
            }

            if (removed) { delete removed; }
            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // make the root node empty
    {
        cout << "Testing UTree: Removal: Making the root node empty." << endl;
        cout << "Expects: The root was removed and replaced." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            obj.insert(createAccount(0, "Poopsockington"));

            // make the root UNode empty, which should delete the UNode itself
            // and make the root point to nullptr
            if (obj.removeUser("Poopsockington", 0, removed)) {
                if (tester.getURoot(obj) == nullptr) {
                    if (removed->getUsername() == "Poopsockington") {
                        // verify that we can replace the root
                        obj.insert(createAccount(0, "Pain."));

                        if (tester.getURoot(obj)->getUsername() == "Pain.") {
                            passed = true;
                        }
                    }
                }
            }

            if (removed) { delete removed; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // make a middle node empty
    {
        cout << "Testing UTree: Removal: Making a middle node empty." << endl;
        cout << "Expects: The middle node was deleted." << endl;
        UTree obj;
        bool passed = false;
        DNode *removed = nullptr;

        try {
            obj.insert(createAccount(0, "Stinker"));
            obj.insert(createAccount(0, "Dinker"));
            obj.insert(createAccount(0, "what"));

            // make the root UNode empty, which should delete the UNode itself
            // and make the root point to nullptr
            if (obj.removeUser("Dinker", 0, removed)) {
                if (tester.getUNodeRight(tester.getURoot(obj))->getUsername() == "what") {
                    if (removed->getUsername() == "Dinker") {
                        passed = true;
                    }
                }
            }

            if (removed) { delete removed; }
            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    return;
}

void uTreeRotationTests(int &numTestsPassed, int &numTests) {
    Tester tester;

    // L rotation
    {
        cout << "Testing UTree: Rotation: L rotation." << endl;
        cout << "Expects: A proper L rotation." << endl;
        UTree obj;
        bool passed = false;

        try {
            obj.insert(createAccount(0, "Fella1"));
            obj.insert(createAccount(0, "Fella2"));
            obj.insert(createAccount(0, "Fella3"));
            obj.insert(createAccount(0, "Fella4"));

            UNode *root = tester.getURoot(obj);
            UNode *left = tester.getUNodeLeft(root);
            UNode *right = tester.getUNodeRight(root);

            // verify that an L rotation took place and that the nodes are in the proper
            // positions
            if (root->getUsername() == "Fella2" && left->getUsername() == "Fella1") {
                if (right->getUsername() == "Fella3") {
                    if (tester.getUNodeRight(right)->getUsername() == "Fella4") {
                        passed = true;
                    }
                }
            }

            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // R rotation
    {
        cout << "Testing UTree: Rotation: R rotation." << endl;
        cout << "Expects: A proper R rotation." << endl;
        UTree obj;
        bool passed = false;

        try {
            obj.insert(createAccount(0, "Fella4"));
            obj.insert(createAccount(0, "Fella3"));
            obj.insert(createAccount(0, "Fella2"));
            obj.insert(createAccount(0, "Fella1"));

            UNode *root = tester.getURoot(obj);
            UNode *left = tester.getUNodeLeft(root);
            UNode *right = tester.getUNodeRight(root);

            // verify that an R rotation took place and that the nodes are in the proper
            // positions
            if (root->getUsername() == "Fella3" && right->getUsername() == "Fella4") {
                if (left->getUsername() == "Fella2") {
                    if (tester.getUNodeLeft(left)->getUsername() == "Fella1") {
                        passed = true;
                    }
                }
            }

            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // L & RL
    {
        cout << "Testing UTree: Insertion: Insert 26 elems on the right, verifying the order of insertion." << endl;
        cout << "Expects: Rotations should maintain order." << endl;
        UTree obj;
        bool passed = true;

        try {
            // insert alphabet from a-z to trigger L & RL rotations
            for (int i = 0; i < NUM_CHARS; i++) {
                string letter(1, ALPHABET[i]);

                if (!obj.insert(createAccount(0, letter))) {
                    passed = false;
                }
            }

            // create a vector rather than an array for easy insertion and put
            // the tree into it
            vector<UNode*> arr;
            tester.putUIntoArr(obj, arr);

            // iterate through the vector, verifying that all usernames are greater
            // than the previous
            for (uint i = 0; i < arr.size(); i++) {
                if (!i) { continue; }
                if (arr.at(i)->getUsername() < arr.at(i - 1)->getUsername()) {
                    passed = false;
                    break;
                }
            }

            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    cout << endl;

    // R & LR
    {
        cout << "Testing UTree: Insertion: Insert 26 elems on the left, verifying the order of insertion." << endl;
        cout << "Expects: Rotations should maintain order." << endl;
        UTree obj;
        bool passed = true;

        try {
            // insert the alphabet from z-a to trigger R & LR rotations
            for (int i = NUM_CHARS - 1; i >= 0; i--) {
                string letter(1, ALPHABET[i]);

                if (!obj.insert(createAccount(0, letter))) {
                    passed = false;
                }
            }

            // create a vector rather than an array for easy insertion and put
            // the tree into it
            vector<UNode*> arr;
            tester.putUIntoArr(obj, arr);

            // iterate through the vector, verifying that all usernames are greater
            // than the previous
            for (uint i = 0; i < arr.size(); i++) {
                if (!i) { continue; }
                if (arr.at(i)->getUsername() < arr.at(i - 1)->getUsername()) {
                    passed = false;
                    break;
                }
            }

            if (!tester.verifyUHeights(obj)) { passed = false; }
        } catch (...) {
            passed = false;
        }

        assertFinish(passed, numTests, numTestsPassed);
    }

    return;
}

void uTreeInsertionTimeRun() {
    Tester tester;

    cout << "Testing UTree: Insertion time." << endl;
    tester.uTreeInsertionTime(NUM_TRIALS, NUM_INSERTIONS);

    return;
}

// acts as a shortcut to create account arguments more easily
Account createAccount(int disc, string username) {
    Account acc(username, disc, false, "", "");
    return acc;
}

// prints out if a test passed or failed and increments test counters
void assertFinish(bool expr, int &numTests, int &numTestsPassed) {
    if (expr) {
        cout << "\tTest Passed." << endl;
        numTestsPassed++;
    }
    else { cout << "\tTest Failed." << endl; }

    numTests++;

    return;
}

// main container to verify the sizes of all DNodes in a DTree
bool Tester::verifyDSizes(DTree &obj) {
    return verifyDSizes(obj, obj._root);
}

// node helper for verifyheights(DTree&)
bool Tester::verifyDSizes(DTree &obj, DNode *&node) {
    // the rundown for this fxn is if at any point, balanced gets turned to false,
    // keep it set to false.
    bool balanced = true;

    if (obj.checkImbalance(node)) { balanced = false; }

    if (node->_left) {
        balanced = (balanced) ? verifyDSizes(obj, node->_left) : false;
    }

    if (node->_right) {
        balanced = (balanced) ? verifyDSizes(obj, node->_right) : false;
    }

    return balanced;
}

// main container to verify the heights of all UNodes in a UTree
bool Tester::verifyUHeights(UTree &obj) {
    return verifyUHeights(obj, obj._root);
}

// node helper for verifyUHeights(UTree&)
bool Tester::verifyUHeights(UTree &obj, UNode *&node) {
    // the rundown for this fxn is if at any point, balanced gets turned to false,
    // keep it set to false.
    bool balanced = true;
    int balanceFactor = obj.checkImbalance(node);

    if (balanceFactor > 1 || balanceFactor < -1) { balanced = false; }

    if (node->_left) {
        balanced = (balanced) ? verifyUHeights(obj, node->_left) : false;
    }

    if (node->_right) {
        balanced = (balanced) ? verifyUHeights(obj, node->_right) : false;
    }

    return balanced;
}

// converts the DTree into a vector for order checking
void Tester::putDIntoArr(DTree &obj, vector<DNode*> &arr) {
    putDIntoArr(arr, obj._root);
    return;
}

// similar to the method in DTree, put all DNodes into an array (vect here)
// in order
void Tester::putDIntoArr(vector<DNode*> &arr, DNode *&node) {
    if (node->_left) {
        putDIntoArr(arr, node->_left);
    }

    arr.push_back(node);

    if (node->_right) {
        putDIntoArr(arr, node->_right);
    }

    return;
}

// return the root of a DTree
DNode* Tester::getDRoot(DTree &obj) {
    return obj._root;
}

// return the left child of a Dnode
DNode* Tester::getDNodeLeft(DNode *node) {
    return node->_left;
}

// return the right child of a dnode
DNode* Tester::getDNodeRight(DNode *node) {
    return node->_right;
}

// verify that the overloaded = worked
bool Tester::checkDTreeEquality(DTree &copyTo, DTree &copyFrom) {
    copyTo = copyFrom;

    if (&copyTo != &copyFrom) {
        return checkDNodeEquality(copyTo._root, copyFrom._root);
    }

    return false;
}

// recursive helper for the above
bool Tester::checkDNodeEquality(DNode *lhs, DNode *rhs) {
    // if both of the nodes dont exist, weve reached the end. if one of them doesnt exist
    // something went wrong
    if (!(lhs || rhs)) { return true; }
    else if ((!lhs && rhs) || (lhs && !rhs)) { return false; }

    bool properCopy = false;

    Account lhsAcc = lhs->getAccount();
    Account rhsAcc = rhs->getAccount();

    // verify all acc details
    if (lhsAcc.getUsername() == rhsAcc.getUsername() && lhsAcc.getDiscriminator() == rhsAcc.getDiscriminator()
            && lhsAcc.hasNitro() == rhsAcc.hasNitro() && lhsAcc.getBadge() == rhsAcc.getBadge()
            && lhsAcc.getStatus() == lhsAcc.getStatus() && &lhsAcc != &rhsAcc)
    {
        // verify all node members
        if (lhs->getSize() == rhs->getSize() && lhs->getNumVacant() == rhs->getNumVacant()
                && lhs->isVacant() == rhs->isVacant())
        {
            // verify deep copy
            if (lhs != rhs) {
                properCopy = true;
            }
        }
    }

    return properCopy && checkDNodeEquality(lhs->_left, rhs->_left) && checkDNodeEquality(lhs->_right, rhs->_right);
}

// return the root of a UTree
UNode* Tester::getURoot(UTree& tree) {
    return tree._root;
}

// return the left child of a UNode
UNode* Tester::getUNodeLeft(UNode* node) {
    return node->_left;
}

// return the right child of a UNode
UNode* Tester::getUNodeRight(UNode* node) {
    return node->_right;
}

// converts the UTree into a vector for order checking
void Tester::putUIntoArr(UTree &obj, vector<UNode*> &arr) {
    putUIntoArr(arr, obj._root);
    return;
}

// similar to the method for DTree, put all Unodes into an array (vect) in order
void Tester::putUIntoArr(vector<UNode*> &arr, UNode *&node) {
    if (node->_left) {
        putUIntoArr(arr, node->_left);
    }

    arr.push_back(node);

    if (node->_right) {
        putUIntoArr(arr, node->_right);
    }

    return;
}

// test insertion time into a DTree
void Tester::dTreeInsertionTime(int numTrials, int N) {
    const int SCALING = 2;

    // start and end time keep track of when the test was started and finished
    clock_t startTime;
    clock_t endTime;

    // timeTaken is a shortcut for endTime - startTime
    double timeTaken = 0.0;

    // stores the previous times of each iteration to compare
    double prevTime = 0.0;

    for (int i = 0; i < numTrials; i++) {
        DTree *obj = new DTree;

        int name = 0;
        int disc = 0;

        // insert N elements, keeping track of start and end time
        startTime = clock();
        for (int i = 0; i < N; i++) {
            disc = i % MAX_DISC;
            if (!disc) { name++; }
            obj->insert(createAccount(disc, to_string(name)));
        }
        endTime = clock();

        // output how long the enqueue test took
        timeTaken = double(endTime - startTime) / CLOCKS_PER_SEC;
        cout << "\tInserted " << N << " accounts into DTree, took " << timeTaken
             << " seconds ";

        // if there was actually a previous iteration, output how much longer
        // this one took (should float around 2x for O(n) since our scaling is 2)
        if (prevTime) {
            cout << '(' << double(timeTaken / prevTime)
                 << "x longer than previous insertion)";
        }

        // store into the previous time for tracking
        prevTime = timeTaken;

        cout << endl;

        // scale our number of elements by 2
        N *= SCALING;

        delete obj;
    }

    return;
}

void Tester::uTreeInsertionTime(int numTrials, int N) {
    const int SCALING = 2;

    // start and end time keep track of when the test was started and finished
    clock_t startTime;
    clock_t endTime;

    // timeTaken is a shortcut for endTime - startTime
    double timeTaken = 0.0;

    // stores the previous times of each iteration to compare
    double prevTime = 0.0;

    for (int i = 0; i < numTrials; i++) {
        UTree *obj = new UTree;

        int name = 0;
        int disc = 0;

        startTime = clock();
        for (int i = 0; i < N; i++) {
            disc = i % MAX_DISC;
            if (!disc) { name++; }
            obj->insert(createAccount(disc, to_string(name)));
        }
        endTime = clock();

        // output how long the enqueue test took
        timeTaken = double(endTime - startTime) / CLOCKS_PER_SEC;
        cout << "\tInserted " << N << " accounts into UTree, took " << timeTaken
             << " seconds ";

        // if there was actually a previous iteration, output how much longer
        // this one took (should float around 2x for O(n) since our scaling is 2)
        if (prevTime) {
            cout << '(' << double(timeTaken / prevTime)
                 << "x longer than previous insertion)";
        }

        // store into the previous time for tracking
        prevTime = timeTaken;

        cout << endl;

        // scale our number of elements by 2
        N *= SCALING;

        delete obj;
    }

    return;
}
