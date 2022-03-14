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
#include <map>
#include "json.hpp"
#include "calculation.hpp"

using json = nlohmann::json;


int   get_no_roof_surfaces(json& j);
void  list_all_vertices(json& j);
void  visit_roofsurfaces(json& j);



/*
* set the null value for buildings without geometry and invalid buildings?
*/
namespace volume {

    /*
    * Function: 
    * calculate the volume of each Building object(building object is one kind of city object)
    * 
    * !NB!: 
    * sum up all the volumes of children(BuildingPart) of one Building
    * and add to the parent Building's attributes list
    * 
    * !NB!: 
    * each Building object may have several childen, type of children: BuildingPart
    * 
    * !NB!: 
    * in myfile.city.json, Building object's geometry is a null list: 
    * "geometry": []
    * 
    * geometry type of BuildingPart: 
    * Solid(array depth: 4)
    * 
    * array depth of "values"(in "geometry" -> "semantics" -> "values"):
    * (array depth of Solid) - 2 = 2
    */
    void calculate_volume(json& j)
    {
        /*
        * std::vector<Vertex> v_each_triangulated_face
        * --> [v1, v2, v3]
        * --> store the 3 vertices for a single triangulated face:
        * --> each element : v1, v2, v3, type: Vertex
        *
        * std::vector<std::vector<Vertex>> v_one_solid
        * --> [[v1, v2, v3], [v4, v5, v6] ... [vn, vn+1, vn+2]]
        * --> store all the triangulated_faces for a single solid
        * --> each element: [v1, v2, v3] -- a vector contains 3 vertices
        */


        /*
        * define vectors
        ************************************************************************************/

        std::vector<Vertex> v_each_triangulated_face;
        v_each_triangulated_face.reserve(3); // maintain 3 elements

        std::vector<std::vector<Vertex>> v_one_shell; // in this case no inner shells, one solid is one shell

        /***********************************************************************************/


        /*
        * std::map<std::string, double> volume_dictionary;
        * --> each key is the Building ID
        * --> each value is the volume of the corresponding Building
        ************************************************************************************/

        std::map<std::string, double> volume_dictionary;

        // initialize -- only store the volume of Building(not BuildingPart)
        for (auto& co : j["CityObjects"].items())
        {
            if (co.value()["type"] == "Building")
            {
                volume_dictionary.insert(std::pair<std::string, double>(co.key(), 0));
            }
        }

        /***********************************************************************************/


        /*
        * get the vertices with their ACTUAL coordinates
        * store all the vertices of each BuildingPart object
        * 
        * calculate the volume of each building(sum up all BuildingParts)
        * 
        * write the calculated volume to the attributes of each building
        ************************************************************************************/

        for (auto& co : j["CityObjects"].items()) {
            //std::cout << "CityObject: " << co.key() << '\n';
            

            /*
            * for each BuildingPart object:
            * --> geometry list is not null
            * --> has one "parent" object
            */
            if (co.value()["geometry"].size() != 0 &&
                co.value()["parents"].size() == 1)
            {
                std::cout << "CityObject has geometry: " << co.key() << '\n';
                //std::cout << "parent id: " << co.value()["parents"][0] << '\n';


                // geometry of BuildingPart object
                for (auto& g : co.value()["geometry"]) {
                    if (g["type"] == "Solid" &&
                        g["boundaries"].size() == 1) // without inner shells
                    { 
                        for (auto& shell : g["boundaries"]) {

                            /*
                            * for each triangulated surface
                            */
                            for (auto& surface : shell) {
                                //std::cout << "---" << '\n';

                                /*
                                * each shell: [ [[v1, v2, v3]], [[v4, v5, v6]]...[[...]] ] -- without inner shells
                                * each surface in each shell: [[v1, v2, v3]] -- without inner rings
                                * each ring: [v1, v2, v3]
                                */
                                for (auto& ring : surface) {

                                    // ring: [v1, v2, v2]
                                    for (auto& v : ring) {
                                        std::vector<int> vi = j["vertices"][v.get<int>()];
                                        double x = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
                                        double y = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
                                        double z = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();
                                        //std::cout << v << " (" << x << ", " << y << ", " << z << ")" << '\n';

                                        // store the 3 vertices of each triangulated face
                                        // x, y, z, vid
                                        v_each_triangulated_face.emplace_back(Vertex(x, y, z, v));
                                    } // end for: xyz coordinates of one vertex


                                    /*
                                    * add the 3 vertices vector to the one_solid vector
                                    * clear the items in the 3 vertices vector
                                    */
                                    v_one_shell.emplace_back(v_each_triangulated_face);
                                    v_each_triangulated_face.clear();

                                } // end for: each triangulated surface

                            } // end for: each surface: [[e1, e2, e3]] -- without inner ring

                        } // end for: each shell in "boundaries"

                    } // end if: Solid and without inner shells

                } // end for: each item in geometry


                // calculate volume for the shell of this BuildingPart object
                // one BuildingPart has one shell in this case
                double volume_one_shell = Volume::calculate_volume_one_shell(v_one_shell);
                v_one_shell.clear(); // after calculation, elements need to be cleared

                // sum up volume of this BuildingPart object's "parent" Building object
                std::string parentBuilding_id = co.value()["parents"][0];
                volume_dictionary[parentBuilding_id] += volume_one_shell;

            }// end if: BuildingPart object

        } // end for: each item in building object
        std::cout << '\n';

        /***********************************************************************************/


        /*
        * iterate volume_dictionary
        ************************************************************************************/

        std::cout << "volume dictionary: " << '\n';
		std::map<std::string, double>::iterator it;
        int count = 0;
		for (it = volume_dictionary.begin(); it != volume_dictionary.end(); ++it)
		{
            std::cout << it->first << "    " << it->second << '\n';
            ++count;
		}
        std::cout << "total elements in volume dictionary: " << count << '\n';

        /***********************************************************************************/

    }

}


