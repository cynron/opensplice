/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2013 PrismTech
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE
 *
 *   for full copyright notice and license terms.
 *
 */
#include <limits.h>

#include "os_defs.h"
#include "os_stdlib.h"

#include "q_avl.h"
#include "q_unused.h"

#define AVL_INDKEY(offset) (-(offset) - 1)
#define IS_INDKEY(x) ((x) < 0)
#define LOAD_DIRKEY(base, x) ((base) + (x))
#define LOAD_INDKEY(base, x) (*((char **) ((base) - (x) - 1)))
#define LOAD_KEY(base, x) (IS_INDKEY (x) ? LOAD_INDKEY (base, x) : LOAD_DIRKEY (base, x))

#define MAX_TREE_HEIGHT (12 * sizeof (void *))

STRUCT_AVLNODE (avlnode, char *);
STRUCT_AVLTREE (avltree, char *);

static  struct avlnode *node_from_onode (struct avltree *tree, char *onode)
{
  if (onode == NULL)
    return NULL;
  else
    return (struct avlnode *) (onode + tree->avlnodeoffset);
}

static  const struct avlnode *cnode_from_onode (const struct avltree *tree, const char *onode)
{
  if (onode == NULL)
    return NULL;
  else
    return (const struct avlnode *) (onode + tree->avlnodeoffset);
}

static  char *onode_from_node (struct avltree *tree, struct avlnode *node)
{
  if (node == NULL)
    return NULL;
  else
    return (char *) node - tree->avlnodeoffset;
}

static  const char *conode_from_node (const struct avltree *tree, const struct avlnode *node)
{
  if (node == NULL)
    return NULL;
  else
    return (const char *) node - tree->avlnodeoffset;
}

static void avl_init_common (void *vtree, os_int32 avlnodeoffset, os_int32 keyoffset, avlcompare_fun_t comparekk, avlaugment_fun_t augment)
{
  struct avltree *tree = vtree;
  tree->root = NULL;
  tree->avlnodeoffset = avlnodeoffset;
  tree->keyoffset = keyoffset;
  tree->comparekk = comparekk;
  tree->augment = augment;
}

void avl_init (void *vtree, os_size_t avlnodeoffset, os_size_t keyoffset, avlcompare_fun_t comparekk, avlaugment_fun_t augment)
{
  assert ((os_int32) avlnodeoffset >= 0);
  assert ((os_int32) keyoffset >= 0);
  avl_init_common (vtree, (os_int32) avlnodeoffset, (os_int32) keyoffset, comparekk, augment);
}

void avl_parent_from_node (const void *vtree, const void *node, avlparent_t *parent)
{
  const struct avltree *tree = vtree;
  parent->addr = (char *) conode_from_node (tree, node);
}

void avl_init_indkey (void *vtree, os_size_t avlnodeoffset, os_size_t keyoffset, avlcompare_fun_t comparekk, avlaugment_fun_t augment)
{
  assert ((os_int32) avlnodeoffset >= 0);
  assert ((os_int32) keyoffset >= 0);
  avl_init_common (vtree, (os_int32) avlnodeoffset, AVL_INDKEY ((os_int32) keyoffset), comparekk, augment);
}

static void treedestroy (struct avltree *tree, char *onode, void (*freefun) (void *node))
{
  if (onode)
  {
    struct avlnode *n = node_from_onode (tree, onode);
    n->parent = NULL;
    treedestroy (tree, n->left, freefun);
    treedestroy (tree, n->right, freefun);
    n->left = NULL;
    n->right = NULL;
    freefun (onode);
  }
}

void avl_free (void *vtree, void (*freefun) (void *node))
{
  struct avltree *tree = vtree;
  char *root = tree->root;
  tree->root = NULL;
  if (freefun)
    treedestroy (tree, root, freefun);
}

#define HEIGHTOF(tree)  ((tree) == NULL ? 0 : (tree)->height)

