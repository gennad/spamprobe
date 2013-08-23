///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id$
//
// Copyright (C) 2007 Burton Computer Corporation
// ALL RIGHTS RESERVED
//
// This program is open source software; you can redistribute it
// and/or modify it under the terms of the Q Public License (QPL)
// version 1.0. Use of this software in whole or in part, including
// linking it (modified or unmodified) into other programs is
// subject to the terms of the QPL.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Q Public License for more details.
//
// You should have received a copy of the Q Public License
// along with this program; see the file LICENSE.txt.  If not, visit
// the Burton Computer Corporation or CoolDevTools web site
// QPL pages at:
//
//    http://www.burton-computer.com/qpl.html
//    http://www.cooldevtools.com/qpl.html
//

#ifndef _LRUCache_h
#define _LRUCache_h

#include <map>
#include <list>
#include "util.h"

template<class KeyType, class ValueType> class LRUCache
{
public:
  struct LRUCacheNode;
  typedef LRUCacheNode NodeType;
  typedef Ref<NodeType> RefType;
  typedef list<RefType> ListType;
  typedef typename list<RefType>::iterator IterType;

  class LRUCacheNodeKeyCompare;
  typedef map<NodeType *,NodeType *,LRUCacheNodeKeyCompare> MapType;
  typedef typename MapType::iterator MapIterType;

  struct LRUCacheNode
  {
    KeyType key;
    ValueType value;
    bool isLocked;
    IterType currentPosition;
  };

  class LRUCacheNodeKeyCompare : public binary_function<NodeType *, NodeType *, bool>
  {
  public:
    bool operator()(const NodeType *a,
                    const NodeType *b)
    {
      return a->key < b->key;
    }
  };

  class iterator
  {
    friend class LRUCache;

    iterator(MapType *index,
             MapIterType iter)
      : m_index(index), m_iterator(iter)
    {
    }

  public:
    bool operator==(const iterator &other)
    {
      return m_iterator == other.m_iterator;
    }

    bool operator!=(const iterator &other)
    {
      return m_iterator != other.m_iterator;
    }

    iterator &operator++()
    {
      ++m_iterator;
      return *this;
    }

    iterator operator++(int)
    {
      iterator old_value(m_index, m_iterator);
      ++m_iterator;
      return old_value;
    }

    const KeyType &key()
    {
      assert(m_iterator != m_index->end());
      return m_iterator->second->key;
    }

    const ValueType &value()
    {
      assert(m_iterator != m_index->end());
      return m_iterator->second->value;
    }

    bool isLocked()
    {
      assert(m_iterator != m_index->end());
      return m_iterator->second->isLocked;
    }

  private:
    MapType *m_index;
    MapIterType m_iterator;
  };

public:
  LRUCache(int max_size)
    : m_maxSize(max_size), m_normalCount(0), m_lockedCount(0)
  {
  }

  ~LRUCache()
  {
  }

  bool get(const KeyType &key,
           ValueType &value);

  void put(const KeyType &key,
           ValueType value,
           bool is_locked);

  void dump();

  void clear();

  iterator begin()
  {
    return iterator(&m_index, m_index.begin());
  }

  iterator end()
  {
    return iterator(&m_index, m_index.end());
  }

  iterator find(const KeyType &key);

  void unlock(const KeyType &key)
  {
    setLocked(findNode(key), false);
  }

  void lock(const KeyType &key)
  {
    setLocked(findNode(key), true);
  }

  int size()
  {
    return m_normalCount + m_lockedCount;
  }

  int maxSize()
  {
    return m_maxSize;
  }

  int lockedCount()
  {
    return m_lockedCount;
  }

  int unlockedCount()
  {
    return m_normalCount;
  }

private:
  void addNewNode(const KeyType &key,
                  ValueType value,
                  bool is_locked);

  NodeType *findNode(const KeyType &key);

  void setLocked(NodeType *node,
                 bool is_locked);

  void removeNode(NodeType *node);

  void eliminateOldObjects();

  void dump(ListType &nodes);

  void dump(NodeType *node);

  void setLocked(const KeyType &key,
                 bool is_locked)
  {
    setLocked(findNode(key), is_locked);
  }

private:
  int m_maxSize;
  int m_lockedCount;
  int m_normalCount;
  ListType m_normalList;
  ListType m_lockedList;
  MapType m_index;
};

