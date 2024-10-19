#include "rbtree.h"

#include <stdlib.h>

#include <stdio.h>

node_t* _create_new_node(const key_t key, rbtree* t);
void _delete_rbtree_node(node_t* node, node_t* nil);
void _left_rotate(rbtree* t, node_t* x);
void _right_rotate(rbtree* t, node_t* x);
void _fix_up_from_insertion(rbtree* t, node_t* z);
void _rb_transplant(rbtree* t, node_t* u, node_t* v);
node_t* _min_node_in(rbtree* t, node_t* current);
node_t* _max_node_in(rbtree* t, node_t* current);
void _fix_up_from_deletion(rbtree* t, node_t* x);

// TODO: initialize struct if needed
rbtree *new_rbtree(void) {
  node_t* nilNode = malloc(sizeof(node_t));
  nilNode->color = RBTREE_BLACK;
  nilNode->parent = NULL;
  nilNode->left = NULL;
  nilNode->right = NULL;
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = nilNode;
  p->root = p->nil;
  return p;
}

// TODO: reclaim the tree nodes's memory
void delete_rbtree(rbtree *t) {
  if (t == NULL) {
    return;
  }
  _delete_rbtree_node(t->root, t->nil);
  free(t->nil);
  free(t);
}

// TODO: implement insert
node_t *rbtree_insert(rbtree *t, const key_t key) {

  node_t* z = _create_new_node(key, t);
  node_t* x = t->root;
  node_t* y = t->nil;

  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  z->parent = y;
  if (y == t->nil) {
    t->root = z;
  } else if (z->key < y->key) {
    y->left = z;
  } else {
    y->right = z;
  }

  _fix_up_from_insertion(t, z);

  return z;
}

// TODO: implement find
node_t *rbtree_find(const rbtree *t, const key_t key) {
  if (t->root == NULL) {
    return NULL;
  }
  node_t* current = t->root;
  while (current != t->nil) {
    if (current->key == key) {
      return current;
    }
    if (current->key < key) {
      current = current->right;   
    }
    if (current->key > key) {
      current = current->left; 
    }
  }
  return NULL;
}

// TODO: implement find
node_t *rbtree_min(const rbtree *t) {
  if (t == NULL)
    return NULL;
  node_t* result = _min_node_in(t, t->root);
  if (result == t->nil) {
    return NULL;
  } else {
    return result;
  }
}

// TODO: implement find
node_t *rbtree_max(const rbtree *t) {
  if (t == NULL)
    return NULL;
  node_t* result = _max_node_in(t, t->root);
  if (result == t->nil) {
    return NULL;
  } else {
    return result;
  }
}

// TODO: implement erase
int rbtree_erase(rbtree *t, node_t *p) {
  node_t* x = t->nil;
  node_t* z = p;
  node_t* y = p;
  color_t Y_ORIGIN_COLOR = y->color;

  if (z->left == t->nil) {
    x = z->right;
    _rb_transplant(t, z, z->right);
  } else if (z->right == t->nil) {
    x = z->left;
    _rb_transplant(t, z, z->left);
  } else { // 둘 다 값이 있을 때 
    x = z->right;

    node_t* tttt = z->right;

    while (tttt->left != t->nil)
    {
      tttt = tttt->left;
    }

    y = _min_node_in(t, z->right);

    Y_ORIGIN_COLOR = y->color;
    
    x = y->right;
    
    if (y != z->right) {
      _rb_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    } else {
      if (y->parent != t->nil)
        x->parent = y;
    }

    _rb_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (Y_ORIGIN_COLOR == RBTREE_BLACK) {
    _fix_up_from_deletion(t, x);
  }

  free(p);

  return 0;
}

// TODO: implement to_array
// qsort한 값과 비교함
// inorder 순회하면 정렬 값 얻을 수 있음 
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  _inorder(t, t->root, arr, n, 0);
  return 0;
}

int _inorder(const rbtree* t, const node_t* node, key_t *arr, const size_t n, int index) {
  if (index > n || node == NULL || node == t->nil) {
    return index;
  }
  if (node->left != t->nil) {
    index = _inorder(t, node->left, arr, n, index);
  }
  arr[index] = node->key;
  index++;
  if (node->right != t->nil) {
    index = _inorder(t, node->right, arr, n, index);
  }
  return index;
}

