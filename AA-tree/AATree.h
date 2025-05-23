#include <functional>
#include <stdexcept>
#include <algorithm>

template <typename T>
class AATree
{
public:

  class Iterator;
  class Comparator;

  AATree();
  AATree(std::initializer_list<T> list);

  ~AATree();

  AATree & operator=(AATree otherTree);

  void insert(const T & data);
  void remove(const T & data);
  void clear();

  void swap(AATree & firstTree, AATree & secondTree);

  bool contains(const T & data);
  bool isEmpty();
  size_t getSize();

  Iterator begin();
  Iterator end();

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

  Node * insert(Node * node, const T & data);
  Node * skew(Node * node);
  Node * split(Node * node);
  Node * remove(Node * node, const T & data);
  bool isLeaf(Node * node);
  void deleteSubtree(Node * node);
  Node * amendLevel(Node * node);
  size_t getSize(Node * node);
};

//Создаём класс итератор для перемещения по узлам дерева в порядке от меньшего к большему
template <typename T>
class AATree<T>::Iterator
{
public:
  friend class AATree<T>;

  Iterator();

  ~Iterator() = default;

  Iterator & operator=(const Iterator &) = default;
  Iterator & operator++();
  Iterator operator++(int);
  Iterator & operator--();
  Iterator operator--(int);

  T & operator*();
  const T & operator*() const;
  T * operator->();
  const T * operator->() const;

  bool operator==(const Iterator & otherIterator);
  bool operator!=(const Iterator & otherIterator);

private:
  Node * node;
  Node * treeRoot;

  Iterator(Node * node, Node * treeRoot);
  Node * getParent(Node * treeRoot, Node * child);
};

//Конструктор итератора без параметров
template <typename T>
AATree<T>::Iterator::Iterator()
{
  this->node = nullptr;
  this->treeRoot = nullptr;
}

//Консруктор итератора с параметрами
template <typename T>
AATree<T>::Iterator::Iterator(Node * node, Node * treeRoot)
{
  this->node = node;
  this->treeRoot = treeRoot;
}

//Префиксный инкремент для итератора
template <typename T>
typename AATree<T>::Iterator & AATree<T>::Iterator::operator++()
{
  if (node == nullptr)
  {
    throw std::logic_error("Error: Unable to use increment with iterator.\n");
  }

  //1 случай: Если есть правое поддерево
  if (node->right != nullptr)
  {
    //Спускаемся к минимальному узлу в правом поддереве (т.е. к следующему по возрастанию значению)
    node = node->right;
    while (node->left != nullptr)
    {
      node = node->left;
    }
  }
  //2 случай: Если нет правого поддерева
  else
  {
    //Поднимаемся до первого родителя, для которого текущий узел находится в левом поддереве
    Node * parent = getParent(treeRoot, node);
    while (parent != nullptr && node == parent->right)
    {
      node = parent;
      parent = getParent(treeRoot, parent);
    }
    node = parent;
  }

  return *this;
}

//Постфиксный инкремент для итератора
template <typename T>
typename AATree<T>::Iterator AATree<T>::Iterator::operator++(int)
{
  Iterator currIterator = *this;
  ++(*this);

  return currIterator;
}

//Префиксный декремент для итератора
template <typename T>
typename AATree<T>::Iterator & AATree<T>::Iterator::operator--()
{
  //1 случай: Если итератор на end()
  if (node == nullptr)
  {
    //Спускаемся к максимальному узлу
    node = treeRoot;
    if (node == nullptr)
    {
      throw std::logic_error("Error: Unable to use decrement with iterator.\n");
    }
    while (node->right != nullptr)
    {
      node = node->right;
    }
  }
  //2 случай: Если есть левое поддерево
  else if (node->left != nullptr)
  {
    //Спускаемся к минимальному узлу в левом поддереве (т.е. к следующему по убыванию значению)
    node = node->left;
    while (node->right != nullptr)
    {
      node = node->right;
    }
  }
  //3 случай: Если нет левого поддерева
  else
  {
    //Поднимаемся до первого родителя, для которого текущий узел находится в правом поддереве
    Node * parent = getParent(treeRoot, node);
    while (parent != nullptr && node == parent->left)
    {
      node = parent;
      parent = getParent(treeRoot, parent);
    }
    node = parent;
  }

  return *this;
}