static char *tree_rebalance_one (struct avltree *tree, char *onode)
{
  struct avlnode *node = node_from_onode (tree, onode);
  struct avlnode *noderight = node_from_onode (tree, node->right);
  struct avlnode *nodeleft  = node_from_onode (tree, node->left);
  int heightright = HEIGHTOF (noderight);
  int heightleft  = HEIGHTOF (nodeleft);

  if (heightright + 1 < heightleft)
  {
    /*                                                      */
    /*                            *                         */
    /*                          /   \                       */
    /*                       n+2      n                     */
    /*                                                      */
    struct avlnode *nodeleftleft, *nodeleftright;
    int heightleftright;
    assert (nodeleft);
    nodeleftleft = node_from_onode (tree, nodeleft->left);
    nodeleftright = node_from_onode (tree, nodeleft->right);
    heightleftright = HEIGHTOF (nodeleftright);

    if (HEIGHTOF (nodeleftleft) >= heightleftright)
    {
      /*                                                        */
      /*                *                    n+2|n+3            */
      /*              /   \                  /    \             */
      /*           n+2      n      -->      /   n+1|n+2         */
      /*           / \                      |    /    \         */
      /*         n+1 n|n+1                 n+1  n|n+1  n        */
      /*                                                        */
      struct avlnode *nlp;
      node->left = nodeleft->right;
      nodeleft->parent = node->parent;
      node->parent = onode_from_node (tree, nodeleft);
      if (nodeleftright)
        nodeleftright->parent = onode_from_node (tree, node);
      nodeleft->right = onode_from_node (tree, node);
      node->height = 1 + heightleftright;
      nodeleft->height = 1 + node->height;
      nlp = node_from_onode (tree, nodeleft->parent);
      if (nlp == NULL)
        tree->root = onode_from_node (tree, nodeleft);
      else if (node == node_from_onode (tree, nlp->left))
        nlp->left = onode_from_node (tree, nodeleft);
      else
        nlp->right = onode_from_node (tree, nodeleft);

      /* nodeleft */
      /* /     \ */
      /*      node */
      /*      /   \ */
      /*  nodelr */
      if (tree->augment)
      {
        tree->augment (onode_from_node (tree, node));
        tree->augment (onode_from_node (tree, nodeleft));
      }
      return nodeleft->parent;
    }
    else
    {
      /*                                                        */
      /*                *                     n+2               */
      /*              /   \                 /     \             */
      /*           n+2      n      -->    n+1     n+1           */
      /*           / \                    / \     / \           */
      /*          n  n+1                 n   L   R   n          */
      /*             / \                                        */
      /*            L   R                                       */
      /*                                                        */
      struct avlnode *nlrp;
      assert (nodeleftright);
      nodeleftright->parent = node->parent;
      nodeleft->right = nodeleftright->left;
      if (nodeleft->right)
        node_from_onode (tree, nodeleft->right)->parent = onode_from_node (tree, nodeleft);
      node->left = nodeleftright->right;
      if (node->left)
        node_from_onode (tree, node->left)->parent = onode_from_node (tree, node);
      nodeleftright->left = onode_from_node (tree, nodeleft);
      nodeleft->parent = onode_from_node (tree, nodeleftright);
      nodeleftright->right = onode_from_node (tree, node);
      node->parent = onode_from_node (tree, nodeleftright);
      nodeleft->height = node->height = heightleftright;
      nodeleftright->height = heightleft;
      nlrp = node_from_onode (tree, nodeleftright->parent);
      if (nlrp == NULL)
        tree->root = onode_from_node (tree, nodeleftright);
      else if (node == node_from_onode (tree, nlrp->left))
        nlrp->left = onode_from_node (tree, nodeleftright);
      else
        nlrp->right = onode_from_node (tree, nodeleftright);

      /*       nodeleftright */
      /*       /           \ */
      /* nodeleft          node */
      /* /     \          /   \ */
      /*   nodelrl    nodelrr */
      if (tree->augment)
      {
        tree->augment (onode_from_node (tree, nodeleft));
        tree->augment (onode_from_node (tree, node));
        tree->augment (onode_from_node (tree, nodeleftright));
      }
      return nodeleftright->parent;
    }
  }
  else if (heightleft + 1 < heightright)
  {
    /* similar to the above, just interchange 'left' <--> 'right' */
    struct avlnode *noderightright, *noderightleft;
    int heightrightleft;
    assert (noderight);
    noderightright = node_from_onode (tree, noderight->right);
    noderightleft = node_from_onode (tree, noderight->left);
    heightrightleft = HEIGHTOF (noderightleft);

    if (HEIGHTOF (noderightright) >= heightrightleft)
    {
      struct avlnode *nrp;
      node->right = noderight->left;
      noderight->parent = node->parent;
      node->parent = onode_from_node (tree, noderight);
      if (noderightleft)
        noderightleft->parent = onode_from_node (tree, node);
      noderight->left = onode_from_node (tree, node);
      node->height = 1 + heightrightleft;
      noderight->height = 1 + node->height;
      nrp = node_from_onode (tree, noderight->parent);
      if (nrp == NULL)
        tree->root = onode_from_node (tree, noderight);
      else if (node == node_from_onode (tree, nrp->left))
        nrp->left = onode_from_node (tree, noderight);
      else
        nrp->right = onode_from_node (tree, noderight);

      /*    noderight */
      /*     /     \ */
      /*  node */
      /* /   \ */
      /*    noderl */
      if (tree->augment)
      {
        tree->augment (onode_from_node (tree, node));
        tree->augment (onode_from_node (tree, noderight));
      }
      return noderight->parent;
    }
    else
    {
      struct avlnode *nrlp;
      assert (noderightleft);
      noderightleft->parent = node->parent;
      noderight->left = noderightleft->right;
      if (noderight->left)
        node_from_onode (tree, noderight->left)->parent = onode_from_node (tree, noderight);
      node->right = noderightleft->left;
      if (node->right)
        node_from_onode (tree, node->right)->parent = onode_from_node (tree, node);
      noderightleft->right = onode_from_node (tree, noderight);
      noderight->parent = onode_from_node (tree, noderightleft);
      noderightleft->left = onode_from_node (tree, node);
      node->parent = onode_from_node (tree, noderightleft);
      noderight->height = node->height = heightrightleft;
      noderightleft->height = heightright;
      nrlp = node_from_onode (tree, noderightleft->parent);
      if (nrlp == NULL)
        tree->root = onode_from_node (tree, noderightleft);
      else if (node == node_from_onode (tree, nrlp->left))
        nrlp->left = onode_from_node (tree, noderightleft);
      else
        nrlp->right = onode_from_node (tree, noderightleft);

      /*       noderightleft */
      /*       /           \ */
      /*   node          noderight */
      /*  /     \          /   \ */
      /*     noderll    noderlr */
      if (tree->augment)
      {
        tree->augment (onode_from_node (tree, node));
        tree->augment (onode_from_node (tree, noderight));
        tree->augment (onode_from_node (tree, noderightleft));
      }
      return noderightleft->parent;
    }
  }
  else
  {
    /* Rebalance happens only on insert & delete, and for both cases
       tree->augment() requires walking all the way to the root,
       even if rebalancing doesn't really require doing so. It could
       be implemented (more cleanly, arguably) as insert ;
       tree->augment() all the way up ; rebalance, but that would
       mean running up to the root twice, for no good reason, so we
       combine them. (Besides we don't care all that much for
       performance, in this particular instance.) */
    int height =
      (heightleft < heightright ? heightright : heightleft) + 1;
    node->height = height;
    if (tree->augment)
      tree->augment (onode_from_node (tree, node));
    return node->parent;
  }
}