/*
* calculate orientation
* use the original file to calculate the orientation
* to avoid the triangulated faces which point inwards
*/
namespace orientation {
    void test(json& j)
    {
		for (auto& co : j["CityObjects"].items()) {
			if (co.value()["type"] == "BuildingPart") {
                for (auto& g : co.value()["geometry"])
                {
                    auto& sur = g["semantics"]["surfaces"];
                    auto& val = g["semantics"]["values"];
                    sur[0]["orientation"] = "NW";
                    sur[3]["type"] = "mysurface";
                    sur[4]["type"] = "wallmysurface";

                    val[0][6] = 999;
                }
			}
		}
    }


    /*
    * orientation
    */
    void calculate_orientation(
        json& j, 
        std::map<std::string, std::vector<RoofSurface>>& roof_surfaces_dictionary)
    {
        for (auto& co : j["CityObjects"].items())
        { // each city object 

            if (co.value()["geometry"].size() != 0) // BuildingPart
            {
                for (auto& g : co.value()["geometry"]) { // each city object may only have ONE geometry

                    if (g["type"] == "Solid") 
                    { 
                        std::vector<RoofSurface> roof_surfaces; // store oriented roof surfaces for each BuildingPart
                        roof_surfaces_dictionary.insert(
                            std::pair<std::string, std::vector<RoofSurface>>(co.key(), roof_surfaces));
                        
                        for (int i = 0; i < g["boundaries"].size(); ++i) { // g["boundaries"]: [[ [[1,2,3,4]], [[5,6,7,8]] ]]

                            for (int j = 0; j < g["boundaries"][i].size(); ++j) { // g["boundaries"][i]: [ [[1,2,3,4]], [[5,6,7,8]] ]
                                int sem_index = g["semantics"]["values"][i][j];
                                if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) {
                                    std::cout << "index: " << j << '\n';
                                    std::cout << "BuildingPart: " << co.key() << '\n';
                                    std::cout << "semantic size: " << g["semantics"]["surfaces"].size() << '\n';

                                    // g["boundaries"][i][j] : [[1,2,3,4]]
                                    std::cout << "RoofSurface: " << g["boundaries"][i][j] << '\n';
                                }
                            } 
                        } 
                    } // end if
                } // end for: each geometry

            } // end if: BuildingPart            

        } // end for: each city object


        /*
        * iterate roof_surfaces_dictionary
        ************************************************************************************/

