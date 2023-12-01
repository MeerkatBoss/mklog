/**
 * @file SimpleList.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 *
 * @brief Simple generic linked list implementation
 *
 * @version 0.1
 * @date 2023-08-16
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_UTILS_SIMPLELIST_H
#define __MEERKAT_LOGS_UTILS_SIMPLELIST_H

#include <cassert>
#include <cstddef>

namespace mklog
{

namespace utils
{

template <typename TValue>
class SimpleList
{
private:
  struct ListNode
  {
    TValue    value;
    ListNode* next;
  };

  ListNode* listHead;
  size_t    listSize;

public:
  class iterator
  {
  private:
    const SimpleList* parentList;
    ListNode*         lastNode;
    ListNode*         curNode;

    iterator(const SimpleList* parentList, ListNode* lastNode,
             ListNode* curNode)
        : parentList(parentList), lastNode(lastNode), curNode(curNode)
    {
    }

  public:
    friend class SimpleList;

    iterator(const iterator& other)
        : lastNode(other.lastNode), curNode(other.curNode)
    {
    }

    iterator& operator=(const iterator& other)
    {
      curNode  = other.curNode;
      lastNode = other.lastNode;
      return *this;
    }

    TValue& operator*()
    {
      assert(curNode != nullptr && "Cannot dereference end iterator");
      return curNode->value;
    }

    const TValue& operator*() const
    {
      assert(curNode != nullptr && "Cannot dereference end iterator");
      return curNode->value;
    }

    iterator& operator++()
    {
      assert(curNode != nullptr && "Cannot increment end iterator");
      curNode = curNode->next;

      if (curNode == nullptr)
        lastNode = nullptr;

      return *this;
    }

    iterator operator++(int)
    {
      assert(curNode != nullptr && "Cannot increment end iterator");

      ListNode* oldNode = curNode;
      curNode           = curNode->next;

      if (curNode == nullptr)
        lastNode = nullptr;

      return iterator(oldNode);
    }

    bool operator==(const iterator& other) const
    {
      return parentList == other.parentList && curNode == other.curNode;
    }
    bool operator!=(const iterator& other) const { return !(*this == other); }

    ~iterator() {}
  };

  SimpleList() : listHead(nullptr) {}

  void pushFront(TValue value)
  {
    ListNode* oldHead = listHead;

    listHead = new ListNode{.value = value, .next = oldHead};
    ++listSize;
  }

  template <typename... TArgs>
  void emplaceFront(TArgs... args)
  {
    ListNode* oldHead = listHead;

    listHead = new ListNode{.value = TValue(args...), .next = oldHead};
    ++listSize;
  }

  void popFront()
  {
    assert(listSize > 0 && "Cannot pop from empty list");
    assert(listHead != nullptr && "Corrupted list");

    ListNode* oldHead = listHead;

    listHead = listHead->next;

    delete oldHead;
    --listSize;
  }

  void erase(iterator& it)
  {
    assert(it != end() && "Cannot erase element at end()");
    assert(it.parentList == this &&
           "Cannot erase element with foreign iterator");

    ListNode* curNode = it.curNode;

    if (it.lastNode == nullptr)
    {
      listHead = curNode->next;
    }
    else
    {
      it.lastNode->next = curNode->next;
    }

    delete curNode;
    --listSize;
  }

  size_t size() { return listSize; }
  bool   empty() { return size() == 0; }

  void clear()
  {
    while (!empty())
    {
      popFront();
    }
  }

  iterator begin() { return iterator(this, nullptr, listHead); }
  iterator end() { return iterator(this, nullptr, nullptr); }
};

} // namespace utils

} // namespace mklog

#endif /* SimpleList.h */
