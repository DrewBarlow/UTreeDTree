/***************************
* File:     dtree.cpp
* Project:  Project 2
* Author:   Drew Barlow
* Date:     3/13/2021
* Section:  4
* E-mail:   abarlow1@umbc.edu
*
* Implementation of DTree.h.
***************************/
#include "dtree.h"

/**
 * Destructor, deletes all dynamic memory.
 */
DTree::~DTree() {
    clear();
}

/**
 * Overloaded assignment operator, makes a deep copy of a DTree.
 * @param rhs Source DTree to copy
 * @return Deep copy of rhs
 */
DTree& DTree::operator=(const DTree& rhs) {
    if (this != &rhs) {
        clear();

        // do not copy anything over if there is nothing in the rhs tree
        if (rhs._root) {
            this->_root = this->copyInOrder(this->_root, rhs._root);
        }
    }

    return *this;
}

// preconditions: the overloaded assignment operator was called on this instance
// postconditions: a deep copy all of the nodes in the order in which they appear
//                 is put into this DTree
DNode* DTree::copyInOrder(DNode *thisNode, DNode *rhsNode) {
    // return a nullptr to copy if we've reached an RHS node that DNE
    if (!rhsNode) {
        return rhsNode;
    }

    // copy the important elems from the RHS' node to this one's to return
    thisNode = new DNode(rhsNode->getAccount());
    thisNode->_size = rhsNode->getSize();
    thisNode->_numVacant = rhsNode->getNumVacant();
    thisNode->_vacant = rhsNode->isVacant();

    // recurse down the left and right nodes, copying them in the order that they're
    // present
    if (rhsNode->_left) {
        thisNode->_left = this->copyInOrder(thisNode->_left, rhsNode->_left);
    }

    if (rhsNode->_right) {
        thisNode->_right = this->copyInOrder(thisNode->_right, rhsNode->_right);
    }

    return thisNode;
}

/**
 * Dynamically allocates a new DNode in the tree.
 * Should also update heights and detect imbalances in the traversal path
 * an insertion.
 * @param newAcct Account object to be contained within the new DNode
 * @return true if the account was inserted, false otherwise
 */
bool DTree::insert(Account newAcct) {
    // create a new DNode from the account that is going to be inserted, as well
    // as variables to check for insertion and root reassignment
    DNode *insertMe = new DNode(newAcct);
    DNode *inserted = nullptr;
    bool replacedRoot = false;

    // given that the root exists, check if it is vacant and insert into it if it is
    if (this->_root) {
        if (this->_root->isVacant() && !this->retrieve(insertMe->getDiscriminator())) {
            this->_root = replaceVacant(this->_root, insertMe);
            replacedRoot = true;
        }
    }

    // if we haven't inserted into the vacant root, perform an otherwise normal
    // BST insertion of the new DNode
    if (!replacedRoot) {
        inserted = insert(this->_root, insertMe);
        this->_root = (inserted) ? inserted : this->_root;
    }

    // if we neither inserted nor replaced the root, delete the dynamically allocated node
    if (!(inserted || replacedRoot)) { delete insertMe; }

    // return the inserted node container as a boolean. if we inserted, a DNode
    // is stored here and true is returned, otherwise the node container is nullptr
    // and is false
    return inserted;
}

DNode* DTree::insert(DNode *currNode, DNode *insertMe) {
    // if currNode is nullptr, return the node to insert as what will be set equal
    // to the previous node's left/right ptr
    if (!currNode) {
        return insertMe;
    } else if (insertMe->getDiscriminator() < currNode->getDiscriminator()) {

        // if there is a node that exists to the left of currNode, check if it is
        // vacant and replace it accordingly, returning the current node afterwards
        if (currNode->_left) {
            if (currNode->_left->isVacant()) {
                if (checkReplacementCandidacy(currNode->_left, insertMe)) {
                    currNode->_left = replaceVacant(currNode->_left, insertMe);
                    return currNode;
                }
            }
        }

        // if the node to the left wasn't vacant or didnt exist, perform a normal
        // BST insertion
        DNode* inserted = insert(currNode->_left, insertMe);

        // if we failed to insert, stop execution of the function and return nullptr
        if (!inserted) { return inserted; }
        currNode->_left = inserted;
    } else if (insertMe->getDiscriminator() > currNode->getDiscriminator()) {

        // similarly to the left node, check if the right node is vacant and replace
        // it if it is
        if (currNode->_right) {
            if (currNode->_right->isVacant()) {
                if (checkReplacementCandidacy(currNode->_right, insertMe)) {
                    currNode->_right = replaceVacant(currNode->_right, insertMe);
                    return currNode;
                }
            }
        }

        // if there was no right node or it wasnt vacant, BST insert as normal
        // and return nullptr if unsuccessful
        DNode* inserted = insert(currNode->_right, insertMe);

        if (!inserted) { return inserted; }
        currNode->_right = inserted;
    } else {
        return nullptr;
    }

    // update the size of the current node, given that an insertion happened
    updateSize(currNode);

    // rebalance the current node if need be and return it/the new root as the DNode that
    // will be set equal to the previous node's right/left ptr
    return rebalance(currNode);
}

