#ifndef ESET_HPP
#define ESET_HPP

#include <functional>
#include <utility>
#include <cstddef>

template<class Key, class Compare = std::less<Key>>
class ESet {
private:
    enum Color { RED, BLACK };
    
    struct Node {
        Key key;
        Color color;
        Node *left, *right, *parent;
        size_t size;
        
        Node(const Key& k, Color c = RED, Node* p = nullptr)
            : key(k), color(c), left(nullptr), right(nullptr), parent(p), size(1) {}
    };
    
    Node* root;
    Compare comp;
    size_t tree_size;
    
    inline size_t getSize(Node* n) const { return n ? n->size : 0; }
    
    inline void updateSize(Node* n) {
        if (n) n->size = 1 + getSize(n->left) + getSize(n->right);
    }
    
    inline void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
        updateSize(x);
        updateSize(y);
    }
    
    inline void rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (!y->parent) root = x;
        else if (y == y->parent->left) y->parent->left = x;
        else y->parent->right = x;
        x->right = y;
        y->parent = x;
        updateSize(y);
        updateSize(x);
    }
    
    void fixInsert(Node* z) {
        while (z->parent && z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateRight(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }
    
    void transplant(Node* u, Node* v) {
        if (!u->parent) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }
    
    Node* minimum(Node* n) const {
        while (n->left) n = n->left;
        return n;
    }
    
    Node* maximum(Node* n) const {
        while (n->right) n = n->right;
        return n;
    }
    
    void fixDelete(Node* x, Node* xp) {
        while (x != root && (!x || x->color == BLACK)) {
            if (!xp) break;  // Safety check
            
            if (x == xp->left) {
                Node* w = xp->right;
                if (!w) break;  // Safety check
                
                if (w->color == RED) {
                    w->color = BLACK;
                    xp->color = RED;
                    rotateLeft(xp);
                    w = xp->right;
                    if (!w) break;  // Safety check after rotation
                }
                if ((!w->left || w->left->color == BLACK) && 
                    (!w->right || w->right->color == BLACK)) {
                    w->color = RED;
                    x = xp;
                    xp = x->parent;
                } else {
                    if (!w->right || w->right->color == BLACK) {
                        if (w->left) w->left->color = BLACK;
                        w->color = RED;
                        rotateRight(w);
                        w = xp->right;
                        if (!w) break;  // Safety check after rotation
                    }
                    w->color = xp->color;
                    xp->color = BLACK;
                    if (w->right) w->right->color = BLACK;
                    rotateLeft(xp);
                    x = root;
                }
            } else {
                Node* w = xp->left;
                if (!w) break;  // Safety check
                
                if (w->color == RED) {
                    w->color = BLACK;
                    xp->color = RED;
                    rotateRight(xp);
                    w = xp->left;
                    if (!w) break;  // Safety check after rotation
                }
                if ((!w->right || w->right->color == BLACK) && 
                    (!w->left || w->left->color == BLACK)) {
                    w->color = RED;
                    x = xp;
                    xp = x->parent;
                } else {
                    if (!w->left || w->left->color == BLACK) {
                        if (w->right) w->right->color = BLACK;
                        w->color = RED;
                        rotateLeft(w);
                        w = xp->left;
                        if (!w) break;  // Safety check after rotation
                    }
                    w->color = xp->color;
                    xp->color = BLACK;
                    if (w->left) w->left->color = BLACK;
                    rotateRight(xp);
                    x = root;
                }
            }
        }
        if (x) x->color = BLACK;
    }
    
    void deleteNode(Node* z) {
        Node* y = z;
        Node* x, *xp;
        Color y_original_color = y->color;
        
        if (!z->left) {
            x = z->right;
            xp = z->parent;
            transplant(z, z->right);
            // Update sizes from xp to root
            Node* n = xp;
            while (n) {
                updateSize(n);
                n = n->parent;
            }
        } else if (!z->right) {
            x = z->left;
            xp = z->parent;
            transplant(z, z->left);
            // Update sizes from xp to root
            Node* n = xp;
            while (n) {
                updateSize(n);
                n = n->parent;
            }
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            
            if (y->parent == z) {
                xp = y;
                transplant(z, y);
                y->left = z->left;
                y->left->parent = y;
                y->color = z->color;
                // Update sizes from y to root
                Node* n = y;
                while (n) {
                    updateSize(n);
                    n = n->parent;
                }
            } else {
                xp = y->parent;
                transplant(y, y->right);
                // Update sizes from xp to root
                Node* n = xp;
                while (n) {
                    updateSize(n);
                    n = n->parent;
                }
                y->right = z->right;
                y->right->parent = y;
                transplant(z, y);
                y->left = z->left;
                y->left->parent = y;
                y->color = z->color;
                // Update sizes from y to root
                n = y;
                while (n) {
                    updateSize(n);
                    n = n->parent;
                }
            }
        }
        
        delete z;
        
        if (y_original_color == BLACK) {
            fixDelete(x, xp);
        }
    }
    
    Node* copyTree(Node* n, Node* p) {
        if (!n) return nullptr;
        Node* newNode = new Node(n->key, n->color, p);
        newNode->size = n->size;
        newNode->left = copyTree(n->left, newNode);
        newNode->right = copyTree(n->right, newNode);
        return newNode;
    }
    
    void destroyTree(Node* n) {
        if (!n) return;
        destroyTree(n->left);
        destroyTree(n->right);
        delete n;
    }
    
public:
    class iterator {
    private:
        const ESet* tree;
        Node* node;
        
    public:
        iterator(const ESet* t = nullptr, Node* n = nullptr) : tree(t), node(n) {}
        
        const Key& operator*() const { return node->key; }
        const Key* operator->() const { return &node->key; }
        
        iterator& operator++() {
            if (!node) return *this;
            if (node->right) {
                node = tree->minimum(node->right);
            } else {
                Node* p = node->parent;
                Node* current = node;
                while (p && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                node = p;
            }
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator& operator--() {
            if (!node) {
                if (tree->root) node = tree->maximum(tree->root);
                return *this;
            }
            if (node->left) {
                node = tree->maximum(node->left);
            } else {
                Node* p = node->parent;
                Node* current = node;
                while (p && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                if (p) {
                    node = p;
                }
                // If p is nullptr, we're at begin(), don't change node
            }
            return *this;
        }
        
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const {
            return node == other.node;
        }
        
        bool operator!=(const iterator& other) const {
            return node != other.node;
        }
        
        friend class ESet;
    };
    
    ESet() : root(nullptr), tree_size(0) {}
    
    ~ESet() { destroyTree(root); }
    
    ESet(const ESet& other) : root(nullptr), comp(other.comp), tree_size(other.tree_size) {
        root = copyTree(other.root, nullptr);
    }
    
    ESet& operator=(const ESet& other) {
        if (this != &other) {
            destroyTree(root);
            comp = other.comp;
            tree_size = other.tree_size;
            root = copyTree(other.root, nullptr);
        }
        return *this;
    }
    
    ESet(ESet&& other) noexcept : root(other.root), comp(other.comp), tree_size(other.tree_size) {
        other.root = nullptr;
        other.tree_size = 0;
    }
    
    ESet& operator=(ESet&& other) noexcept {
        if (this != &other) {
            destroyTree(root);
            root = other.root;
            comp = other.comp;
            tree_size = other.tree_size;
            other.root = nullptr;
            other.tree_size = 0;
        }
        return *this;
    }
    
    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Key key(std::forward<Args>(args)...);
        
        Node* parent = nullptr;
        Node* current = root;
        
        while (current) {
            parent = current;
            if (comp(key, current->key)) {
                current = current->left;
            } else if (comp(current->key, key)) {
                current = current->right;
            } else {
                return {iterator(this, current), false};
            }
        }
        
        Node* newNode = new Node(key, RED, parent);
        
        if (!parent) {
            root = newNode;
        } else if (comp(key, parent->key)) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }
        
        Node* n = parent;
        while (n) {
            updateSize(n);
            n = n->parent;
        }
        
        tree_size++;
        fixInsert(newNode);
        
        return {iterator(this, newNode), true};
    }
    
    size_t erase(const Key& key) {
        Node* current = root;
        
        while (current) {
            if (comp(key, current->key)) {
                current = current->left;
            } else if (comp(current->key, key)) {
                current = current->right;
            } else {
                deleteNode(current);
                tree_size--;
                return 1;
            }
        }
        
        return 0;
    }
    
    iterator find(const Key& key) const {
        Node* current = root;
        
        while (current) {
            if (comp(key, current->key)) {
                current = current->left;
            } else if (comp(current->key, key)) {
                current = current->right;
            } else {
                return iterator(this, current);
            }
        }
        
        return end();
    }
    
    size_t count_less_than(const Key& key) const {
        Node* current = root;
        size_t count = 0;
        
        while (current) {
            if (comp(current->key, key)) {
                // current->key < key, so all left subtree and current are < key
                count += 1 + getSize(current->left);
                current = current->right;
            } else {
                // current->key >= key, go left
                current = current->left;
            }
        }
        
        return count;
    }
    
    size_t count_less_or_equal(const Key& key) const {
        Node* current = root;
        size_t count = 0;
        
        while (current) {
            if (comp(key, current->key)) {
                // key < current->key, go left
                current = current->left;
            } else {
                // key >= current->key, so all left subtree and current are <= key
                count += 1 + getSize(current->left);
                current = current->right;
            }
        }
        
        return count;
    }
    
    size_t range(const Key& l, const Key& r) const {
        if (comp(r, l)) return 0;
        return count_less_or_equal(r) - count_less_than(l);
    }
    
    size_t size() const noexcept { return tree_size; }
    
    iterator lower_bound(const Key& key) const {
        Node* current = root;
        Node* result = nullptr;
        
        while (current) {
            if (!comp(current->key, key)) {
                result = current;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        return iterator(this, result);
    }
    
    iterator upper_bound(const Key& key) const {
        Node* current = root;
        Node* result = nullptr;
        
        while (current) {
            if (comp(key, current->key)) {
                result = current;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        return iterator(this, result);
    }
    
    iterator begin() const noexcept {
        return root ? iterator(this, minimum(root)) : end();
    }
    
    iterator end() const noexcept {
        return iterator(this, nullptr);
    }
};

#endif