        std::cout << "roof surfaces dictionary: " << '\n';
        std::map<std::string, std::vector<RoofSurface>>::iterator it;
        int count = 0;
        for (it = roof_surfaces_dictionary.begin(); it != roof_surfaces_dictionary.end(); ++it)
        {
            std::string key = it->first;
            std::cout << "key: " << key << "    ";
            roof_surfaces_dictionary[key].emplace_back(RoofSurface());
            std::cout << "value: " << roof_surfaces_dictionary[key][0].roof_orientation << '\n';
            ++count;
        }
        std::cout << "total elements in roof surfaces dictionary: " << count << '\n';

        /***********************************************************************************/
    }
}


int main(int argc, const char* argv[]) {
    
    /*
    * from teacher
    ***********************************************************************************/

    /*
    * INPUT files
    ***********************************************************************************/

    std::string filename = "/cube.city.json";
    std::string filename_triangulated = "/cube.triangulated.city.json";

    /**********************************************************************************/

    //-- reading the (original)file with nlohmann json: https://github.com/nlohmann/json  
    std::ifstream input(DATA_PATH + filename);
    json j;
    input >> j;
    input.close();

    // -- reading the (triangulated)file
    std::ifstream input_triangulated(DATA_PATH + filename_triangulated);
    json j_triangulated;
    input_triangulated >> j_triangulated;
    input_triangulated.close();

    //-- get total number of RoofSurface in the file
    int noroofsurfaces = get_no_roof_surfaces(j); // depends on the geometry type: Multisurface, solid...
    std::cout << "Total RoofSurface: " << noroofsurfaces << '\n';

    //visit_roofsurfaces(j);

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
    /*for (auto& co : j["CityObjects"]) {
        if (co["type"] == "Building") {
            co["attributes"]["volume"] = rand();
        }
    }*/

    //-- write to disk the modified city model (myfile.city.json)
    /*std::string writefilename = "/testwrite.json";
    std::ofstream o(DATA_PATH + writefilename);
    o << j.dump(2) << std::endl;
    o.close();
    std::cout << '\n';*/

    /**********************************************************************************/

    std::cout << "my output: " << '\n';
    //std::cout << "list all vertices" << '\n';
    volume::calculate_volume(j_triangulated);

    std::cout << '\n';

    std::cout << "orientation test" << '\n';
    std::map<std::string, std::vector<RoofSurface>> roof_surfaces_dictionary;
    orientation::calculate_orientation(j, roof_surfaces_dictionary);
    std::cout << "done" << '\n';
    //std::cout << "test write: " << '\n';
    //orientation::test(j);

    /*std::string writefilename = "/testwrite.json";
    std::ofstream o(DATA_PATH + writefilename);
    o << j << std::endl;
    o.close();
    std::cout << "done" << '\n';*/

    return 0;
}



// Visit every 'RoofSurface' in the CityJSON model and output its geometry (the arrays of indices)
// Useful to learn to visit the geometry boundaries and at the same time check their semantics.
// A city json object can contain several city objects, each city object may have ONE geometry
void visit_roofsurfaces(json& j) {
    for (auto& co : j["CityObjects"].items()) { // each city object 
        
        for (auto& g : co.value()["geometry"]) { // each city object may only have ONE geometry
            
            if (g["type"] == "Solid") { // type of geometry, may be multisurface, solid... 
                
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


// Returns the number of 'RooSurface' in the CityJSON model
int get_no_roof_surfaces(json& j) {
    int total = 0;
    for (auto& co : j["CityObjects"].items()) { // for each city object
        
        for (auto& g : co.value()["geometry"]) { // each city object can only have one geometry
            
            if (g["type"] == "Solid") { // type of geometry, may be multisurface, solid...
                
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
            if (g["type"] == "Solid") { // geometry type: Solid
                for (auto& shell : g["boundaries"]) {
                    for (auto& surface : shell) {
                        for (auto& ring : surface) {
                            std::cout << "---" << std::endl;
                            for (auto& v : ring) {
                                std::vector<int> vi = j["vertices"][v.get<int>()];
                                double x = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
                                double y = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
                                double z = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();
                                //std::cout << std::setprecision(2) << std::fixed << v << " (" << x << ", " << y << ", " << z << ")" << std::endl;
                                std::cout << v << " (" << x << ", " << y << ", " << z << ")" << '\n';
                            }
                        }
                    }
                }
            }
        }
    }
}







