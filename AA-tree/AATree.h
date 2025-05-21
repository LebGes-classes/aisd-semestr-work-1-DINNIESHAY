#include <functional>

template <typename T>
class AATree
{
public:

  class Comparator;

  AATree();
  ~AATree();

  void insert(T & data);
  bool contains(T & data);
  vid remove(T & data);
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

  Node * insert(Node * node, T & data);
  Node * skew(Node * node);
  Node * split(Node * node);
  Node * remove(Node * node, T & data);
  bool contains(Node * node, T & data);
  void clear(Node * node);
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
  //Создаём компаратор
  Comparator compare;

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
    if (node.level == node.right.right.level)
    {
      Node * rightChild = node->right;
      node->right = rightChild->left;
      rightChild->left = node;
      rightChild->level += 1;
      node = rightChild;
    }

  return node;
}
