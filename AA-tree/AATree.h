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

//������ �����, ������� ����������� �� std::less � ���������� ������ ���� �  
template <typename T>
class AATree<T>::Comparator : public std::less<T>
{
  //std::less ������������ operator()
};

//����������� ��� ����������
template <typename T>
AATree<T>::AATree()
{
  this->root = nullptr;
}

//����������
template <typename T>
AATree<T>::~AATree()
{
  clear();
}

//���������������� �����, ������� �������� ���������� �����
template <typename T>
void AATree<T>::insert(T & data)
{
  root = insert(root, data);
}

template <typename T>
typename AATree<T>::Node * AATree<T>::insert(Node * node, T & data)
{
  //������ ����������
  Comparator compare;

  //���� ���� null, �� ������ ����� ���� � ����� ���������
  if (node == nullptr)
  {
    node = new Node(data);
    return node;
  }
  //���� ����� �������� ������, ��� � ����� ���������
  else if (compare(data, node->data))
  {
    node->left = insert(node->left, data)
  }
  //���� ����� �������� ������, ��� � ������ ���������
  else if (compare(node->data, data))
  {
    node->right = insert(node->right, data)
  }

  //����������� ������
  node = skew(node);
  node = split(node);

  return node;
}

//��������� ����� �������������� �����, �������� ������ �������
template <typename T>
typename AATree<T>::Node * AATree<T>::skew(Node * node)
{
  if (node != nullptr && node->left != nullptr)
    //���������, ������ �� ������ ������� ���� � ��� ����� ���
    if (node->left->level == node->level)
    {
      Node * leftChild = node->left;
      node->left = leftChild->right;
      leftChild->right = node;
      node = leftChild;
    }

  return node;
}

//��������� ��� ���������������� ������ �������������� �����, �������� ����� �������
template <typename T>
typename AATree<T>::Node * AATree<T>::split(Node * node)
{
  if (node != nullptr && node->right != nullptr && node->right->right != nullptr)
    //���������, ������ �� ������ ������� ���� � ��� ������ ����
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
