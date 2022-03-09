/*
+------------------------------------------------------------------------------+
|                                                                              |
|                                                                              |
|                                                                              |
|                                                                              |
|  _____________ ______  ___  ____   ___  ___  ___  ___    __         ___  ___ |
| / ___/ __/ __ <  / _ \/ _ \/ / /  |_  |/ _ \|_  ||_  |  / / _    __/ _ \|_  ||
|/ (_ / _// /_/ / / // / // /_  _/ / __// // / __// __/_ / _ \ |/|/ / // / __/ |
|\___/___/\____/_/\___/\___/ /_/(_)____/\___/____/____(_)_//_/__,__/\___/____/ |
|                                                                              |
|                                  __       __                                 |
|                                 / /  ___ / /__                               |
|                                / _ \/ -_) / _ \                              |
|                               /_//_/\__/_/ .__/                              |
|                                         /_/                                  |
|                                                                              |
|                                                                              |
|                                                                              |
|                                                                              |
|                                                                              |
+------------------------------------------------------------------------------+
+------------------------------------------------------------------------------+
|                                                                              |
|                                 Hugo Ledoux                                  |
|                             h.ledoux@tudelft.nl                              |
|                                  2022-02-22                                  |
|                                                                              |
+------------------------------------------------------------------------------+
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"
#include "calculation.hpp"

using json = nlohmann::json;


int   get_no_roof_surfaces(json& j);
void  list_all_vertices(json& j);
// void  Hugo_visit_roofsurfaces(json& j);

void visit_roofsurfaces(json& j);


int main(int argc, const char* argv[]) {
    //-- reading the file with nlohmann json: https://github.com/nlohmann/json  
    std::string filename = "/cube.json";
    std::ifstream input(DATA_PATH + filename);
    json j;
    input >> j;
    input.close();

    //-- get total number of RoofSurface in the file
    int noroofsurfaces = get_no_roof_surfaces(j); // depends on the geometry type: Multisurface, solid...
    std::cout << "Total RoofSurface: " << noroofsurfaces << '\n';

    // list_all_vertices(j);

    visit_roofsurfaces(j);

    //-- print out the number of Buildings in the file
    int nobuildings = 0;
    for (auto& co : j["CityObjects"]) {
        if (co["type"] == "Building") {
            nobuildings += 1;
        }
    }
    std::cout << "There are " << nobuildings << " Buildings in the file" << '\n';

    //-- print out the number of vertices in the file
    std::cout << "Number of vertices " << j["vertices"].size() << '\n';

    //-- add an attribute "volume"
    for (auto& co : j["CityObjects"]) {
        if (co["type"] == "Building") {
            co["attributes"]["volume"] = rand();
        }
    }

    //std::cout << "list all vertices" << '\n';
    //list_all_vertices(j);

    /*
    * test determinant calculation
    */
    std::vector<Vertex> each_triangulated_face;
    each_triangulated_face.reserve(3);

    std::vector<std::vector<Vertex>> one_solid;

    Vertex v1(3, 3, 0), v2(2, 3, 4);
    Vertex v3(3, 2, 4), v4(3, 3, 4);
    
    each_triangulated_face.emplace_back(v1);
    each_triangulated_face.emplace_back(v2);
    each_triangulated_face.emplace_back(v3);

    one_solid.emplace_back(each_triangulated_face);

    each_triangulated_face[0] = v2;
    each_triangulated_face[1] = v1;
    each_triangulated_face[2] = v4;

    one_solid.emplace_back(each_triangulated_face);

    each_triangulated_face[0] = v4;
    each_triangulated_face[1] = v1;
    each_triangulated_face[2] = v3;

    one_solid.emplace_back(each_triangulated_face);

    each_triangulated_face[0] = v4;
    each_triangulated_face[1] = v3;
    each_triangulated_face[2] = v2;

    one_solid.emplace_back(each_triangulated_face);

    double volume_one_solid = Volume::calculate_volume(one_solid);
    std::cout << "volume of one solid: " << volume_one_solid << '\n';

    //-- write to disk the modified city model (myfile.city.json)
    /*std::string writefilename = "/testwrite.json";
    std::ofstream o(DATA_PATH + writefilename);
    o << j.dump(2) << std::endl;
    o.close();*/

   
    return 0;
}


