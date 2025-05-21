﻿#include <functional>

template <typename T>
class AATree
{
public:

  class Comparator;

  AATree();
  ~AATree();

  void insert(T & data);
  bool contains(T & data);
  Node * remove(T & data);
  void clear();

private:

  struct Node
  {
    T data;
    int level;
    Node * left;
    Node * right;

    Node(T data)
    {
      this->data = data;
      level = 1;
      left = nullptr;
      right = nullptr;
    }
  };

  Node * root;
  Comparator compare;

  Node * insert(Node * node, T & data);
  Node * skew(Node * node);
  Node * split(Node * node);
  Node * remove(Node * node, T & data);
  bool isLeaf(Node * node);
  void deleteSubtree(Node * node);
};

//Создаём класс, который наследуется от std::less и сравнивает данные типа Т  
template <typename T>
class AATree<T>::Comparator : public std::less<T>
{
  //std::less поддерживает operator()
};

//Конструктор без параметров
template <typename T>
AATree<T>::AATree()
{
  this->root = nullptr;
}

//Деструктор
template <typename T>
AATree<T>::~AATree()
{
  clear();
}

//Пользовательский метод, который вызывает внутренний метод
template <typename T>
void AATree<T>::insert(T & data)
{
  root = insert(root, data);
}

template <typename T>
typename AATree<T>::Node * AATree<T>::insert(Node * node, T & data)
{
  //Если узел null, то создаём новый узел с новым значением
  if (node == nullptr)
  {
    node = new Node(data);
    return node;
  }
  //Если новое значение меньше, идём в левое поддерево
  else if (compare(data, node->data))
  {
    node->left = insert(node->left, data)
  }
  //Если новое значение больше, идём в правое поддерево
  else if (compare(node->data, data))
  {
    node->right = insert(node->right, data)
  }

  //Балансируем дерево
  node = skew(node);
  node = split(node);

  return node;
}

//Устраняем левое горизонтальное ребро, совершая правый поворот
template <typename T>
typename AATree<T>::Node * AATree<T>::skew(Node * node)
{
  if (node != nullptr && node->left != nullptr)
    //Проверяем, одного ли уровня текущий узел и его левый сын
    if (node->left->level == node->level)
    {
      Node * leftChild = node->left;
      node->left = leftChild->right;
      leftChild->right = node;
      node = leftChild;
    }

  return node;
}

//Устраняем два последовательных правых горизонтальных ребра, совершая левый поворот
template <typename T>
typename AATree<T>::Node * AATree<T>::split(Node * node)
{
  if (node != nullptr && node->right != nullptr && node->right->right != nullptr)
    //Проверяем, одного ли уровня текущий узел и его правый внук
    if (node->level == node->right->right->level)
    {
      Node * rightChild = node->right;
      node->right = rightChild->left;
      rightChild->left = node;
      rightChild->level += 1;
      node = rightChild;
    }

  return node;
}

//Проверка, есть ли узел с заданным значением в дереве
template <typename T>
bool AATree<T>::contains(T & data)
{
  Node * currNode = root;

  bool isFound = false;

  //Пока не нашли нужного значения, идем по узлам дерева вниз
  while (currNode != nullptr && !isFound)
  {
    if (compare(data, currNode->data))
    {
      currNode = currNode->left;
    }
    else if (compare(currNode->data, data))
    {
      currNode = currNode->right;
    }
    else
    {
      isFound = true;
    }
  }

  return isFound;
}

//Пользовательский метод для удаления узла с заданным значением
template <typename T>
typename AATree<T>::Node * AATree<T>::remove(T & data)
{
  //Возвращаем внутренний метод для удаления узла с заданным значением
  return remove(root, data);
}

//Внутренний метод для удаления узла с заданным значением
template <typename T>
typename AATree<T>::Node * AATree<T>::remove(Node * node, T & data)
{
  if (node != nullptr)
  { //Пока не нашлось нужное значение, идем по узлам вниз
    if (compare(data, node->data))
    {
      node->left = remove(node->left, data);
    }
    else if (compare(node->data, data))
    {
      node->right = remove(node->right, data);
    }
    //Если нашелся узел с нужным значением
    else
    { 
      Node * tempNode = nullptr;
      
      //Если узел является листом
      if (isLeaf(node))
      {
        //Удаляем узел
        delete node;
        return nullptr;
      }
      //Если у узла нет левого сына
      else if (node->left == nullptr)
      {
        tempNode = node->right;
        //Узел принимает все данные своего правого сына, становится им
        *node = *tempNode;
        //Удаляем копию сына
        delete tempNode;
      }
      //Если у узла нет правого сына
      else if (node->right == nullptr)
      {
        tempNode = node->left;
        //Узел принимает все данные своего левого сына, становится им
        *node = *tempNode;
        //Удаляем копию сына
        delete tempNode;
      }
      //Если у узла есть и правый, и левый сын
      else
      {
        //Находим левого крайнего потомка правого сына узла
        tempNode = node->right;
        while (tempNode->left != nullptr)
        {
          tempNode = tempNode->left;
        }
        //Узел принимает все данные найденного узла
        node->data = tempNode->data;
        //Удаляем копию найденного узла
        node->right = remove(node->right, tempNode->data);
      }
    }
  }
  //Балансируем дерево
  node = skew(node);
  node = split(node);
  
  return node;
}

//Проверка, является ли узел листом дерева
template <typename T>
bool AATree<T>::isLeaf(Node * node)
{
  bool isLeaf;

  if (node->level == 1)
  {
    isLeaf = true;
  }
  else
  {
    isLeaf = false;
  }

  return isLeaf;
}

//Удаление целого дерева
template <typename T>
void AATree<T>::clear()
{
  //Вызов рекурсивной функции, удаляющей поддеревья
  deleteSubtree(root);
}

//Удаление поддерева
template <typename T>
void AATree<T>::deleteSubtree(Node * node)
{
  if (node != nullptr)
  {
    //Удаление поддерева слева
    deleteSubtree(node->left);
    //Удаление поддерева справа
    deleteSubtree(node->right);

    //Удаление узла
    delete node;

    node = nullptr;
  }
}
