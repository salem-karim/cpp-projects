#include <climits>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

typedef struct TreeNode {
  int value;
  TreeNode *left;
  TreeNode *right;
} TreeNode;

// initialize functions
bool searchValue(TreeNode *Tree, int value, string &path);
bool searchSubTree(TreeNode *Tree, TreeNode *Subtree);

TreeNode *importTree(string filename, TreeNode *Tree) {
  // char * to string works
  string line;
  ifstream file(filename);
  if (file.is_open()) {
    while (getline(file, line)) {
      if (line.empty())
        continue;
      int num = stoi(line);
      if (Tree == nullptr) {
        Tree = new TreeNode;
        Tree->value = num;
        Tree->left = nullptr;
        Tree->right = nullptr;
      } else {
        TreeNode *current = Tree;
        TreeNode *parent = nullptr;
        while (current != nullptr) {
          parent = current;
          if (num < current->value) {
            current = current->left;
          } else {
            current = current->right;
          }
        }
        if (num < parent->value) {
          parent->left = new TreeNode;
          parent->left->value = num;
          parent->left->left = nullptr;
          parent->left->right = nullptr;
        } else {
          parent->right = new TreeNode;
          parent->right->value = num;
          parent->right->left = nullptr;
          parent->right->right = nullptr;
        }
      }
    }
    file.close();
  } else {
    cout << "File not found" << endl;
  }
  return Tree;
}

int traverseAndCheckBalance(TreeNode *Tree, int &maxVal, int &minVal,
                            double &sum, int &count, bool &isAVL) {
  if (Tree == nullptr)
    return 0;
  // calculate the height of the right and left subtree
  // then calculate the stats and print the whole tree
  int rightHeight = 0;
  int leftHeight = 0;
  if (Tree->right != nullptr) {
    rightHeight =
        traverseAndCheckBalance(Tree->right, maxVal, minVal, sum, count, isAVL);
  }
  if (Tree->left != nullptr) {
    leftHeight =
        traverseAndCheckBalance(Tree->left, maxVal, minVal, sum, count, isAVL);
  }

  // Calculate balance factor
  int balanceFactor = rightHeight - leftHeight;

  // Update max, min, sum, and count
  if (Tree->value > maxVal)
    maxVal = Tree->value;
  if (Tree->value < minVal)
    minVal = Tree->value;
  sum += Tree->value;
  count++;

  // Check if the balance factor violates AVL conditions
  if (balanceFactor > 1 || balanceFactor < -1) {
    cout << "bal(" << Tree->value << ") = " << balanceFactor
         << " (AVL violation)!" << endl;
    isAVL = false;
  } else {
    cout << "bal(" << Tree->value << ") = " << balanceFactor << endl;
  }

  // Return the height of the subtree rooted at the current node
  return max(leftHeight, rightHeight) + 1;
}

void deleteTree(TreeNode *Tree) {
  if (Tree != nullptr) {
    deleteTree(Tree->left);
    deleteTree(Tree->right);
    delete Tree;
  }
}

int main(int argc, char *argv[]) {
  TreeNode *Tree = nullptr;
  int min = INT_MAX;
  int max = INT_MIN;
  string path;
  double sum = 0;
  int count = 0;
  bool isAVL = true;
  if (argc == 3) {
    TreeNode *SubTree = nullptr;
    Tree = importTree(argv[1], Tree);
    SubTree = importTree(argv[2], SubTree);
    // Wenn die Subtree Datei nur eine Zahl hat sollen verschiedene Funktionen
    // aufgerufen werden
    if (SubTree == nullptr || Tree == nullptr) {
      return 0;
    } else if (SubTree->left == nullptr && SubTree->right == nullptr) {
      // run if only searching for a single value
      cout << (searchValue(Tree, SubTree->value, path)
                   ? to_string(SubTree->value) + " found " + path
                   : to_string(SubTree->value) + " not found!");
    } else {
      // searching for a subtree
      cout << (searchSubTree(Tree, SubTree) ? "Subtree found"
                                            : "Subtree not found!");
    }
  } else if (argc == 2) {
    Tree = importTree(argv[1], Tree);
    traverseAndCheckBalance(Tree, max, min, sum, count, isAVL);
    if (isAVL)
      cout << "AVL: yes" << endl;
    else
      cout << "AVL: no" << endl;
    cout << "min: " << min << ',';
    cout << " max: " << max << ',';
    if (count != 0) {
      cout << " avg: " << sum / count << endl;
    } else {
      cout << " avg: 0" << endl;
    }
  } else {
    cout << "No Arguments" << endl;
  }
  deleteTree(Tree);
  return 0;
}

bool searchSubTree(TreeNode *Tree, TreeNode *Subtree) {
  if (Tree->value == Subtree->value) {
    // this if statement checks if the conditions to find the
    // rest of the subtree are met.
    // for example:
    // the subtree has another left child but the tree does not:
    // return false, subtree cant be possible
    if ((Subtree->right != nullptr && Subtree->left != nullptr &&
         (Tree->right == nullptr || Tree->left == nullptr)) ||
        (Subtree->right != nullptr && Tree->right == nullptr) ||
        (Subtree->left != nullptr && Tree->left == nullptr)) {
      return false;
    }
    // now begin the search again from this point on
    if (Subtree->right != nullptr && Subtree->left != nullptr &&
        Tree->right != nullptr && Tree->left != nullptr) {
      if (!searchSubTree(Tree->right, Subtree->right)) {
        return false;
      }
      if (!searchSubTree(Tree->left, Subtree->left)) {
        return false;
      }
    } else if (Subtree->right != nullptr && Tree->right != nullptr &&
               Subtree->value > Tree->value) {
      if (!searchSubTree(Tree->right, Subtree->right)) {
        return false;
      }
    } else if (Subtree->left != nullptr && Tree->left != nullptr &&
               Subtree->value < Tree->value) {
      if (!searchSubTree(Tree->left, Subtree->left)) {
        return false;
      }
    }
    return true;
  } else {
    // if the value is wrong continue searching.
    if (Tree->right != nullptr && Subtree->value > Tree->value) {
      if (searchSubTree(Tree->right, Subtree)) {
        return true;
      }
    }
    if (Tree->left != nullptr && Subtree->value < Tree->value) {
      if (searchSubTree(Tree->left, Subtree)) {
        return true;
      }
    }
    return false;
  }
}

bool searchValue(TreeNode *Tree, int value, string &path) {
  // creating temppath to reset the path in case its the wrong one
  string temppath = path;
  if (Tree->value == value) {
    // if value is found add it to the path and return to collapse
    path += to_string(Tree->value);
    return true;
  } else {
    // if the value is not at the current position, look for children to search
    // through.
    if (Tree->right != nullptr && value > Tree->value) {
      path += to_string(Tree->value) + ", ";
      if (searchValue(Tree->right, value, path)) {
        return true;
      }
      path = temppath;
    }
    if (Tree->left != nullptr && value < Tree->value) {
      path += to_string(Tree->value) + ", ";
      if (searchValue(Tree->left, value, path)) {
        return true;
      }
      path = temppath;
    }
    return false;
  }
}
