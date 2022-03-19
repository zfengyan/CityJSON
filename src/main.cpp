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
void print_roofsurfaces_vertices(json& j);

class errorProcess;

class calculateVolume;

class writeAttributes;
class writeFiles;


class errorProcess {
public:
    static void error_preprocess(json& errorjson, std::vector<ErrorObject>& error_objects)
    {
        for (auto& f : errorjson["features"].items()) // -- each building
        {
            if (!f.value()["validity"]) // false -- each false building
            {
                //std::cout << "key: " << f.key() << '\n';
                ErrorObject eobj;
                eobj.building_id = f.value()["id"];

                for (auto& p : f.value()["primitives"]) // each building part of false building
                {
                    if (!p["validity"]) // for each buildingpart, the error type is the same
                    {      
                        eobj.building_part_id = p["id"];

                        for (auto& e : p["errors"]) // for each error
                        {
                            //std::cout << e["code"].get<int>() << " "; // check the error code for one building part

                            if (e["code"].get<int>() == 102) // error 102
                            {
                                eobj.error_code = e["code"].get<int>();
                                eobj.error_type = e["description"].get<std::string>();

                                std::string str_id = e["id"].get<std::string>();
                                int b_index = atoi(str_id.c_str());
                                eobj.boundaries_index.emplace_back(b_index);

                            }

                            if (e["code"].get<int>() == 302) // unclosed shell
                            {

                            }

                            if (e["code"].get<int>() == 303) // non manifold
                            {
                                eobj.error_code = e["code"].get<int>();
                                eobj.error_type = e["description"].get<std::string>();

                                std::string str_id = e["id"].get<std::string>();
                                int b_index = atoi(str_id.c_str());
                                eobj.boundaries_index.emplace_back(b_index);
                            }

                        }

                    }


                } // end for: building part

                error_objects.emplace_back(eobj); // some error objects don't have geometry

               
            }// end if: validity = false
        }
    }


    /*
    * process error consecutive points: error 102
    */
    static void error_process_consecutive_points(
        json& jsonfile,
        std::vector<ErrorObject>& error_objects)
    {
        for (auto& eobj : error_objects)
        {
            if (eobj.error_code == 102) // each consecutive points object
            {
                auto& bo = jsonfile["CityObjects"][eobj.building_id]; // building object
                std::string building_part_key = bo["children"][0]; // one children of building object
                auto& bo_part = jsonfile["CityObjects"][building_part_key]; // building part

                for (auto& g : bo_part["geometry"]) {
                    for (auto& shell : g["boundaries"]){
                        // surface position in boundaries list: shell[index] -- [[1,2,3], [4,5,6]]
                        for (auto& index : eobj.boundaries_index)
                        {
                            auto& surface = shell[index];
                            for (auto& ring : surface)
                            {
                                std::cout << ring << '\n';
                                for (auto& v : ring)
                                {
                                    std::vector<int> vi = jsonfile["vertices"][v.get<int>()];
                                    double x = (vi[0] * jsonfile["transform"]["scale"][0].get<double>()) + jsonfile["transform"]["translate"][0].get<double>();
                                    double y = (vi[1] * jsonfile["transform"]["scale"][1].get<double>()) + jsonfile["transform"]["translate"][1].get<double>();
                                    double z = (vi[2] * jsonfile["transform"]["scale"][2].get<double>()) + jsonfile["transform"]["translate"][2].get<double>();
                                   
                                    //std::cout << v << " (" << x << " " << y << " " << z << ")" << '\n';
                                }
                            }
                        }
                        
                    }
                }

                std::cout << '\n';
                
            } // end if: consecutive points object
            
        }
        
    }


