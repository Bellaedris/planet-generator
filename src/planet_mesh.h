#include "mesh.h"
#include "FastNoiseLite.h"

#include <math.h>
#include <map>

struct Triangle
{
    std::vector<int> triangle;

    Triangle(int a, int b, int c)
    {
        triangle = std::vector<int>{a, b, c};
    }

    int operator[](int i) {
        return triangle[i];
    }
};

class PlanetMesh
{
private:
    void GenerateBaseIcosahedron();
    void Subdivide(int iterations);
    int GetMidpointIndex(std::map<int, int> cache, int a, int b);

    void generateNoise();

    std::vector<Vector> vertices;
    std::vector<Triangle> triangles;

public:
    PlanetMesh();
    PlanetMesh(int subdivisions);
    Mesh GenerateMesh();
};