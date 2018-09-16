#include "loadobj.hpp"

#include <smorgasbord/gpu/graphics.hpp>
#include <smorgasbord/util/log.hpp>

#include <glm/glm.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace glm;
using namespace std;
using namespace Smorgasbord;

unique_ptr<Smorgasbord::MeshData> Smorgasbord::LoadOBJ(
	unique_ptr<istream> _file)
{
	unique_ptr<MeshData> meshData(new MeshData());
	MeshData &mesh = *meshData;
	istream &file = *_file;
	
	// Read file
	
	string line;
	string token;
	size_t spacePos;
	size_t lastSpacePos;
	stringstream stream; // helper stream
	
	bool hasNormal = false;
	bool hasTexCoord = false;
	bool reachedFaces = false;
	
	while (!file.eof())
	{
		/// if the last line is empty, getline() doesn't modify the string,
		/// so the previous line would have stuck in the variable
		line = "";
		
		if (file.peek() == '#') // if comment then skip line
		{
			getline(file, line);
			continue;
		}
		
		getline(file, line);
		
		spacePos = line.find(' ');
		string type = line.substr(0, spacePos); // line type
		
		stream.clear(); // reset EOF flag if set
		stream.str(line.substr(spacePos + 1));
		
		vec3 value;
		if (type == "v") // vertex position
		{
			stream >> value.x >> value.y >> value.z;
			mesh.p.push_back(value);
		}
		else if (type == "vn") // vertex normal
		{
			stream >> value.x >> value.y >> value.z;
			mesh.n.push_back(value);
		}
		else if (type == "vt") // texture coordiante
		{
			stream >> value.x >> value.y;
			value.y = 1.0f - value.y; // flip y (obj -> opengl)
			mesh.t.push_back(value.xy());
		}
		else if (type == "f") // face
		{
			if (!reachedFaces)
			{
				hasNormal = mesh.n.size() > 0;
				hasTexCoord = mesh.t.size() > 0;
				reachedFaces = true;
			}
			
			int n = 0;
			lastSpacePos = spacePos;
			
			while (lastSpacePos != string::npos)
			{
				spacePos = line.find(' ', spacePos + 1);
				
				if (spacePos == string::npos)
				{
					token = line.substr(lastSpacePos + 1);
				}
				else
				{
					token = line.substr(
						lastSpacePos + 1, spacePos - (lastSpacePos + 1));
				}
				
				/// OBJ stores indices in the following order:
				/// position index / texture coordinate index / normal index
				
				std::replace(token.begin(), token.end(), '/', ' ');
				stream.clear(); // reset EOF flag if set
				stream.str(token);
				
				uint32_t pi; // vertex index
				uint32_t ti; // texture coordinate index
				uint32_t ni; // normal index
				
				// vertex indices
				{
					stream >> pi;
					mesh.fp.push_back(pi - 1); // OBJ indexes from 1
				}
				
				// texture coordinates
				if (hasTexCoord)
				{
					stream >> ti;
					mesh.ft.push_back(ti - 1); // OBJ indexes from 1
				}
				
				// normals
				if (hasNormal)
				{
					stream >> ni;
					mesh.fn.push_back(ni - 1); // OBJ indexes from 1
				}
				
				lastSpacePos = spacePos;
				n++;
			}
			
			mesh.c.push_back(n);
		}
	}
	
	_file.reset(); //file.close();
	
	meshData->UpdateStatistics();
	return meshData;
}