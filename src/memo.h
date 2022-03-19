#pragma once

/*
 * test determinant calculation
 ***********************************************************************************/

//std::vector<Vertex> each_triangulated_face;
//each_triangulated_face.reserve(3);
//
//std::vector<std::vector<Vertex>> one_solid;
//
//Vertex v1(3, 3, 0, 0), v2(2, 3, 4, 1);
//Vertex v3(3, 2, 4, 2), v4(3, 3, 4, 3);
//
//each_triangulated_face.emplace_back(v1);
//each_triangulated_face.emplace_back(v2);
//each_triangulated_face.emplace_back(v3);
//
//one_solid.emplace_back(each_triangulated_face);
//
//each_triangulated_face[0] = v2;
//each_triangulated_face[1] = v1;
//each_triangulated_face[2] = v4;
//
//one_solid.emplace_back(each_triangulated_face);
//
//each_triangulated_face[0] = v4;
//each_triangulated_face[1] = v1;
//each_triangulated_face[2] = v3;
//
//one_solid.emplace_back(each_triangulated_face);
//
//each_triangulated_face[0] = v4;
//each_triangulated_face[1] = v3;
//each_triangulated_face[2] = v2;
//
//one_solid.emplace_back(each_triangulated_face);
//
//double volume_one_solid = Volume::calculate_volume_one_solid(one_solid);
//std::cout << "volume of one solid: " << volume_one_solid << '\n';

/**********************************************************************************/



/*
* juedge geometry
***********************************************************************************/

//int ngeometry = 0;
//for (auto& co : j["CityObjects"]) { // each city object 
//	if (co["type"] == "Building") {
//		std::cout << co["geometry"].size() << " ";
//	}
//}

/**********************************************************************************/



/*
* test volume_dictionary
***********************************************************************************/

//std::map<std::string, double>::iterator it;
//for (it = volume_dictionary.begin(); it != volume_dictionary.end(); ++it)
//{
//    std::cout << it->first << " " << it->second << '\n';
//}

/**********************************************************************************/



/*
* boundaries list size of each BuildingPart: if it contains inner shells
***********************************************************************************/

//for (auto& co : j["CityObjects"].items()) {
//    //std::cout << "CityObject: " << co.key() << '\n';
//
//
//    /*
//    * for each BuildingPart object:
//    * --> geometry list is not null
//    * --> has one "parent" object
//    */
//    if (co.value()["geometry"].size() != 0 &&
//        co.value()["parents"].size() == 1)
//    {
//        for (auto& g : co.value()["geometry"])
//        {
//            std::cout << g["boundaries"].size() << '\n'; // if size() == 1 --> no inner shells
//        }
//
//    }
//}

/**********************************************************************************/



/*
* get the actual coordinates of vertices
***********************************************************************************/

//void test_vertices(json& j)
//{
//    std::vector<int> vi = j["vertices"][0];
//    double x = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
//    double y = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
//    double z = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();
//    //std::cout << std::setprecision(2) << std::fixed << v << " (" << x << ", " << y << ", " << z << ")" << std::endl;
//    std::cout << " (" << x << ", " << y << ", " << z << ")" << '\n';
//}

/**********************************************************************************/



/*
* add attributes in semantic objects
***********************************************************************************/

//void test_write_semantic_attributes(json& j)
//{
//    for (auto& co : j["CityObjects"].items()) {
//        if (co.value()["type"] == "BuildingPart") {
//            for (auto& g : co.value()["geometry"])
//            {
//                auto& sur = g["semantics"]["surfaces"];
//                auto& val = g["semantics"]["values"];
//                sur[0]["orientation"] = "NW";
//                sur[3]["type"] = "mysurface";
//                sur[4]["type"] = "wallmysurface";
//
//                val[0][6] = 999;
//            }
//        }
//    }
//}

/**********************************************************************************/



/*
 * test normal vector
 ***********************************************************************************/

