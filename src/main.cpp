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
#include <cassert>
#include "json.hpp"
#include "calculation.hpp"



using json = nlohmann::json;



int   get_no_roof_surfaces(json& j);
void  list_all_vertices(json& j);
void  visit_roofsurfaces(json& j);



class calculateVolume;
class calculateOrientation;
class writeAttributes;
class writeFiles;



/*
* set the null value for buildings without geometry and invalid buildings?
*/
class calculateVolume {

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
    static void calculate_volume(json& j)
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

};



/*
* calculate orientation
* use the original file to calculate the orientation
* to avoid the triangulated faces which point inwards
*/
class calculateOrientation {
public:  
    /*
    * orientation
    */
    static void calculate_orientation(
        json& jsonfile, 
        std::map<std::string, std::vector<RoofSurface>>& roof_surfaces_dictionary)
    {
        for (auto& co : jsonfile["CityObjects"].items()) // each city object 
        {   
            if (co.value()["geometry"].size() != 0) // each BuildingPart
            {
                for (auto& g : co.value()["geometry"]) { // each city object may only have ONE geometry

                    if (g["type"] == "Solid") // valid geometry for one BuildingPart
                    { 
                        std::vector<RoofSurface> roof_surfaces; // store oriented roof surfaces for each BuildingPart
                        roof_surfaces_dictionary.insert(
                            std::pair<std::string, std::vector<RoofSurface>>(co.key(), roof_surfaces));

                        // track the index in "semantics->surfaces" of the newly-added roofsurface with attributes in each solid
                        int new_semantic_surfaces_index = (int)g["semantics"]["surfaces"].size();
                        
                        // traverse each roof in "boundaries" to find the RoofSurfaces
                        for (int i = 0; i < g["boundaries"].size(); ++i) { // g["boundaries"]: [[ [[1,2,3,4]], [[5,6,7,8]] ]]
                            for (int j = 0; j < g["boundaries"][i].size(); ++j) { // g["boundaries"][i]: [ [[1,2,3,4]], [[5,6,7,8]] ]         
                                
                                int sem_index = g["semantics"]["values"][i][j]; // semantic values index
                                
                                // if it's a roof surface
                                if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) 
                                {
                                    RoofSurface roof;
                                    roof.BuildingPart_id = co.key();
                                    roof.type = "RoofSurface";
                                    roof.boundaries_index = j;
                                    roof.semantics_surfaces_index = new_semantic_surfaces_index;
                                    ++new_semantic_surfaces_index; // once added a new roof surface, the index += 1
                                    
                                    //std::cout << "RoofSurface: " << g["boundaries"][i][j] << '\n';

                                    // construct vertices of this roof
                                    // g["boundaries"][i][j] : [[1,2,3,4]], each roof face
                                    for (int m = 0; m < g["boundaries"][i][j].size(); ++m)
                                    {
                                        // g["boundaries"][i][j][m] : [1,2,3,4]
                                        auto N = g["boundaries"][i][j][m].size();
                                        //assert(N >= 3);
                                        for (int n = 0; n < 3; ++n) // get the first 3 vertices
                                        {                   
                                            int v = g["boundaries"][i][j][m][n].get<int>();
                                            std::vector<int> vi = jsonfile["vertices"][v];
                                            
                                            double x = (vi[0] * jsonfile["transform"]["scale"][0].get<double>()) + jsonfile["transform"]["translate"][0].get<double>();
                                            double y = (vi[1] * jsonfile["transform"]["scale"][1].get<double>()) + jsonfile["transform"]["translate"][1].get<double>();
                                            double z = (vi[2] * jsonfile["transform"]["scale"][2].get<double>()) + jsonfile["transform"]["translate"][2].get<double>();
                                            
                                            //std::cout << v << " (" << x << ", " << y << ", " << z << ")" << '\n';

                                            // add the vertex to the vector according to the default order
                                            roof.RoofVertices.emplace_back(Vertex(x, y, z, v));
                                        }
                                        
                                    } //end for: each roof surface

                                    // calculate orientation of this roof surface
                                    roof.orientation = RoofSurface::calculate_orientation(roof);

                                    // add this roof to the roof surfaces dictionary
                                    roof_surfaces_dictionary[co.key()].emplace_back(roof);

                                }
                            } 
                        } 
                    } // end if

                } // end for: each geometry of one BuildingPart object

            } // end if: BuildingPart            

        } // end for: each city object


        /*
        * iterate roof_surfaces_dictionary
        ************************************************************************************/