// preconditions: an insertion is in process and a node is vacant
// postconditions: true is returned if the replacement node will maintain the order
//                 of the tree if inserted, and isnt already present in a nonvacant
//                 node of the tree
bool DTree::checkReplacementCandidacy(DNode *vacantNode, DNode *replacement) {
    // verify that the node to insert into the vacant does not already exist,
    // else do nothing and return the vacant node
    if (!this->retrieve(replacement->getDiscriminator())) {

        // get the discriminator of the child nodes if they exist, otherwise get the max values
        int leftDisc = (vacantNode->_left) ? vacantNode->_left->getDiscriminator() : 0;
        int rightDisc = (vacantNode->_right) ? vacantNode->_right->getDiscriminator() : MAX_DISC;

        // verify that the tree's order will be maintained for this node to be inserted
        if (replacement->getDiscriminator() >= leftDisc && replacement->getDiscriminator() <= rightDisc) {
            return true;
        }
    }

    return false;
}


// preconditions: called on insertion when checkReplacementCandidacy() returns true
// postconditions: the vacantNode's spot in the tree is replaced by the replacement
DNode* DTree::replaceVacant(DNode *vacantNode, DNode *replacement) {
    // take the left and right pointers from vacant, as well as the size,
    // and put them into replacement
    replacement->_left = vacantNode->_left;
    replacement->_right = vacantNode->_right;
    replacement->_size = vacantNode->_size;

    delete vacantNode;

    // update the number of vacant nodes for this subtree
    updateNumVacant(replacement);

    // return the address of the replacement node
    return replacement;
}

/**
 * Removes the specified DNode from the tree.
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool DTree::remove(int disc, DNode*& removed) {
    // do not do anything if there is nothing in the tree and return false
    // if this is the case
    if (_root) {
        return remove(_root, disc, removed);
    }

    return false;
}

// preconditions: the shell of remove() is called and root is passed into this helper
// postconditions: the node to remove is made vacant and is put into the replaced arg
bool DTree::remove(DNode *currNode, int disc, DNode *&removed) {
    bool markedVacant = false;

    // if the node passed does not exist, nothing can be removed and false
    // should be returned.
    // all other cases work as a normal binary search algorithm
    if (!currNode) {
        return false;
    } else if (disc < currNode->getDiscriminator()) {
        markedVacant = remove(currNode->_left, disc, removed);
    } else if (disc > currNode->getDiscriminator()) {
        markedVacant = remove(currNode->_right, disc, removed);
    } else if (disc == currNode->getDiscriminator()) {
        // if we've found our node to remove, update the vacancy of said node
        // and store it's information inside of removed
        currNode->_vacant = true;

        removed = new DNode();
        *removed = *currNode;

        markedVacant = true;
    }

    // update the number of vacancies for this subtree and return the result of
    // removal, markedVacant
    updateNumVacant(currNode);

    return markedVacant;
}

/**
 * Retrieves the specified Account within a DNode.
 * @param disc discriminator int to search for
 * @return DNode with a matching discriminator, nullptr otherwise
 */
DNode* DTree::retrieve(int disc) {
    // stop execution of the function if there is no data in the tree and return false
    if (_root) {
        return _root->retrieve(disc);
    }

    return nullptr;
}

