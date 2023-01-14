//
// Created by pi on 12/29/22.
//

#include <iostream>
#include <algorithm>

/**
 * --- First approach: implement merge sort by hand ----
 * Comment: speed is O(log_n) on average
 *
 * Implementation:
 * The sorting strategy is as follows having two Boxes B1(p1(x1_min,y1_min), p2(x1_max, y1_max)) and
 * B2(p1(x2_min,y2_min), p2(x2_max,y2_max)) we compare their most right column (since they want us to sort by columns).
 * If x1_min < x2_min, we pick the first box or if (x1_min == x2_min && y1_min > y2_min) we still pick the first box
 * otherwise we pick the second box.
 *
 *
 * --- Second approach, Intra-sort ----
 * Comment: Intra-sort used by C++ STD lib is more stable, avg speed is still about O(log_n)
 * Implementation: define a (<) operator for box and just give it to std::sort() function.
 * **/

struct Box {
    int xmin,xmax,ymin,ymax, id;
};

void merge(Box * a, const Box * b, int start, int mid, int finish);
void sort(Box * a, Box * b, int start, int finish);

void mergeSort(Box * a, int n){
    Box b[n];
    for (int i=0;i<n;++i) b[i]=a[i];
    sort(a,b,0,n);
}

/***
 * @param a to where write the merged result
 * @param b from where to merge
 * @param start start
 * @param finish finish
 ***/
void sort(Box * a, Box * b, int start, int finish){
    if (finish-start <= 1) return;
    int mid = (start+finish)/2;
    sort(b,a,start,mid); // makes 1st half of "b" sorted
    sort(b,a,mid,finish); // makes 2nd half of "b" sorted
    merge(a,b,start,mid,finish); // now makes "a" merged
}

void merge(Box * a, const Box * b, int start, int mid, int finish){
    int leftIt=start, rightIt=mid;
    for (int i=start;i<finish;++i){
        if ( (leftIt < mid) && (
                rightIt>=finish ||
                (
                    b[leftIt].xmin < b[rightIt].xmin ||
                    (b[leftIt].xmin == b[rightIt].xmin && b[leftIt].ymin > b[rightIt].ymin)
                )
            ))
        {
            (a[i] = b[leftIt], ++leftIt);
        }
        else (a[i] = b[rightIt], ++rightIt);
    }
}

void print(Box * p, int n){
    for (int i = 0; i < n; ++i){
        std::cout << p[i].xmin << " " << p[i].xmax << " " << p[i].ymin << " " << p[i].ymax << std::endl;
    }
}

void printId(Box * p, int n){
    for (int i = 0; i < n; ++i){
        std::cout << p[i].id << " ";
    }
    std::cout << '\n';
}

int main(){
    // --- approach 1 ----
    std::cout << "First approach test cases...\n";
    // ---- test1 ----
    std::cout << "First test case...  ";
    Box box1{3, 5, 2, 3, 1};
    Box box2{2, 4, 3, 4, 2};
    Box a1_2[] = {box1, box2};
    Box * a1_1 = a1_2;

    mergeSort(a1_1, 2);
    printId(a1_1, 2);
    std::cout << "Expected: " << "2 1 \n";

    // ---- test2 ----
    std::cout << "Second test case...  ";
    Box a2_2[] = { {2,2,6,4,1}, {3,6,7,7,2},
                 {1,8,5,9,3}, {7,10,12,12,4},
                 {8,8,13,10,5}, {9,5,4,7,6}
    };
    Box * a2_1 = a2_2; // copy for the second approach
    mergeSort(a2_1, 6);
    printId(a2_1,6);
    std::cout << "Expected: " << "3 1 2 4 5 6 \n";


    // ---- approach 2 ----
    std::cout << "\n\nSecond approach test cases...\n";
    auto comparator = [](const Box & rhs, const Box & lhs){
            if (rhs.xmin < lhs.xmin ||
                (rhs.xmin == lhs.xmin && rhs.ymin > lhs.ymin) ) return true;
            else return false;
        };

    // ---- test1 ----
    std::cout << "First test case... ";
    std::sort(std::begin(a1_2), std::end(a1_2), comparator);
    printId(a1_2,2);
    std::cout << "Expected: " << "2 1 \n";


    // ---- test2 ----
    std::cout << "Second test case...  ";
    std::sort(std::begin(a2_2), std::end(a2_2), comparator);
    printId(a2_1, 6);
    std::cout << "Expected: " << "3 1 2 4 5 6 \n";
}