static void tree_rebalance (struct avltree *tree, char *onode)
{
  while (onode)
    onode = tree_rebalance_one (tree, onode);
}

static int comparenk (const struct avltree *tree, const char *a, const void *b)
{
  return tree->comparekk (LOAD_KEY (a, tree->keyoffset), b);
}

static int comparenn (const struct avltree *tree, const char *a, const char *b)
{
  return tree->comparekk (LOAD_KEY (a, tree->keyoffset), LOAD_KEY (b, tree->keyoffset));
}

void *avl_lookup (const void *vtree, const void *key, avlparent_t *parent)
{
  const struct avltree *tree = vtree;
  const struct avlnode *tmp, *prev;
  if (tree->root == NULL)
  {
    if (parent)
      parent->addr = NULL;
    return NULL;
  }
  else
  {
    int c;
    tmp = cnode_from_onode (tree, tree->root);
    prev = NULL;
    /* return an arbitrarily selected matching node (here the first
       one encountered), but that need not be the first one to be
       inserted with this key */
    while (tmp && (c = comparenk (tree, conode_from_node (tree, tmp), key)) != 0)
    {
      prev = tmp;
      if (c > 0)
        tmp = cnode_from_onode (tree, tmp->left);
      else
        tmp = cnode_from_onode (tree, tmp->right);
    }
  }
  if (parent)
  {
    /* Parent for insertion: if key not found, this is simply prev;
       but if it was found, return the rightmost element with the same
       key, so that an in-order treewalk visits nodes with the same
       key value the in the order of inserting them (nice for a
       priority queue) */
    if (tmp != NULL)
    {
      const struct avlnode *t = tmp;
      while (t)
      {
        prev = t;
        if (comparenk (tree, conode_from_node (tree, t), key) > 0)
          t = cnode_from_onode (tree, t->left);
        else
          t = cnode_from_onode (tree, t->right);
      }
    }
    parent->addr = (char *) conode_from_node (tree, prev);
  }
  return (void *) conode_from_node (tree, tmp);
}