        std::cout << "roof surfaces dictionary: " << '\n';
        std::map<std::string, std::vector<RoofSurface>>::iterator it;
        int count = 0;
        int countRoof = 0;
        for (it = roof_surfaces_dictionary.begin(); it != roof_surfaces_dictionary.end(); ++it)
        {
            std::string key = it->first;
            std::cout << key << "    ";
            std::cout << "roof surface(s): " << it->second.size() << '\n';
            
            ++count;
            countRoof += (int)it->second.size();
        }
        std::cout << '\n';
        std::cout << "total elements( num of BuildingParts ): " << count << '\n';
        std::cout << "total roof surfaces: " << countRoof << '\n';
        std::cout << '\n';

        /***********************************************************************************/
    }

};



/*
* write attributes
*/
class writeAttributes {
public:
    /*
    * write the orientation to the:
    * "semantics" -> "surfaces"
    * "semantics" -> "values"
    */
    static void write_orientation(
        json& jsonfile,
        std::map<std::string, std::vector<RoofSurface>>& roof_surfaces_dictionary)
    {
        for (auto& co : jsonfile["CityObjects"].items()) {
            if (co.value()["type"] == "BuildingPart") {
                std::string key = co.key();

                for (auto& g : co.value()["geometry"])
                {
                    if (g["type"] == "Solid") // valid geometry for one BuildingPart
                    {
                        auto& sur = g["semantics"]["surfaces"];
                        auto& val = g["semantics"]["values"];

                        for (auto& roof : roof_surfaces_dictionary[key])
                        {
                            sur[roof.semantics_surfaces_index]["type"] = roof.type;
                            sur[roof.semantics_surfaces_index]["BuildingPart_id"] = roof.BuildingPart_id;
                            sur[roof.semantics_surfaces_index]["boundaries_index"] = roof.boundaries_index;
                            sur[roof.semantics_surfaces_index]["orientation"] = roof.orientation;

                            // change the corresponding index values in "semantics" -> "values" array
                            // Solid array depth: 4
                            // semantics -> values array depth: 4 - 2 = 2
                            val[0][roof.boundaries_index] = roof.semantics_surfaces_index;

                        } // end for: each roof surface of one BuildingPart

                    } // end if: Solid geometry

                } // end for: each geometry

            } // end if: BuildingPart

        } // end for: each city object

    }
};



/*
* write files
*/
class writeFiles {
public:
    static void write_json_file(json& j, std::string& writefilename)
    {
        std::ofstream o(DATA_PATH + writefilename); // std::string writefilename = "/testwrite.json";
        o << j << std::endl;
        o.close();
    }
};



int main(int argc, const char* argv[]) {
    
    /*
    * from teacher
    ***********************************************************************************/

    /*
    * INPUT files
    * OUTPUT file name
    * Modify INPUT and OUTPUT files here
    ***********************************************************************************/

    std::string filename = "/cube.city.json";
    std::string filename_triangulated = "/cube.triangulated.city.json";
    std::string writefilename = "/testwrite.cube.city.json";

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
    std::cout << "Total Buildings: " << nobuildings << '\n'; //std::cout << "There are " << nobuildings << " Buildings in the file" << '\n';
    
    //-- print out the number of BuildingParts in the file
    int nobuildingparts = 0;
    for (auto& co : j["CityObjects"]) {
        if (co["type"] == "BuildingPart") {
            nobuildingparts += 1;
        }
    }
    std::cout << "Total BuildingParts: " << nobuildingparts << '\n';

    //-- print out the number of vertices in the file
    std::cout << "Number of vertices " << j["vertices"].size() << '\n';

    std::cout << '\n';

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
    std::cout << '\n';


    /*
    * volume
    ***********************************************************************************/

    //calculateVolume::calculate_volume(j_triangulated); // use triangulated file to calculte the volume
    //calculateVolume::write_volume(j); // write attributes to the original file
    //std::cout << '\n';

    /**********************************************************************************/


    /*
    * orientation
    ***********************************************************************************/

    std::cout << "orientation test" << '\n';
    std::cout << '\n';
    std::map<std::string, std::vector<RoofSurface>> roof_surfaces_dictionary;
    calculateOrientation::calculate_orientation(j, roof_surfaces_dictionary);
    writeAttributes::write_orientation(j, roof_surfaces_dictionary); // write attributes

    /**********************************************************************************/


    std::cout << "processing done" << '\n';
    std::cout << '\n';


    /*
    * write files
    ***********************************************************************************/

    //std::cout << "writing files..." << '\n';    
    //writeFiles::write_json_file(j, writefilename);
    //std::cout << "writing files done" << '\n';

    /**********************************************************************************/


    /*
    * test orientation calculation
    */
    double alpha = atan(1.414 /1);
    std::cout << alpha;

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
                            for (auto& v : ring) 
                            {
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





