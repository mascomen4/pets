#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

// ---- Task 1 ----

/**
 * Using the signed bit representation.
 * @param
 * @return
 */
string signed_bit_int2binary(int n){
    if (abs(n) > 65535){
        cerr << "Sorry, the number is too big" << endl;
    }

    int power, cmp;
    string res(17, '0'); // 16 bits, and one for the sign

    if (n > 0) res[0] = '1'; // otherwise it's already 0
    n = abs(n);

    while (n > 0){
        cmp = 1; power = 0;
        while (cmp*2 <= n){
            cmp *= 2;
            ++power;
        }
        res[16 - power] = '1';
        n -= cmp;
    }
    return res;
}

// ---- Task 2 ----

/**
 * Constraints: complexity should be less than O(n^2);
 */


struct ListNode{
    ListNode * prev{};
    ListNode * next{};
    ListNode * rand{};
    string data;
};

void print_list(ListNode *head){
    ListNode* curr = head;
    while (curr){
        std::cout << curr->data << " ";
        curr = curr->next;
    }
    std::cout << endl;
}

class List{
public:
    List (ListNode * head, ListNode * tail, int count): head(head), tail(tail), count(count) {}

    void print(){
        print_list(head);
    }

    void Serialize (FILE * file){
        std::fwrite(&count, sizeof(int), 1, file);
        ListNode* curr = head;
        while (curr){
            int str_size = static_cast<int>(curr->data.size()) + 1;
            std::fwrite(&str_size, sizeof(int), 1, file);
            char buf[str_size];
            auto cstr = curr->data.c_str();
            for (int i = 0; i < str_size; ++i) buf[i] = cstr[i];
            std::fwrite(buf, sizeof(char), str_size, file);
            curr = curr->next;
        }
    };

    void Deserialize(FILE * file){
        head = new ListNode;
        tail = nullptr;
        ListNode *curr = head, *dummy = new ListNode{}, *prev = dummy;
        std::fread(&count, sizeof(int), 1, file);
        for (int i = 0; i < count; ++i){
            int str_size;
            std::fread(&str_size, sizeof(int), 1, file);
            char buf[str_size];
            std::fread(buf, sizeof(char), str_size, file);
            string data(buf);

            curr->data = data;
            curr->prev = prev;
            prev->next = curr;
            tail = curr;

            curr = new ListNode{};
            prev = prev->next;
        }
        delete dummy;
    };

private:
    ListNode * head;
    ListNode * tail;
    int count;
};

// ---- Task 3 ----

struct vec3{
    double x{};
    double y{};
    double z{};
};

/**
 * The normal of two vectors is their cross-product. Two vectors are defined from p1,p2,p3
 * @param p1
 * @param p2
 * @param p3
 * @return
 */
vec3 calc_normal(vec3 p1, vec3 p2, vec3 p3){
    vec3 v1{p2.x - p1.x, p2.y - p1.y, p2.z - p1.z}, v2{p3.x - p1.x, p3.y - p1.y, p3.z - p1.z}; // POD class
    vec3 normal{
        -v2.y * v1.z + v2.z * v1.y,
        v2.x * v1.z - v2.z * v1.x,
        -v2.x * v1.y + v2.y * v1.x
    };
    return normal;
}

void calc_mesh_normals(vec3* normals, const vec3* verts, const int* faces, int nverts, int nfaces){
    for (int i = 0; i < nfaces; ++i){
        vec3 normal = calc_normal( verts[faces[i*3]], verts[faces[i*3+1]], verts[faces[i*3+2]] );
//        std::cout << "normal: " << normal.x << " " << normal.y << " " << normal.z << endl;

        normals[faces[i*3]].x += normal.x;
        normals[faces[i*3]].y += normal.y;
        normals[faces[i*3]].z += normal.z;

        normals[faces[i*3+1]].x += normal.x;
        normals[faces[i*3+1]].y += normal.y;
        normals[faces[i*3+1]].z += normal.z;

        normals[faces[i*3+2]].x += normal.x;
        normals[faces[i*3+2]].y += normal.y;
        normals[faces[i*3+2]].z += normal.z;
    }

    for (int i = 0; i < nverts; ++i){
        double len = sqrt(normals[i].x*normals[i].x + normals[i].y*normals[i].y + normals[i].z*normals[i].z);
        normals[i].x /= len;
        normals[i].y /= len;
        normals[i].z /= len;
    }
}


int main() {
    cout << "Test for 1 task" << endl;
    string binary = signed_bit_int2binary(7);
    cout << binary << endl;

    cout << "\nTest for 2 task" << endl;
    ListNode * head = new ListNode, *tail;
    ListNode * curr = head, *prev = new ListNode;
    int count = 10;
    for (int i = 0; i < count; ++i){
        curr->data = string(i+1, 'a');
        curr->prev = prev;
        prev->next = curr;

        curr = new ListNode;
        tail = curr;
        prev = prev->next;
    }

    List l(head, tail, count);
    auto fw = fopen("file", "wb");
    l.Serialize(fw);
    fclose(fw);
    l.print();

    auto fr = fopen("file", "rb");
    l.Deserialize(fr);
    fclose(fr);
    l.print();

    cout << "\nTest for 3 task" << endl;

    vec3 verts[4];
    verts[0].x = 2.;
    verts[0].y = 0.;
    verts[0].z = 0.;

    verts[1].x = 0.;
    verts[1].y = 3.;
    verts[1].z = 0.;

    verts[2].x = -2.;
    verts[2].y = 0.;
    verts[2].z = 0.;

    verts[3].x = 0.;
    verts[3].y = 0.;
    verts[3].z = 5.;

    vec3 normals[4];
    int faces[4*3] = { 0, 1, 2, 0, 1, 3, 1, 2, 3, 0, 2, 3  };
    calc_mesh_normals(normals, verts,faces, 4, 4);
    cout << "Calculated normals: \n" <<
          "(" << normals[0].x << " " << normals[0].y << " " << normals[0].z << ")" <<endl <<
          "(" << normals[1].x << " " << normals[1].y << " " << normals[1].z << ")" <<endl <<
          "(" << normals[2].x << " " << normals[2].y << " " << normals[2].z << ")" <<endl <<
          "(" << normals[3].x << " " << normals[3].y << " " << normals[3].z << ")"<<endl;

    return 0;
}
