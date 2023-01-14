//
// Created by pi on 12/28/22.
//

#include <iostream>

struct Box {
    int xmin,xmax,ymin,ymax;
};

// merge sort implementation
void merge(Box * a, int begin, int mid, int end, Box * b){
    int i = begin, j = mid; // use two "pointers" in array
    for (int k = begin; k < end; ++k){ // in merge sort we need to iterate over each element
        if ( i < mid && (j >= end || (a[i].xmin < a[j].xmin || (a[i].xmin == a[j].xmin && a[i].ymin > a[j].ymin))) ){     // we could exhaust all elements from right halve, so we need
                                                                                                                          // to check it and if so, we simply put the remaining elements
            b[k] = a[i];
            ++i;
        }
        else{
            b[k] = a[j];
            ++j;
        }
    }
}

void splitMerge(Box * b, int begin, int end, Box * a){
    if (end - begin <= 1) return;
    int mid = (begin + end) / 2;
    splitMerge(a, begin, mid, b); // sorts left half of A. the sorted result is in A
    splitMerge(a, mid, end, b); // sorts right half of A. the sorted result is in A
    merge(b, begin, mid, end, a); // merges two sorted halves, the merged result is in B
}

void print(Box * p, int n){
    for (int i = 0; i < n; ++i){
        std::cout << p[i].xmin << " " << p[i].xmax << " " << p[i].ymin << " " << p[i].ymax << std::endl;
    }
}

void mergeSort(Box * a, Box * b, int n){
    for (int i=0; i<n; ++i){
        b[i] = a[i];
    }
    splitMerge(b, 0, n, a); // sort from b to a. sorted elements are stored in a.
}


int main(){
    // ---- test1 ----
    Box box1{3, 5, 2, 3};
    Box box2{2, 4, 3, 4};
    Box a[] = {box1, box2};
    Box b[2];

    mergeSort(a, b, 2);

    print(a, 2);

    // TODO: dive deeper into the understanding of merge sort: pay attention to the details about indexed implementation. Write one more time this algorithm try by yourself as much as you can.
    // TODO: write more the test cases.


}

