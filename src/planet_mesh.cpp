#include "planet_mesh.h"

PlanetMesh::PlanetMesh()
{
    GenerateBaseIcosahedron();
}

PlanetMesh::PlanetMesh(int subdivisions)
{
    GenerateBaseIcosahedron();
    Subdivide(subdivisions);
}

#pragma region base sphere generation
// generate basic icosahedron
// https://peter-winslow.medium.com/creating-procedural-planets-in-unity-part-1-df83ecb12e91
void PlanetMesh::GenerateBaseIcosahedron()
{
    // An icosahedron has 12 vertices, and
    // since it's completely symmetrical the
    // formula for calculating them is kind of
    // symmetrical too:

    float t = (1.0f + sqrt(5.0f)) / 2.0f;

    vertices.push_back(normalize(Vector(-1, t, 0)));
    vertices.push_back(normalize(Vector(1, t, 0)));
    vertices.push_back(normalize(Vector(-1, -t, 0)));
    vertices.push_back(normalize(Vector(1, -t, 0)));
    vertices.push_back(normalize(Vector(0, -1, t)));
    vertices.push_back(normalize(Vector(0, 1, t)));
    vertices.push_back(normalize(Vector(0, -1, -t)));
    vertices.push_back(normalize(Vector(0, 1, -t)));
    vertices.push_back(normalize(Vector(t, 0, -1)));
    vertices.push_back(normalize(Vector(t, 0, 1)));
    vertices.push_back(normalize(Vector(-t, 0, -1)));
    vertices.push_back(normalize(Vector(-t, 0, 1)));

    triangles.push_back(Triangle(0, 11, 5));
    triangles.push_back(Triangle(0, 5, 1));
    triangles.push_back(Triangle(0, 1, 7));
    triangles.push_back(Triangle(0, 7, 10));
    triangles.push_back(Triangle(0, 10, 11));
    triangles.push_back(Triangle(1, 5, 9));
    triangles.push_back(Triangle(5, 11, 4));
    triangles.push_back(Triangle(11, 10, 2));
    triangles.push_back(Triangle(10, 7, 6));
    triangles.push_back(Triangle(7, 1, 8));
    triangles.push_back(Triangle(3, 9, 4));
    triangles.push_back(Triangle(3, 4, 2));
    triangles.push_back(Triangle(3, 2, 6));
    triangles.push_back(Triangle(3, 6, 8));
    triangles.push_back(Triangle(3, 8, 9));
    triangles.push_back(Triangle(4, 9, 5));
    triangles.push_back(Triangle(2, 4, 11));
    triangles.push_back(Triangle(6, 2, 10));
    triangles.push_back(Triangle(8, 6, 7));
    triangles.push_back(Triangle(9, 8, 1));
}

void PlanetMesh::Subdivide(int subdivisions)
{
    std::map<int, int> midPointCache;
    for (int i = 0; i < subdivisions; i++)
    {
        std::vector<Triangle> newTriangles;
        for (Triangle triangle : triangles)
        {
            int a = triangle.triangle[0];
            int b = triangle.triangle[1];
            int c = triangle.triangle[2];

            int ab = GetMidpointIndex(midPointCache, a, b);
            int bc = GetMidpointIndex(midPointCache, b, c);
            int ca = GetMidpointIndex(midPointCache, c, a);

            newTriangles.push_back(Triangle(a, ab, ca));
            newTriangles.push_back(Triangle(b, bc, ab));
            newTriangles.push_back(Triangle(c, ca, bc));
            newTriangles.push_back(Triangle(ab, bc, ca));
        }

        triangles = newTriangles;
    }
}

int PlanetMesh::GetMidpointIndex(std::map<int, int> cache, int a, int b)
{
    int smallerIndex = std::min(a, b);
    int greaterIndex = std::max(a, b);
    int key = (smallerIndex << 16) + greaterIndex;

    int ret;
    if (cache.find(key) != cache.end())
        return cache.at(key);

    // if the index has not been stored, compute the midpoint
    Vector midPoint = normalize((vertices[a] + vertices[b]) / 2);
    vertices.push_back(midPoint);
    ret = vertices.size() - 1;

    cache.emplace(key, ret);
    return ret;
}
#pragma endregion

#pragma region edit the sphere with noise
void PlanetMesh::generateNoise() {
    FastNoiseLite noise;

    noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
}
#pragma endregion

/**
 * computes the normal of a face
 */
void PlanetMesh::FaceNormal(Vector &normal, int a, int b, int c) {
    Vector AB, AC;

    AB = vertices[b] - vertices[a];

    AC = vertices[c] - vertices[a];

    normal = cross(AB, AC);
}

void PlanetMesh::UpdateNormals() {
    
    Vector normal;

    // initialize the normals
    normals.resize(vertices.size());
    for(int i = 0; i < vertices.size(); i++) {
        normals[i] = Vector(0, 0, 0);
    }

    //compute the normal of each face
    for(Triangle t : triangles) {
        int a = t[0];
        int b = t[1];
        int c = t[2];

        FaceNormal(normal, a, b, c);

        // update the normal of each vertex
        normals[a] = normals[a] + normal;
        normals[b] = normals[b] + normal;
        normals[c] = normals[c] + normal;
    }

    //normalize all the values
    for(int i = 0; i < normals.size(); i++) {
        normals[i] = normalize(normals[i]);
    }
}

Mesh PlanetMesh::GenerateMesh() {
    Mesh planet = Mesh(GL_TRIANGLES);

    UpdateNormals();

    for(int i = 0; i < vertices.size(); i++) {
        planet.vertex(vertices[i]);
        planet.normal(normals[i]);
    }

    for(Triangle tri : triangles) {
        planet.triangle(tri[0], tri[1], tri[2]);
    }

    return planet;
}