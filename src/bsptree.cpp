#include "./bsptree.hpp"
#include <cmath>

double points_distance(Point& p1, Point& p2, int dim)
{
    double result = 0;
    for(int i = 0; i < dim; i++)
    {
        result += pow(p1[i] - p2[i], 2);
    }
    return sqrt(result);
}

HRect::HRect(Point p1, Point p2, int dim)
{
    vector<double> new_mins(dim, 0);
    vector<double> new_maxs(dim, 0);

    for (int i = 0; i < dim; i++)
    {
        bool is_large = p1[i] > p2[i];
        new_mins[i] = is_large ? p2[i] : p1[i];
        new_maxs[i] = is_large ? p1[i] : p2[i];
    }

    mins = new_mins;
    maxs = new_maxs;
}

bool interact(HRect &r1, HRect &r2)
{
    bool result = true;
    int dim = r1.mins.size();
    for (int i = 0; i < dim; i++)
    {
        bool c1 = r2.mins[i] >= r1.mins[i] && r2.mins[i] < r1.maxs[i];
        bool c2 = r2.maxs[i] >= r1.mins[i] && r2.maxs[i] < r1.maxs[i];

        if (!(c1 || c2))
        {
            result = false;
            break;
        }
    }
    return result;
}

bool is_include(const HRect &r, Point p)
{
    bool result = true;
    int dim = r.mins.size();
    for (int i = 0; i < dim; i++)
    {
        if (!(p[i] >= r.mins[i] && p[i] <= r.maxs[i]))
        {
            result = false;
            break;
        }
    }
    return result;
}

bool is_equal(Point& p1, Point& p2)
{
    bool result = true;
    int dim = p1.size();
    for (int i = 0; i < dim; i++)
    {
        if(p1[i] != p2[i])
        {
            result = false;
            break;
        }
    }
    return result;
}

BSPTree::BSPTree(int n_dim, int n_leaf, vector<Point> ps)
{
    dim = n_dim;
    leaf_size = n_leaf;
    points = ps;

    if(points.size() >= leaf_size)
    {
        split();
    }
}

BSPTree::~BSPTree()
{
    delete first;
    delete second;
}

void BSPTree::split()
{
    Point temp = Point(dim, 0);
    for(auto p: points) 
    {
        for(int i = 0; i < dim; i++) 
        {
            temp[i] += p[i];
        }
    }

    int point_num = points.size();
    for(int i = 0; i < dim; i++) 
    {
        temp[i] /= point_num;
    }

    pivot = temp;
    Point fp, sp;

    double temp_distance = 0;
    for (auto p: points)
    {
        double d = points_distance(pivot, p, dim);
        if (d > temp_distance)
        {
            temp_distance = d;
            fp = p;
        }
    }

    double temp_distance = 0;
    for (auto p: points)
    {
        double d = points_distance(fp, p, dim);
        if (d > temp_distance)
        {
            temp_distance = d;
            sp = p;
        }
    }
    
    vector<Point> fps, sps;
    for (auto p: points)
    {
        double fd, sd;
        fd = points_distance(fp, p, dim);
        sd = points_distance(sp, p, dim);
        if (fd < sd) 
        {
            fps.push_back(p);
        }
        else
        {
            sps.push_back(p);
        }
    }

    fps.push_back(fp);
    sps.push_back(sp);

    first = new BSPTree(dim, leaf_size, fps);
    second = new BSPTree(dim, leaf_size, sps);
}

void BSPTree::insert(Point p)
{
    
}