void avl_init_node (void *vnode, avlparent_t parent)
{
  struct avlnode *node = vnode;
  node->left = NULL;
  node->right = NULL;
  node->parent = parent.addr;
  node->height = 1;
}

void avl_insert (void *vtree, void *vnode)
{
  struct avltree *tree = vtree;
  struct avlnode *node = node_from_onode (tree, vnode);
  if (tree->augment)
    tree->augment (vnode);
  if (tree->root == NULL)
    tree->root = vnode;
  else
  {
    int c = comparenn (tree, node->parent, vnode);
    if (c > 0)
      node_from_onode (tree, node->parent)->left = vnode;
    else /* c = 0 => right child, so later in in-order treewalk */
      node_from_onode (tree, node->parent)->right = vnode;
    tree_rebalance (tree, node->parent);
  }
}

void avl_delete (void *vtree, void *vnode)
{
  struct avltree *tree = vtree;
  struct avlnode *node = node_from_onode (tree, vnode);
  struct avlnode *parent = node_from_onode (tree, node->parent);
  struct avlnode *largest;
  char *rebalance_start;

  if (node->left == NULL)
  {
    struct avlnode *right = node_from_onode (tree, node->right);
    if (parent == NULL)
      tree->root = node->right;
    else if ((char *) vnode == parent->left)
      parent->left = node->right;
    else
      parent->right = node->right;
    if (right)
      right->parent = node->parent;
    if (parent)
    {
      if (tree->augment)
        tree->augment (onode_from_node (tree, parent));
      tree_rebalance (tree, onode_from_node (tree, parent));
    }
    return;
  }

  if (node->right == NULL)
  {
    struct avlnode *left = node_from_onode (tree, node->left);
    if (parent == NULL)
      tree->root = node->left;
    else if ((char *) vnode == parent->left)
      parent->left = node->left;
    else
      parent->right = node->left;
    left->parent = node->parent;
    if (parent)
    {
      if (tree->augment)
        tree->augment (onode_from_node (tree, parent));
      tree_rebalance (tree, onode_from_node (tree, parent));
    }
    return;
  }

  /* Search the left orphaned subtree for the largest value it
     contains. This will necessarily be smaller than the deleted node,
     and all the nodes in the right subtree.  */
  largest = node_from_onode (tree, node->left);
  if (largest->right == NULL)
  {
    /*        node                 largest  */
    /*       /    \                 /   \   */
    /*      /      \               a     b  */
    /*  largest     b    -->                */
    /*  /    \                              */
    /* a    (nil)                           */
    largest->parent = node->parent;
    largest->right  = node->right;
    largest->height = node->height;
    if (largest->right)
      node_from_onode (tree, largest->right)->parent = onode_from_node (tree, largest);
    rebalance_start = onode_from_node (tree, largest);
  }
  else
  {
    while (largest->right)
      largest = node_from_onode (tree, largest->right);
    /*        node                 largest   */
    /*       /    \                 /   \    */
    /*      /      \               /     \   */
    /*     a        b             a       b  */
    /*   /   \     ...           / \     ... */
    /* ...   ...          -->  ... ...       */
    /*        c                      c       */
    /*       / \                    / \      */
    /*     ... largest            ...  d     */
    /*         /    \                        */
    /*        d    (nil)                     */
    rebalance_start = largest->parent;
    node_from_onode (tree, rebalance_start)->right = largest->left;
    if (largest->left)
      node_from_onode (tree, largest->left)->parent = rebalance_start;

    largest->right  = node->right;
    largest->left   = node->left;
    largest->parent = node->parent;
    largest->height = node->height;
    node_from_onode (tree, node->left)->parent = onode_from_node (tree, largest);
    node_from_onode (tree, node->right)->parent = onode_from_node (tree, largest);
  }

  if (parent == NULL)
    tree->root = onode_from_node (tree, largest);
  else if (vnode == parent->left)
    parent->left = onode_from_node (tree, largest);
  else
    parent->right = onode_from_node (tree, largest);

  if (tree->augment)
    tree->augment (rebalance_start);
  tree_rebalance (tree, rebalance_start);
}