// preconditions: the outer retrieve() shell is called and root is passed into this helper
// postconditions: the node with the discriminator that matches the passed arg
//                 is returned, else nullptr
DNode* DNode::retrieve(int disc) {
    // this function acts as a normal Binary search function, checking the left
    // and right nodes for the discriminator until it is or isnt found and returning
    // the result
    if (disc < this->getDiscriminator()) {
        if (this->_left) {
            return this->_left->retrieve(disc);
        }
    } else if (disc > this->getDiscriminator()) {
        if (this->_right) {
            return this->_right->retrieve(disc);
        }
    } else if (disc == this->_account._disc) {
        // if we have found the node with the discriminator but it's vacant,
        // return a nullptr, else return the node we've found
        if (this->isVacant()) {
            return nullptr;
        }

        return this;
    }

    return nullptr;
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void DTree::clear() {
    // do not delete anything if nothing is allocated
    if (this->_root) {
        clear(this->_root);
    }

    // set root to nullptr for cleanup
    this->_root = nullptr;

    return;
}

// preconditions: the outer clear() shell is called and root is passed into this function
// postconditions: the entire tree is deallocated
void DTree::clear(DNode *currNode) {
    // perform this function on left and right nodes, given that they exist
    if (currNode->_left) { clear(currNode->_left); }
    if (currNode->_right) { clear(currNode->_right); }

    // deallocate the current node and set all to nullptr for cleanup
    currNode->_left = nullptr;
    currNode->_right = nullptr;
    delete currNode;
    currNode = nullptr;

    return;
}

/**
 * Prints all accounts' details within the DTree.
 */
void DTree::printAccounts() const {
    // if the root exists, print all account information in three, else notify
    // the user that nothing is stored in it
    if (this->_root) {
        this->_root->printAccount();
    } else {
        cout << "No account information stored." << endl;
    }

    return;
}

// preconditions: the outer shell of printAccounts() is called and root exists
// postconditions: the details each node's account is printed in order
void DNode::printAccount() const {
    // because we want printAccount to be an inorder traversal, we check the
    // left node and print that first before printing this node
    if (this->_left) {
        this->_left->printAccount();
    }

    cout << this->getAccount() << endl;

    // after printing this node and the left ones, do the same for the right nodes
    if (this->_right) {
        this->_right->printAccount();
    }

    return;
}

/**
 * Dump the DTree in the '()' notation.
 */
void DTree::dump(DNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getAccount().getDiscriminator() << ":" << node->getSize() << ":" << node->getNumVacant();
    dump(node->_right);
    cout << ")";
}

/**
 * Returns the number of valid users in the tree.
 * @return number of non-vacant nodes
 */
int DTree::getNumUsers() const {
    // return 0 if there are no accounts stored, else recurse down the tree
    if (_root) {
        return _root->getNumUsers();
    }

    return 0;
}

// preconditions: the outer shell of getNumUsers() is called (by utree in this implementation)
// postconditions: the number of nonvacant users in this subtree is returned
int DNode::getNumUsers() const {
    // given that there are left and right children, call getNumUsers on them
    // to repeat the process for their children and store the number here.
    // if there is no left or right child, just store 0.
    int leftCount = (this->_left) ? this->_left->getNumUsers() : 0;
    int rightCount = (this->_right) ? this->_right->getNumUsers() : 0;

    // rather than storing an integer if this node is vacant, we can just
    // use the boolean returned by isVacant. If the node is vacant, flip the
    // true to a false (or rather, the 1 to a 0) and return it along with the
    // leftCount and rightCount. Else, we add 0 to the leftCount and rightCount
    return !this->isVacant() + leftCount + rightCount;
}

/**
 * Updates the size of a node based on the imedaite children's sizes
 * @param node DNode object in which the size will be updated
 */
void DTree::updateSize(DNode* node) {
    // use the node helper fuction here
    node->calcNodeSize();

    return;
}

// preconditions: the outer shell of updateSize() is called on this node
// postconditions: this node's size is updated and returned
int DNode::calcNodeSize() {
    // add up all of the child nodes' sizes into this node's size.
    // if they dont exist, just use 0
    this->_size = (this->_left) ? this->_left->getSize() : 0;
    this->_size += (this->_right) ? this->_right->getSize() : 0;
    this->_size += 1;

    return this->_size;
}

/**
 * Updates the number of vacant nodes in a node's subtree based on the immediate children
 * @param node DNode object in which the number of vacant nodes in the subtree will be updated
 */
void DTree::updateNumVacant(DNode* node) {
    // traverse down the tree and update the vacant nodes in the case of a rebalance
    if (node->_left) {
        updateNumVacant(node->_left);
    }

    if (node->_right) {
        updateNumVacant(node->_right);
    }

    // similarly to calcNodeSize, get the number of vacant nodes from the children
    // and their children if they exist and add them to this node's, else use 0.
    node->_numVacant = (node->_left) ? node->_left->getNumVacant() : 0;
    node->_numVacant += (node->_right) ? node->_right->getNumVacant() : 0;
    node->_numVacant += node->isVacant();

    return;
}

/**
 * Checks for an imbalance, defined by 'Discord' rules, at the specified node.
 * @param checkImbalance DNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
bool DTree::checkImbalance(DNode* node) {
    // for the conditions of imbalance:
    // 1. child1 >= child2 + 50% of child 2
    // 2. child1 or child2 must be greater than 4
    const float PLUS_FIFTY_PERCENT = 1.5;
    const int MIN_IMBALANCE_MARKER = 4;

    // get the sizes of the left and right children as floats for multiplication
    // with PLUS_FIFTY_PERCENT, else use 0 if they dont exist
    float leftSize = (node->_left) ? node->_left->getSize() : 0;
    float rightSize = (node->_right) ? node->_right->getSize() : 0;

    // create a flag to check that at least one of the nodes is greater than four,
    // then calculate each node's size + 50%
    bool greaterThanFour = leftSize >= MIN_IMBALANCE_MARKER || rightSize >= MIN_IMBALANCE_MARKER;
    float rightPlusFifty = rightSize * PLUS_FIFTY_PERCENT;
    float leftPlusFifty = leftSize * PLUS_FIFTY_PERCENT;

    // return true if the two conditions are met and the node is imbalanced, else false
    return (leftSize >= rightPlusFifty || rightSize >= leftPlusFifty) && greaterThanFour;
}

/**
 * Begins and manages the rebalancing process for a 'Discord' tree (returns a pointer).
 * @param node DNode root of the subtree to balance
 * @return DNode root of the balanced subtree
 */
DNode* DTree::rebalance(DNode *node) {
    // verify that the current node is unbalanced. if it is, balance it. else,
    // return the node param unchanged
    if (checkImbalance(node)) {
        // calculate how many nodes will go into this array, as no vacant nodes
        // will
        int arrSize = node->getSize() - node->getNumVacant();
        DNode **arr = new DNode*[arrSize];

        // pass an index counter by reference to keep track of where each step
        // in the recursive process will need to insert a node
        int index = 0;
        treeToArr(node, arr, index);

        // set this node equal to the new subtree's root that will be taken
        // from the conversion from array to tree function
        node = arrToTree(arr, 0, arrSize - 1);

        // update all of the nodes' vacancy values, then delete the array used
        // to rebalance and return the new subtree's root
        updateNumVacant(node);

        delete [] arr;
    }

    return node;
}

// preconditions: rebalance is called and there is an imbalance
// postconditions: the tree of a root/subroot is converted into an array, not including
//                 vacant nodes
void DTree::treeToArr(DNode *node, DNode **arr, int &index) {
    // if the given node does not exist, stop exection of this recursion
    if (!node) { return; }
    DNode *nodeRight = node->_right;

    treeToArr(node->_left, arr, index);

    // if this current node is vacant, do not add it to our new tree array
    if (!node->isVacant()) {
        arr[index++] = node;
    } else {
        delete node;
    }

    treeToArr(nodeRight, arr, index);

    return;
}

// preconditions: a subtree was converted into an array
// postconditions: a new tree is formed from the array by process of recursion
DNode* DTree::arrToTree(DNode **arr, int leftIndex, int rightIndex) {
    // if the leftIndex int is greater than the right index, there are no more
    // nodes to take from the array
    if (leftIndex > rightIndex) { return nullptr; }

    // calculate the middle index of this recursive call, then make this subroot
    // the node at that index
    int middleIndex = (leftIndex + rightIndex) / 2;
    DNode *node = arr[middleIndex];

    // set the left and right children of this new root to the middle indices
    // of the next recursive call
    node->_left = arrToTree(arr, leftIndex, middleIndex - 1);
    node->_right = arrToTree(arr, middleIndex + 1, rightIndex);

    updateSize(node);

    return node;
}

/**
 * Overloaded << operator for an Account to print out the account details
 * @param sout ostream object
 * @param acct Account objec to print
 * @return ostream object containing stream of account details
 */
ostream& operator<<(ostream& sout, const Account& acct) {
    sout << "Account name: " << acct.getUsername() <<
            "\n\tDiscriminator: " << acct.getDiscriminator() <<
            "\n\tNitro: " << acct.hasNitro() <<
            "\n\tBadge: " << acct.getBadge() <<
            "\n\tStatus: " << acct.getStatus();
    return sout;
}
