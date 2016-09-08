#ifndef LIST_HPP	
#define LIST_HPP

#include <memory>

template <typename T>
struct Node
{
	Node(T d, std::shared_ptr<Node<T>> n, std::shared_ptr<Node<T>> p) 
    :data(d), next(n), prev(p) {}
	T data;
	std::shared_ptr<Node<T>> next;
  std::shared_ptr<Node<T>> prev;
};

template <typename T>
class LinkedList
{
public:
	LinkedList() :head(nullptr), tail(nullptr), size(0) {}
	LinkedList(LinkedList const & list) { head = clone(list.head); } 
	LinkedList(LinkedList const && list) { head = list.head; }
	int  size() { return size; }
	void push_back(T t) { push_back(t, head); }
  void pop_front();
	void operator=(LinkedList const &); // COPY 
	void operator=(LinkedList const &&); // MOVE
  T& operator[](size_t const &);
  const T& operator[](size_t const &) const;
private:
	void push_back(T, std::shared_ptr<Node<T>>&);
  void pop_front(std::shared_ptr<Node<T>>&);
  T& get(size_t const & i) { return get(i, head); }
  T& get(size_t const &, std::shared_ptr<Node<T>> &);
	std::shared_ptr<Node<T>> clone(std::shared_ptr<Node<T>>);
	std::shared_ptr<Node<T>> head;
  std::shared_ptr<Node<T>> tail;
  int size;
};

template <typename T>
void LinkedList<T>::pop_front()
{
  if (!head) return;
  head = head->next;
  --size;
}

template <typename T>
void LinkedList<T>::push_back(T data, std::shared_ptr<Node<T>>& list)
{
  if (!list) 
  {
    list = std::make_shared<Node<T>>(data, list, tail);
    tail = list;
    ++size;
  }
  else
  {
    auto newNode = std::make_shared<Node<T>>(data, nullptr, tail);
    tail->next = newNode;
    tail = newNode;
    ++size;
  }
}

template <typename T>
std::shared_ptr<Node<T>> LinkedList<T>::clone(std::shared_ptr<Node<T>> list)
{
	if (!list) return nullptr;
	return std::make_shared<Node<T>>(list->data, clone(list->next));
}

template <typename T>
void LinkedList<T>::operator=(LinkedList const & list) // Deep copy
{
	head = clone(list.head);
}

template <typename T>
void LinkedList<T>::operator=(LinkedList const && list) // Shallow copy
{
	head = list.head;
}

template <typename T>
T& LinkedList<T>::get(const size_t & i, std::shared_ptr<Node<T>> & list)
{
  if (!list) return;
  if( i == 0 ) return list->data;
  else return get(i - 1, list->next);
}

template <typename T>
T& LinkedList<T>::operator[](const size_t & i)
{
  return this->get(i);
}
  
template<typename T>
const T& LinkedList<T>::operator[](const size_t & i) const
{
  return this->get(i);
}

#endif