void avl_swapnode (void *vtree, void *vold, void *vnew)
{
  struct avltree *tree = vtree;
  struct avlnode *old = node_from_onode (tree, vold);
  struct avlnode *new = node_from_onode (tree, vnew);
  struct avlnode *parent = node_from_onode (tree, old->parent);
  assert (comparenn (tree, vold, vnew) == 0);

  /* Update parent/child relationships */
  new->parent = old->parent;
  if (parent == NULL)
    tree->root = vnew;
  else if (vold == parent->left)
    parent->left = vnew;
  else
    parent->right = vnew;

  new->left = old->left;
  new->right = old->right;
  if (old->left)
    node_from_onode (tree, old->left)->parent = vnew;
  if (old->right)
    node_from_onode (tree, old->right)->parent = vnew;

  /* Update augmented data */
  if (tree->augment)
    tree->augment (vnew);
}

static struct avlnode *findpred (const struct avltree *tree, const struct avlnode *n)
{
  if (n->left)
  {
    n = cnode_from_onode (tree, n->left);
    while (n->right)
      n = cnode_from_onode (tree, n->right);
    return (struct avlnode *) n;
  }
  else
  {
    const struct avlnode *p = cnode_from_onode (tree, n->parent);
    while (p && n == cnode_from_onode (tree, p->left))
    {
      n = p;
      p = cnode_from_onode (tree, p->parent);
    }
    return (struct avlnode *) p;
  }
}

static struct avlnode *findsucc (const struct avltree *tree, const struct avlnode *n)
{
  if (n->right)
  {
    n = cnode_from_onode (tree, n->right);
    while (n->left)
      n = cnode_from_onode (tree, n->left);
    return (struct avlnode *) n;
  }
  else
  {
    const struct avlnode *p = cnode_from_onode (tree, n->parent);
    while (p && n == cnode_from_onode (tree, p->right))
    {
      n = p;
      p = cnode_from_onode (tree, p->parent);
    }
    return (struct avlnode *) p;
  }
}

static struct avlnode *findmin (const struct avltree *tree)
{
  const struct avlnode *n = cnode_from_onode (tree, tree->root);
  if (n)
  {
    while (n->left)
      n = cnode_from_onode (tree, n->left);
  }
  return (struct avlnode *) n;
}

static struct avlnode *findmax (const struct avltree *tree)
{
  const struct avlnode *n = cnode_from_onode (tree, tree->root);
  if (n)
  {
    while (n->right)
      n = cnode_from_onode (tree, n->right);
  }
  return (struct avlnode *) n;
}

void *avl_findmin (const void *vtree)
{
  const struct avltree *tree = vtree;
  return (void *) conode_from_node (tree, findmin (tree));
}

void *avl_findmax (const void *vtree)
{
  const struct avltree *tree = vtree;
  return (void *) conode_from_node (tree, findmax (tree));
}

void *avl_findpred (const void *vtree, const void *vnode)
{
  const struct avltree *tree = vtree;
  const struct avlnode *n = cnode_from_onode (tree, vnode);
  if (n == NULL)
    return avl_findmax (vtree);
  else
    return (void *) conode_from_node (tree, findpred (tree, n));
}

