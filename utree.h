/***************************
* File:     utree.h
* Project:  Project 2
* Author:   Drew Barlow
* Date:     3/13/2021
* Section:  4
* E-mail:   abarlow1@umbc.edu
*
* Header definition of UTree class.
***************************/
#pragma once

#include "dtree.h"
#include <fstream>
#include <sstream>

#define DEFAULT_HEIGHT 0

class Grader;   /* For grading purposes */
class Tester;   /* Forward declaration for testing class */

class UNode {
    friend class Grader;
    friend class Tester;
    friend class UTree;
public:
    UNode() {
        _dtree = new DTree();
        _height = DEFAULT_HEIGHT;
        _left = nullptr;
        _right = nullptr;
    }

    ~UNode() {
        delete _dtree;
        _dtree = nullptr;
    }

    /* Getters */
    DTree*& getDTree() {return _dtree;}
    int getHeight() const {return _height;}
    string getUsername() const {return _dtree->getUsername();}

private:
    DTree* _dtree;
    int _height;
    UNode* _left;
    UNode* _right;

    /* IMPLEMENT (optional): Additional helper functions */
    bool insert(Account acc);
    bool remove(int disc, DNode *&removed);
    DNode* findDisc(int disc);
    int getNumUsers();
    void calcNodeHeight();
    void traverseAccounts();
    bool isEmpty();
};

class UTree {
    friend class Grader;
    friend class Tester;

public:
    UTree():_root(nullptr){}

    /* IMPLEMENT: destructor */
    ~UTree();

    /* IMPLEMENT: Basic operations */
    void loadData(string infile, bool append = true);
    bool insert(Account newAcct);
    bool removeUser(string username, int disc, DNode*& removed);
    UNode* retrieve(string username);
    DNode* retrieveUser(string username, int disc);
    int numUsers(string username);
    void clear();
    void printUsers() const;
    void dump() const {dump(_root);}
    void dump(UNode* node) const;


    /* IMPLEMENT: "Helper" functions */
    void updateHeight(UNode* node);
    int checkImbalance(UNode* node);
    UNode* rebalance(UNode* node);

private:
    UNode* _root;

    /* IMPLEMENT (optional): any additional helper functions here! */
    bool insert(UNode *currNode, Account insertMe);
    bool remove(UNode *currNode, string username, int disc, DNode *&removed);
    UNode* retrieve(UNode *currNode, string username);
    DNode* retrieveUser(UNode *currNode, string username, int disc);
    int numUsers(UNode *currNode, string username);
    void clear(UNode *currNode);
    UNode* rotateLeft(UNode *oldRoot);
    UNode* rotateRight(UNode *oldRoot);
    UNode* findReplacement(UNode *empty);
    UNode* replacementTraversal(UNode *candidate);
};