//Постфиксный декремент для итератора
template <typename T>
typename AATree<T>::Iterator AATree<T>::Iterator::operator--(int)
{
  Iterator currIterator = *this;
  --(*this);

  return currIterator;
}

//Получение ссылки на данные в узле
//Неконстантная версия позволяет изменять данные в узле
template <typename T>
T & AATree<T>::Iterator::operator*()
{
  if (node == nullptr)
  {
    throw std::logic_error("Error: Dereferencing end iterator.");
  }

  return node->data;
}

//Получение константной ссылки на данные в узле
//Константная версия не позволяет изменять данные в узле
template <typename T>
const T & AATree<T>::Iterator::operator*() const
{
  if (node == nullptr)
  {
    throw std::logic_error("Error: Dereferencing end iterator.");
  }

  return node->data;
}

//Получение указателя на данные в узле
template <typename T>
T * AATree<T>::Iterator::operator->()
{
  if (node == nullptr)
  {
    throw std::logic_error("Error: Dereferencing end iterator.");
  }
  return &(node->data);
}

//Получение константного указателя на данные в узле
template <typename T>
const T * AATree<T>::Iterator::operator->() const
{
  if (node == nullptr)
  {
    throw std::logic_error("Error: Dereferencing end iterator.");
  }
  return &(node->data);
}

//Оператор == для итератора
template <typename T>
bool AATree<T>::Iterator::operator==(const Iterator & otherIterator)
{
  bool isEqual;

  if (node == otherIterator.node)
  {
    isEqual = true;
  }
  else
  {
    isEqual = false;
  }

  return isEqual;
}

//Оператор != для итератора
template <typename T>
bool AATree<T>::Iterator::operator!=(const Iterator & otherIterator)
{
  bool isUnequal;

  if (!(*this == otherIterator))
  {
    isUnequal = true;
  }
  else
  {
    isUnequal = false;
  }

  return isUnequal;
}

//Внутренний метод для нахождения родителя для данного узла
template <typename T>
typename AATree<T>::Node * AATree<T>::Iterator::getParent(Node * ancestor, Node * child)
{
  if (ancestor == nullptr || child == nullptr || ancestor == child)
  {
    return nullptr;
  }

  if (ancestor->left == child || ancestor->right == child)
  {
    return ancestor;
  }

  if (AATree<T>::compare(child->data, ancestor->data))
  {
    return getParent(ancestor->left, child);
  }
  else
  {
    return getParent(ancestor->right, child);
  }
}

//Создаём класс, который наследуется от std::less и сравнивает данные типа Т  
//std::less поддерживает operator()
template <typename T>
class AATree<T>::Comparator : public std::less<T> {};

//Конструктор без параметров
template <typename T>
AATree<T>::AATree()
{
  this->root = nullptr;
}

//Конструктор со списком инициализации
template <typename T>
AATree<T>::AATree(std::initializer_list<T> list)
{
  for (auto data: list)
  {
    insert(data);
  }
}

//Деструктор
template <typename T>
AATree<T>::~AATree()
{
  clear();
}

//Оператор = для дерева
template<typename T>
AATree<T> & AATree<T>::operator=(AATree otherTree)
{
  swap(*this, otherTree);

  return *this;
}

//Пользовательский метод, который вызывает внутренний метод
template <typename T>
void AATree<T>::insert(const T & data)
{
  root = insert(root, data);
}

