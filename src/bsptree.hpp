#ifndef BSP_TREE_HPP
#define BSP_TREE_HPP

#include <vector>

using namespace std;
typedef vector<double> Point;

struct HRect
{
    vector<double> mins;
    vector<double> maxs;
    HRect(Point p1, Point p2, int dim);
};

class BSPTree
{
private:
    int dim;
    int leaf_size;
    BSPTree *first;
    BSPTree *second;
    Point pivot;
    vector<Point> points;
    void split();

public:
    BSPTree(int n_dim,  int n_leaf, vector<Point> ps);
    ~BSPTree();
    void insert(Point p);
    vector<Point> range_query(HRect& rect);
    Point point_query(Point& p);
};
#endif