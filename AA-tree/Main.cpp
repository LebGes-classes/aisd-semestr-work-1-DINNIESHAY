#include <iostream>
#include "AATree.h"

int main() {
  try {
    //Создание дерева и вставка элементов
    AATree<int> tree;
    std::cout << "Creating tree and adding elements\n";
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    std::cout << "Tree size: " << tree.getSize() << " (expected: 7)\n";
    std::cout << "Tree is empty: " << (tree.isEmpty() ? "true" : "false") << " (expected: false)\n";

    //Проверка наличия элементов
    std::cout << "\nContains\n";
    std::cout << "Contains 5: " << (tree.contains(5) ? "true" : "false") << " (expected: true)\n";
    std::cout << "Contains 9: " << (tree.contains(9) ? "true" : "false") << " (expected: false)\n";

    //Обход дерева с помощью итератора в порядке возврастания элементов
    std::cout << "\nIterator traversal (in-order)\n";
    std::cout << "Elements: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
      std::cout << *it << " ";
    }
    std::cout << "(expected: 2 3 4 5 6 7 8)\n";

    //Удаление элементов
    std::cout << "\nRemoval\n";
    tree.remove(3);
    tree.remove(7);
    std::cout << "Tree size after removal: " << tree.getSize() << " (expected: 5)\n";
    std::cout << "Contains 3: " << (tree.contains(3) ? "true" : "false") << " (expected: false)\n";
    std::cout << "Contains 7: " << (tree.contains(7) ? "true" : "false") << " (expected: false)\n";

    //Обход после удаления
    std::cout << "\nIterator after removal\n";
    std::cout << "Elements: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
      std::cout << *it << " ";
    }
    std::cout << "(expected: 2 4 5 6 8)\n";

    //Очистка дерева
    std::cout << "\nClear\n";
    tree.clear();
    std::cout << "Tree size after clear: " << tree.getSize() << " (expected: 0)\n";
    std::cout << "Tree is empty: " << (tree.isEmpty() ? "true" : "false") << " (expected: true)\n";

    //Инициализация списком
    std::cout << "\nInitializer list\n";
    AATree<int> treeTwo = { 10, 30, 20, 15, 25 };
    std::cout << "Elements: ";
    for (auto it = treeTwo.begin(); it != treeTwo.end(); ++it) {
      std::cout << *it << " ";
    }
    std::cout << "(expected: 10 15 20 25 30)\n";

    //Проверка работы оператора присваивания
    std::cout << "\nAssignment operator\n";
    AATree<int> treeThree;
    treeThree = treeTwo;
    std::cout << "Elements in tree 3: ";
    for (auto it = treeThree.begin(); it != treeThree.end(); ++it) {
      std::cout << *it << " ";
    }
    std::cout << "(expected: 10 15 20 25 30)\n";

    //Обратный итератор (через декремент)
    std::cout << "\nReverse traversal\n";
    std::cout << "Elements in reverse: ";
    auto it = treeThree.end();
    --it;
    while (it != treeThree.begin()) {
      std::cout << *it << " ";
      --it;
    }
    std::cout << *it << " ";
    std::cout << "(expected: 30 25 20 15 10)\n";

    //Попытка удаления несуществующего элемента
    std::cout << "\nRemove non-existent element\n";
    try {
      treeThree.remove(100);
      std::cout << "No exception thrown (expected: no exception)\n";
    }
    catch (const std::exception & e) {
      std::cout << "Exception: " << e.what() << "\n";
    }

  }
  catch (const std::exception & e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}