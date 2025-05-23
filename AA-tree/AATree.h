#include <functional>
#include <stdexcept>

template <typename T>
class AATree
{
public:

  class Comparator;

  AATree();
  AATree(std::initializer_list<T> list);

  ~AATree();

  AATree & operator=(AATree otherTree);

  void insert(T & data);
  Node * remove(T & data);
  void clear();

  void swap(AATree & firstTree, AATree & secondTree);

  bool contains(T & data);
  bool isEmpty();
  size_t getSize();

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
  Node * amendLevel(Node * node);
  size_t getSize(Node * node);
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

template<typename T>
AATree<T> & AATree<T>::operator=(AATree otherTree)
{
  swap(*this, other);

  return *this;
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
    node->left = insert(node->left, data);
  }
  //Если новое значение больше, идём в правое поддерево
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
      //Если узел является листом
      if (isLeaf(node))
      {
        //Удаляем узел
        delete node;
        return nullptr;
      }
      //Если у узла нет левого поддерева
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
      //Если у узла есть левое поддерево
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
    correctLevel = min(node->left->level, node->right->level) + 1;
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
  std::swap(firstTree.root, secondTree.root)
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