    /*
    * non manifold error 
    */
    static void error_process_non_manifold(
        json& jsonfile,
        std::vector<ErrorObject>& error_objects)
    {
        for (auto& eobj : error_objects)
        {
            if (eobj.error_code == 303) // each non-manifold object
            {
                auto& bo = jsonfile["CityObjects"][eobj.building_id]; // building object
                std::string building_part_key = bo["children"][0]; // one children of building object
                auto& bo_part = jsonfile["CityObjects"][building_part_key]; // building part

                for (auto& g : bo_part["geometry"]) {
                    for (auto& shell : g["boundaries"]) {
                        // surface position in boundaries list: shell[index] -- [[1,2,3], [4,5,6]]
                        for (auto& index : eobj.boundaries_index)
                        {
                            std::cout << "index: " << index << '\n';
                            auto& surface = shell[index];

                            for (auto& ring : surface)
                            {
                                std::cout << ring << '\n';
                                for (auto& v : ring)
                                {
                                    std::vector<int> vi = jsonfile["vertices"][v.get<int>()];
                                    double x = (vi[0] * jsonfile["transform"]["scale"][0].get<double>()) + jsonfile["transform"]["translate"][0].get<double>();
                                    double y = (vi[1] * jsonfile["transform"]["scale"][1].get<double>()) + jsonfile["transform"]["translate"][1].get<double>();
                                    double z = (vi[2] * jsonfile["transform"]["scale"][2].get<double>()) + jsonfile["transform"]["translate"][2].get<double>();

                                    std::cout << v << " (" << x << " " << y << " " << z << ")" << '\n';
                                }
                                std::cout << '\n';
                            }
                        }

                    }
                }

                std::cout << '\n';

            } // end if: non-manifold object

        }
    }

};



/*
* calculate volume
*/
class calculateVolume {
public:
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
    * 
    *  
    * std::map<std::string, double> volume_dictionary;
    * --> each key is the Building ID
    * --> each value is the volume of the corresponding Building
    */
    static void calculate_volume(json& j, std::map<std::string, double>& volume_dictionary)
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
                //std::cout << "CityObject has geometry: " << co.key() << '\n';
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
                                * each surface in each shell: [[v1, v2, v3]] -- in triangulated file, there are no inner rings
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
        //std::cout << '\n';

        /***********************************************************************************/


        /*
        * iterate volume_dictionary
        ************************************************************************************/

        std::cout << "volume dictionary: " << '\n';
		std::map<std::string, double>::iterator it;
        int count = 0;
		for (it = volume_dictionary.begin(); it != volume_dictionary.end(); ++it)
		{
            //std::cout << it->first << "    " << it->second << '\n';
            ++count;
		}
        std::cout << "total elements in volume dictionary: " << count << '\n';

        /***********************************************************************************/

    }

};



/*
* calculate floor and write to attributes of Buildings
*/
class calculateFloor {
public:
    // function to calculate the number of floor.
    static void cal_floor(json& j)
    {
        for (auto& co : j["CityObjects"].items()) {
            //std::cout << co.value()["type"] << std::endl;
            if (co.value()["type"] == "Building") {
                //std::cout << co.value()["attributes"] << std::endl;
                for (auto& g : co.value()) {
                    //std::cout << g << std::endl;
                    if (g["h_dak_max"] == nullptr || g["h_dak_min"] == nullptr || g["h_maaiveld"] == nullptr)
                        break;
                    else {
                        double h_dak_max = g["h_dak_max"];
                        double h_dak_min = g["h_dak_min"];
                        double h_maaiveld = g["h_maaiveld"];
                        double height = (h_dak_max - h_dak_min) * 0.7 + (h_dak_min - h_maaiveld);
                        double floor = height / 3;
                        if (floor >= int(height / 3) + 0.5) floor = int(height / 3) + 1;
                        else if (floor<int(height / 3) + 0.5) floor = int(height / 3);
                        g.push_back({ "floor", floor });
                        break;
                    }
                }
            }
        }
    }
};