//std::vector<Vertex> roofvertices;
//roofvertices.emplace_back(Vertex(1, 1, 0));
//roofvertices.emplace_back(Vertex(0, 1, 0));
//roofvertices.emplace_back(Vertex(0, 1, 1));
//Vector3d& normal = Vector3d::find_normal(roofvertices);
//std::cout << "normal: " << " " << normal.x << " " << normal.y << " " << normal.z << '\n';

/**********************************************************************************/



/*
 * test area in 2d
 ***********************************************************************************/

//RoofSurface roof;
//
//roof.exteriorSurface.emplace_back(Vertex(0, 0, 0));
//roof.exteriorSurface.emplace_back(Vertex(2, 0, 1));
//roof.exteriorSurface.emplace_back(Vertex(2, 2, 2));
//roof.exteriorSurface.emplace_back(Vertex(0, 2, 3));
//
//std::vector<Vertex> interior;
//interior.emplace_back(Vertex(1, 1, 0));
//interior.emplace_back(Vertex(1, 2, 0));
//interior.emplace_back(Vertex(2, 2, 0));
//interior.emplace_back(Vertex(2, 1, 0));
//
//roof.interiorSurfaces.emplace_back(interior);
//
//std::cout << "area 2d: " << " " << RoofSurface::calculate_area_2d(roof) << '\n';

/**********************************************************************************/



/*
* test interior vertices size
***********************************************************************************/

//std::cout << "inner faces: " << '\n';
//std::cout << "roof surfaces dictionary: " << '\n';
//std::map<std::string, std::vector<RoofSurface>>::iterator it;
//
//for (it = roof_surfaces_dictionary.begin(); it != roof_surfaces_dictionary.end(); ++it)
//{
//    std::string key = it->first;
//    std::cout << key << " " << '\n';
//    std::cout << "interior vertices list size for each roof surface of this BuildingPart: " << '\n';
//    for (auto& surface : it->second)
//    {
//        std::cout << surface.interiorVertices.size() << " ";
//    }
//    std::cout << '\n';
//}

/**********************************************************************************/


