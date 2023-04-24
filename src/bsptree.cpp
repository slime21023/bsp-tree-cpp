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
    first = nullptr;
    second = nullptr;

    if(points.size() > leaf_size)
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
    first->pivot = fp;
    second->pivot = sp;
}

void BSPTree::insert(Point p)
{
    bool is_leaf = (first == nullptr) && (second == nullptr);
    if(is_leaf)
    {
        points.push_back(p);
    }
    else
    {
        double fd, sd;
        fd = points_distance(p, first->pivot, dim);
        sd = points_distance(p, second->pivot, dim);

        if(fd < sd)
        {
            first->insert(p);
        } 
        else
        {
            second->insert(p);
        }
    }

    if (is_leaf && points.size() > leaf_size)
    {
        split();
    }
}

vector<Point> BSPTree::range_query(HRect& rect)
{
    bool is_leaf = (first == nullptr) && (second == nullptr);
    vector<Point> ps;

    auto close_first = [this] (Point p) -> bool {
        double fd, sd;
        fd = points_distance(p, first->pivot, dim);
        sd = points_distance(p, second->pivot, dim);
        return (fd < sd);
    };

    if (!is_leaf)
    {
        Point min_point, max_point;
        for (int i = 0; i < dim; i++)
        {
            min_point.push_back(rect.mins[i]);
            max_point.push_back(rect.maxs[i]);
        }

        bool min_close_first = close_first(min_point);
        bool max_close_first = close_first(max_point);

        if (min_close_first == true && max_close_first == true)
        {
            vector<Point> fpoints = first->range_query(rect);
            ps.insert(ps.end(), fpoints.begin(), fpoints.end());
        }
        else if (min_close_first == false && max_close_first == false)
        {
            vector<Point> spoints = second->range_query(rect);
            ps.insert(ps.end(), spoints.begin(), spoints.end());
        }
        else
        {
            vector<Point> fpoints = first->range_query(rect);
            vector<Point> spoints = second->range_query(rect);
            ps.insert(ps.end(), fpoints.begin(), fpoints.end());
            ps.insert(ps.end(), spoints.begin(), spoints.end());
        }
    }
    else
    {
        for (auto p : points) {
            if (is_include(rect, p)) {
                ps.push_back(p);
            }
        }
    }

    return ps;
}

Point BSPTree::point_query(Point& p)
{
    bool is_leaf = (first == nullptr) && (second == nullptr);
    Point result;

    auto close_first = [this] (Point p) -> bool {
        double fd, sd;
        fd = points_distance(p, first->pivot, dim);
        sd = points_distance(p, second->pivot, dim);
        return (fd < sd);
    };

    if (!is_leaf)
    {
        bool is_close_first = close_first(p);
        if (is_close_first)
        {
            result = first->point_query(p);
        }
        else 
        {
            result = second->point_query(p);
        }
    }
    else
    {
        for (auto op : points)
        {
            if(is_equal(p, op))
                return op;
        }
    }

    return result;
}