template <typename T>
typename AATree<T>::Node * AATree<T>::insert(Node * node, const T & data)
{
  //1 случай: Если узел null, то создаём новый узел с новым значением
  if (node == nullptr)
  {
    node = new Node(data);
    return node;
  }
  //2 случай: Если новое значение меньше, идём в левое поддерево
  else if (compare(data, node->data))
  {
    node->left = insert(node->left, data);
  }
  //3 случай: Если новое значение больше, идём в правое поддерево
  else if (compare(node->data, data))
  {
    node->right = insert(node->right, data);
  }
  else
  {
    throw std::logic_error("Error: Element already exists.\n");
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
bool AATree<T>::contains(const T & data)
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
void AATree<T>::remove(const T & data)
{
  //Вызываем внутренний метод для удаления узла с заданным значением
  root = remove(root, data);
}

//Внутренний метод для удаления узла с заданным значением
template <typename T>
typename AATree<T>::Node * AATree<T>::remove(Node * node, const T & data)
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
      //1 случай: Если узел является листом
      if (isLeaf(node))
      {
        //Удаляем узел
        delete node;
        return nullptr;
      }
      //2 случай: Если у узла нет левого поддерева
      else if (node->left == nullptr)
      {
        //Преемник - минимальный узел в правом поддереве
        Node * successor = node->right;
        while (successor->left != nullptr)
        {
          successor = successor->left;
        }
        node->data = successor->data;
        node->right = remove(node->right, successor->data);
      }
      //3 случай: Если у узла есть левое поддерево
      else
      {
        //Предшественник - максимальный узел в левом поддереве
        Node * predecessor = node->left;
        while (predecessor->right != nullptr)
        {
          predecessor = predecessor->right;
        }
        node->data = predecessor->data;
        node->left = remove(node->left, predecessor->data);
      }
    }

    //Исправляем уровень узла
    node = amendLevel(node);
    //Балансируем дерево
    node = skew(node);
    if (node->right != nullptr)
    {
      node->right = skew(node->right);
      if (node->right->right != nullptr)
      {
        node->right->right = skew(node->right->right);
      }
    }
    node = split(node);
    if (node->right != nullptr)
    {
      node->right = split(node->right);
    }
  }
  
  return node;
}

//Проверка, является ли узел листом дерева
template <typename T>
bool AATree<T>::isLeaf(Node * node)
{
  bool isLeaf;

  if (node != nullptr && node->right == nullptr && node->left == nullptr)
  {
    isLeaf = true;
  }
  else
  {
    isLeaf = false;
  }

  return isLeaf;
}

//Исправление значения уровня узла
template <typename T>
typename AATree<T>::Node * AATree<T>::amendLevel(Node * node)
{
  int correctLevel = 1;

  if (node != nullptr && node->left != nullptr && node->right != nullptr)
  {
    correctLevel = std::min(node->left->level, node->right->level) + 1;
  }
  else if (node != nullptr && (node->left != nullptr || node->right != nullptr))
  {
    correctLevel = (node->left != nullptr) ? node->left->level : node->right->level;
  }

  if (correctLevel < node->level)
  {
    node->level = correctLevel;
    if (node->right != nullptr && correctLevel < node->right->level)
    {
      node->right->level = correctLevel;
    }
  }

  return node;
}

//Удаление целого дерева
template <typename T>
void AATree<T>::clear()
{
  //Вызов внутренней рекурсивной функции, удаляющей поддеревья
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

//Обмен данных деревьев
template <typename T>
void AATree<T>::swap(AATree & firstTree, AATree & secondTree)
{
  std::swap(firstTree.root, secondTree.root);
}

//Проверка, является ли дерево пустым
template <typename T>
bool AATree<T>::isEmpty()
{
  bool treeIsEmpty;

  if (root == nullptr)
  {
    treeIsEmpty = true;
  }
  else
  {
    treeIsEmpty = false;
  }

  return treeIsEmpty;
}

//Получение размера дерева (т.е. количества узлов)
template <typename T>
size_t AATree<T>::getSize()
{
  //Вызов внутреннего рекурсивного метода
  size_t size = getSize(root);

  return size;
}

//Внутренний рекурсивный метод для нахождения количества узлов
template <typename T>
size_t AATree<T>::getSize(Node * node)
{
  size_t size = 0;

  if (node != nullptr)
  {
    size = getSize(node->left) + getSize(node->right) + 1;
  }

  return size;
}

//Получение итератора, указывающего на первый (наименьший) элемент в дереве
template<typename T>
AATree<T>::Iterator AATree<T>::begin()
{
  if (isEmpty())
  {
    return end();
  }

  Node * minNode = root;
  while (minNode->left != nullptr)
  {
    minNode = minNode->left;
  }

  return Iterator(minNode, root);
}

//Получение итератора, указывающего на последний (несуществующий) элемент в дереве
template <typename T>
AATree<T>::Iterator AATree<T>::end()
{
  return Iterator(nullptr, root);
}