template<class KeyType, class ValueType>
bool LRUCache<KeyType, ValueType>::get(const KeyType &key,
                                       ValueType &value)
{
  NodeType *node = findNode(key);
  if (node == 0) {
    return false;
  }

  if (!node->isLocked) {
    m_normalList.splice(m_normalList.begin(), m_normalList, node->currentPosition);
    node->currentPosition = m_normalList.begin();
    assert((*node->currentPosition).ptr() == node);
  }

  value = node->value;
  return true;
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::put(const KeyType &key,
                                       ValueType value,
                                       bool is_locked)
{
  NodeType key_node;
  key_node.key = key;
  MapIterType i = m_index.find(&key_node);
  if (i == m_index.end()) {
    addNewNode(key, value, is_locked);
  } else {
    NodeType *node = i->second;
    setLocked(node, is_locked);
    node->value = value;
  }
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::dump()
{
  cerr << "locked: " << lockedCount() << ": ";
  cerr << "locked: ";
  dump(m_lockedList);
  cerr << "normal: " << unlockedCount() << ": ";
  dump(m_normalList);
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::clear()
{
  m_index.clear();
  m_normalList.clear();
  m_lockedList.clear();
  m_normalCount = 0;
  m_lockedCount = 0;
}

template<class KeyType, class ValueType>
typename LRUCache<KeyType, ValueType>::iterator LRUCache<KeyType, ValueType>::find(const KeyType &key)
{
  NodeType key_node;
  key_node.key = key;
  return iterator(&m_index, m_index.find(&key_node));
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::addNewNode(const KeyType &key,
                                              ValueType value,
                                              bool is_locked)
{
  eliminateOldObjects();
  RefType node(new NodeType);
  node->key = key;
  node->value = value;
  node->isLocked = is_locked;
  if (is_locked) {
    node->currentPosition = m_lockedList.insert(m_lockedList.begin(), node);
    m_lockedCount += 1;
  } else {
    node->currentPosition = m_normalList.insert(m_normalList.begin(), node);
    m_normalCount += 1;
  }
  m_index.insert(make_pair(node.ptr(), node.ptr()));
}

template<class KeyType, class ValueType>
typename LRUCache<KeyType, ValueType>::NodeType *LRUCache<KeyType, ValueType>::findNode(const KeyType &key)
{
  NodeType key_node;
  key_node.key = key;
  MapIterType map_iter = m_index.find(&key_node);
  return (map_iter == m_index.end()) ? 0 : map_iter->second;
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::setLocked(NodeType *node,
                                             bool is_locked)
{
  if (node) {
    if (is_locked) {
      if (!node->isLocked) {
        m_normalCount -= 1;
        m_lockedCount += 1;
        node->isLocked = true;
        m_lockedList.splice(m_lockedList.begin(), m_normalList, node->currentPosition);
        node->currentPosition = m_lockedList.begin();
        assert((*node->currentPosition).ptr() == node);
      }
    } else {
      if (node->isLocked) {
        m_normalCount += 1;
        m_lockedCount -= 1;
        node->isLocked = false;
        m_normalList.splice(m_normalList.begin(), m_lockedList, node->currentPosition);
        node->currentPosition = m_normalList.begin();
        assert((*node->currentPosition).ptr() == node);
      }
    }
  }
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::removeNode(NodeType *node)
{
  if (node->isLocked) {
    m_lockedCount -= 1;
    m_index.erase(node);
    m_lockedList.erase(node->currentPosition);
  } else {
    m_normalCount -= 1;
    m_index.erase(node);
    m_normalList.erase(node->currentPosition);
  }
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::eliminateOldObjects()
{
  while (size() >= m_maxSize && m_normalCount > 0) {
    RefType nr(m_normalList.back());
    removeNode(nr.ptr());
  }
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::dump(ListType &nodes)
{
  for (IterType i = nodes.begin(); i != nodes.end(); ++i) {
    dump((*i).ptr());
  }
  cerr << endl;
}

template<class KeyType, class ValueType>
void LRUCache<KeyType, ValueType>::dump(NodeType *node)
{
  cerr << "Node(" << node->key << "," << node->value << "," << node->isLocked << ")";
}

#endif
