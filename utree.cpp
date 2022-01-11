/***************************
* File:     utree.cpp
* Project:  Project 2
* Author:   Drew Barlow
* Date:     3/13/2021
* Section:  4
* E-mail:   abarlow1@umbc.edu
*
* Implementation of UTree.h.
***************************/
#include "utree.h"

/**
 * Destructor, deletes all dynamic memory.
 */
UTree::~UTree() {
    clear();
}

/**
 * Sources a .csv file to populate Account objects and insert them into the UTree.
 * @param infile path to .csv file containing database of accounts
 * @param append true to append to an existing tree structure or false to clear before importing
 */
void UTree::loadData(string infile, bool append) {
    std::ifstream instream(infile);
    string line;
    char delim = ',';
    const int numFields = 5;
    string fields[numFields];

    /* Check to make sure the file was opened */
    if(!instream.is_open()) {
        std::cerr << __FUNCTION__ << ": File " << infile << " could not be opened or located" << endl;
        exit(-1);
    }

    /* Should we append or clear? */
    if(!append) this->clear();

    /* Read in the data from the .csv file and insert into the UTree */
    while(std::getline(instream, line)) {
        std::stringstream buffer(line);

        /* Quick check to make sure each line is formatted correctly */
        int delimCount = 0;
        for(unsigned int c = 0; c < buffer.str().length(); c++) if(buffer.str()[c] == delim) delimCount++;
        if(delimCount != numFields - 1) {
            throw std::invalid_argument("Malformed input file detected - ensure each line contains 5 fields deliminated by a ','");
        }

        /* Populate the account attributes -
         * Each line always has 5 sections of data */
        for(int i = 0; i < numFields; i++) {
            std::getline(buffer, line, delim);
            fields[i] = line;
        }
        Account newAcct = Account(fields[0], std::stoi(fields[1]), std::stoi(fields[2]), fields[3], fields[4]);
        this->insert(newAcct);
    }
}

/**
 * Dynamically allocates a new UNode in the tree and passes insertion into DTree.
 * Should also update heights and detect imbalances in the traversal path after
 * an insertion.
 * @param newAcct Account object to be inserted into the corresponding DTree
 * @return true if the account was inserted, false otherwise
 */
bool UTree::insert(Account newAcct) {
    bool inserted = this->insert(this->_root, newAcct);

    // once we've reached the root and a node was actually inserted, rotate the
    // root if it needs it
    if (inserted) {
        this->_root = rebalance(this->_root);
    }

    return inserted;
}

// preconditions: the insert shell was called for account
// postconditions: an account is inserted into a UNode if a username matches,
//                 else a new UNode is created and the tree is rebalanced if it needs it
bool UTree::insert(UNode *currNode, Account insertMe) {
    // if the tree does not yet have a root, make one
    if (!this->_root) {
        this->_root = new UNode();

        return this->_root->insert(insertMe);
    }

    // we need to use these booleans to determine if we should rotate the left or right
    // nodes later after insertion
    bool leftInsert = false;
    bool rightInsert = false;

    if (insertMe.getUsername() < currNode->getUsername()) {
        // if there is a left node, continue to perform recursion down until there
        // is an empty node to insert at
        if (currNode->_left) {
            leftInsert = this->insert(currNode->_left, insertMe);

            // if we didn't successfully insert the node, stop execution and return false
            if (!leftInsert) { return false; }
        } else {
            // if we're at a node that does not exist, we can insert our node here
            currNode->_left = new UNode();
            bool inserted = currNode->_left->insert(insertMe);

            updateHeight(currNode);

            return inserted;
        }
    } else if (insertMe.getUsername() > currNode->getUsername()) {
        // if there is a right node, continue to traverse the tree until we find a node
        // to insert into
        if (currNode->_right) {
            rightInsert = this->insert(currNode->_right, insertMe);

            // if there wasn't an insertion, stop execution
            if (!rightInsert) { return false; }
        } else {
            // if the next node does not exist, make a new UNode in it's place
            currNode->_right = new UNode();
            bool inserted = currNode->_right->insert(insertMe);

            updateHeight(currNode);

            return inserted;
        }
    } else {
        // if the usernames are equal, insert the account into this node
        return currNode->insert(insertMe);
    }

    updateHeight(currNode);

    // if we inserted into a node successfully, check the node path we went down
    // for if it needs a rebalance
    if (leftInsert) { currNode->_left = rebalance(currNode->_left); }
    else if (rightInsert) { currNode->_right = rebalance(currNode->_right); }

    return true;
}