/*
* calculate orientation
* use the original file to calculate the orientation
* to avoid the triangulated faces which point inwards
*/
class buildRoofSurfaces {
public:  
    /*
    * orientation and area of roof surfaces
    * @param:
    * roof_surfaces_dictionary 
    * -- key: buildingpart_id
    * -- values: a vector contains the roof surface objects of this buildingpart
    */
    static void build_roof_surfaces(
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

                            int r_id = 0;
                            std::string connect = "."; // used for build roof id
                            std::string r_marker = "roof";

                            for (int j = 0; j < g["boundaries"][i].size(); ++j) { // g["boundaries"][i]: [ [[1,2,3,4]], [[5,6,7,8]] ] -- contains multiple surfaces         
                                
                                int sem_index = g["semantics"]["values"][i][j]; // semantic values index
                                
                                // if it's a roof surface
                                if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) 
                                {
                                    RoofSurface roof; // build a new RoofSurface object
                                    roof.BuildingPart_id = co.key();
                                    roof.type = "RoofSurface";
                                    roof.roof_id = co.key() + connect + r_marker + connect + std::to_string(r_id);
                                    ++r_id; // once used r_id, ++r_id -- unique for one buildingpart
                                    roof.boundaries_index = j;
                                    roof.semantics_surfaces_index = new_semantic_surfaces_index;
                                    ++new_semantic_surfaces_index; // once added a new roof surface, the index += 1
                                    
                                    //std::cout << "RoofSurface: " << g["boundaries"][i][j] << '\n';

                                    // construct vertices of this roof
                                    // g["boundaries"][i][j] : [[1,2,3,4], [4,5,6],...[]], each roof face, may contain inner faces
                                    std::vector<Vertex> one_interior;
                                    for (int m = 0; m < g["boundaries"][i][j].size(); ++m)
                                    {
                                        // g["boundaries"][i][j][m] : [1,2,3,4], [4,5,6], ... []
                                        auto N = g["boundaries"][i][j][m].size();
                                      
                                        for (int n = 0; n < N; ++n) // get vertices for this face
                                        {                   
                                            int v = g["boundaries"][i][j][m][n].get<int>();
                                            std::vector<int> vi = jsonfile["vertices"][v];
                                            
                                            double x = (vi[0] * jsonfile["transform"]["scale"][0].get<double>()) + jsonfile["transform"]["translate"][0].get<double>();
                                            double y = (vi[1] * jsonfile["transform"]["scale"][1].get<double>()) + jsonfile["transform"]["translate"][1].get<double>();
                                            double z = (vi[2] * jsonfile["transform"]["scale"][2].get<double>()) + jsonfile["transform"]["translate"][2].get<double>();
                                            
                                            //std::cout << v << " (" << x << ", " << y << ", " << z << ")" << '\n';

                                            // add the vertex to the vector according to the default order
                                            // exterior: CCW
                                            // inner: CW
                                            if (m == 0)roof.exteriorSurface.emplace_back(Vertex(x, y, z, v));
                                            else {
                                                one_interior.emplace_back(Vertex(x, y, z, v));
                                                roof.interiorSurfaces.emplace_back(one_interior);
                                                one_interior.clear();
                                            }
                                                      
                                        }
                                        
                                    } //end for: each roof surface

                                    // calculate orientation of this roof surface
                                    // roof.orientation = RoofSurface::calculate_orientation(roof);

                                    // calculate the area of this roof surface
                                    roof.area = RoofSurface::calculate_area_3d(roof);

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
        
        std::cout << '\n';
        std::cout << "roof surfaces dictionary: " << '\n';
        std::map<std::string, std::vector<RoofSurface>>::iterator it;
        int count = 0;
        int countRoof = 0;
        for (it = roof_surfaces_dictionary.begin(); it != roof_surfaces_dictionary.end(); ++it)
        {
            std::string key = it->first;
            //std::cout << key << "    ";
            //std::cout << "roof surface(s): " << it->second.size() << '\n';

            //for (auto& roof : roof_surfaces_dictionary[key])
                //std::cout << roof.roof_id << '\n';
            
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
* write attributes: volume, orientation, area
*/
class writeAttributes {
public:
    /*
    * write the orientation to the:
    * "semantics" -> "surfaces"
    * "semantics" -> "values"
    */
    static void write_orientation_area(
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
                            //sur[roof.semantics_surfaces_index]["BuildingPart_id"] = roof.BuildingPart_id;
                            //sur[roof.semantics_surfaces_index]["boundaries_index"] = roof.boundaries_index;
                            sur[roof.semantics_surfaces_index]["roof_id"] = roof.roof_id;

                            // orientation
                            if(roof.orientation != "null")sur[roof.semantics_surfaces_index]["orientation"] = roof.orientation;
                            else sur[roof.semantics_surfaces_index]["orientation"] = nullptr;

                            // area
                            if (roof.area > epsilon)sur[roof.semantics_surfaces_index]["area"] = ceil(roof.area * 100) / 100; // restrict the decimal places
                            else sur[roof.semantics_surfaces_index]["area"] = nullptr;

                            // area_tri
                            if (roof.area_tri > epsilon)sur[roof.semantics_surfaces_index]["area_tri"] = ceil(roof.area_tri * 100) / 100; // restrict the decimal places
                            else sur[roof.semantics_surfaces_index]["area_tri"] = nullptr;

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


    /*
    * write the volume to the building object
    * set the null value for buildings without geometry
    */
    static void write_volume(
        json& jsonfile,
        std::map<std::string, double>& volume_dictionary)
    {
		for (auto& co : jsonfile["CityObjects"].items()) {
            if (co.value()["type"] == "Building") {
                double volume = volume_dictionary[co.key()];

                if (abs(volume) > epsilon) co.value()["attributes"]["volume"] = ceil(volume * 100) / 100; // restrict the decimal places
                else co.value()["attributes"]["volume"] = nullptr; // set null for buildings without geometry
            
            }
		}
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



/*
* use generated file to find triangles belong to which original surface
*/
class RoofSurfaceTriangles {
public:

    /*
    * @param:
    * original_write: after adding new semantic objects(the roof surfaces are marked with roof_id)
    * triangulated_write: triangulated file of original_write
    * roof_surfaces_dictionary: roof surfaces dictionary
    * -- key: buildingpart id
    * -- value: a vector contains roof surface objects of each building
    */
    static void roof_surface_triangles(
        json& j_triangulated_write,
        std::map<std::string, std::vector<RoofSurface>>& roof_surfaces_dictionary)
    {
        for (auto& co : j_triangulated_write["CityObjects"].items()) { // each city object 

            for (auto& g : co.value()["geometry"]) { // each city object may only have ONE geometry

                if (g["type"] == "Solid") { // type of geometry, may be multisurface, solid... 
                    
                    std::string key_bp = co.key(); // key: building part id
                    //std::cout << "key: " << key_bp << '\n';

                    for (int i = 0; i < g["boundaries"].size(); i++) { // index of each primitive in "boundaries"

                        for (int j = 0; j < g["boundaries"][i].size(); j++) { // for each primitive in "boundaries"
                            int sem_index = g["semantics"]["values"][i][j];

                            if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) { // if it's a roof surface
                                std::string rid = g["semantics"]["surfaces"][sem_index]["roof_id"];
                                
                                for (auto& roof : roof_surfaces_dictionary[key_bp]) // each roof
                                {
                                    if (rid == roof.roof_id) {
                                        //std::cout << roof.roof_id << " ";
                                        //std::cout << g["boundaries"][i][j] << " ";

                                        std::vector<Vertex> one_triangle;
                                        for (auto& v : g["boundaries"][i][j][0]) // each vertex
                                        {
                                            //std::cout << v.get<int>() << " ";
                                            std::vector<int> vi = j_triangulated_write["vertices"][v.get<int>()];
                                            double x = (vi[0] * j_triangulated_write["transform"]["scale"][0].get<double>()) + j_triangulated_write["transform"]["translate"][0].get<double>();
                                            double y = (vi[1] * j_triangulated_write["transform"]["scale"][1].get<double>()) + j_triangulated_write["transform"]["translate"][1].get<double>();
                                            double z = (vi[2] * j_triangulated_write["transform"]["scale"][2].get<double>()) + j_triangulated_write["transform"]["translate"][2].get<double>();
                                            one_triangle.emplace_back(Vertex(x, y, z));
                                        }
                                        roof.triangles.emplace_back(one_triangle);
                                        one_triangle.clear();
                                            
                                        //std::cout << '\n';
                                    }
                                }
                                
                            }
                        } 
                    } 
                } // end if
            } // end for: each geometry
        } // end for: each city object
    }
};



class calculateArea {
public:

    // area of one triangle, using Heron's formula
    static double one_triangle_area(std::vector<Vertex>& one_triangle)
    {
        double a = sqrt((one_triangle[0].x - one_triangle[1].x) * (one_triangle[0].x - one_triangle[1].x) + (one_triangle[0].y - one_triangle[1].y) * (one_triangle[0].y - one_triangle[1].y) + (one_triangle[0].z - one_triangle[1].z) * (one_triangle[0].z - one_triangle[1].z));
        double b = sqrt((one_triangle[1].x - one_triangle[2].x) * (one_triangle[1].x - one_triangle[2].x) + (one_triangle[1].y - one_triangle[2].y) * (one_triangle[1].y - one_triangle[2].y) + (one_triangle[1].z - one_triangle[2].z) * (one_triangle[1].z - one_triangle[2].z));
        double c = sqrt((one_triangle[2].x - one_triangle[0].x) * (one_triangle[2].x - one_triangle[0].x) + (one_triangle[2].y - one_triangle[0].y) * (one_triangle[2].y - one_triangle[0].y) + (one_triangle[2].z - one_triangle[0].z) * (one_triangle[2].z - one_triangle[0].z));
        double s = (a + b + c) * 0.5;
        double area = sqrt(s * (s - a) * (s - b) * (s - c));
        return area;
    }


    // calculate area and
    // @param: roof_surfaces_dictionary
    static void calculate_tri_area(std::map<std::string, std::vector<RoofSurface>>& roof_surfaces_dictionary)
    {
        std::map<std::string, std::vector<RoofSurface>>::iterator it;
        for (it = roof_surfaces_dictionary.begin(); it != roof_surfaces_dictionary.end(); ++it)
        {
            std::string key = it->first;

            for (auto& roof : it->second) // each roof
            {
                for (auto& onetri : roof.triangles) // each triangle of each roof
                {
                    roof.area_tri += one_triangle_area(onetri); // sum up area

                    // add triangle to triangle_list
                    Triangle t;
                    t.v0 = onetri[0];
                    t.v1 = onetri[1];
                    t.v2 = onetri[2];
                    t.t_area = one_triangle_area(onetri);

                    roof.triangle_list.emplace_back(t);

                    //roof.area_tri += t.t_area;
                }
                //std::cout << "tri_area: " << roof.area_tri << '\n';
            }
        }
    }
};



class calculateOrientation {
public:
    
    // orientation
    static void calculate_orientation(std::map<std::string, std::vector<RoofSurface>>& roof_surfaces_dictionary)
    {
        std::map<std::string, std::vector<RoofSurface>>::iterator it;
        for (it = roof_surfaces_dictionary.begin(); it != roof_surfaces_dictionary.end(); ++it)
        {
            std::string key = it->first;
            std::cout << "key: " << key << '\n';

            for (auto& roof : it->second) // each roof
            {
                // find the triangle with the maximum area -- use as the triangle to find normal
                double max_area = 0;
                for (int i = 0; i != roof.triangle_list.size(); ++i) // each triangle of each roof
                {
                    max_area = max_area > roof.triangle_list[i].t_area ? max_area : roof.triangle_list[i].t_area;
                }
                int max_area_index = 0;
                for (int i = 0; i != roof.triangle_list.size(); ++i)
                {
                    if (abs(roof.triangle_list[i].t_area - max_area) < epsilon)max_area_index = i;
                }

                // use the biggest triangle to catch the normal of this roof:
               
                //std::cout << "triangles size: " << " ";
                //std::cout << roof.triangle_list.size() << '\n';

                // avoid triangle_list.size() == 0, due to consecutive points(potentially)
                if (roof.triangle_list.size() == 0)roof.orientation = "null";
                else {
                    roof.roof_normal = Vector3d::get_normal(roof.triangle_list[max_area_index]);
                    roof.get_orientation();
                }

                //std::cout << "roof normal: " << " ";
                //std::cout << roof.roof_normal.x << " " << roof.roof_normal.y << " " << roof.roof_normal.z << '\n';
                //std::cout << "corrsponding triangle: " << '\n';
                //roof.triangle_list[max_area_index].print();
                //std::cout << '\n';
                //roof.get_orientation();

      
            }
        }
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

    std::string filename = "/myfile.city.json";
    std::string filename_triangulated = "/myfile.triangulated.city.json";
    std::string writefilename = "/testwrite.reduce.myfile.city.json";

    std::string errorfile = "/error.report.json";

    std::string testfile = "/NL.IMBAG.Pand.0503100000004247.city.json";

    std::string test_write_filename = "/write.NL.IMBAG.Pand.0503100000004247.city.json";
    //std::string test_write_filename = writefilename;

    std::string testfile_write_triangulated = "/write.NL.IMBAG.Pand.0503100000004247.triangulated.city.json";

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

    // -- reading the error report file
    std::ifstream input_error(DATA_PATH + errorfile);
    json j_error;
    input_error >> j_error;
    input_error.close();

    // -- reading the test file
    std::ifstream input_test(DATA_PATH + testfile);
    json j_test;
    input_test >> j_test;
    input_test.close();

    /**********************************************************************************/

    std::ifstream input_test_write(DATA_PATH + test_write_filename);
    json j_test_write;
    input_test_write >> j_test_write;
    input_test_write.close();
    
    
    std::ifstream input_test_write_triangulated(DATA_PATH + testfile_write_triangulated);
    json j_test_write_triangulated;
    input_test_write_triangulated >> j_test_write_triangulated;
    input_test_write_triangulated.close();

    /**********************************************************************************/

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

    //print_roofsurfaces_vertices(j_test);


    /*
    * volume
    ***********************************************************************************/

    std::map<std::string, double> volume_dictionary;
    calculateVolume::calculate_volume(j_triangulated, volume_dictionary); // use triangulated file to calculte the volume
    writeAttributes::write_volume(j, volume_dictionary); // write attributes to the original file
    std::cout << '\n';

    /**********************************************************************************/


    /*
    * floor -- calculate and write to attributes
    ***********************************************************************************/
    
    calculateFloor::cal_floor(j);

    /**********************************************************************************/


    /*
    * build surfaces
    ***********************************************************************************/

    std::cout << "building surfaces: " << '\n';
    //std::cout << '\n';
    std::map<std::string, std::vector<RoofSurface>> roof_surfaces_dictionary;
    buildRoofSurfaces::build_roof_surfaces(j, roof_surfaces_dictionary);
    writeAttributes::write_orientation_area(j, roof_surfaces_dictionary); // write attributes

    // First output
    /**********************************************************************************/
    std::cout << "first writing files..." << '\n';
    std::string FirstOutput = "/myfile.output.first.city.json";
    writeFiles::write_json_file(j, FirstOutput);
    std::cout << "first writing files done" << '\n';


    // Second input
    /**********************************************************************************/

    std::cout << "Second input: " << '\n';
    std::string SecondInput = "/myfile.input.second.city.json";
    std::string SecondInput_triangulated = "/myfile.input.second.triangulated.city.json";

    std::ifstream input_second(DATA_PATH + SecondInput);
    json j_second;
    input_second >> j_second;
    input_second.close();

    std::ifstream input_second_triangulated(DATA_PATH + SecondInput_triangulated);
    json j_second_triangulated;
    input_second_triangulated >> j_second_triangulated;
    input_second_triangulated.close();

    std::cout << "Second input file: " << SecondInput << '\n';

    std::cout << "roof surface triangles: " << '\n'; // !NB! : use second triangulated file
    RoofSurfaceTriangles::roof_surface_triangles(j_second_triangulated, roof_surfaces_dictionary);
    //std::cout << '\n';


    //calculate area
    std::cout << "calculating area..." << '\n';
    calculateArea::calculate_tri_area(roof_surfaces_dictionary);
    std::cout << "done" << '\n';

    //calculate orientation
    std::cout << "calculating orientation..." << '\n';
    calculateOrientation::calculate_orientation(roof_surfaces_dictionary);
    std::cout << "done" << '\n';

    //update attributes
    writeAttributes::write_orientation_area(j_second, roof_surfaces_dictionary); // write attributes

    std::cout << "processing done" << '\n';
    std::cout << '\n';

    std::cout << "second writing files..." << '\n';
    std::string SecondOutput = "/myfile.output.second.city.json";
    writeFiles::write_json_file(j_second, SecondOutput);
    std::cout << "second writing files done" << '\n';

    /**********************************************************************************/   


    /*
    * error process
    */
    std::vector<ErrorObject> error_objects;
    errorProcess::error_preprocess(j_error, error_objects); // pass j_error as argument


    /**********************************************************************************/
    // consecutive points

    /*for (auto& eobj : error_objects) {
        if(eobj.error_code == 102)std::cout << eobj.building_id << '\n';
    }*/

    //std::cout << '\n';
    //std::cout << "consecutive points: " << '\n';
    //errorProcess::error_process_consecutive_points(j, error_objects);

    /**********************************************************************************/
    // non-manifold
    // 
    //std::cout << "error_objects size: " << error_objects.size() << '\n';
    //std::cout << '\n';

    //std::cout << "non manifold: " << '\n';
    //errorProcess::error_process_non_manifold(j, error_objects); // pass j as argument
    //std::cout << '\n';   

    //for (auto& eobj : error_objects) {
    //    if (eobj.error_code == 303) {
    //        for (auto& index : eobj.boundaries_index)std::cout << index << " ";
    //    }
    //}

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


void print_roofsurfaces_vertices(json& jsonfile) {
    for (auto& co : jsonfile["CityObjects"].items()) { // each city object 

        for (auto& g : co.value()["geometry"]) { // each city object may only have ONE geometry

            if (g["type"] == "Solid") { // type of geometry, may be multisurface, solid... 

                for (int i = 0; i < g["boundaries"].size(); i++) { // index of each primitive in "boundaries"

                    for (int j = 0; j < g["boundaries"][i].size(); j++) { // for each primitive in "boundaries"
                        int sem_index = g["semantics"]["values"][i][j];

                        if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) {
                            std::cout << "RoofSurface: " << g["boundaries"][i][j] << '\n';

                            for (auto& ring : g["boundaries"][i][j])
                            {
                                for (auto& v : ring)
                                {
                                    std::vector<int> vi = jsonfile["vertices"][v.get<int>()];
                                    double x = (vi[0] * jsonfile["transform"]["scale"][0].get<double>()) + jsonfile["transform"]["translate"][0].get<double>();
                                    double y = (vi[1] * jsonfile["transform"]["scale"][1].get<double>()) + jsonfile["transform"]["translate"][1].get<double>();
                                    double z = (vi[2] * jsonfile["transform"]["scale"][2].get<double>()) + jsonfile["transform"]["translate"][2].get<double>();
                                    //std::cout << std::setprecision(2) << std::fixed << v << " (" << x << ", " << y << ", " << z << ")" << std::endl;
                                    std::cout << "v" << " " << x << " " << y << " " << z << " " << '\n';
                                }
                            }
                            std::cout << '\n';
                        }
                    } // end for: each primitive
                } // end for: index of each primitive
            } // end if
        } // end for: each geometry
    } // end for: each city object

}