/*
* build dictionary -- need to test
***********************************************************************************/
//static void build_roofsurface_dictionary(
//	json& jsonfile,
//	std::map<std::string, std::vector<RoofSurface>>& roof_surfaces_dictionary)
//{
//	for (auto& co : jsonfile["CityObjects"].items()) // each city object 
//	{
//		std::cout << "test";
//		if (co.value()["geometry"].size() != 0) // each BuildingPart
//		{
//
//			auto& g = co.value()["geometry"]; // each city object may only have ONE geometry                                      
//			std::vector<RoofSurface> roof_surfaces; // store oriented roof surfaces for each BuildingPart
//			roof_surfaces_dictionary.insert(
//				std::pair<std::string, std::vector<RoofSurface>>(co.key(), roof_surfaces));
//
//			// track the index in "semantics->surfaces" of the newly-added roofsurface with attributes in each solid
//			int new_semantic_surfaces_index = (int)g["semantics"]["surfaces"].size();
//
//			int r_id = 0;
//			std::string connect = "."; // used for build roof id
//			std::string r_marker = "roof";
//
//			// traverse each roof in "boundaries" to find the RoofSurfaces
//			// g["boundaries"]: [[ [[1,2,3,4]], [[5,6,7,8]] ]]
//			for (int j = 0; j < g["boundaries"][0].size(); ++j) { // g["boundaries"][0]: [ [[1,2,3,4]], [[5,6,7,8]] ] -- contains multiple surfaces         
//
//				int sem_index = g["semantics"]["values"][0][j]; // semantic values index
//
//				// if it's a roof surface
//				if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0)
//				{
//					RoofSurface roof; // build a new RoofSurface object
//					roof.BuildingPart_id = co.key();
//					roof.type = "RoofSurface";
//					roof.roof_id = co.key() + connect + r_marker + connect + std::to_string(r_id);
//					++r_id; // once used r_id, ++r_id -- unique for one buildingpart
//					roof.boundaries_index = j;
//					roof.semantics_surfaces_index = new_semantic_surfaces_index;
//					++new_semantic_surfaces_index; // once added a new roof surface, the index += 1
//
//					std::cout << "RoofSurface: " << g["boundaries"][0][j] << '\n';
//
//					// construct vertices of this roof
//					// g["boundaries"][0][j] : [[1,2,3,4], [4,5,6],...[]], each roof face, may contain inner faces
//					std::vector<Vertex> one_interior;
//					for (int m = 0; m < g["boundaries"][0][j].size(); ++m)
//					{
//						// g["boundaries"][i][j][m] : [1,2,3,4], [4,5,6], ... []
//						auto N = g["boundaries"][0][j][m].size();
//
//						for (int n = 0; n < N; ++n) // get vertices for this face
//						{
//							int v = g["boundaries"][0][j][m][n].get<int>();
//							std::vector<int> vi = jsonfile["vertices"][v];
//
//							double x = (vi[0] * jsonfile["transform"]["scale"][0].get<double>()) + jsonfile["transform"]["translate"][0].get<double>();
//							double y = (vi[1] * jsonfile["transform"]["scale"][1].get<double>()) + jsonfile["transform"]["translate"][1].get<double>();
//							double z = (vi[2] * jsonfile["transform"]["scale"][2].get<double>()) + jsonfile["transform"]["translate"][2].get<double>();
//
//							//std::cout << v << " (" << x << ", " << y << ", " << z << ")" << '\n';
//
//							// add the vertex to the vector according to the default order
//							// exterior: CCW
//							// inner: CW
//							if (m == 0)roof.exteriorSurface.emplace_back(Vertex(x, y, z, v));
//							else {
//								one_interior.emplace_back(Vertex(x, y, z, v));
//								roof.interiorSurfaces.emplace_back(one_interior);
//								one_interior.clear();
//							}
//
//						}
//
//					} //end for: each roof surface
//
//					// calculate orientation of this roof surface
//					// roof.orientation = RoofSurface::calculate_orientation(roof);
//
//					// calculate the area of this roof surface
//					roof.area = RoofSurface::calculate_area_3d(roof);
//
//					// add this roof to the roof surfaces dictionary
//					roof_surfaces_dictionary[co.key()].emplace_back(roof);
//
//				}
//			}
//
//		} // end if: BuildingPart            
//
//	} // end for: each city object
//
//
//	/*
//	* iterate roof_surfaces_dictionary
//	************************************************************************************/
//
//	std::cout << '\n';
//	std::cout << "roof surfaces dictionary: " << '\n';
//	std::map<std::string, std::vector<RoofSurface>>::iterator it;
//	int count = 0;
//	int countRoof = 0;
//	for (it = roof_surfaces_dictionary.begin(); it != roof_surfaces_dictionary.end(); ++it)
//	{
//		std::string key = it->first;
//		//std::cout << key << "    ";
//		//std::cout << "roof surface(s): " << it->second.size() << '\n';
//
//		for (auto& roof : roof_surfaces_dictionary[key])std::cout << roof.roof_id << '\n';
//
//		++count;
//		countRoof += (int)it->second.size();
//	}
//	std::cout << '\n';
//	std::cout << "total elements( num of BuildingParts ): " << count << '\n';
//	std::cout << "total roof surfaces: " << countRoof << '\n';
//	std::cout << '\n';
//
//	/***********************************************************************************/
//}
/**********************************************************************************/


/*
* need to re-write:
* !NB! orientation - normal selection
* 
* find_normal()
* 
* calculate_orientation()
* 
* !NB! volume process: invalid solid - set volume as null
* 
* !NB! precision issue -- keep 2 behind dot?
*/