// ------------------------------------------------------
// -----------------other function-----------------------
// ------------------------------------------------------

node_t* _create_new_node(const key_t key, rbtree* t) {
  node_t* newNode = malloc(sizeof(node_t));
  newNode->color = RBTREE_RED;
  newNode->key = key;
  newNode->left = t->nil;
  newNode->right = t->nil;
  newNode->parent = t->nil;
  return newNode;
}

void _delete_rbtree_node(node_t* node, node_t* nil) {
  if (node == nil) {
    return;
  }
  _delete_rbtree_node(node->left, nil);
  _delete_rbtree_node(node->right, nil);
  free(node);
}

void _left_rotate(rbtree* t, node_t* x) {
  node_t* y = x->right;
  // y의 left를 x의 right로
  x->right = y->left;
  if (y->left != t->nil) {
    y->left->parent = x;
  }
  // y의 부모 재설정
  y->parent = x->parent;
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else { // X가 right인경우
    x->parent->right = y;
  }
  // x의 부모 설정
  y->left = x;
  x->parent = y;
}

void _right_rotate(rbtree* t, node_t* x) {
  node_t* y = x->left;
  x->left = y->right;
  if (y->right != t->nil) {
    y->right->parent = x;
  }
  // y의 부모 재설정
  y->parent = x->parent;
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->right) {
    x->parent->right = y;
  } else { // X가 right인경우
    x->parent->left = y;
  }
  // x의 부모 설정
  y->right = x;
  x->parent = y;
}

void _fix_up_from_insertion(rbtree* t, node_t* z) {
  while (z->parent->color == RBTREE_RED) { // parent가 black이면 그냥 넘어간다
    if (z->parent == z->parent->parent->left) {
      node_t* y = z->parent->parent->right; // uncle
      if (y->color == RBTREE_RED) { // uncle을 black으로 바꿔서 균형을 이룬다
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; // 재귀적 실행
      } else { // uncle이 black인 경우 rotate 필요 
        // z가 right이면, left 형태로 바꿔서 처리
        if (z == z->parent->right) {
          z = z->parent;
          _left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK; 
        z->parent->parent->color = RBTREE_RED;
        _right_rotate(t, z->parent->parent);
      }
    } else {
      node_t* y = z->parent->parent->left;
      if (y->color == RBTREE_RED) { // uncle을 black으로 바꿔서 균형을 이룬다
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; // 재귀적 실행
      } else { // uncle이 black인 경우 rotate 필요 
        if (z == z->parent->left) {
          z = z->parent;
          _right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK; 
        z->parent->parent->color = RBTREE_RED;
        _left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

void _rb_transplant(rbtree* t, node_t* u, node_t* v) {
  if (u->parent == t->nil) {
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

node_t* _max_node_in(rbtree* t, node_t* current) {
  if (current == NULL) 
    return NULL;
  while (current != t->nil && current->right != t->nil)
    current = current->right;
  return current;
}

node_t* _min_node_in(rbtree* t, node_t* current) {
  if (current == NULL) 
    return NULL;
  while (current != t->nil && current->left != t->nil)
    current = current->left;
  return current;
}

void _fix_up_from_deletion(rbtree* t, node_t* x) {
  while (x != t->root && (x == t->nil || x->color == RBTREE_BLACK))
  {
    if (x == x->parent->left) {
      node_t* w = x->parent->right;
      // case 1
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK; 
        x->parent->color = RBTREE_RED;
        _left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // case 2
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // case 3
        // case 4로 만든다(red를 오른쪽으로 옮겨줌)
        if (w->right->color == RBTREE_BLACK) { // left가 red
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          _right_rotate(t, w);
          w = x->parent->right;
        }
        // case 4
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        _left_rotate(t, x->parent);
        x = t->root; // 종료 시킴
      }
    } else {
      node_t* w = x->parent->left;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        _right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent; 
      } else {
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          _left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        _right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;  
}