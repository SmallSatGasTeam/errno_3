#ifndef LIST_HPP	
#define LIST_HPP

struct Node
{
	Node(float d, Node* n) 
    :data(d), next(n) {}
	float data;
	Node* next;
};

class LinkedList
{
public:
	LinkedList() :head(nullptr), listSize(0) {}
	LinkedList(LinkedList const & list) { head = clone(list.head); } 
	LinkedList(LinkedList const && list) { head = list.head; }
	int  size() { return listSize; }
	void push_back(float t) { push_back(t, head); }
  void pop_front();
  void clear() { clear(head); }
	void operator=(LinkedList const &); // COPY 
	void operator=(LinkedList const &&); // MOVE
  float& operator[](const int);
private:
	void push_back(float, Node* &); 
  void clear(Node* &);
  float& get(int const i) { return get(i, head); }
  float& get(int const, Node* &);
	Node* clone(Node*);
	Node* head;
  Node* tail;
  int listSize;
  float dummy = 0;
};

void LinkedList::clear(Node* & list)
{
  if (!list) return;
  auto next = list->next;
  delete list;
  clear(next);
}

void LinkedList::pop_front()
{
  if (!head) return;
  auto temp = head;
  head = head->next;
  delete temp;
  --listSize;
}

void LinkedList::push_back(float data, Node* & list)
{
  if (!list) 
  {
    list = new Node(data, list);
    tail = list;
    ++listSize;
  }
  else
  {
    auto newNode = new Node(data, nullptr);
    tail->next = newNode;
    tail = newNode;
    ++listSize;
  }
}

Node* LinkedList::clone(Node* list)
{
	if (!list) return nullptr;
	return new Node(list->data, clone(list->next));
}

void LinkedList::operator=(LinkedList const & list) // Deep copy
{
	head = clone(list.head);
}

void LinkedList::operator=(LinkedList const && list) // Shallow copy
{
	head = list.head;
}

float& LinkedList::get(const int i, Node* & list)
{
  if (!list) return dummy;
  if( i == 0 ) return list->data;
  else return get(i - 1, list->next);
}

float& LinkedList::operator[](const int i)
{
  return this->get(i);
}

#endif
