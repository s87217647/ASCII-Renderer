#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

struct Vec3 {
    float x, y, z;
};

struct Surface {
    int v1, v2, v3;
};

int main(){
    // parsing files - getting vertex and surface
    ifstream file("pyramid.obj");
    string line;

    vector<Vec3> vertices;
    vector<Surface> surfaces;
    
    vertices.push_back(Vec3{0.0f, 0.0f, 0.0f}); // Sentinal 0 idx
     
    
    while (getline(file, line)) {
        stringstream ss(line);
        string prefix;
        ss >> prefix;
        if (prefix == "v") {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (prefix == "f") {
            Surface surface;
            ss >> surface.v1 >> surface.v2 >> surface.v3;
            surfaces.push_back(surface);
        }    
    }

    

    // Guess this is my world now
    cout << "This is the end, hold your breath and count to ten" << endl;
    return 0;

    
}