#include "objloader.h"

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

int resolveIndex(const std::string& token, int count) {
    if (token.empty()) {
        return -1;
    }
    try {
        const int index = std::stoi(token);
        if (index > 0) {
            return index - 1;
        }
        if (index < 0) {
            return count + index;
        }
    } catch (const std::exception&) {
    }
    return -1;
}

// Parses "v", "v/vt", "v//vn" and "v/vt/vn".
std::array<int, 3> parseFaceVertex(const std::string& token, int vertexCount, int texcoordCount,
                                   int normalCount) {
    std::array<std::string, 3> parts;
    size_t part = 0;
    for (char c : token) {
        if (c == '/') {
            if (++part >= parts.size()) {
                break;
            }
        } else {
            parts[part] += c;
        }
    }

    return {resolveIndex(parts[0], vertexCount), resolveIndex(parts[1], texcoordCount),
            resolveIndex(parts[2], normalCount)};
}

}  // namespace

bool loadOBJ(const std::string& path, std::vector<glm::vec3>& vertices, std::vector<Face>& faces) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: could not open OBJ file '" << path << "'\n";
        return false;
    }

    vertices.clear();
    faces.clear();

    int texcoordCount = 0;
    int normalCount = 0;
    size_t skippedFaces = 0;
    std::string lineText;

    while (std::getline(file, lineText)) {
        if (!lineText.empty() && lineText.back() == '\r') {
            lineText.pop_back();
        }

        std::istringstream stream(lineText);
        std::string type;
        if (!(stream >> type)) {
            continue;
        }

        if (type == "v") {
            glm::vec3 vertex(0.0f);
            if (stream >> vertex.x >> vertex.y >> vertex.z) {
                vertices.push_back(vertex);
            }
        } else if (type == "vt") {
            ++texcoordCount;
        } else if (type == "vn") {
            ++normalCount;
        } else if (type == "f") {
            const int vertexCount = static_cast<int>(vertices.size());
            std::vector<std::array<int, 3>> polygon;
            std::string token;
            bool valid = true;

            while (stream >> token) {
                const std::array<int, 3> indices =
                    parseFaceVertex(token, vertexCount, texcoordCount, normalCount);
                if (indices[0] < 0 || indices[0] >= vertexCount) {
                    valid = false;
                    break;
                }
                polygon.push_back(indices);
            }

            if (!valid || polygon.size() < 3) {
                ++skippedFaces;
                continue;
            }

            for (size_t i = 1; i + 1 < polygon.size(); ++i) {
                faces.push_back(Face{{polygon[0], polygon[i], polygon[i + 1]}});
            }
        }
    }

    std::cout << "Loaded '" << path << "': " << vertices.size() << " vertices, " << faces.size()
              << " faces\n";
    if (skippedFaces > 0) {
        std::cerr << "Warning: skipped " << skippedFaces << " invalid faces\n";
    }

    return !vertices.empty() && !faces.empty();
}
