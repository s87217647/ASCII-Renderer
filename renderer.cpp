#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    // Operator Overloading for Vector Math
    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(float s) const       { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const       { return {x / s, y / s, z / s}; }

    void print() const{
        printf("x: %f, y : %f, z: %f \n", x, y ,z);
    }

    // Magnitude / Length
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalization (Unit Vector)
    Vec3 normalized() const {
        float len = length();
        return (len > 0.0f) ? *this / len : Vec3{0, 0, 0};
    }
};

// --- Vector Operations ---

// Dot Product: a · b
inline float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Cross Product: a × b
inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}


struct Surface {
    int v1, v2, v3;
};

#define resolutionX 45
#define resolutionY 15
void printScreen(float screenBrightness[resolutionY][resolutionX]){
    // find max brightness, divide it to a unit, find brighness of each spot

    float maxBright = 0.0f;
    for(int i = 0; i < resolutionY; i ++){
        for(int j = 0; j < resolutionX; j ++){
            maxBright = max(maxBright, screenBrightness[i][j]);
        }
    }


    char screen[resolutionY][resolutionX + 1];
    float brightnessUnit = maxBright / 9;
    int brightness;

    string brightnessLevel = " .:-=+*#%@";

    for(int i = 0; i < resolutionY; i ++){
        for(int j = 0; j <= resolutionX; j ++){
            if (j == resolutionX){
                screen[i][j] = '\0';
                break;
            }

            brightness = screenBrightness[i][j] / brightnessUnit;
            
     
            screen[i][j] = brightnessLevel[brightness];
        }
    }

    for(int i = 0; i < resolutionY; i ++){
        cout << screen[i] << endl;
    }

}


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

    //Plane coordinates setup

    Vec3 camera = Vec3{5.0f, 5.0f, 5.0f};
    Vec3 origin = Vec3{0.0f, 0.0f, 0.0f};

    Vec3 viewDir = (origin - camera).normalized();

    float viewDistance = 2.5;
    Vec3 planeCenter = camera + (viewDir * viewDistance);
    
    Vec3 planeHorizontal = cross(viewDir, Vec3{0.0f, 0.0f, 1.0f}).normalized();
    Vec3 planeVertical = cross(planeHorizontal, viewDir).normalized();


    // making the plane
    Vec3 planeStartPt = planeCenter + planeHorizontal * 3 + planeVertical * 2;

    Vec3 xIncreamental = planeHorizontal * -6 / resolutionX; // double so planeCetner will be in the middle
    Vec3 yIncreamental = planeVertical * -4 / resolutionY;


    Vec3 pixelLocation[resolutionY][resolutionX];

    pixelLocation[0][0] = planeStartPt;

    for(int i = 1; i < resolutionY; i++){
        pixelLocation[i][0] = pixelLocation[i - 1][0] + yIncreamental;
    }

    for (int i = 0; i < resolutionY; i++){
        for(int j = 1; j < resolutionX; j++){
            pixelLocation[i][j] = pixelLocation[i][j - 1] + xIncreamental;
        }
    }


    //initailize the screen
    char screen[resolutionY][resolutionX + 1];
    for (int i = 0; i < resolutionY; i++){
        for(int j = 0; j < resolutionX; j++){
            screen[i][j] = '.';
        }
        screen[i][resolutionX] = '\0';
    }

    // view ray: camera pt -> pixel see if it hits the obj

    // ray casting loop

    for(auto & row : pixelLocation){
        for(Vec3 &pixel : row){
            Vec3 rayDir = (pixel - vPt).normalized();

            for(Surface &s : surfaces ){
                Vec3 v1 = vertices[s.v1], v2 = vertices[s.v2], v3 = vertices[s.v3];
                Vec3 surfaceNorm = cross(v2 - v1, v3 - v2);
                
            }
        }
    }




    //print screen
    // for (int i = 0; i < resolutionY; i++){

    //     cout << i << ": ";
    //     for(float cell : screenBrightness[i]){
    //         cout << cell << " ";
    //     }
    //     cout << endl;
    // }

    // for (int i = 0; i < resolutionY; i++){
    //     cout << i << ": " << screen[i] << endl;
    // }




    // ray: viewPt -> pixel, if it cross the surface
    // experiment, imagine we have a sphere, instead of .obj file
    
    


    
    // Vec3 currentPixel = planeStartPt;

    // for (int i = 0; i <= resolutionX; i++){
    //     for(int j = 0; j <= resolutionY; j ++){
            
    //     }
    // }
    






    // Guess this is my world now
    cout << "This is the end, hold your breath count to ten" << endl;
    return 0;
    
}