// preconditions: we're at a UNode in which we can insert the account
// postconditions: the dtree attempts to insert the node, returns true if it did else false
bool UNode::insert(Account acc) {
    return this->_dtree->insert(acc);
}

/**
 * Removes a user with a matching username and discriminator.
 * @param username username to match
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool UTree::removeUser(string username, int disc, DNode*& removed) {
    bool removedNode = this->remove(this->_root, username, disc, removed);

    // if we did remove a node and the root is empty, we know we removed from the root
    // so we can replace it with something else
    if (removedNode && !this->_root->getNumUsers()) {
        this->_root = findReplacement(this->_root);
    }

    return removedNode;
}

// preconditions: removeUser() is called
// postconditions: a DNode is removed from a UNode's dtree and if the unode is empty,
//                 it is delete and replaced
bool UTree::remove(UNode *currNode, string username, int disc, DNode *&removed) {
    // we only want to check this node for removal if it actually exists... duh
    if (currNode) {
        if (username < currNode->getUsername()) {
            bool removedNode = this->remove(currNode->_left, username, disc, removed);

            // if we removed a node from the left side and the left node is now empty
            // then we can find a replacement for it and delete it
            if (removedNode && !currNode->_left->getNumUsers()) {
                currNode->_left = findReplacement(currNode->_left);
                updateHeight(currNode);
            }

            return removedNode;
        } else if (username > currNode->getUsername()) {
            bool removedNode = this->remove(currNode->_right, username, disc, removed);

            // if we removed a node from the right side and the right node is now empty
            // then we can find a replacement for it and delete it
            if (removedNode && !currNode->_right->getNumUsers()) {
                currNode->_right = findReplacement(currNode->_right);
                updateHeight(currNode);
            }

            return removedNode;
        } else if (username == currNode->getUsername()) {
            // if we found a matching username in this node, we can attempt to remove
            // from this nodes DTree
            return currNode->remove(disc, removed);
        }
    }

    // if none of the conditions above occur, then nothing was removed and we return false
    return false;
}

// preconditions: we found a unode with a matching username for removal
// postconditions: the dtree attempts to remove the node, returns true if it did else false
bool UNode::remove(int disc, DNode *&removed) {
    return this->_dtree->remove(disc, removed);
}

// preconditions: a DNode was removed from a UNode's DTree
// postconditions: a the node passed into this function is replaced by the next
//                 largest UNode in the tree
UNode* UTree::findReplacement(UNode *empty) {
    UNode *trav = empty;

    // we need to go down left, then all the way to the right to find the largest
    // node in this subtree
    if (trav->_left) {
        // find the deepest node in the left subtree and copy the dtree of it into
        // the empty node's, then delete the found node
        trav = replacementTraversal(trav->_left);
        *empty->_dtree = *trav->_dtree;

        delete trav;
        trav = empty;
    } else {
        // if there is no left node, return the next node to the right of empty
        trav = trav->_right;
        delete empty;
    }

    return trav;
}

// preconditions: there was a left node found in the findReplacement() function
// postconditions: the deepest right node of the left subtree found in findReplacement()
//                 is returned
UNode* UTree::replacementTraversal(UNode *candidate) {
    UNode *returnMe = nullptr;

    // if there is a node to the right of the passed node, verify if it is the deepest node
    if (candidate->_right) {

        // if the next node is the deepest node, then store it for returning and
        // replace the right pointer of this node with the right node's left node
        if (!candidate->_right->_right) {
            returnMe = candidate->_right;
            candidate->_right = candidate->_right->_left;

            updateHeight(candidate);

            // if the node needs to be rebalanced, do it
            if (candidate->_right) {
                candidate = rebalance(candidate);
            }

            return returnMe;
        } else {
            // if the next node wasn't the deepest node, perform recursion until we're there
            UNode* replacement = replacementTraversal(candidate->_right);
            updateHeight(candidate);

            // if this node is now unbalanced, rebalance it and return the replacement
            candidate = rebalance(candidate);

            return replacement;
        }
    }

    // if there was no right node, then this node is the one that will replace
    // the empty node
    return candidate;
}

/**
 * Retrieves a set of users within a UNode.
 * @param username username to match
 * @return UNode with a matching username, nullptr otherwise
 */