void *avl_findsucc (const void *vtree, const void *vnode)
{
  const struct avltree *tree = vtree;
  const struct avlnode *n = cnode_from_onode (tree, vnode);
  if (n == NULL)
    return avl_findmin (vtree);
  else
    return (void *) conode_from_node (tree, findsucc (tree, n));
}

void avl_walk (void *vtree, avlwalk_fun_t f, void *a)
{
#if 1

  struct avltree *tree = vtree;
  struct avlnode *n = findmin (tree), *nn;
  while (n)
  {
    nn = findsucc (tree, n);
    f (onode_from_node (tree, n), a);
    n = nn;
  }

#else

  /* Arguably, this upper bound is overkill, we know there can be at
     most 2**32 nodes in the tree, so 1.5 * Log[2, 2**32] would
     do. But this is copy-pasteable to wherever we need it. */
#define MAX_TREEHEIGHT (3 * CHAR_BIT * sizeof (void *) / 2)

  struct avltree *tree = vtree;
  struct avlnode *todo[MAX_TREEHEIGHT], **todop = todo;
  *todop = node_from_onode (tree, tree->root);
  while (*todop)
  {
    struct avlnode *right, *n;
    /* First locate the minimum value in this subtree */
    n = node_from_onode (tree, (*todop)->left);
    while (n)
    {
      *++todop = n;
      n = node_from_onode (tree, n->left);
    }
    /* Then process it and its parents until a node N is hit that has
       a right subtree, with (by definition) key values in between N
       and the parent of N */
    do {
      right = node_from_onode (tree, (*todop)->right);
      f (onode_from_node (tree, *todop), a);
    } while (todop-- > todo && right == NULL);
    /* Continue with right subtree rooted at 'right' before processing
       the parent node of the last node processed in the loop above */
    *++todop = right;
  }

#endif
}

int avl_empty (const void *vtree)
{
  const struct avltree *tree = vtree;
  return tree->root == NULL;
}

void avl_augment_update (void *vtree, void *vnode)
{
  struct avltree *tree = vtree;
  if (tree->augment)
  {
    struct avlnode *node = node_from_onode (tree, vnode);
    while (node)
    {
      tree->augment (node);
      node = node_from_onode (tree, node->parent);
    }
  }
}

void *avl_lookup_predeq (const void *vtree, const void *key)
{
  const struct avltree *tree = vtree;
  if (tree->root == NULL)
    return NULL;
  else
  {
    const struct avlnode *tmp;
    const struct avlnode *cand;
    int c;
    tmp = cnode_from_onode (tree, tree->root);
    cand = NULL;
    while (tmp && (c = comparenk (tree, conode_from_node (tree, tmp), key)) != 0)
    {
      if (c < 0)
      {
        cand = tmp;
        tmp = cnode_from_onode (tree, tmp->right);
      }
      else
      {
        tmp = cnode_from_onode (tree, tmp->left);
      }
    }
    return (void *) conode_from_node (tree, tmp ? tmp : cand);
  }
}

void *avl_lookup_succeq (const void *vtree, const void *key)
{
  const struct avltree *tree = vtree;
  if (tree->root == NULL)
    return NULL;
  else
  {
    const struct avlnode *tmp;
    const struct avlnode *cand;
    int c;
    tmp = cnode_from_onode (tree, tree->root);
    cand = NULL;
    while (tmp && (c = comparenk (tree, conode_from_node (tree, tmp), key)) != 0)
    {
      if (c > 0)
      {
        cand = tmp;
        tmp = cnode_from_onode (tree, tmp->left);
      }
      else
      {
        tmp = cnode_from_onode (tree, tmp->right);
      }
    }
    return (void *) conode_from_node (tree, tmp ? tmp : cand);
  }
}

void avl_walkrange (void *vtree, const void *min, const void *max, avlwalk_fun_t f, void *a)
{
  struct avltree *tree = vtree;
  struct avlnode *n, *nn;
  n = node_from_onode (tree, avl_lookup_succeq (vtree, min));
  while (n && comparenk (tree, conode_from_node (tree, n), max) <= 0)
  {
    nn = findsucc (tree, n);
    f (onode_from_node (tree, n), a);
    n = nn;
  }
}

/* SHA1 not available (unoffical build.) */