// Visit every 'RoofSurface' in the CityJSON model and output its geometry (the arrays of indices)
// Useful to learn to visit the geometry boundaries and at the same time check their semantics.
// A city json object can contain several city objects, each city object may have ONE geometry
void Hugo_visit_roofsurfaces(json& j) {
    for (auto& co : j["CityObjects"].items()) { // each city object 
        
        for (auto& g : co.value()["geometry"]) { // each city object may only have ONE geometry
            
            if (g["type"] == "MultiSurface") { // type of geometry, may be multisurface, solid... 
                
                for (int i = 0; i < g["boundaries"].size(); i++) { // index of each primitive in "boundaries"
                   
                    for (int j = 0; j < g["boundaries"][i].size(); j++) { // for each primitive in "boundaries"
                        int sem_index = g["semantics"]["values"][i][j];
                        
                        if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) {
                            std::cout << "RoofSurface: " << g["boundaries"][i][j] << '\n';
                        }
                    } // end for: each primitive
                } // end for: index of each primitive
            } // end if
        } // end for: each geometry
    } // end for: each city object
}


// Visit every 'RoofSurface' in the CityJSON model and output its geometry (the arrays of indices)
// Useful to learn to visit the geometry boundaries and at the same time check their semantics.
void visit_roofsurfaces(json& j) {
    for (auto& co : j["CityObjects"].items()) { // each city object 

        for (auto& g : co.value()["geometry"]) { // each city object may only have ONE geometry

            if (g["type"] == "MultiSurface") { // type of geometry, may be multisurface, solid... 

                for (int i = 0; i < g["boundaries"].size(); i++) { // index of each primitive in "boundaries"
					int semantic_index = g["semantics"]["values"][i];

					if (g["semantics"]["surfaces"][semantic_index]["type"].get<std::string>().compare("RoofSurface") == 0) {
                        if (g["boundaries"][i].size() == 1) // no inner face
                        {
                            std::cout << "RoofSurface index in boundaries: " << i << " ";
                            std::cout << "RoofSurface: " << " ";

                            // print the surface list
                            int Noneface = (int)g["boundaries"][i][0].size();
                            std::cout << "[ [ ";
                            for (int j = 0; j != Noneface - 1; ++j) // except for the last element
                            {
                                std::cout << g["boundaries"][i][0][j] << ", ";
                            }
                            std::cout << g["boundaries"][i][0][Noneface - 1] << " ] ]" << '\n'; // print the last element
                        }
                        else {
                            std::cout << "The RoofSurface: " << i << " contains inner face" << '\n';
                        } // end else
                        
					} // end if: if the current surface belongs to "RoofSurface"

                } // end for: index of each primitive
            } // end if
        } // end for: each geometry
    } // end for: each city object
}


// Returns the number of 'RooSurface' in the CityJSON model
int get_no_roof_surfaces(json& j) {
    int total = 0;
    for (auto& co : j["CityObjects"].items()) { // for each city object
        
        for (auto& g : co.value()["geometry"]) { // each city object can only have one geometry
            
            if (g["type"] == "MultiSurface") { // type of geometry, may be multisurface, solid...
                
                for (auto& shell : g["semantics"]["values"]) {
                    
                    for (auto& s : shell) {
                        if (g["semantics"]["surfaces"][s.get<int>()]["type"].get<std::string>().compare("RoofSurface") == 0) {
                            total += 1;
                        }
                    }
                } // end for: each item in "values" array
            }
        } // end for: geometry
    } // end for: CityObjects
    return total;
}


// CityJSON files have their vertices compressed: https://www.cityjson.org/specs/1.1.1/#transform-object
// this function visits all the surfaces and print the (x,y,z) coordinates of each vertex encountered
void list_all_vertices(json& j) {
    for (auto& co : j["CityObjects"].items()) {
        std::cout << "= CityObject: " << co.key() << std::endl;
        for (auto& g : co.value()["geometry"]) {
            if (g["type"] == "MultiSurface") { // geometry type
                for (auto& shell : g["boundaries"]) {
                    for (auto& surface : shell) {
                        for (auto& ring : surface) {
                            std::cout << "---" << std::endl;
                            for (auto& v : ring) {
                                std::vector<int> vi = j["vertices"][v.get<int>()];
                                double x = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
                                double y = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
                                double z = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();
                                std::cout << std::setprecision(2) << std::fixed << v << " (" << x << ", " << y << ", " << z << ")" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
}