UNode* UTree::retrieve(string username) {
    return this->retrieve(this->_root, username);
}

// preconditions: the retrieve() shell is called
// postconditions: a UNode is returned if the username matches, else nullptr
UNode* UTree::retrieve(UNode *currNode, string username) {
    if (currNode) {
        if (username < currNode->getUsername()) {
            return this->retrieve(currNode->_left, username);
        } else if (username > currNode->getUsername()) {
            return this->retrieve(currNode->_right, username);
        } else if (username == currNode->getUsername()) {
            return currNode;
        }
    }

    return nullptr;
}

/**
 * Retrieves the specified Account within a DNode.
 * @param username username to match
 * @param disc discriminator to match
 * @return DNode with a matching username and discriminator, nullptr otherwise
 */
DNode* UTree::retrieveUser(string username, int disc) {
    return this->retrieveUser(this->_root, username, disc);
}

// preconditions: the retrieveUser() shell is called
// postconditions: the DNode of a UNode's DTree is returned if a username matches, else nullptr
DNode* UTree::retrieveUser(UNode *currNode, string username, int disc) {
    if (currNode) {
        if (username < currNode->getUsername()) {
            return this->retrieveUser(currNode->_left, username, disc);
        } else if (username > currNode->getUsername()) {
            return this->retrieveUser(currNode->_right, username, disc);
        } else if (username == currNode->getUsername()) {
            return currNode->findDisc(disc);
        }
    }

    return nullptr;
}

// preconditions: a UNode is found with a matching username
// postconditions: the account with the matching discriminator is returned if it is found,
//                 else nullptr
DNode* UNode::findDisc(int disc) {
    return this->getDTree()->retrieve(disc);
}

/**
 * Returns the number of users with a specific username.
 * @param username username to match
 * @return number of users with the specified username
 */
int UTree::numUsers(string username) {
    return this->numUsers(this->_root, username);
}

// preconditions: the numUsers() shell is called
// postconditions: the amount of users in a UNode is returned if the username matches
//                 else 0
int UTree::numUsers(UNode *currNode, string username) {
    if (currNode) {
        if (username < currNode->getUsername()) {
            return this->numUsers(currNode->_left, username);
        } else if (username > currNode->getUsername()) {
            return this->numUsers(currNode->_right, username);
        } else if (username == currNode->getUsername()) {
            return currNode->getNumUsers();
        }
    }

    return 0;
}

