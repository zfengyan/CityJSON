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