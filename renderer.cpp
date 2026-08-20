#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <chrono>
#include <thread>


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

inline Vec3 rotate(Vec3 v, float angleX, float angleY, float angleZ){
    float x1 = v.x;
    float y1 = v.y * cos(angleX) - v.z * sin(angleX);
    float z1 = v.y * sin(angleX) + v.z * cos(angleX);

    // 2. Rotate around Y-axis (Yaw)
    float x2 = x1 * cos(angleY) + z1 * sin(angleY);
    float y2 = y1;
    float z2 = -x1 * sin(angleY) + z1 * cos(angleY);

    // 3. Rotate around Z-axis (Roll)
    float x3 = x2 * cos(angleZ) - y2 * sin(angleZ);
    float y3 = x2 * sin(angleZ) + y2 * cos(angleZ);
    float z3 = z2;

    return {x3, y3, z3};
}


struct Surface {
    int v1, v2, v3;
};

#define resolutionX 120
#define resolutionY 40


void printScreen(float screenBrightness[resolutionY][resolutionX]){
    float maxBright = 0.0f;
    for(int i = 0; i < resolutionY; i ++){
        for(int j = 0; j < resolutionX; j ++){
            maxBright = max(maxBright, screenBrightness[i][j]);
        }
    }
    
    int brightness;
    string brightnessLevel = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    float brightnessUnit = maxBright / (brightnessLevel.length() - 1);
    
    
    char screen[resolutionY][resolutionX + 1];
    for(int i = 0; i < resolutionY; i ++){
        screen[i][resolutionX] = '\n';
    }
    
    
    for(int i = 0; i < resolutionY; i ++){
        for(int j = 0; j < resolutionX; j ++){
            brightness = screenBrightness[i][j] / brightnessUnit;
            screen[i][j] = brightnessLevel[brightness];
        }
    }
    
    string frame = "\033[?25l\033[2J\033[3J\033[H";

    for(int i = 0; i < resolutionY; i ++){
        frame.append(screen[i], resolutionX + 1);
    }
    
    cout.write(frame.c_str(), frame.length());
    cout.flush();
}


int main(){
    // parsing files - getting vertex and surface
    ifstream file("torus.obj");
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

    Vec3 camera = Vec3{0.0f, 5.0f, 0.0f} * 2;
    float viewDistance = 40.0;
    
    
    
    // Set up view plane
    Vec3 viewDir = camera.normalized() * -1;
    Vec3 planeCenter = camera + (viewDir * viewDistance);
    
    Vec3 planeHorizontal = cross(viewDir, Vec3{0.0f, 0.0f, 1.0f}).normalized();
    Vec3 planeVertical = cross(planeHorizontal, viewDir).normalized();
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


    const float epsilon = 1e-5f;

    float screenBrightness[resolutionY][resolutionX];
    


    int FPS = 30;
    const auto frameTime = chrono::milliseconds(1000 / FPS);
    

    // Rotate before the main loop
    // for(Vec3 & v : vertices){
    //     v = rotate(v, 0.0, (M_PI / 180) * 90, 0.0);
    // }

    //main loop
    while(true){
        auto start = chrono::high_resolution_clock::now();
        
        for(auto & row : screenBrightness){
            for (auto & cell : row){ 
                cell = 0.0f;
            }
        }

        for(Vec3 & v : vertices){
            v = rotate(v, (M_PI / 180) * 8, (M_PI / 180) * 5, (M_PI / 180) * 3);
        }


        for (int i = 0; i < resolutionY; i++){
            for(int j = 0; j < resolutionX; j++){
                Vec3 rayDir = (pixelLocation[i][j] - camera).normalized();
                // ray = camera + rayDir * t 
                for(Surface &s : surfaces ){
                    Vec3 v1 = vertices[s.v1], v2 = vertices[s.v2], v3 = vertices[s.v3];
                    Vec3 surfaceNorm = cross(v3 - v1, v2 - v1).normalized();
                    // render if ray hits the right side, within the triangle

                    if (abs(dot(surfaceNorm, rayDir)) < epsilon || dot(rayDir, surfaceNorm) > 0){ // parallel, no intersection 
                        continue; // skip rendering if casted ray is parallel to the plane or on the wrong side.
                    }

                    float t = dot((v1 - camera), surfaceNorm) / dot(rayDir, surfaceNorm);
                    Vec3 hit = camera + rayDir * t;

                    // check if the hit is within the triangle
                    Vec3 c1 = cross((v2 - v1),hit - v1), c2 = cross((v3 - v2), hit - v2), c3 = cross((v1 - v3), hit - v3);

                    if (dot(c1, surfaceNorm) < 0 and dot(c2, surfaceNorm) < 0 and dot(c3, surfaceNorm) < 0){
                        screenBrightness[i][j] += abs(dot(rayDir, surfaceNorm));
                    }
                }
            }
    }
        printScreen(screenBrightness);

        auto elapsed = chrono::high_resolution_clock::now() - start;
        
        if(elapsed < frameTime){
            this_thread::sleep_for(frameTime - elapsed);
        }
    }



    cout << "This is the end, hold your breath count to ten?" << endl;
    return 0;
    
}