// preconditions: a UNode is found with a matching username from numUsers()
// postconditions: the amount of accounts in this node's DTree is returned
int UNode::getNumUsers() {
    return this->_dtree->getNumUsers();
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void UTree::clear() {
    // only perform this function if there is memory to deallocate
    if (this->_root) {
        clear(this->_root);
    }

    this->_root = nullptr;

    return;
}

// preconditions: the clear() shell is called
// postconditions: all nodes are deallocated and set to nullptr
void UTree::clear(UNode *currNode) {
    if (currNode->_left) { clear(currNode->_left); }
    if (currNode->_right) { clear(currNode->_right); }

    delete currNode;
    currNode = nullptr;

    return;
}

/**
 * Prints all accounts' details within every DTree.
 */
void UTree::printUsers() const {
    // only print the users if there are any, otherwise output that there
    // are no accounts
    if (this->_root) {
        this->_root->traverseAccounts();
    } else {
        cout << "No accounts stored." << endl;
    }

    return;
}

// preconditions: the printUsers() shell was called
// postconditions: the accounts of this UTree are all printed in order
void UNode::traverseAccounts() {
    if (this->_left) {
        this->_left->traverseAccounts();
    }

    this->getDTree()->printAccounts();

    if (this->_right) {
        this->_right->traverseAccounts();
    }

    return;
}

/**
 * Dumps the UTree in the '()' notation.
 */
void UTree::dump(UNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getUsername() << ":" << node->getHeight() << ":" << node->getDTree()->getNumUsers();
    dump(node->_right);
    cout << ")";
}

/**
 * Updates the height of the specified node.
 * @param node UNode object in which the height will be updated
 */
void UTree::updateHeight(UNode* node) {
    node->calcNodeHeight();

    return;
}

// preconditions: a UNode needs to have its height updated
// postconditions: the node's height is updated based on its children's
void UNode::calcNodeHeight() {
    // if there is neither a left nor a right node, put 0 in it's place
    int left = (this->_left) ? this->_left->getHeight() : 0;
    int right = (this->_right) ? this->_right->getHeight() : 0;

    // take the max between the two heights and use that as this node's height
    this->_height = (left > right) ? left : right;

    // if there are no children, this is a leaf node, so its height should still be 0.
    // there was a child, then the height should be the height of the greatest child + 1
    this->_height += (this->_left || this->_right) ? 1 : 0;

    return;
}

// preconditions: an imbalance has occurred in the tree and a right rotation is needed
// postconditions: a right rotation is performed, where the node passed in becomes
//                 the right child of it's left child and it takes the left node's right child
//                 as its left child
UNode* UTree::rotateRight(UNode *oldRoot) {
    UNode *newRoot = oldRoot->_left;
    oldRoot->_left = newRoot->_right;
    newRoot->_right = oldRoot;

    // update the heights of both nodes
    updateHeight(oldRoot);
    updateHeight(newRoot);

    return newRoot;
}

// preconditions: an imbalance has occurred in the tree and a left rotation is needed
// postconditions: a left rotation is performed, where the node passed in becomes
//                 the left child of it's right child and it takes the right node's left child
//                 as its right child
UNode* UTree::rotateLeft(UNode *oldRoot) {
    UNode *newRoot = oldRoot->_right;
    oldRoot->_right = newRoot->_left;
    newRoot->_left = oldRoot;

    // update the heights of both nodes
    updateHeight(oldRoot);
    updateHeight(newRoot);

    return newRoot;
}

/**
 * Checks for an imbalance, defined by AVL rules, at the specified node.
 * @param node UNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
int UTree::checkImbalance(UNode* node) {
    int leftSize = (node->_left) ? node->_left->getHeight() : -1;
    int rightSize = (node->_right) ? node->_right->getHeight() : -1;

    // return the balance factor of this node's immediate children
    return leftSize - rightSize;
}

/**
 * Begins and manages the rebalance procedure for an AVL tree (returns a pointer).
 * @param node UNode object where an imbalance occurred
 * @return UNode object replacing the unbalanced node's position in the tree
 */
UNode* UTree::rebalance(UNode* node) {
    const int RIGHT_IMBALANCE_MARKER = -2;  // used to determine when to L & RL rotate
    const int LEFT_IMBALANCE_MARKER = 2;  // used to determine when to R & LR rotate
    const int HEAVY_RIGHT_CHILD = 1;
    const int HEAVY_LEFT_CHILD = -1;

    int rootImbalanceNum = checkImbalance(node);

    // if the balance factor is negative, then the right subtree is heavier and
    // if it is less than -1, it needs a rebalnce.
    if (rootImbalanceNum <= RIGHT_IMBALANCE_MARKER) {
        // if the right child has a balance factor of 1, meaning it has a heavier
        // left node, then rotate it to the right before rotating to the left
        if (checkImbalance(node->_right) == HEAVY_RIGHT_CHILD) {
            node->_right = rotateRight(node->_right);
        }

        node = rotateLeft(node);

    // the logic for when to right rotate is the same as left rotate, but with
    // a positive balance factor instead of negative
    } else if (rootImbalanceNum >= LEFT_IMBALANCE_MARKER) {
        // if the left child has a balance factor of -1, meaning it has a heavier
        // right node, then rotate it to the left before rotating to the right
        if (checkImbalance(node->_left) == HEAVY_LEFT_CHILD) {
            node->_left = rotateLeft(node->_left);
        }

        node = rotateRight(node);
    }

    updateHeight(node);

    return node;
}
